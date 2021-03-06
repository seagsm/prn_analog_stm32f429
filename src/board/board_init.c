#include "board_init.h"

/*
  Board init function.
*/
BOARD_ERROR be_board_init_main_init(void)
{
    uint16_t u16_step = 0U;
    BOARD_ERROR be_result = BOARD_ERR_OK;

    //gv_board_sys_tick_init();
    /* Init interrupt priority group. */
    NVIC_init();
    /*TODO: It should be moved to suitable place. */
    __enable_irq();


    while (u16_step < BOARD_INIT_TOTAL_STEPS)
    {
        be_result = BOARD_ERR_OK;
        switch (u16_step)
		{
            case (0U):
                be_result = be_board_gpio_init();  /* Init GPIO. */
                /* for measurement only be_result = be_board_int_cap_init(); */
                break;
            case (1U):
               /* be_result = be_board_uart_init(); */  /* Init UART modules. */
                be_result = board_lcd_display_init();
                break;
            case (2U):
                /* be_result = board_encoder_emulation_init();*/
                board_adc_dma_init();
                board_encoder_emulation_init();
                board_encoder_emulation_start();
                board_motor_init();
                
                break;
            case (3U):
               /* be_result = board_motor_init(); */ /* Init TIM3 for PWM, TIM4 for PWM counter. */
                break;
            case (4U):
               /* be_result = be_board_capture_pwm_init();*/
                break;
            case (5U):
             /*   be_result = board_spi_4_dma_slave_configuration();*/

             /*   test(); */
                break;
            case (6U):
                be_result = BOARD_ERR_OK;
                break;

            default:
                be_result = BOARD_ERR_ERROR;
                break;
        }
        /* If during initialisation something going wrong. */
        if(be_result != BOARD_ERR_OK)
        {
            /* TODO: Print u16_step like number of error step. */
            break;
        }
        u16_step++;
    }

    if(be_result == BOARD_ERR_OK)
    {
        v_board_stateSetRequiredState(BOARD_SYSTEM_READY_TO_RUN);
    }
    else
    {
        v_board_stateSetRequiredState(BOARD_SYSTEM_FAULT);
    }
    return(be_result);
}