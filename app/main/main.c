#include "main.h"
#include "dc_motor.h"
#include "gpio.h"
#include "l298n.h"
#include "stm32l4xx_hal.h"
#include "tim.h"
#include "usart.h"
#include <assert.h>

static l298n_err_t l298n_channel_pwm_set_compare(void*, uint32_t);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM1_Init();
    MX_USART2_UART_Init();

    assert(HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL) == HAL_OK);
    assert(HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3) == HAL_OK);
    assert(HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4) == HAL_OK);

    l298n_t l298n = {};

    l298n_initialize(&l298n, &(l298n_channel_t[2]){});

    dc_motor_t dc_motor = {};

    dc_motor_initialize(&dc_motor,
                        &(dc_motor_config_t){.ref_voltage = 6.0F,
                                             .max_speed = 100.0F,
                                             .min_speed = 0.0F,
                                             .stall_speed = 1.0F},
                        &(dc_motor_interface_t){});

    while (1) {
    }
}
