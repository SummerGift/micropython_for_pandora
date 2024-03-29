/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-13     zylx         first version
 */

//#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#ifdef RT_USING_PWM

#define DBG_SECTION_NAME     "drv.pwm"
#define DBG_LEVEL     DBG_INFO
#include <rtdbg.h>

#define MAX_PERIOD 65535
#define MIN_PERIOD 3
#define MIN_PULSE 2

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspPostInit 0 */

  /* USER CODE END TIM3_MspPostInit 0 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration    
    PB0     ------> TIM3_CH3 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM3_MspPostInit 1 */

  /* USER CODE END TIM3_MspPostInit 1 */
  }

}

#define BSP_USING_PWM3

enum
{
#ifdef BSP_USING_PWM1
    PWM1_INDEX,
#endif
#ifdef BSP_USING_PWM2
    PWM2_INDEX,
#endif
#ifdef BSP_USING_PWM3
    PWM3_INDEX,
#endif
};

struct stm32_pwm
{
    struct rt_device_pwm pwm_device;
    TIM_HandleTypeDef    tim_handle;
    rt_uint8_t channel;
    char *name;
};

#define PWM3_CONFIG                             \
    {                                           \
       .tim_handle.Instance     = TIM3,         \
       .name                    = "pwm3",       \
       .channel                 = 0             \
    }

static struct stm32_pwm stm32_pwm_obj[] =
{
#ifdef BSP_USING_PWM1
    PWM1_CONFIG,
#endif

#ifdef BSP_USING_PWM2
    PWM2_CONFIG,
#endif

#ifdef BSP_USING_PWM3
    PWM3_CONFIG,
#endif
};

static rt_err_t drv_pwm_control(struct rt_device_pwm *device, int cmd, void *arg);
static struct rt_pwm_ops drv_ops =
{
    drv_pwm_control
};

static rt_err_t drv_pwm_enable(TIM_HandleTypeDef *htim, struct rt_pwm_configuration *configuration, rt_bool_t enable)
{
    /* Converts the channel number to the channel number of Hal library */
    rt_uint32_t channel = 0x04 * (configuration->channel - 1);

    if (!enable)
    {
        HAL_TIM_PWM_Stop(htim, channel);
    }
    else
    {
        HAL_TIM_PWM_Start(htim, channel);
    }

    return RT_EOK;
}

static rt_err_t drv_pwm_get(TIM_HandleTypeDef *htim, struct rt_pwm_configuration *configuration)
{
    /* Converts the channel number to the channel number of Hal library */
    rt_uint32_t channel = 0x04 * (configuration->channel - 1);
    rt_uint64_t tim_clock;

    if (htim->Instance == TIM15 || htim->Instance == TIM16 || htim->Instance == TIM17)
    {
        tim_clock = HAL_RCC_GetPCLK2Freq() * 2;
    }
    else
    {
        tim_clock = HAL_RCC_GetPCLK1Freq();
    }
    
    if (__HAL_TIM_GET_CLOCKDIVISION(htim) == TIM_CLOCKDIVISION_DIV2)
    {
        tim_clock = tim_clock / 2;
    }
    else if (__HAL_TIM_GET_CLOCKDIVISION(htim) == TIM_CLOCKDIVISION_DIV4)
    {
        tim_clock = tim_clock / 4;
    }

    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    tim_clock /= 1000000UL;
    configuration->period = (__HAL_TIM_GET_AUTORELOAD(htim) + 1) * (htim->Instance->PSC + 1) * 1000UL / tim_clock;
    configuration->pulse = (__HAL_TIM_GET_COMPARE(htim, channel) + 1) * (htim->Instance->PSC + 1) * 1000UL / tim_clock;

    return RT_EOK;
}

static rt_err_t drv_pwm_set(TIM_HandleTypeDef *htim, struct rt_pwm_configuration *configuration)
{
    rt_uint32_t period, pulse;
    rt_uint64_t tim_clock, psc;
    /* Converts the channel number to the channel number of Hal library */
    rt_uint32_t channel = 0x04 * (configuration->channel - 1);

    if (htim->Instance == TIM15 || htim->Instance == TIM16 || htim->Instance == TIM17)
    {
        tim_clock = HAL_RCC_GetPCLK2Freq() * 2;
    }
    else
    {
        tim_clock = HAL_RCC_GetPCLK1Freq();
    }

    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    tim_clock /= 1000000UL;
    period = (unsigned long long)configuration->period * tim_clock / 1000ULL ;
    psc = period / MAX_PERIOD + 1;
    period = period / psc;
    __HAL_TIM_SET_PRESCALER(htim, psc - 1);

    if (period < MIN_PERIOD)
    {
        period = MIN_PERIOD;
    }
    __HAL_TIM_SET_AUTORELOAD(htim, period - 1);

    pulse = (unsigned long long)configuration->pulse * tim_clock / psc / 1000ULL;
    if (pulse < MIN_PULSE)
    {
        pulse = MIN_PULSE;
    }
    else if (pulse > period)
    {
        pulse = period;
    }
    __HAL_TIM_SET_COMPARE(htim, channel, pulse - 1);
    __HAL_TIM_SET_COUNTER(htim, 0);

    /* Update frequency value */
    HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE);

    return RT_EOK;
}

static rt_err_t drv_pwm_control(struct rt_device_pwm *device, int cmd, void *arg)
{
    struct rt_pwm_configuration *configuration = (struct rt_pwm_configuration *)arg;
    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)device->parent.user_data;

    switch (cmd)
    {
    case PWM_CMD_ENABLE:
        return drv_pwm_enable(htim, configuration, RT_TRUE);
    case PWM_CMD_DISABLE:
        return drv_pwm_enable(htim, configuration, RT_FALSE);
    case PWM_CMD_SET:
        return drv_pwm_set(htim, configuration);
    case PWM_CMD_GET:
        return drv_pwm_get(htim, configuration);
    default:
        return RT_EINVAL;
    }
}

static rt_err_t stm32_hw_pwm_init(struct stm32_pwm *device)
{
    rt_err_t result = RT_EOK;
    TIM_HandleTypeDef *tim = RT_NULL;
    TIM_OC_InitTypeDef oc_config = {0};
    TIM_MasterConfigTypeDef master_config = {0};
    TIM_ClockConfigTypeDef clock_config = {0};

    RT_ASSERT(device != RT_NULL);

    tim = (TIM_HandleTypeDef *)&device->tim_handle;

    /* configure the timer to pwm mode */
    tim->Init.Prescaler = 0;
    tim->Init.CounterMode = TIM_COUNTERMODE_UP;
    tim->Init.Period = 0;
    tim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(tim) != HAL_OK)
    {
        LOG_E("%s time base init failed", device->name);
        result = -RT_ERROR;
        goto __exit;
    }

    clock_config.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(tim, &clock_config) != HAL_OK)
    {
        LOG_E("%s clock init failed", device->name);
        result = -RT_ERROR;
        goto __exit;
    }

    if (HAL_TIM_PWM_Init(tim) != HAL_OK)
    {
        LOG_E("%s pwm init failed", device->name);
        result = -RT_ERROR;
        goto __exit;
    }

    master_config.MasterOutputTrigger = TIM_TRGO_RESET;
    master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(tim, &master_config) != HAL_OK)
    {
        LOG_E("%s master config failed", device->name);
        result = -RT_ERROR;
        goto __exit;
    }

    oc_config.OCMode = TIM_OCMODE_PWM1;
    oc_config.Pulse = 0;
    oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_config.OCFastMode = TIM_OCFAST_DISABLE;

    /* config pwm channel */
    if (device->channel & 0x01)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_1) != HAL_OK)
        {
            LOG_E("%s channel1 config failed", device->name);
            result = -RT_ERROR;
            goto __exit;
        }
    }

    if (device->channel & 0x02)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_2) != HAL_OK)
        {
            LOG_E("%s channel2 config failed", device->name);
            result = -RT_ERROR;
            goto __exit;
        }
    }

    if (device->channel & 0x04)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_3) != HAL_OK)
        {
            LOG_E("%s channel3 config failed", device->name);
            result = -RT_ERROR;
            goto __exit;
        }
    }

    if (device->channel & 0x08)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_4) != HAL_OK)
        {
            LOG_E("%s channel4 config failed", device->name);
            result = -RT_ERROR;
            goto __exit;
        }
    }

    /* pwm pin configuration */
    HAL_TIM_MspPostInit(tim);

    /* enable update request source */
    __HAL_TIM_URS_ENABLE(tim);

__exit:
    return result;
}

static void pwm_get_channel(void)
{
#ifdef BSP_USING_PWM2_CH1
    stm32_pwm_obj[PWM2_INDEX].channel |= 1 << 0;
#endif
#ifdef BSP_USING_PWM2_CH4
    stm32_pwm_obj[PWM2_INDEX].channel |= 1 << 3;
#endif
#ifdef BSP_USING_PWM3_CH1
    stm32_pwm_obj[PWM3_INDEX].channel |= 1 << 0;
#endif
#ifdef BSP_USING_PWM3_CH2
    stm32_pwm_obj[PWM3_INDEX].channel |= 1 << 1;
#endif
#ifdef BSP_USING_PWM3_CH3
    stm32_pwm_obj[PWM3_INDEX].channel |= 1 << 2;
#endif
}

static int stm32_pwm_init(void)
{
    int i = 0;
    int result = RT_EOK;

    pwm_get_channel();

#ifdef BSP_USING_PWM3
    __HAL_RCC_TIM3_CLK_ENABLE();
#endif

    for (i = 0; i < sizeof(stm32_pwm_obj) / sizeof(stm32_pwm_obj[0]); i++)
    {
        /* pwm init */
        if (stm32_hw_pwm_init(&stm32_pwm_obj[i]) != RT_EOK)
        {
            LOG_E("%s init failed", stm32_pwm_obj[i].name);
            result = -RT_ERROR;
            goto __exit;
        }
        else
        {
            LOG_D("%s init success", stm32_pwm_obj[i].name);

            /* register pwm device */
            if (rt_device_pwm_register(rt_calloc(1, sizeof(struct rt_device_pwm)), stm32_pwm_obj[i].name, &drv_ops, &stm32_pwm_obj[i].tim_handle) == RT_EOK)
            {
                LOG_D("%s register success", stm32_pwm_obj[i].name);
            }
            else
            {
                LOG_E("%s register failed", stm32_pwm_obj[i].name);
                result = -RT_ERROR;
            }
        }
    }

__exit:
    return result;
}
INIT_DEVICE_EXPORT(stm32_pwm_init);
#endif /* RT_USING_PWM */
