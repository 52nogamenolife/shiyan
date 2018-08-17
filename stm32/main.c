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
extern u16 motor1,motor2,motor3,motor4;//控制步进电机

u8 adapter1[2]={4,255},adapter2[2]={4,255},adapter3[2]={4,255},adapter4[2]={4,255};//步进电机的转动时间
u16 mg1=1850,mg2=1850,mg3=1850,mg4=1850;//控制最上方的舵机
u16 usart1_len,usart2_len;//串口数据长度

u8 looptime=30,delaytime=100;
extern u8 b_flag,s_flag;//电杠回缩,停止
extern u8 L_flag,R_flag,P_flag,F_flag,G_flag,B_flag;//左手 右手 放下 脱机 读取rfid 左右臂舵机回转

extern u16 ultrasonic1,ultrasonic2;//超声波返回的定时器计数值


void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}


 int main(void)
 {		
	RCC_init();
	switch_GPIO_init();
	 
	//KEY_Init(); //初始化与案件链接的硬件接口
	delay_init();	    	 //延时函数初始化	 
	delay_ms(255);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	 
	uart1_init(9600);	 //串口初始化为115200 负责蓝牙通信 上下机
	delay_ms(255);
	 
 	uart2_init(9600);	 //串口初始化为115200 负责读取rfid
delay_ms(255);
	
	LED_Init();			     //LED端口初始化
delay_ms(255);
	
 	TIM3_PWM_Init(1999,719);	 //720分频。PWM频率=72000000/720/2000=50hz
	delay_ms(255);
		
	TIM4_Int_Init(9999,71);	 //分频2。PWM频率=72000000/72/1000=1Khz
	delay_ms(255);
	
	 //超声波配置
	TIM1_Configuration(19999,71);
delay_ms(255);

	ultrasonic_GPIO_init();
	delay_ms(255);
	
	ultrasonic_IRQ_init();
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
	GPIO_ResetBits(GPIOE,GPIO_Pin_5);
	 delay_ms(255);
	
while(1)
		{
		
		if(L_flag==1){
			int i;
			
			for(i=0;i<looptime;i++){
				TIM_SetCompare1(TIM3,mg1);
				delay_ms(30);
				TIM_SetCompare1(TIM3,2000);
				delay_ms(delaytime);
			}
			
      L_flag=2;

		}
		
		if(R_flag==1){
			int i;
			TIM_SetCompare2(TIM3,1850);//右手舵机
			
			for(i=0;i<looptime;i++){
				TIM_SetCompare2(TIM3,mg2);
				delay_ms(30);
				TIM_SetCompare2(TIM3,2000);
				delay_ms(delaytime);
			}
			
		R_flag=2;
		

		}

		if(P_flag==1||GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7)){//放下 第一次到达 或者开关有效
				int i;
				for(i=0;i<looptime;i++){
				TIM_SetCompare3(TIM3,mg3);
				TIM_SetCompare4(TIM3,mg4);
				delay_ms(30);
				TIM_SetCompare3(TIM3,2000);
				TIM_SetCompare4(TIM3,2000);
				delay_ms(delaytime);
			}
	
			L_flag=0;
			R_flag=0;
			
			for(i=0;i<looptime;i++){
				TIM_SetCompare1(TIM3,mg1);
				TIM_SetCompare2(TIM3,mg2);
				delay_ms(30);
				TIM_SetCompare1(TIM3,2000);
				TIM_SetCompare2(TIM3,2000);
				delay_ms(delaytime);
			}
			
				P_flag=2;//我方好了！
		}	
		
		if(G_flag){
		u16 t;
		t=Read_flag();//读取旗子信息
		G_flag=0;
		USART_SendData(USART1,t);
		}
		if(B_flag==1){//到达拐角点 下面发送信号转会左右臂舵机
			int i;
			
			for(i=0;i<looptime;i++){
				TIM_SetCompare3(TIM3,mg3);
				TIM_SetCompare4(TIM3,mg4);
				delay_ms(30);
				TIM_SetCompare3(TIM3,2000);
				TIM_SetCompare4(TIM3,2000);
				delay_ms(delaytime);
			}
			
		B_flag=2;
		}
		
		if(F_flag==1){
			int i;
			//抓住  电缸前进后退
			backward(1);
			for(i=0;i<adapter1[0];i++)
			delay_ms(adapter1[1]);
			stop();
			backward(2);
			for(i=0;i<adapter2[0];i++)
			delay_ms(adapter2[1]);
			stop();
			backward(3);
			for(i=0;i<adapter3[0];i++)
			delay_ms(adapter3[1]);
			stop();
			backward(4);
			for(i=0;i<adapter4[0];i++)
			delay_ms(adapter4[1]);
			stop();
			F_flag=2;
			b_flag=0;
		}
		if(b_flag==1){
			//抓住
			int i;
			forward(1);
			for(i=0;i<adapter1[0];i++)
			delay_ms(adapter1[1]);
			stop();
			forward(2);
			for(i=0;i<adapter2[0];i++)
			delay_ms(adapter2[1]);
			stop();
			forward(3);
			for(i=0;i<adapter3[0];i++)
			delay_ms(adapter3[1]);
			stop();
			forward(4);
				for(i=0;i<adapter4[0];i++)
			delay_ms(adapter4[1]);
			stop();
			b_flag=2;
			F_flag=0;
		}
		if(s_flag){
			stop();
			s_flag=0;
				}
		if(ultrasonic1>=7100||ultrasonic2>=7100){
			F_flag=3;
		}
		
	}
	
 }
