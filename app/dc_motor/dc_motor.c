#include "dc_motor.h"
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <string.h>

static inline float32_t dc_motor_get_clamped_position(dc_motor_t const* dc_motor,
                                                      float32_t position)
{
    if (fabsf(position) < dc_motor->config.min_position) {
        return dc_motor->config.min_position;
    } else if (fabsf(position) > dc_motor->config.max_position) {
        return dc_motor->config.max_position;
    }

    return position;
}

static inline float32_t dc_motor_get_clamped_speed(dc_motor_t const* dc_motor, float32_t speed)
{
    if (fabsf(speed) < dc_motor->config.min_speed) {
        return copysignf(dc_motor->config.min_speed, speed);
    } else if (fabsf(speed) > dc_motor->config.max_speed) {
        return copysignf(dc_motor->config.max_speed, speed);
    }

    return speed;
}

static inline float32_t dc_motor_get_clamped_acceleration(dc_motor_t const* dc_motor,
                                                          float32_t acceleration)
{
    if (fabsf(acceleration) < dc_motor->config.min_acceleration) {
        return copysignf(dc_motor->config.min_acceleration, acceleration);
    } else if (fabsf(acceleration) > dc_motor->config.max_acceleration) {
        return copysignf(dc_motor->config.max_acceleration, acceleration);
    }

    return acceleration;
}

static inline dc_motor_direction_t dc_motor_speed_to_direction(dc_motor_t const* dc_motor,
                                                               float32_t speed)
{
    return fabsf(speed) < fabsf(dc_motor->config.stall_speed)
               ? DC_MOTOR_DIRECTION_STOP
               : (speed > 0.0F ? DC_MOTOR_DIRECTION_FORWARD : DC_MOTOR_DIRECTION_BACKWARD);
}

static inline float32_t dc_motor_speed_to_voltage(dc_motor_t const* dc_motor, float32_t speed)
{
    assert(dc_motor->config.max_speed - dc_motor->config.min_speed != 0.0F);

    return (speed - dc_motor->config.min_speed) * dc_motor->config.ref_voltage /
           (dc_motor->config.max_speed - dc_motor->config.min_speed);
}

static dc_motor_err_t dc_motor_driver_init(dc_motor_t const* dc_motor)
{
    return dc_motor->interface.driver_init
               ? dc_motor->interface.driver_init(dc_motor->interface.driver_user)
               : DC_MOTOR_ERR_NULL;
}

static dc_motor_err_t dc_motor_driver_deinit(dc_motor_t const* dc_motor)
{
    return dc_motor->interface.driver_deinit
               ? dc_motor->interface.driver_deinit(dc_motor->interface.driver_user)
               : DC_MOTOR_ERR_NULL;
}

static dc_motor_err_t dc_motor_driver_start(dc_motor_t const* dc_motor)
{
    return dc_motor->interface.driver_start
               ? dc_motor->interface.driver_start(dc_motor->interface.driver_user)
               : DC_MOTOR_ERR_NULL;
}

static dc_motor_err_t dc_motor_driver_stop(dc_motor_t const* dc_motor)
{
    return dc_motor->interface.driver_stop
               ? dc_motor->interface.driver_stop(dc_motor->interface.driver_user)
               : DC_MOTOR_ERR_NULL;
}

static dc_motor_err_t dc_motor_driver_set_voltage(dc_motor_t const* dc_motor, float32_t voltage)
{
    return dc_motor->interface.driver_set_voltage
               ? dc_motor->interface.driver_set_voltage(dc_motor->interface.driver_user, voltage)
               : DC_MOTOR_ERR_NULL;
}

static dc_motor_err_t dc_motor_driver_set_direction(dc_motor_t const* dc_motor,
                                                    dc_motor_direction_t direction)
{
    return dc_motor->interface.driver_set_direction
               ? dc_motor->interface.driver_set_direction(dc_motor->interface.driver_user,
                                                          direction)
               : DC_MOTOR_ERR_NULL;
}

static dc_motor_err_t dc_motor_set_direction(dc_motor_t* dc_motor, dc_motor_direction_t direction)
{
    if (direction == dc_motor->direction) {
        return DC_MOTOR_ERR_OK;
    }

    dc_motor->direction = direction;

    return dc_motor_driver_set_direction(dc_motor, direction);
}

static dc_motor_err_t dc_motor_set_voltage(dc_motor_t* dc_motor, uint32_t voltage)
{
    if (voltage == dc_motor->voltage) {
        return DC_MOTOR_ERR_OK;
    }

    dc_motor->voltage = voltage;

    return dc_motor_driver_set_voltage(dc_motor, voltage);
}

dc_motor_err_t dc_motor_initialize(dc_motor_t* dc_motor,
                                   dc_motor_config_t const* config,
                                   dc_motor_interface_t const* interface)
{
    assert(dc_motor && config && interface);

    memset(dc_motor, 0, sizeof(*dc_motor));
    memcpy(&dc_motor->config, config, sizeof(*config));
    memcpy(&dc_motor->interface, interface, sizeof(*interface));

    return dc_motor_driver_init(dc_motor);
}

dc_motor_err_t dc_motor_deinitialize(dc_motor_t* dc_motor)
{
    assert(dc_motor);

    dc_motor_err_t err = dc_motor_driver_deinit(dc_motor);

    memset(dc_motor, 0, sizeof(*dc_motor));

    return err;
}

dc_motor_err_t dc_motor_reset(dc_motor_t* dc_motor)
{
    assert(dc_motor);

    dc_motor->direction = DC_MOTOR_DIRECTION_STOP;
    dc_motor->voltage = 0.0F;
    dc_motor->prev_speed = 0.0F;
    dc_motor->prev_position = 0.0F;
    dc_motor->prev_acceleration = 0.0F;
    dc_motor->is_running = false;

    return dc_motor_driver_stop(dc_motor);
}

dc_motor_err_t dc_motor_start(dc_motor_t* dc_motor)
{
    assert(dc_motor);

    dc_motor_err_t err = DC_MOTOR_ERR_OK;

    if (!dc_motor->is_running) {
        err |= dc_motor_driver_start(dc_motor);
        dc_motor->is_running = true;
    }

    return err;
}

dc_motor_err_t dc_motor_stop(dc_motor_t* dc_motor)
{
    assert(dc_motor);

    dc_motor_err_t err = DC_MOTOR_ERR_OK;

    if (dc_motor->is_running) {
        err |= dc_motor_driver_stop(dc_motor);
        dc_motor->is_running = false;
    }

    return err;
}

dc_motor_err_t dc_motor_set_position(dc_motor_t* dc_motor,
                                     float32_t position,
                                     float32_t sampling_time)
{
    assert(dc_motor && sampling_time > 0.0F);

    position = dc_motor_get_clamped_position(dc_motor, position);

    float32_t speed = (position - dc_motor->prev_position) / sampling_time;

    dc_motor_err_t err = dc_motor_set_speed(dc_motor, speed, sampling_time);
    if (err == DC_MOTOR_ERR_OK) {
        dc_motor->prev_position = position;
        dc_motor->prev_speed = speed;
    }

    return err;
}

dc_motor_err_t dc_motor_set_speed(dc_motor_t* dc_motor, float32_t speed, float32_t sampling_time)
{
    assert(dc_motor && sampling_time > 0.0F);

    dc_motor_direction_t direction = dc_motor_speed_to_direction(dc_motor, speed);

    dc_motor_err_t err = dc_motor_set_direction(dc_motor, direction);
    if (err != DC_MOTOR_ERR_OK || direction == DC_MOTOR_DIRECTION_STOP) {
        return err;
    }

    speed = dc_motor_get_clamped_speed(dc_motor, speed);

    float32_t voltage = dc_motor_speed_to_voltage(dc_motor, speed);

    err |= dc_motor_set_voltage(dc_motor, voltage);
    if (err == DC_MOTOR_ERR_OK) {
        dc_motor->prev_speed = speed;
    }

    return err;
}

dc_motor_err_t dc_motor_set_acceleration(dc_motor_t* dc_motor,
                                         float32_t acceleration,
                                         float32_t sampling_time)
{
    assert(dc_motor && sampling_time > 0.0F);

    acceleration = dc_motor_get_clamped_acceleration(dc_motor, acceleration);

    float32_t speed = (acceleration + dc_motor->prev_acceleration) * sampling_time / 2.0F;

    dc_motor_err_t err = dc_motor_set_speed(dc_motor, speed, sampling_time);
    if (err == DC_MOTOR_ERR_OK) {
        dc_motor->prev_speed = speed;
        dc_motor->prev_acceleration = acceleration;
    }

    return err;
}
