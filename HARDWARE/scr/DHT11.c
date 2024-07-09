#include "stm32f10x.h"                  // Device header
#include  "dht11.h"
#include "delay.h"
unsigned int rec_data[4];



void DH11_GPIO_Init_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void DH11_GPIO_Init_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING; //????
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}




void DHT11_Start(void)
{
	DH11_GPIO_Init_OUT(); //首先将14号引脚转变为输出模式
	
	dht11_high; //用高电平30us来告知温湿度传感器开启
	delay_us(30);
	
	dht11_low; //再拉低20ms等待拉高
	delay_ms(20);
	
	dht11_high; //再拉高
	delay_us(30);
	
	DH11_GPIO_Init_IN(); //那么温湿度传感器开始传入数据
}



char DHT11_Rec_Byte(void)
{
	unsigned char i = 0;
	unsigned char data;
	
	for(i=0;i<8;i++) 
	{
		while( Read_Data == 0); 
		delay_us(30);
		
		data <<= 1; 
		
		if( Read_Data == 1 ) 
		{
			data |= 1; 
		}
		
		while( Read_Data == 1 ); 
	}
	
	return data;
}


void DHT11_REC_Data(void)
{
	unsigned int R_H,R_L,T_H,T_L;
	unsigned char RH,RL,TH,TL,CHECK;
	
	DHT11_Start(); 
	dht11_high; 
	
	if(Read_Data == 0 ) 
	{
		while( Read_Data == 0); 
		while( Read_Data == 1); 
		
		R_H = DHT11_Rec_Byte();
		R_L = DHT11_Rec_Byte();
		T_H = DHT11_Rec_Byte();
		T_L = DHT11_Rec_Byte();
		CHECK = DHT11_Rec_Byte();
		
		dht11_low;
		delay_us(55); 
		dht11_high; 
		
		if(R_H + R_L + T_H + T_L == CHECK) //??????,??????????????
		{
			RH = R_H;
			RL = R_L;
			TH = T_H;
			TL = T_L;
		}
	}
	rec_data[0] = RH;
	rec_data[1] = RL;
	rec_data[2] = TH;
	rec_data[3] = TL;
}




