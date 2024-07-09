#include "iic.h"
#include "delay.h"

/*-------------------------------------------------*/
/*函数名：初始化IIC接口                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void IIC_Init(void)
{			
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );    //使能GPIOB时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);                    //初始化PB8.9
	
	IIC_SCL=1;   //SCL拉高
	IIC_SDA=1;   //SDA拉高
}
/*-------------------------------------------------*/
/*函数名：IIC起始信号                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void IIC_Start(void)
{
	SDA_OUT();     //SDA线输出模式
	IIC_SDA=1;	   //SDA拉高  	  
	IIC_SCL=1;     //SCL拉高
	delay_us(4);   //延时
 	IIC_SDA=0;     //SCL高电平的时候，SDA由高到低，发出一个起始信号
	delay_us(4);   //延时
	IIC_SCL=0;     //SCL拉低
}

/*-------------------------------------------------*/
/*函数名：IIC停止信号                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void IIC_Stop(void)
{
	SDA_OUT();     //SDA线输出模式
	IIC_SCL=0;     //SCL拉低
	IIC_SDA=0;     //SDA拉低//STOP:when CLK is high DATA change form low to high
 	delay_us(4);   //延时
	IIC_SCL=1;     //SCL拉高
	IIC_SDA=1;     //SDA拉高 SCL高电平，SDA由低到高，发出停止信号
	delay_us(4);   //延时 						   	
}

/*-------------------------------------------------*/
/*函数名：等待应答                                 */
/*参  数：无                                       */
/*返回值：0：成功  1：失败                         */
/*-------------------------------------------------*/
u8 IIC_Wait_Ack(void)
{
	u8 timeout=0;
	SDA_IN();                  //SDA线输出模式 
	IIC_SDA=1;delay_us(1);	   //SDA拉高 延时  
	IIC_SCL=1;delay_us(1);	   //SCL拉高 延时 
	
	while(READ_SDA)            //等待SDA遍地，表示应答带来，不然一直while循环，直到超时
	{
		timeout++;             //超时计数+1
		if(timeout>250)        //如果大于250
		{
			IIC_Stop();        //发送停止信号
			return 1;          //返回1，表示失败
		}
	}
	IIC_SCL=0;                 //SCL拉低
	return 0;                  //返回0，表示成功
} 

/*-------------------------------------------------*/
/*函数名：发送应答                                 */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void IIC_Ack(void)
{
	IIC_SCL=0;   //SCL拉低
	SDA_OUT();   //SDA线输出模式 
	IIC_SDA=0;   //SDA拉低，表示应答
	delay_us(2); //延时
	IIC_SCL=1;   //SCL拉高
	delay_us(2); //延时
	IIC_SCL=0;   //SCL拉低
}

/*-------------------------------------------------*/
/*函数名：不发送应答                               */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/    
void IIC_NAck(void)
{
	IIC_SCL=0;   //SCL拉低
	SDA_OUT();   //SDA线输出模式 
	IIC_SDA=1;   //SDA拉高，表示不应答
	delay_us(2); //延时
	IIC_SCL=1;   //SCL拉高
	delay_us(2); //延时
	IIC_SCL=0;   //SCL拉低
}	

/*-------------------------------------------------*/
/*函数名：发送一个字节                             */
/*参  数：txd：发送的字节                          */
/*返回值：无                                       */
/*-------------------------------------------------*/ 			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	                  //SDA线输出模式 
    IIC_SCL=0;                    //SCL拉低，开始数据传输
    for(t=0;t<8;t++)              //for循环，一位一位的发送，从最高位 位7开始
    {              
        IIC_SDA=(txd&0x80)>>7;    //除了位7外，其余全屏蔽为0，然后右移到位0，给SDA数据线
        txd<<=1; 	              //左移一位，准备下一次发送
		delay_us(2);              //延时
		IIC_SCL=1;                //SCL拉高
		delay_us(2);              //延时
		IIC_SCL=0;	              //SCL拉低
		delay_us(2);              //延时
    }	 
} 	    

/*-------------------------------------------------*/
/*函数名：读取一个字节                             */
/*参  数：ack：是否发送应答 1:发 0：不发           */
/*返回值：读取的数据                               */
/*-------------------------------------------------*/   
u8 IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
	
	SDA_IN();              //SDA设置为输入
    for(i=0;i<8;i++ )      //for循环，一位一位的读取，从最高位 位7开始
	{
        IIC_SCL=0;        //SCL拉低    
        delay_us(2);      //延时
		IIC_SCL=1;        //SCL拉高 
        receive<<=1;      //左移一位，准备下次的读取
        if(READ_SDA)receive++;     //如果读取的是高电平，也就是1，receive+1
		delay_us(1);      //延时
    }					 
    if (!ack)          //不需要发送
        IIC_NAck();    //发送nACK
    else               //需要发送
        IIC_Ack();     //发送ACK   
    return receive;
}



























