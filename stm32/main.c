#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "rfid.h"
#include "adapter.h"
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
u8 adapter1,adapter2,adapter3,adapter4;//步进电机的转动时间
u16 mg1,mg2,mg3,mg4;//控制最上方的舵机
u16 usart1_len,usart2_len;//串口数据长度
extern u8 L_flag,R_flag,P_flag,F_flag,G_flag;//左手 右手 放下 脱机 读取rfid
 int main(void)
 {		
	u16 t=0;
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	 
	 
	uart1_init(115200);	 //串口初始化为115200 负责蓝牙通信 上下机
 	uart2_init(115200);	 //串口初始化为115200 负责读取rfid
	LED_Init();			     //LED端口初始化
 	TIM3_PWM_Init(1999,719);	 //720分频。PWM频率=72000000/720/2000=50hz
	TIM4_Int_Init(71,1);	 //不分频。PWM频率=72000000/72/2=500Khz
   	while(1)
	{
 		delay_ms(30);//这个延时决定了电机的参数获取速度，即电机的PWM的周期
		
		get_motor();//获取八个电机的运行参数
		
		
		if(L_flag)
			TIM_SetCompare1(TIM3,mg1);//左手舵机
		if(R_flag)
			TIM_SetCompare2(TIM3,mg2);//右手舵机
		if(P_flag){//放下
			TIM_SetCompare1(TIM3,0);
			TIM_SetCompare2(TIM3,0);
		}
		
		
		
		if(1)//到达顶部
			TIM_SetCompare3(TIM3,mg3);//左臂舵机
			TIM_SetCompare4(TIM3,mg4);//右臂舵机
		
		
		
		if(G_flag){
		t=Read_flag();//读取旗子信息
		G_flag=0;
		USART_SendData(USART1,t);
		}
		
		if(F_flag){
			//抓住
			backward(1);
			delay_ms(adapter1);
			backward(2);
			delay_ms(adapter2);
			backward(3);
			delay_ms(adapter3);
			backward(4);
			delay_ms(adapter4);	
			
			F_flag=2;
		}
	}
	
	
 }
