#ifndef L298N_L298N_H
#define L298N_L298N_H

#include "l298n_channel.h"
#include "l298n_config.h"

typedef struct {
    l298n_channel_t channels[2];
} l298n_t;

l298n_err_t l298n_initialize(l298n_t* l298n, l298n_channel_t (*channels)[2]);
l298n_err_t l298n_deinitialize(l298n_t* l298n);

l298n_err_t l298n_get_voltage(l298n_t const* l298n, l298n_channel_type_t type, float32_t* voltage);
l298n_err_t l298n_set_voltage(l298n_t const* l298n, l298n_channel_type_t type, float32_t voltage);

l298n_err_t l298n_set_direction(l298n_t const* l298n,
                                l298n_channel_type_t type,
                                l298n_direction_t direction);
l298n_err_t l298n_set_forward_direction(l298n_t const* l298n, l298n_channel_type_t type);
l298n_err_t l298n_set_backward_direction(l298n_t const* l298n, l298n_channel_type_t type);
l298n_err_t l298n_set_soft_stop_direction(l298n_t const* l298n, l298n_channel_type_t type);
l298n_err_t l298n_set_fast_stop_direction(l298n_t const* l298n, l298n_channel_type_t type);

#endif // L298N_L298N_H