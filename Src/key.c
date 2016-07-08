/*-----------------------------------------------------------------
 * Name:    key.c   
 * Purpose: key driver  
 *-----------------------------------------------------------------
 * 
 * Copyright (c) *reserve
 
||                       _      _               ||
||    /\  /\  __  _  __ | | __ | | ____  ___ _  ||
||   /  \/  \/ _ ` |/ _ ` |/ _ ` |/ _  \/ _ ` | ||
||  / /\  /\  (_|    (_|    (_|    (_)   (_)  | ||
|| /_/  \/  \_.__, |\__, _|\__, _|\____/\___. | ||
|| =====================================|____/  ||
||                                              ||

 -----------------------------------------------------------------*/
 
/********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************/
#include "stm32f4xx_hal.h"
#include "key.h"
/********************************************************************************************************
 *                                                 MACROS
 ********************************************************************************************************/


/********************************************************************************************************
 *                                               CONSTANTS
 ********************************************************************************************************/

 
/********************************************************************************************************
 *                                               GLOBAL VARIABLES
 ********************************************************************************************************/
uint8_t  Key_Up = 0;
uint32_t Time_1ms = 0;

/********************************************************************************************************
 *                                               EXTERNAL VARIABLES
 ********************************************************************************************************/
extern TIM_HandleTypeDef htim2;
 
/********************************************************************************************************
 *                                               EXTERNAL FUNCTIONS
 ********************************************************************************************************/


/********************************************************************************************************
 *                                               LOCAL VARIABLES
 ********************************************************************************************************/

 
/********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************/

 
/********************************************************************************************************
 *                                               PUBLIC FUNCTIONS
 ********************************************************************************************************/
 
/*********************************************************************
 * @fn        Key_Scan()
 *
 * @brief     按键扫描驱动 
 *
 * @param     None
 *
 * @return    Key_None：按键无效   Key_Short：按键短按   Key_Long：按键长按
 */
KeyStatus Key_Scan(void)
{
	if(Key_Up == 1)        // 有按键松起
	{
		
		Key_Up = 0;
		if(Time_1ms > Long_Thre)       
		{
			//Time_1ms = 0;          // 启动定时器前已清零
			return Key_Long;         // 按键按下时间大于2s      长按
		}
		else if(Time_1ms > Short_Thre) 
		{
			//Time_1ms = 0;
			return Key_Short;        // 按键按下时间小于2s      短按 
		}			
		else                           
		{
			//Time_1ms = 0;
			return Key_None;         // 视作按键抖动
		}
	}
	return Key_None;
}
/*********************************************************************
 * @fn        HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)()
 *
 * @brief     TIM2 detection callbacks.
 *
 * @param     htim：所用定时器结构体   
 *
 * @return    None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the __HAL_TIM_PeriodElapsedCallback could be implemented in the user file
   */
	Time_1ms++; 
	//HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
}
/*********************************************************************
 * @fn        HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)()
 *
 * @brief     EXTI line detection callbacks.
 *
 * @param     GPIO_Pin: Specifies the pins connected EXTI line  
 *
 * @return    None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static uint8_t CAPTURE_STA = 0;	  //捕获状态
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = Key_Mode_Pin;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	
	/* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
	if(CAPTURE_STA == 1)               // 已经捕获到按键按下
	{
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		HAL_GPIO_Init(Key_Mode_GPIO_Port, &GPIO_InitStruct);
		
		//HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
		HAL_TIM_Base_Stop_IT(&htim2);   // 按键按下，开启定时器
		Key_Up = 1;
		CAPTURE_STA = 0;
	}
	else                               // 还未捕获到按键按下
	{
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		HAL_GPIO_Init(Key_Mode_GPIO_Port, &GPIO_InitStruct);
		
		//HAL_GPIO_TogglePin(LED4_GPIO_Port, LED3_Pin);
		Time_1ms = 0;
		HAL_TIM_Base_Start_IT(&htim2);   // 按键按下，开启定时器
		CAPTURE_STA = 1;
	}
}
//	if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == 0)      // 下降沿     开发板按键按下为高电平
//	{
//			HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
//			HAL_TIM_Base_Stop_IT(&htim2);    // 按键松起，关闭定时器
//			Key_Up = 1;                      // 按键松起标志
//	}
//	else                                                // 上升沿                                 
//	{
//		if(Key_Up == 0)	      // 上一次按键响应完成
//		{
//			HAL_GPIO_TogglePin(LED3_GPIO_Port, LED4_Pin);
//			Time_1ms = 0;
//			HAL_TIM_Base_Start_IT(&htim2);   // 按键按下，开启定时器
//		}
//	}
 /********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************/

/*********************************************************************
 * @fn      
 *
 * @brief   
 *
 * @param   
 *
 * @return  
 */
 
 


/*********************************************************************
 */
