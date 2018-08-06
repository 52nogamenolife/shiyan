#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
 
/************************************************
 ALIENTEK精英STM32开发板实验9
 PWM输出实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

#define dis_catch1 1850
#define dis_catch2 1850
u16 motor1=0,motor2=0,motor3=0,motor4=0;
//u8 catch_1=0,catch_2=0;	
 int main(void)
 {		
 
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
 	LED_Init();			     //LED端口初始化
 	TIM3_PWM_Init(1999,719);	 //720分频。PWM频率=72000000/720/2000=50hz
	TIM3_PWM_Init(71,0);	 //不分频。PWM频率=72000000/900=80Khz
   	while(1)
	{
 		delay_ms(30);	 		
		
		get_motor();
		
		TIM_SetCompare1(TIM3,mg1);//1号舵机
		TIM_SetCompare2(TIM3,mg2);//5号舵机
		TIM_SetCompare3(TIM3,mg3);//2号舵机
		TIM_SetCompare4(TIM3,mg4);//6号舵机
		
		TIM_SetCompare1(TIM4,motor1);//1号步进电机
		TIM_SetCompare2(TIM4,motor2);//2号步进电机
		TIM_SetCompare3(TIM4,motor3);//3号步进电机
		TIM_SetCompare4(TIM4,motor4);//4号步进电机
		
	}	 
 }

