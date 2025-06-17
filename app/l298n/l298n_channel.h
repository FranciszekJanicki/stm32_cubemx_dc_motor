#ifndef L298N_L298N_CHANNEL_H
#define L298N_L298N_CHANNEL_H

#include "l298n_config.h"
#include <stdbool.h>

typedef struct {
    l298n_channel_config_t config;
    l298n_channel_interface_t interface;
} l298n_channel_t;

l298n_err_t l298n_channel_initialize(l298n_channel_t* channel,
                                     l298n_channel_config_t const* config,
                                     l298n_channel_interface_t const* interface);
l298n_err_t l298n_channel_deinitialize(l298n_channel_t* channel);

l298n_err_t l298n_channel_get_voltage(l298n_channel_t const* channel, float32_t* voltage);
l298n_err_t l298n_channel_set_voltage(l298n_channel_t const* channel, float32_t voltage);

l298n_err_t l298n_channel_set_direction(l298n_channel_t const* channel,
                                        l298n_direction_t direction);
l298n_err_t l298n_channel_set_forward_direction(l298n_channel_t const* channel);
l298n_err_t l298n_channel_set_backward_direction(l298n_channel_t const* channel);
l298n_err_t l298n_channel_set_soft_stop_direction(l298n_channel_t const* channel);
l298n_err_t l298n_channel_set_fast_stop_direction(l298n_channel_t const* channel);

#endif // L298N_L298N_CHANNEL_H