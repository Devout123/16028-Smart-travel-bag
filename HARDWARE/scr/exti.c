#include "exti.h"
#include "water.h"
#include "beep.h"
#include "usart1.h"  
#include "fire.h"
#include "delay.h"
#include "stm32f10x.h"
void EXTIX_Init(void)
{
	
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

 WATER_Check_Init();

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟
 //GPIOE.2 中断线以及中断初始化配置   下降沿触发
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;	//fire
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器



	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能火焰传感器所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
 

}

void EXTI1_IRQHandler(void)
{
	delay_ms(10);
	if(Water==1)	 	 
	{				 
		BEEP=0;	
	}
	EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE0上的中断标志位  
}
