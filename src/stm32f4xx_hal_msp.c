#include "main.h"

void HAL_MspInit(void) {
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();   
    /* PendSV_IRQn interrupt configuration */
    //HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
}