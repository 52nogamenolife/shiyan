#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
 
/************************************************
 ALIENTEK��ӢSTM32������ʵ��9
 PWM���ʵ��  
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

#define dis_catch1 1850
#define dis_catch2 1850
u16 motor1=0,motor2=0,motor3=0,motor4=0;
//u8 catch_1=0,catch_2=0;	
 int main(void)
 {		
 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
 	TIM3_PWM_Init(1999,719);	 //720��Ƶ��PWMƵ��=72000000/720/2000=50hz
	TIM3_PWM_Init(71,0);	 //����Ƶ��PWMƵ��=72000000/900=80Khz
   	while(1)
	{
 		delay_ms(30);	 		
		
		get_motor();
		
		TIM_SetCompare1(TIM3,mg1);//1�Ŷ��
		TIM_SetCompare2(TIM3,mg2);//5�Ŷ��
		TIM_SetCompare3(TIM3,mg3);//2�Ŷ��
		TIM_SetCompare4(TIM3,mg4);//6�Ŷ��
		
		TIM_SetCompare1(TIM4,motor1);//1�Ų������
		TIM_SetCompare2(TIM4,motor2);//2�Ų������
		TIM_SetCompare3(TIM4,motor3);//3�Ų������
		TIM_SetCompare4(TIM4,motor4);//4�Ų������
		
	}	 
 }

