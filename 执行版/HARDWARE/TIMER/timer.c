#include "timer.h"
#include "led.h"
#include "usart.h"
#include "ultrasonic.h"
#include "adapter.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTE精英STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20120904
//1,增加TIM3_PWM_Init函数。
//2,增加LED0_PWM_VAL宏定义，控制TIM3_CH2脉宽									  
//////////////////////////////////////////////////////////////////////////////////  
   	  
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!

u32 motor1=0,motor2=0,motor3=0,motor4=0;//控制步进电
u8 speed1=32,speed3=10;
u8 adapter_PWM=10;
u8 motorstatus=0;
extern u8 looptime,delaytime,Test;
u8 speed1flag,speed3flag;
extern u16 mg1,mg2,mg3,mg4,mg5,mg6;//控制最上方的舵机
extern u8 adapter1[2],adapter2[2];//步进电机的转动时间
u16 num;
u8 flag;
u8 L_flag=0,R_flag=0,P_flag=0,F_flag=0,G_flag=0,B_flag=0,Ld_flag=0,Rd_flag=0,rfid_send=0,Pd_flag=0,Fd_flag=0,end_flag=0,R_catch=0,L_catch=0;//左手 右手 放下 脱机 读取rfid 左右臂舵机回转 左抓完 右抓完
extern u16 usart1_len,usart2_len;//串口数据长度
u8 b_flag=0,s_flag=0;
char information_all[50];
extern u16 ultrasonic1;//超声波返回的定时器计数值
void Usart_SendString(USART_TypeDef* USARTx,char *str){
	while(*str){
		while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));
		USART_SendData(USARTx,*str);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)); 
		str++;           
  }
}

void TIM4_Int_Init(u16 arr,u16 psc)
{
	

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_Cmd(TIM4,DISABLE); 
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM4,DISABLE); 
							 
}
//定时器4中断服务程序
void TIM4_IRQHandler(void)   //TIM4中断 步进电机的PWM
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
		speed1flag++;speed3flag++;
		if(speed1flag>=speed1){
			speed1flag=0;
			if(motor1>1){
				motor1--;
				if(adapter_PWM<=4){
					
					forward ();
					if(adapter_PWM<=0)
						adapter_PWM=10;
				}
				else{
					stop();
				}
				adapter_PWM--;
				
				/*
				if(motor1>=motornum-300)
					motor1--;
				if(motor1==motornum-300)
					speed1=1;
				*/
				
				PBout(6)=motorstatus;
				PBout(5)=motorstatus;	
				PBout(7)=motorstatus;	
				PBout(9)=motorstatus;	
				motorstatus=!motorstatus;
				}
		}
	}
}

//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM2_PWM_Init(u16 arr,u16 psc)
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能定时器3 4时钟
	
 

   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC1

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
	
   //初始化TIM3
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	TIM_Cmd(TIM2, ENABLE);  //使能TIM3
	
}
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3 4时钟
	
 

   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC1

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
	
   //初始化TIM3
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	
    //设置该引脚为复用输出功能,输出TIM3 CH3的PWM脉冲波形	GPIOB.0
   //初始化TIM3
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC3

	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR3上的预装载寄存器
	
   //初始化TIM3
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC4

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR4上的预装载寄存器
 
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
	

}


void get_motor(void)
{
	if(USART1_RX_STA&0x8000)//是否有接收到东西
		{
			usart1_len = USART1_RX_STA&0x3fff;//得到此次接收到的数据长度	
			
			switch (USART1_RX_BUF[0]) {
				case 'L':
					if(USART1_RX_BUF[1]=='q'){
						if(!L_flag)
							L_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='L'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'L');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'r');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART1_RX_STA=0;
						}
					}
					
					else if(USART1_RX_BUF[1]=='d'){
						if(!Ld_flag)
							Ld_flag=1;
					}
					else if(USART1_RX_BUF[1]=='c'){
						if(!L_catch)
						L_catch=1;
					USART_SendData(USART1, 'L');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'c');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
					USART1_RX_STA=0;
					}
					break;
				case 'R':
					if(USART1_RX_BUF[1]=='q'){
						if(!R_flag)
							R_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='R'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'R');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'r');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART1_RX_STA=0;
						}
					}
					else if(USART1_RX_BUF[1]=='d'){
						if(!Rd_flag)
							Rd_flag=1;
					}
					else if(USART1_RX_BUF[1]=='c'){
						if(!R_catch)
						R_catch=1;
						USART_SendData(USART1, 'R');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'c');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
					USART1_RX_STA=0;
					}
					break;
				case 'P': 
					if(USART1_RX_BUF[1]=='q'){
						if(!P_flag)
							P_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='P'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'P');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'r');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
					USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART1_RX_STA=0;
						}
					}
					if(USART1_RX_BUF[1]=='d'){
						if(!Pd_flag)
					Pd_flag=1;
					USART1_RX_STA=0;
					}
					break;
				case 'e':
					if(USART1_RX_BUF[1]=='n'&&USART1_RX_BUF[2]=='d'){
						if(!end_flag)
					end_flag=1;
						USART_SendData(USART1, 'e');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'n');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'd');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART1_RX_STA=0;
					}
					break;
				case 'F':
					if(USART1_RX_BUF[1]=='q'){
						if(!F_flag)
						F_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='F'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'F');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'r');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART1_RX_STA=0;
						}
					}
					if(USART1_RX_BUF[1]=='d'){
						if(!Fd_flag)
					Fd_flag=1;
					USART1_RX_STA=0;
					}
					break;
				
				case 'b': //爬杆臂松手
					if(USART1_RX_BUF[1]=='q'){
						if(b_flag==0)b_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='b'&&USART1_RX_BUF[1]=='q'){		
							USART_SendData(USART1, 'b');
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							USART_SendData(USART1, 'r');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							USART1_RX_STA=0;
						}
					}
					break;
			
					case 's'://停止
					{
						s_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='s'){		
						USART_SendData(USART1, 's');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART1_RX_STA=0;
						}
					}
					break;
						
						case 'g': //设置步进电机目标行程和速度
					switch(USART1_RX_BUF[1]){
						case 1:
							mg1=USART1_RX_BUF[2];
							mg1*=256;
							mg1+=USART1_RX_BUF[3];
							USART_SendData(USART1, 'T');
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							break;
						
						case 2:
							mg2=USART1_RX_BUF[2];
							mg2*=256;
							mg2+=USART1_RX_BUF[3];
							USART_SendData(USART1, 'T');
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							break;
						case 3:
							mg3=USART1_RX_BUF[2];
							mg3*=256;
							mg3+=USART1_RX_BUF[3];
							USART_SendData(USART1, 'T');
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							break;
						case 4:
							mg4=USART1_RX_BUF[2];
							mg4*=256;
							mg4+=USART1_RX_BUF[3];
							USART_SendData(USART1, 'T');
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							break;
						case 5:
							mg5=USART1_RX_BUF[2];
							mg5*=256;
							mg5+=USART1_RX_BUF[3];
							USART_SendData(USART1, 'T');
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							break;
						case 6:
							mg6=USART1_RX_BUF[2];
							mg6*=256;
							mg6+=USART1_RX_BUF[3];
							USART_SendData(USART1, 'T');
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							USART_SendData(USART1, 0x0d);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
							break;
						default:
							break;
					}
					break;
					default:
						break;
				}
			USART1_RX_STA=0;
					
		}
}
void RCC_init(void){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM5, ENABLE);	//使能定时器3 4时钟
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//Ê¹ÄÜPORTA,PORTEÊ±ÖÓ

}
