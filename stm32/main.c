#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "rfid.h"
#include "adapter.h"
#include "ultrasonic.h"
#include "switch.h"
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
u16 motor1=0,motor2=0,motor3=0,motor4=0;//���Ʋ������
u8 adapter1,adapter2,adapter3,adapter4;//���������ת��ʱ��
u16 mg1,mg2,mg3,mg4;//�������Ϸ��Ķ��
u16 usart1_len,usart2_len;//�������ݳ���
extern u8 L_flag,R_flag,P_flag,F_flag,G_flag,B_flag;//���� ���� ���� �ѻ� ��ȡrfid ���ұ۶����ת
extern u16 ultrasonic1,ultrasonic2;//���������صĶ�ʱ������ֵ
 int main(void)
 {		
	u16 t=0;
	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart1_init(115200);	 //���ڳ�ʼ��Ϊ115200 ��������ͨ�� ���»�
 	uart2_init(115200);	 //���ڳ�ʼ��Ϊ115200 �����ȡrfid
	LED_Init();			     //LED�˿ڳ�ʼ��
 	TIM3_PWM_Init(1999,719);	 //720��Ƶ��PWMƵ��=72000000/720/2000=50hz
	TIM4_Int_Init(71,1);	 //��Ƶ2��PWMƵ��=72000000/72/2=500Khz
	 //����������
	TIM1_Configuration(19999,71);
	ultrasonic_GPIO_init();
	ultrasonic_IRQ_init();
   	while(1)
	{
 		delay_ms(30);//�����ʱ�����˵���Ĳ�����ȡ�ٶȣ��������PWM������
		
		get_motor();//��ȡ�Լ����ð˸���������в���
		
		if(L_flag){
			int i;
			TIM_SetCompare1(TIM3,mg1);//���ֶ��
			
			for(i=0;i<100;i++){
				TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
				delay_ms(30);
				TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
				delay_ms(20);
			}
		}
		
		if(R_flag){
			int i;
			TIM_SetCompare2(TIM3,mg2);//���ֶ��
			
			for(i=0;i<100;i++){
				TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
				delay_ms(30);
				TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
				delay_ms(20);
			}
		}
		if(P_flag){//����
			int i;
			TIM_SetCompare1(TIM3,mg1);
			TIM_SetCompare2(TIM3,mg2);
			
			for(i=0;i<100;i++){
				TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
				TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
				delay_ms(30);
				TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
				TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
				delay_ms(20);
			}
		}	
		
		if(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_7)){//���ﶥ�� ���ݿ���!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			TIM_SetCompare3(TIM3,mg3);//��۶��
			TIM_SetCompare4(TIM3,mg4);//�ұ۶��		
		}
		if(G_flag){
		t=Read_flag();//��ȡ������Ϣ
		G_flag=0;
		USART_SendData(USART1,t);
		}
		if(B_flag){//����սǵ� ���淢���ź�ת�����ұ۶��
			TIM_SetCompare3(TIM3,mg3);
			TIM_SetCompare4(TIM3,mg4);
		}
		
		if(F_flag){
			//ץס  ���ǰ������
			backward(1);
			delay_ms(adapter1);
			stop();
			backward(2);
			delay_ms(adapter2);
			stop();
			backward(3);
			delay_ms(adapter3);
			stop();
			backward(4);
			delay_ms(adapter4);	
			stop();
			
			F_flag=2;
		}
		if(ultrasonic1>=7100||ultrasonic2>=7100){
			F_flag=3;
		}
	}
	
	
 }
