#include "l298n.h"
#include <assert.h>
#include <string.h>

static l298n_channel_t const* l298n_get_channel(l298n_t const* l298n, l298n_channel_type_t type)
{
    assert(l298n);

    return &l298n->channels[type];
}

l298n_err_t l298n_initialize(l298n_t* l298n, l298n_channel_t (*channels)[2])
{
    assert(l298n && channels);

    memset(l298n, 0, sizeof(*l298n));
    memcpy(l298n->channels, *channels, sizeof(*channels));

    return L298N_ERR_OK;
}

l298n_err_t l298n_deinitialize(l298n_t* l298n)
{
    assert(l298n);

    memset(l298n, 0, sizeof(*l298n));

    return L298N_ERR_OK;
}

l298n_err_t l298n_get_voltage(l298n_t const* l298n, l298n_channel_type_t type, float32_t* voltage)
{
    assert(l298n && voltage);

    return l298n_channel_get_voltage(l298n_get_channel(l298n, type), voltage);
}

l298n_err_t l298n_set_voltage(l298n_t const* l298n, l298n_channel_type_t type, float32_t voltage)
{
    assert(l298n);

    return l298n_channel_set_voltage(l298n_get_channel(l298n, type), voltage);
}

l298n_err_t l298n_set_direction(l298n_t const* l298n,
                                l298n_channel_type_t type,
                                l298n_direction_t direction)
{
    assert(l298n);

    return l298n_channel_set_direction(l298n_get_channel(l298n, type), direction);
}

l298n_err_t l298n_set_forward_direction(l298n_t const* l298n, l298n_channel_type_t type)
{
    assert(l298n);

    return l298n_channel_set_forward_direction(l298n_get_channel(l298n, type));
}

l298n_err_t l298n_set_backward_direction(l298n_t const* l298n, l298n_channel_type_t type)
{
    assert(l298n);

    return l298n_channel_set_backward_direction(l298n_get_channel(l298n, type));
}

l298n_err_t l298n_set_soft_stop_direction(l298n_t const* l298n, l298n_channel_type_t type)
{
    assert(l298n);

    return l298n_channel_set_soft_stop_direction(l298n_get_channel(l298n, type));
}

l298n_err_t l298n_set_fast_stop_direction(l298n_t const* l298n, l298n_channel_type_t type)
{
    assert(l298n);

    return l298n_channel_set_fast_stop_direction(l298n_get_channel(l298n, type));
}
