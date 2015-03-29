


#include "board_motor.h"

int32_t int32_possition;

BOARD_ERROR board_motor_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    board_motor_timer_init();
    board_motor_timer_pulse_counter_init();
    board_motor_direction(CW);
    board_motor_enable(ENABLE);
    return(be_result);
}

/* Add one step to step counter. */
void board_motor_step(int8_t i8_step)
{
    int32_possition = int32_possition + i8_step;    /* Add one step to pool. */
    board_motor_direction(int32_possition);         /* Set direction. */

    /* Here place for checking "END" sensors of table. */
    if( int32_possition > 0)
    {
        if( GPIO_ReadInputDataBit(GPIOG, GPIO_G_IN_MOTOR_SIDE_END_SENSOR) == END_SENSOR_OFF)
        {
            TIM_Cmd(TIM3, ENABLE);  /* Start PWM if in forward direction END sensor is off. */
        }
        else
        {
            int32_possition = 0;
        }
    }
    else if( int32_possition < 0)
    {
        if( GPIO_ReadInputDataBit(GPIOG, GPIO_G_IN_ENCODER_SIDE_END_SENSOR) == END_SENSOR_OFF)
        {
            TIM_Cmd(TIM3, ENABLE);  /* Start PWM if in forward direction END sensor is off. */
        }
        else
        {
            int32_possition = 0;
        }
    }
    else
    {}

}

/* Set direction in motor driver. */
static void board_motor_direction(int32_t int32_direction)
{
    /* Direction should be tested. */
    if(int32_direction >= 0)
    {
        GPIO_ResetBits(GPIOA, GPIO_A_OUT_MOTOR_DIR);
    }
    else
    {
        GPIO_SetBits(GPIOA, GPIO_A_OUT_MOTOR_DIR);
    }
}

/* Enable - disable motor driver. */
static void board_motor_enable(uint8_t uint8_enable)
{
    if(uint8_enable == ENABLE)
    {
        GPIO_SetBits(GPIOC, GPIO_C_OUT_MOTOR_ENABLE);
    }
    else
    {
        GPIO_ResetBits(GPIOC, GPIO_C_OUT_MOTOR_ENABLE);
    }
}

/* Initialisation of timer 1 for generation of pulses PWM for motor driver. */
static void board_motor_timer_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    uint16_t u16_pwm_period = BOARD_MOTOR_STEP_PERIOD;
    uint16_t u16_pwm_duty   = BOARD_MOTOR_STEP_DUTY;

    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3,  ENABLE);

    /* Connect clock to port B for TIM3 CH1, pin PB4. */
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP ;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_4;  /*PB4 -> TIM3_CH1 */
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Connect TIM3 pins to AF2 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);

    /*Timer 3 work from 90MHz source clock.*/

    TIM_TimeBaseStructure.TIM_Period                = u16_pwm_period;
    TIM_TimeBaseStructure.TIM_Prescaler             = 0;/* Ftimer=fsys(==90MHz)/(Prescaler+1),for Prescaler=1 ,Ftimer=90MHz */
    TIM_TimeBaseStructure.TIM_ClockDivision         = 0;
    TIM_TimeBaseStructure.TIM_CounterMode           = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter     = 0U;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse           = u16_pwm_duty;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);

    /* Turn on output triger of TIM3 to OnUpdate event. */
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
    TIM_SetCounter(TIM3, 0U);
    TIM_CtrlPWMOutputs(TIM3, ENABLE);

    DBGMCU_APB1PeriphConfig(DBGMCU_TIM3_STOP, ENABLE);

    TIM_Cmd(TIM3, ENABLE);
}

/* Initialisation of TIM4 for caunting of pulses from PWM, generated by TIM2. */
/* This timer should count 9 OnUpdate event from TIM2(PWM) and disable it.
 * It is neccesary for generation 9 pulses for each encoder step.
 */
static void board_motor_timer_pulse_counter_init(void)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* Enable the TIM4 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                      = (unsigned char)TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = TIMER4_PERIOD_INTERUPT_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = TIMER4_PERIOD_INTERUPT_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* TIM4 clock enable*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4, DISABLE);

    /* Time Base configuration */
    TIM_TimeBaseStructure.TIM_Period        = 1;//8U; /* generated  + 1 pulse. */
    TIM_TimeBaseStructure.TIM_Prescaler     = 0U;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0U;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* http://robocraft.ru/blog/ARM/739.html */
    /* Connect TIM4 triger input to TIM3 triger output. */
    TIM_SelectInputTrigger(TIM4, TIM_TS_ITR2);
    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);
    TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_External1);
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);         /* Clear pending interrupt. */
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_SetCounter(TIM4, 0U);                           /* Clear counter. */
    TIM_Cmd(TIM4, ENABLE);                              /* Enable timer. */

}

void TIM4_IRQHandler(void)
{ /* < 700nS. */
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        if(int32_possition > 0)
        {
            int32_possition--;
        }
        else if(int32_possition < 0)
        {
            int32_possition++;
        }
        else
        {}

        if(int32_possition == 0)
        {

            /* Zero point. Stop moving. */
            /* Turn TIM3 PWM off, pulse disable. */
            TIM_Cmd(TIM3, DISABLE);
        }
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  /* Counter overflow, reset interrupt */
    }
}
