#ifndef DC_MOTOR_DC_MOTOR_H
#define DC_MOTOR_DC_MOTOR_H

#include "dc_motor_config.h"
#include <stdbool.h>

typedef struct {
    dc_motor_config_t config;
    dc_motor_interface_t interface;

    bool is_running;
    float32_t voltage;
    float32_t prev_position;
    float32_t prev_speed;
    float32_t prev_acceleration;
    dc_motor_direction_t direction;
} dc_motor_t;

dc_motor_err_t dc_motor_initialize(dc_motor_t* dc_motor,
                                   dc_motor_config_t const* config,
                                   dc_motor_interface_t const* interface);
dc_motor_err_t dc_motor_deinitialize(dc_motor_t* dc_motor);

dc_motor_err_t dc_motor_reset(dc_motor_t* dc_motor);
dc_motor_err_t dc_motor_start(dc_motor_t* dc_motor);
dc_motor_err_t dc_motor_stop(dc_motor_t* dc_motor);

dc_motor_err_t dc_motor_set_position(dc_motor_t* dc_motor,
                                     float32_t position,
                                     float32_t sampling_time);
dc_motor_err_t dc_motor_set_speed(dc_motor_t* dc_motor, float32_t speed, float32_t sampling_time);
dc_motor_err_t dc_motor_set_acceleration(dc_motor_t* dc_motor,
                                         float32_t acceleration,
                                         float32_t sampling_time);

#endif // DC_MOTOR_DC_MOTOR_H