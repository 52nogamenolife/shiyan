#include "ultrasonic.h"


u16 ultrasonic1,ultrasonic2;

void TIM1_Configuration(short arr,short psc){
	
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;


TIM_TimeBaseStructure.TIM_Prescaler = psc; //72M/72=1M 
TIM_TimeBaseStructure.TIM_Period = arr; //20ms overflow
TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //
TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 
/*load */
TIM_ARRPreloadConfig(TIM1, ENABLE);
//clear IT bit
TIM_ClearITPendingBit(TIM1,TIM_IT_Update); 
/* overflow interrupt*/
//TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
/* allow*/
TIM_Cmd(TIM1, ENABLE);
	
}

void ultrasonic_GPIO_init(void){//io口的配置
GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;

/* 
PE9 CH1 DJ // PE10 CH2 MD // PE11 CH6 adjust */
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M
GPIO_Init(GPIOE, &GPIO_InitStructure);

GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource9);
GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource11);


EXTI_InitStructure.EXTI_Line = EXTI_Line9 | EXTI_Line11;
EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
EXTI_InitStructure.EXTI_LineCmd = ENABLE;
EXTI_Init(&EXTI_InitStructure);



RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PE端口时钟
	//舵机与步进电机的IO线
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_10;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.0 1 4 5 6 7 8 9

}

void ultrasonic_IRQ_init(void){//io口中断配置
	NVIC_InitTypeDef NVIC_InitStructure;

/* Configure one bit for preemption priority */
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

/* PB6 PE5 interrupt */
NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; 
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //highest priority
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);

/* PE3 interrupt */
NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; 
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
}

void EXTI9_5_IRQHandler(void){
	
	if(EXTI_GetITStatus(EXTI_Line9) != RESET){
		
	EXTI_ClearITPendingBit(EXTI_Line9);
	
	//Delay(1);

	if(!(GPIOE->IDR & GPIO_Pin_9)&&ultrasonic1){	// PE6=0 falling 
	ultrasonic1=TIM_GetCounter(TIM1)-ultrasonic1;
	if(ultrasonic1<=0)
		ultrasonic1+=20000;
	}
	else{ //Rising
 ultrasonic1=TIM_GetCounter(TIM1);
	}
	
	
	} 
}


void EXTI15_10_IRQHandler(void){

	if(EXTI_GetITStatus(EXTI_Line11) != RESET){
		
	EXTI_ClearITPendingBit(EXTI_Line11);
	
	//Delay(1);

	if(!(GPIOE->IDR & GPIO_Pin_11)&&ultrasonic2){	// PE6=0 falling 
	ultrasonic2=TIM_GetCounter(TIM1)-ultrasonic2;
		if(ultrasonic2<=0)
		ultrasonic2+=20000;
	}
	else{ //Rising
 ultrasonic2=TIM_GetCounter(TIM1);
	}
	
	
	} 
}

void getultrasonic(void){

}
void trig_ultrasonic(void){
	
	GPIO_SetBits(GPIOE,GPIO_Pin_8);
	GPIO_SetBits(GPIOE,GPIO_Pin_10);
	delay_us(20);
	GPIO_ResetBits(GPIOE,GPIO_Pin_8);
	GPIO_ResetBits(GPIOE,GPIO_Pin_10);
}
