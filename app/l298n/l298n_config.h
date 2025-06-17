#ifndef L298N_L298N_CONFIG_H
#define L298N_L298N_CONFIG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef float float32_t;

typedef enum {
    L298N_ERR_OK = 0,
    L298N_ERR_FAIL = 1 << 0,
    L298N_ERR_NULL = 1 << 1,
} l298n_err_t;

typedef enum {
    L298N_DIRECTION_FORWARD,
    L298N_DIRECTION_BACKWARD,
    L298N_DIRECTION_FAST_STOP,
    L298N_DIRECTION_SOFT_STOP,
} l298n_direction_t;

typedef enum {
    L298N_CHANNEL_TYPE_A,
    L298N_CHANNEL_TYPE_B,
} l298n_channel_type_t;

typedef struct {
    void* gpio_user;
    l298n_err_t (*gpio_init)(void*);
    l298n_err_t (*gpio_deinit)(void*);
    l298n_err_t (*gpio_write)(void*, uint32_t, bool);
    l298n_err_t (*gpio_read)(void*, uint32_t, bool*);

    void* pwm_user;
    l298n_err_t (*pwm_init)(void*);
    l298n_err_t (*pwm_deinit)(void*);
    l298n_err_t (*pwm_set_compare)(void*, uint32_t);

    void* adc_user;
    l298n_err_t (*adc_init)(void*);
    l298n_err_t (*adc_deinit)(void*);
    l298n_err_t (*adc_get_conversion)(void*, uint32_t*);
} l298n_channel_interface_t;

typedef struct {
    uint32_t input1_pin;
    uint32_t input2_pin;

    uint32_t sense_resistance;
    float32_t min_current;
    float32_t max_current;
    float32_t min_voltage;
    float32_t max_voltage;
    uint32_t min_compare;
    uint32_t max_compare;
    uint32_t min_conversion;
    uint32_t max_conversion;
} l298n_channel_config_t;

#endif // L298N_L298N_CONFIG_H