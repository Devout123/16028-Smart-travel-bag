     #include "stm32f10x.h"                  // Device header
#include "YL.h"
void TDS_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_0 );
}


uint8_t uv;



void UV_Index(uint32_t D_Index)
{
	if(D_Index < 50)
	{  //查表，把得到的光电流值转换成紫外线等级
    uv = 0;
  }
  else if(D_Index < 227)
	{
    uv = 1;
  }
  else if(D_Index < 318)
	{
    uv = 2;
  }
  else if(D_Index < 408)
	{
    uv = 3;
  }
  else if(D_Index < 503)
	{
    uv = 4;
  }
  else if(D_Index < 606)
	{
    uv = 5;
  }
  else if(D_Index < 696)
	{
    uv = 6;
  }
  else if(D_Index < 795)
	{
    uv = 7;
  }
  else if(D_Index < 881)
	{
    uv = 8;
  }
  else if(D_Index < 976)
	{
    uv = 9;
  }
  else if(D_Index < 1079)
	{
    uv = 10;
  }
  else
	{
    uv = 11;
  }
}

