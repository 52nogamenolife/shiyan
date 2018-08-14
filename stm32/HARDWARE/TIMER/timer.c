#include "timer.h"
#include "led.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTE��ӢSTM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/4
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20120904
//1,����TIM3_PWM_Init������
//2,����LED0_PWM_VAL�궨�壬����TIM3_CH2����									  
//////////////////////////////////////////////////////////////////////////////////  
   	  
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!

extern u16 motor1,motor2,motor3,motor4;//���Ʋ�����
u8 speed1,speed2,speed3,speed4; 
extern u8 looptime,delaytime;
u8 speed1flag,speed2flag,speed3flag,speed4flag; 
extern u16 mg1,mg2,mg3,mg4;//�������Ϸ��Ķ��
extern u8 adapter1[2],adapter2[2],adapter3[2],adapter4[2];//���������ת��ʱ��
u16 num;
u8 flag;
u8 L_flag,R_flag,P_flag,F_flag,G_flag;//���� ���� ���� �ѻ� ��ȡrfid
extern u16 usart1_len,usart2_len;//�������ݳ���
u8 b_flag,s_flag;
char information_all[50];

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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx����
							 
}
//��ʱ��4�жϷ������
void TIM4_IRQHandler(void)   //TIM4�ж�
{
	char i;
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		speed1flag++;speed2flag++;speed3flag++;speed4flag++;
		if(speed1flag==speed1){
		if(motor1){
			if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_6)){
				GPIO_ResetBits(GPIOB,GPIO_Pin_6);
				motor1--;}
			else {
				GPIO_SetBits(GPIOB,GPIO_Pin_6);}}
		speed1flag=0;	
		}
		if(speed2flag==speed2){
		if(motor2){
			if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_7)){
				
				GPIO_ResetBits(GPIOB,GPIO_Pin_7);
				motor2--;
			}
			else {
			GPIO_SetBits(GPIOB,GPIO_Pin_7);
			}}
		speed2flag=0;}
		if(speed3flag==speed3){
			if(motor3){
				if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_8)){
					GPIO_ResetBits(GPIOB,GPIO_Pin_8);
					motor3--;
				}
			else {
				GPIO_SetBits(GPIOB,GPIO_Pin_8);
			}
		}
			speed3flag=0;}
	if(speed4flag==speed4){
		if(motor4){
			if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_9)){
				
				GPIO_ResetBits(GPIOB,GPIO_Pin_9);
				motor4--;
			}
			else {
			GPIO_SetBits(GPIOB,GPIO_Pin_9);
			
			}
		}
				speed3flag=0;}
	}
}

//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB5   TIM3_CH1->PB4 TIM3_CH3->PB0 TIM3_CH4->PB1           
 
   //���ø�����Ϊ�����������,���TIM3 CH1��PWM���岨��	GPIOB.4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���
	
	
   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	
    //���ø�����Ϊ�����������,���TIM3 CH3��PWM���岨��	GPIOB.0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC3

	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR3�ϵ�Ԥװ�ؼĴ���
	
	  //���ø�����Ϊ�����������,���TIM3 CH4��PWM���岨��	GPIOB.1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //TIM_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC4

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR4�ϵ�Ԥװ�ؼĴ���
 
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
	

}


void get_motor(void)
{
	if(USART1_RX_STA&0x8000)//�Ƿ��н��յ�����
		{
		
			usart1_len = USART1_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���	
			switch (USART1_RX_BUF[0]) {
				case 'L':
					if(USART1_RX_BUF[1]=='q'){
						L_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='L'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'L');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�Ƿ������
						USART_SendData(USART1, 'r');
						USART1_RX_STA=0;
						}
					}
					break;
				case 'R': 
					if(USART1_RX_BUF[1]=='q'){
						R_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='R'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'R');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�Ƿ������
						USART_SendData(USART1, 'r');
						USART1_RX_STA=0;
						}
					}
					break;
				case 'P': 
					if(USART1_RX_BUF[1]=='q'){
						P_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='P'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'P');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�Ƿ������
						USART_SendData(USART1, 'r');
						USART1_RX_STA=0;
						}
					}
					break;
				case 'F':
					if(USART1_RX_BUF[1]=='q'){
						F_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='F'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'F');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�Ƿ������
						USART_SendData(USART1, 'r');
						USART1_RX_STA=0;
						}
					}
					break;
				case 'G':
					if(USART1_RX_BUF[1]=='q'){
						G_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='G'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'G');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�Ƿ������
						USART_SendData(USART1, 'r');
						USART1_RX_STA=0;
						}
					}
					break;
				case 'a':{ //ask ѯ��������Ϣ
				
				sprintf(information_all,"motor %d %d %d %d \n speed %d %d %d %d \n diangang %d %d %d %d %d %d %d %d \n mg %d %d %d %d\n mgyanshi looptime%d  delaytime%d ",motor1,motor2,motor3,motor4,
					speed1,speed2,speed3,speed4,
					adapter1[0],adapter1[1],adapter2[0],adapter2[1],adapter3[0],adapter3[1],adapter4[0],adapter4[1]
					,mg1,mg2,mg3,mg4,looptime,delaytime);
				
				}
				case 'b': //���˱�����
					if(USART1_RX_BUF[1]=='q'){
						b_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='h'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'h');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�Ƿ������
						USART_SendData(USART1, 'r');
						USART1_RX_STA=0;
						}
					}
					case 's'://ֹͣ
					{
						s_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='s'){		
						USART_SendData(USART1, 's');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�Ƿ������
						USART1_RX_STA=0;
						}
					}
					break;
				case 't': //���ò������Ŀ���г̺��ٶ�
					switch(USART1_RX_BUF[1]){
						case 1:
							motor1=USART1_RX_BUF[2];
							motor1=(motor1<<8)+USART1_RX_BUF[3];
							speed1=USART1_RX_BUF[4];
							break;
						case 2:
							motor2=USART1_RX_BUF[2];
							motor2=(motor2<<8)+USART1_RX_BUF[3];
							speed2=USART1_RX_BUF[4];
							break;
						case 3:
							motor3=USART1_RX_BUF[2];
							motor3=(motor3<<8)+USART1_RX_BUF[3];
							speed3=USART1_RX_BUF[4];
							break;
						case 4:
							motor4=USART1_RX_BUF[2];
							motor4=(motor4<<8)+USART1_RX_BUF[3];
							speed4=USART1_RX_BUF[4];	
							break;
						default:
							break;
					}
					case 'd'://���õ���г�
					switch(USART1_RX_BUF[1]){
						case 1:
							adapter1[0]=USART1_RX_BUF[2];
						  adapter1[1]=USART1_RX_BUF[3];
							break;
						case 2:
							adapter2[0]=USART1_RX_BUF[2];
						  adapter2[1]=USART1_RX_BUF[3];
							break;
						case 3:
							adapter3[0]=USART1_RX_BUF[2];
						  adapter3[1]=USART1_RX_BUF[3];
							break;
						case 4:
							adapter4[0]=USART1_RX_BUF[2];
						  adapter4[1]=USART1_RX_BUF[3];
							break;
						default:
							break;
					}
					
				}
			
		}
		
			/*
			if(F_flag==2){//�ѻ�����
			motor1=num1;
			motor2=num2;
			motor3=num3;
			motor4=num4;
		}
			else {//ֹͣ����
			motor1=0;
			motor2=0;
			motor3=0;
			motor4=0;
			
			}
			*/
	
	if(1)
		mg1=num;
	if(1)
		mg2=num;
	if(1)
		mg3=num;
	if(1)
		mg4=num;

}
