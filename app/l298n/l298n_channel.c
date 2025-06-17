#include "l298n_channel.h"
#include "l298n_config.h"
#include <assert.h>
#include <string.h>

static l298n_err_t l298n_channel_gpio_init(l298n_channel_t const* channel)
{
    return channel->interface.gpio_init ? channel->interface.gpio_init(channel->interface.gpio_user)
                                        : L298N_ERR_NULL;
}

static l298n_err_t l298n_channel_gpio_deinit(l298n_channel_t const* channel)
{
    return channel->interface.gpio_deinit
               ? channel->interface.gpio_deinit(channel->interface.gpio_user)
               : L298N_ERR_NULL;
}

static l298n_err_t l298n_channel_gpio_write(l298n_channel_t const* channel,
                                            uint32_t pin,
                                            bool state)
{
    return channel->interface.gpio_write
               ? channel->interface.gpio_write(channel->interface.gpio_user, pin, state)
               : L298N_ERR_NULL;
}

static l298n_err_t l298n_channel_pwm_init(l298n_channel_t const* channel)
{
    return channel->interface.pwm_init ? channel->interface.pwm_init(channel->interface.pwm_user)
                                       : L298N_ERR_NULL;
}

static l298n_err_t l298n_channel_pwm_init(l298n_channel_t const* channel)
{
    return channel->interface.pwm_deinit
               ? channel->interface.pwm_deinit(channel->interface.pwm_deinit)
               : L298N_ERR_NULL;
}

static l298n_err_t l298n_channel_pwm_set_compare(l298n_channel_t const* channel, uint32_t compare)
{
    return channel->interface.pwm_set_compare
               ? channel->interface.pwm_set_compare(channel->interface.pwm_user, compare)
               : L298N_ERR_NULL;
}

l298n_err_t l298n_channel_initialize(l298n_channel_t* channel,
                                     l298n_channel_config_t const* config,
                                     l298n_channel_interface_t const* interface)
{
    assert(channel && config && interface);

    memset(channel, 0, sizeof(*channel));
    memcpy(&channel->config, config, sizeof(*config));
    memcpy(&channel->interface, interface, sizeof(*interface));

    l298n_err_t err = l298n_channel_gpio_init(channel);
    err |= l298n_channel_pwm_init(channel);

    return err;
}

l298n_err_t l298n_channel_deinitialize(l298n_channel_t* channel)
{
    assert(channel);

    l298n_err_t err = l298n_channel_degpio_init(channel);
    err |= l298n_channel_pwm_deinit(channel);

    memset(channel, 0, sizeof(*channel));

    return err;
}

l298n_err_t l298n_channel_set_voltage(l298n_channel_t const* channel, float voltage)
{
    assert(channel);

    if (voltage > channel->config.max_voltage) {
        voltage = channel->config.max_voltage;
    } else if (voltage < channel->config.min_voltage) {
        voltage = channel->config.min_voltage;
    }

    uint32_t compare =
        (uint32_t)((voltage - channel->config.min_voltage) *
                       (float32_t)(channel->config.max_compare - channel->config.min_compare) /
                       (channel->config.max_voltage - channel->config.min_voltage) +
                   (float32_t)(channel->config.min_compare));

    if (compare > channel->config.max_compare) {
        compare = channel->config.max_compare;
    } else if (compare < channel->config.min_compare) {
        compare = channel->config.min_compare;
    }

    return l298n_channel_pwm_set_compare(channel, compare);
}

l298n_err_t l298n_channel_set_max_voltage(l298n_channel_t const* channel)
{
    assert(channel);

    return l298n_channel_pwm_set_compare(channel, channel->config.max_compare);
}

l298n_err_t l298n_channel_set_min_voltage(l298n_channel_t const* channel)
{
    assert(channel);

    return l298n_channel_pwm_set_compare(channel, channel->config.min_compare);
}

l298n_err_t l298n_channel_set_direction(l298n_channel_t const* channel, l298n_direction_t direction)
{
    assert(channel);

    l298n_err_t err = L298N_ERR_OK;

    switch (direction) {
        case L298N_DIRECTION_FORWARD:
            return l298n_channel_set_forward_direction(channel);
        case L298N_DIRECTION_BACKWARD:
            return l298n_channel_set_backward_direction(channel);
        case L298N_DIRECTION_SOFT_STOP:
            return l298n_channel_set_soft_stop_direction(channel);
        case L298N_DIRECTION_FAST_STOP:
            return l298n_channel_set_fast_stop_direction(channel);
        default:
            return L298N_ERR_FAIL;
    }
}

l298n_err_t l298n_channel_set_forward_direction(l298n_channel_t const* channel)
{
    assert(channel);

    l298n_err_t err = l298n_channel_gpio_write(channel, channel->config.pin_left, true);
    err |= l298n_channel_gpio_write(channel, channel->config.pin_right, false);

    return err;
}

l298n_err_t l298n_channel_set_backward_direction(l298n_channel_t const* channel)
{
    assert(channel);

    l298n_err_t err = l298n_channel_gpio_write(channel, channel->config.pin_left, false);
    err |= l298n_channel_gpio_write(channel, channel->config.pin_right, true);

    return err;
}

l298n_err_t l298n_channel_set_soft_stop_direction(l298n_channel_t const* channel)
{
    assert(channel);

    l298n_err_t err = l298n_channel_gpio_write(channel, channel->config.pin_left, false);
    err |= l298n_channel_gpio_write(channel, channel->config.pin_right, false);

    return err;
}

l298n_err_t l298n_channel_set_fast_stop_direction(l298n_channel_t const* channel)
{
    assert(channel);

    l298n_err_t err = l298n_channel_gpio_write(channel, channel->config.pin_left, true);
    err |= l298n_channel_gpio_write(channel, channel->config.pin_right, true);

    return err;
}
