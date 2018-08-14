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
u16 motor1=0,motor2=0,motor3=0,motor4=0;//控制步进电机

u8 adapter1[2],adapter2[2],adapter3[2],adapter4[2];//步进电机的转动时间
u16 mg1,mg2,mg3,mg4;//控制最上方的舵机
u16 usart1_len,usart2_len;//串口数据长度

u8 looptime,delaytime;
extern u8 b_flag,s_flag;//电杠回缩,停止
extern u8 L_flag,R_flag,P_flag,F_flag,G_flag,B_flag;//左手 右手 放下 脱机 读取rfid 左右臂舵机回转
extern u16 ultrasonic1,ultrasonic2;//超声波返回的定时器计数值

 int main(void)
 {		
	u16 t=0;
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart1_init(115200);	 //串口初始化为115200 负责蓝牙通信 上下机
 	uart2_init(115200);	 //串口初始化为115200 负责读取rfid
	LED_Init();			     //LED端口初始化
 	TIM3_PWM_Init(1999,719);	 //720分频。PWM频率=72000000/720/2000=50hz
	TIM4_Int_Init(71,1);	 //分频2。PWM频率=72000000/72/2=500Khz
	 //超声波配置
	TIM1_Configuration(19999,71);
	ultrasonic_GPIO_init();
	ultrasonic_IRQ_init();
   	while(1)
	{
 		delay_ms(30);//这个延时决定了电机的参数获取速度，即电机的PWM的周期
		
		get_motor();//获取以及设置八个电机的运行参数
		
		if(L_flag){
			int i;
			TIM_SetCompare1(TIM3,mg1);//左手舵机
			
			for(i=0;i<looptime;i++){
				TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
				delay_ms(delaytime);
				TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
				delay_ms(20);
			}
      L_flag=0;
		}
		
		if(R_flag){
			int i;
			TIM_SetCompare2(TIM3,mg2);//右手舵机
			
			for(i=0;i<100;i++){
				TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
				delay_ms(30);
				TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
				delay_ms(20);
			}
		}
		if(P_flag){//放下
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
		
		if(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_7)){//到达顶部 根据开关!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			TIM_SetCompare3(TIM3,mg3);//左臂舵机
			TIM_SetCompare4(TIM3,mg4);//右臂舵机		
		}
		if(G_flag){
		t=Read_flag();//读取旗子信息
		G_flag=0;
		USART_SendData(USART1,t);
		}
		if(B_flag){//到达拐角点 下面发送信号转会左右臂舵机
			TIM_SetCompare3(TIM3,mg3);
			TIM_SetCompare4(TIM3,mg4);
		}
		
		if(F_flag==1){
			//抓住  电缸前进后退
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
			F_flag=2;
		}
		if(b_flag){
			//抓住
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
		if(ultrasonic1>=7100||ultrasonic2>=7100){
			F_flag=3;
		}
	}
	
 }
