#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"
#define BEEP PBout(7)	// BEEP,蜂鸣器接口		   
#define BEEP1 PBout(10)
void BEEP_Init(void);	//初始化
		 				    
#endif

