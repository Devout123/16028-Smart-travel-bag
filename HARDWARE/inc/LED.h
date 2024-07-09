/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*              实现LED功能的头文件                */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __LED_H
#define __LED_H
#define LED0 PBout(0)


#define LED_OUT(x)   GPIO_WriteBit(GPIOB, GPIO_Pin_0, (BitAction)x)  //设置PB0 的电平，可以点亮熄灭LED3

#define LED_IN_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_0)  //PB0 控制LED3,读取电平状态，可以判断LED3是点亮还是熄灭

#define LED_ON       GPIO_ResetBits(GPIOB, GPIO_Pin_0)        //共阳极，拉低PB0电平，点亮LED3
#define LED_OFF      GPIO_SetBits(GPIOB, GPIO_Pin_0)          //共阳极，拉高PB0电平，熄灭LED3



void LED_Init(void);               //初始化	


#endif
