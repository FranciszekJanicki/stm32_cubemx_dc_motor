#ifndef DC_MOTOR_DC_MOTOR_CONFIG_H
#define DC_MOTOR_DC_MOTOR_CONFIG_H

#include <stdint.h>

typedef float float32_t;

typedef enum {
    DC_MOTOR_ERR_OK = 0,
    DC_MOTOR_ERR_FAIL = 1 << 0,
    DC_MOTOR_ERR_NULL = 1 << 1,
} dc_motor_err_t;

typedef enum {
    DC_MOTOR_DIRECTION_FORWARD,
    DC_MOTOR_DIRECTION_BACKWARD,
    DC_MOTOR_DIRECTION_STOP,
} dc_motor_direction_t;

typedef struct {
    float32_t min_position;
    float32_t max_position;
    float32_t min_speed;
    float32_t max_speed;
    float32_t min_acceleration;
    float32_t max_acceleration;
    float32_t stall_speed;
    float32_t ref_voltage;
} dc_motor_config_t;

typedef struct {
    void* driver_user;
    dc_motor_err_t (*driver_init)(void*);
    dc_motor_err_t (*driver_deinit)(void*);
    dc_motor_err_t (*driver_start)(void*);
    dc_motor_err_t (*driver_stop)(void*);
    dc_motor_err_t (*driver_set_voltage)(void*, float32_t);
    dc_motor_err_t (*driver_set_direction)(void*, dc_motor_direction_t);
} dc_motor_interface_t;

#endif // DC_MOTOR_DC_MOTOR_CONFIG_H