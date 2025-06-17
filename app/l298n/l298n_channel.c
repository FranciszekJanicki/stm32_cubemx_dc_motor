#include "l298n_channel.h"
#include "l298n_config.h"
#include <assert.h>
#include <math.h>
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

static l298n_err_t l298n_channel_gpio_read(l298n_channel_t const* channel,
                                           uint32_t pin,
                                           bool* state)
{
    return channel->interface.gpio_read
               ? channel->interface.gpio_read(channel->interface.gpio_user, pin, state)
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

static l298n_err_t l298n_channel_adc_init(l298n_channel_t const* channel)
{
    return channel->interface.adc_init ? channel->interface.adc_init(channel->interface.adc_user)
        ? L298N_ERR_NULL;
}

static l298n_err_t l298n_channel_adc_deinit(l298n_channel_t const* channel)
{
    return channel->interface.adc_deinit
        ? channel->interface.adc_deinit(channel->interface.adc_user)
        ? L298N_ERR_NULL;
}

static l298n_err_t l298n_channel_adc_get_conversion(l298n_channel_t const* channel,
                                                    uint32_t* conversion)
{
    return channel->interface.adc_get_conversion
        ? channel->interface.adc_get_conversion(channel->interface.adc_user, conversion)
        ? L298N_ERR_NULL;
}

static uint32_t l298n_channel_voltage_to_compare(l298n_channel_t const* channel, float32_t voltage)
{
    voltage = fmaxf(fminf(voltage, channel->config.max_voltage), channel->config.min_voltage);

    uint32_t compare =
        (uint32_t)((voltage - channel->config.min_voltage) *
                       (float32_t)(channel->config.max_compare - channel->config.min_compare) /
                       (channel->config.max_voltage - channel->config.min_voltage) +
                   (float32_t)(channel->config.max_compare));

    compare = fmax(fminf(compare, channel->config.max_compare), channel->config.min_compare);

    return compare;
}

static float32_t l298n_channel_conversion_to_voltage(l298n_channel_t const* channel,
                                                     uint32_t conversion)
{
    conversion =
        fmaxf(fminf(conversion, channel->config.max_conversion), channel->config.min_conversion);

    float32_t voltage =
        (float32_t)(conversion - channel->config.min_voltage) *
            (channel->config.max_voltage - channel->config.min_voltage) /
            (float32_t)(channel->config.max_conversion - channel->config.min_conversion) +
        (float32_t)(channel->config.max_voltage);

    voltage = fmax(fminf(voltage, channel->config.max_voltage), channel->config.min_voltage);

    return voltage;
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
    err |= l298n_adc_init(channel);

    return err;
}

l298n_err_t l298n_channel_deinitialize(l298n_channel_t* channel)
{
    assert(channel);

    l298n_err_t err = l298n_channel_degpio_init(channel);
    err |= l298n_channel_pwm_deinit(channel);
    err |= l298n_adc_deinit(channel);

    memset(channel, 0, sizeof(*channel));

    return err;
}

l298n_err_t l298n_channel_get_voltage(l298n_channel_t const* channel, float32_t* voltage)
{
    assert(channel && voltage);

    uint32_t conversion = {};
    l298n_err_t err = l298n_channel_adc_get_conversion(channel, &conversion);

    *voltage = l298n_channel_conversion_to_voltage(channel, conversion);

    return err;
}

l298n_err_t l298n_channel_set_voltage(l298n_channel_t const* channel, float32_t voltage)
{
    assert(channel);

    uint32_t compare = l298n_channel_voltage_to_compare(channel, voltage);

    return l298n_channel_pwm_set_compare(channel, compare);
}

l298n_err_t l298n_channel_set_direction(l298n_channel_t const* channel, l298n_direction_t direction)
{
    assert(channel);

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

    l298n_err_t err = l298n_channel_gpio_write(channel, channel->config.input1_pin, true);
    err |= l298n_channel_gpio_write(channel, channel->config.input2_pin, false);

    return err;
}

l298n_err_t l298n_channel_set_backward_direction(l298n_channel_t const* channel)
{
    assert(channel);

    l298n_err_t err = l298n_channel_gpio_write(channel, channel->config.input1_pin, false);
    err |= l298n_channel_gpio_write(channel, channel->config.input2_pin, true);

    return err;
}

l298n_err_t l298n_channel_set_soft_stop_direction(l298n_channel_t const* channel)
{
    assert(channel);

    l298n_err_t err = l298n_channel_gpio_write(channel, channel->config.input1_pin, false);
    err |= l298n_channel_gpio_write(channel, channel->config.input2_pin, false);

    return err;
}

l298n_err_t l298n_channel_set_fast_stop_direction(l298n_channel_t const* channel)
{
    assert(channel);

    l298n_err_t err = l298n_channel_gpio_write(channel, channel->config.input1_pin, true);
    err |= l298n_channel_gpio_write(channel, channel->config.input2_pin, true);

    return err;
}
