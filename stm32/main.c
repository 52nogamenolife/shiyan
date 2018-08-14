#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "rfid.h"
#include "adapter.h"
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
u8 adapter1[2],adapter2[2],adapter3[2],adapter4[2];//���������ת��ʱ��
u16 mg1,mg2,mg3,mg4;//�������Ϸ��Ķ��
u16 usart1_len,usart2_len;//�������ݳ���
extern u8 L_flag,R_flag,P_flag,F_flag,G_flag;//���� ���� ���� �ѻ� ��ȡrfid
extern u8 b_flag,s_flag;//��ܻ���,ֹͣ
 int main(void)
 {		
	u16 t=0;
	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	 
	 
	uart1_init(115200);	 //���ڳ�ʼ��Ϊ115200 ��������ͨ�� ���»�
 	uart2_init(115200);	 //���ڳ�ʼ��Ϊ115200 �����ȡrfid
	LED_Init();			     //LED�˿ڳ�ʼ��
 	TIM3_PWM_Init(1999,719);	 //720��Ƶ��PWMƵ��=72000000/720/2000=50hz
	TIM4_Int_Init(71,1);	 //����Ƶ��PWMƵ��=72000000/72/2=500Khz
   	while(1)
	{
 		delay_ms(30);//�����ʱ�����˵���Ĳ�����ȡ�ٶȣ��������PWM������
		
		get_motor();//��ȡ�˸���������в���
		
		
		if(L_flag){
			TIM_SetCompare1(TIM3,mg1);//���ֶ��
			L_flag=0;
		}
		if(R_flag){
			TIM_SetCompare2(TIM3,mg2);//���ֶ��
			R_flag=0;
		}
		if(P_flag){//����
			TIM_SetCompare1(TIM3,0);
			TIM_SetCompare2(TIM3,0);
			P_flag=0;	
		}
		
		
		
		if(1)//���ﶥ��
			TIM_SetCompare3(TIM3,mg3);//��۶��
			TIM_SetCompare4(TIM3,mg4);//�ұ۶��
		
		
		
		if(G_flag){
		t=Read_flag();//��ȡ������Ϣ
		G_flag=0;
		USART_SendData(USART1,t);
		}
		
		if(F_flag){
			//ץס
			backward(1);
			for(int i=0;i<adapter1[0];i++)
			delay_ms(adapter1[0]);
			stop();
			backward(2);
			for(int i=0;i<adapter2[0];i++)
			delay_ms(adapter2[0]);
			stop();
			backward(3);
			for(int i=0;i<adapter3[0];i++)
			delay_ms(adapter3[0]);
			stop();
			backward(4);
			for(int i=0;i<adapter4[0];i++)
			delay_ms(adapter4[0]);
			stop();
			F_flag=0;
		}
		if(b_flag){
			//ץס
			forward(1);
			for(int i=0;i<adapter1[0];i++)
			delay_ms(adapter1[0]);
			stop();
			forward(2);
			for(int i=0;i<adapter2[0];i++)
			delay_ms(adapter2[0]);
			stop();
			forward(3);
			for(int i=0;i<adapter3[0];i++)
			delay_ms(adapter3[0]);
			stop();
			forward(4);
				for(int i=0;i<adapter4[0];i++)
			delay_ms(adapter4[0]);
			stop();
			b_flag=0;
		}
		if(s_flag){
			stop();
			s_flag=0;
				}
	}
	
	
 }
