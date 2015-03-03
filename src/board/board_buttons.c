
#include "board_buttons.h"




BOARD_ERROR be_board_buttons_get_state(BOARD_BUTTONS bb_in, uint8_t *u8_state)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    switch (bb_in)
    {
        case BUTTON_0:
            be_result = BOARD_ERR_RANGE;
          break;
        case BUTTON_1:
          *u8_state = GPIO_ReadInputDataBit(GPIOE, GPIO_E_IN_BUTTON_1);
          break;
        case BUTTON_2:
          *u8_state = GPIO_ReadInputDataBit(GPIOE, GPIO_E_IN_BUTTON_2);
          break;
        case BUTTON_3:
          *u8_state = GPIO_ReadInputDataBit(GPIOE, GPIO_E_IN_BUTTON_3);
          break;
        case BUTTON_4:
          *u8_state = GPIO_ReadInputDataBit(GPIOE, GPIO_E_IN_BUTTON_4);
          break;
        default:
            be_result = BOARD_ERR_RANGE;
            break;
    }   
    return(be_result);
}


BOARD_ERROR be_board_buttons_pressed(BOARD_BUTTONS bb_in)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    switch (bb_in)
    {
        case BUTTON_0:
            /* add press event. */
          break;
        case BUTTON_1:
          board_motor_step(1);
          break;
        case BUTTON_2:
          board_motor_step(-1);
          break;
        case BUTTON_3:
            /* add press event. */
          break;
        case BUTTON_4:
            /* add press event. */
          break;
        default:
            be_result = BOARD_ERR_RANGE;
            break;
    }   
    return(be_result);
}


BOARD_ERROR be_board_buttons_process(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t u8_state;
    
    be_result |= be_board_buttons_get_state(BUTTON_1, &u8_state);
    if(u8_state == BUTTON_PRESSED)
    {
        be_result |= be_board_buttons_pressed(BUTTON_1);
    
    }
    
    be_result |= be_board_buttons_get_state(BUTTON_2, &u8_state);
    if(u8_state == BUTTON_PRESSED)
    {
        be_result |= be_board_buttons_pressed(BUTTON_2);
    
    }
    
    be_result |= be_board_buttons_get_state(BUTTON_3, &u8_state);
    if(u8_state == BUTTON_PRESSED)
    {
        be_result |= be_board_buttons_pressed(BUTTON_3);
    
    }
    
    be_result |= be_board_buttons_get_state(BUTTON_4, &u8_state);
    if(u8_state == BUTTON_PRESSED)
    {
        be_result |= be_board_buttons_pressed(BUTTON_4);
    
    }

    return(be_result);
}












