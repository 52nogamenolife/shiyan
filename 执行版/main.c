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
#include "MG.h"
#include "motor.h"
#include "test.h"
/************************************************
 ALIENTEK精英STM32开发板实验9
 PWM输出实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
extern u8 valid;
extern u8 RFID_init_data[10];
extern u32 motor1,motor2,motor3,motor4;//控制步进电机
extern u8 speed1, speed3;
u8 adapter1[2]={30,100},adapter2[2]={30,100};//步进电机的转动时间
u16 mg1=1900,mg2=1900,mg3=1838,mg4=0x077f,mg5=0x0758,mg6=0x0758;//控制最上方的舵机
u16 usart1_len,usart2_len;//串口数据长度
u8 looptime=30,delaytime=100;
extern u8 b_flag,s_flag;//电杠回缩,停止
extern u8 L_flag,R_flag,P_flag,F_flag,G_flag,B_flag,Ld_flag,Rd_flag,rfid_send,Pd_flag,Fd_flag,end_flag,R_catch,L_catch;//左手 右手 放下 脱机 读取rfid 左右臂舵机回转
extern u16 ultrasonic1;//超声波返回的定时器计数值
extern u8 RFID_BUFFER[3];//rfid的读出的数据

void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}
void test(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
}
void reset(void){
	mg5=0x0758;
	mg6=0x0758;
	mg3=1838;
	mg4=0x077f;
	TIM_SetCompare4(TIM3,mg4);
delay_ms(1000);
	TIM_SetCompare3(TIM3,mg3);
	delay_ms(1000);
	TIM_SetCompare1(TIM2,mg5);
	delay_ms(1000);
	TIM_SetCompare2(TIM2,mg6);
	delay_ms(1000);	
	
}
 int main(void) 
 {		
 	 delay_init();	    	 //延时函数初始化	

	 RCC_init();
	
	 switch_GPIO_init();
	 
	//KEY_Init(); //初始化与案件链接的硬件接口
	 
	delay_ms(255);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	 
	uart1_init(9600);	 //串口初始化为115200 负责蓝牙通信 上下机
	delay_ms(255);
	 
	MG_GPIO_Init();			     //LED端口初始化
delay_ms(255);
	
 	TIM3_PWM_Init(1999,719);	 //720分频。PWM频率=72000000/720/2000=50hz
	delay_ms(255);
	TIM2_PWM_Init(1999,719);	 //720分频。PWM频率=72000000/720/2000=50hz
	 motor_init();
	 delay_ms(255);
	 UP();
	 DIS_motor();
	 adapter_GPIO_init();
	 delay_ms(255);
	 
	TIM4_Int_Init(4,17);	 //分频2。PWM频率=72000000
	
	delay_ms(255);
	
	 //超声波配置
		 GPIO_SetBits(GPIOE,GPIO_Pin_5);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	
	TIM_SetCompare1(TIM3,mg1);
	delay_ms(1000);
	TIM_SetCompare2(TIM3,mg2);
	delay_ms(1000);
	TIM_SetCompare3(TIM3,mg3);
	delay_ms(1000);
	TIM_SetCompare4(TIM3,mg4);
	delay_ms(1000);
	TIM_SetCompare1(TIM2,mg5);
	delay_ms(1000);
	TIM_SetCompare2(TIM2,mg6);
	//test();
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	//test_GPIO_out_init();
	delay_ms(1000);

while(1)
		{
			
			while(0){
	
			}
		if(L_catch==1){
			
			move_mg2(mg2,0x0705);//左手掌
			mg2=0x0705;
			
			Ld_flag=0;
			while(!Ld_flag){
				USART_SendData(USART1, 'L');
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 'd');
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0d);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
			}
			
			move_mg3(mg3,1838);
			mg3=1838;
			
			TIM_SetCompare2(TIM3,2100);
			TIM_SetCompare1(TIM3,2100);
			
			if(R_flag==2){
			move_mg5(mg5,0x0758);
			mg5=0x0758;
			}
			if(R_flag==0){
			move_mg5(mg5,0x0770);
			mg5=0x0770;
			delay_ms(1000);
			TIM_SetCompare1(TIM2,2100);
			}

			TIM_SetCompare1(TIM3,mg1);
			TIM_SetCompare2(TIM3,mg2);
			L_catch=2;
		}
		if(L_flag==1){
			
			move_mg3(mg3,1930);
			mg3=1930;
		
			move_mg5(mg5,0x0700);
			mg5=0x0700;
			L_flag=2;
		}
		if(R_catch==1){
			
			move_mg1(mg1,0x0710);
			mg1=0x0710;
			
			Rd_flag=0;
			while(!Rd_flag){
				USART_SendData(USART1, 'R');
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 'd');
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0d);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
			}
			TIM_SetCompare1(TIM3,2100);
			TIM_SetCompare2(TIM3,2100);
			if(L_flag==2){
			move_mg6(mg6,0x0758);
			mg6=0x0758;
			}
			if(L_flag==0){
			move_mg6(mg6,0x0778);
			mg6=0x0778;
		
			TIM_SetCompare2(TIM2,2100);
			}
			delay_ms(1000);
			move_mg4(mg4,0x077f);
			mg4=0x077f;
			
		TIM_SetCompare1(TIM3,mg1);
		TIM_SetCompare2(TIM3,mg2);
			R_catch=2;
		}
		if(R_flag==1){
			
			move_mg4(mg4,0x0719);
			mg4=0x0719;
	
			move_mg6(mg6,0x0708);
			mg6=0x0708;
			
		R_flag=2;
		}
		if((R_catch+L_catch)==4){
			B_flag=1;
		L_catch=0;
			R_catch=0;
		}
		if(P_flag==1){//放下 第一次到达 或者开关有效
			
			move_mg4(mg4,0x0788	);//右边
			mg4=0x0788;
			move_mg3(mg3,0x077a);//左边
			mg3=0x077a;
			delay_ms(500);
			
			TIM_SetCompare1(TIM3,2100);
			TIM_SetCompare2(TIM3,2100);
			move_mg6(mg6,0x0718);//右边
			mg6=0x0718;
			TIM_SetCompare2(TIM2,2100);
			move_mg5(mg5,0x0710);//左边
			mg5=0x0710;
			
			delay_ms(500);
			
			move_mg1(mg1,0x073a);
			mg1=0x073a;
			move_mg2(mg2,0x780);
			mg2=0x780;
			
			
			
			Pd_flag=0;
			while(!Pd_flag){
				USART_SendData(USART1, 'P');
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 'd');
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0d);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
			}
			reset();
			P_flag=2;//我方好了！
			L_flag=0;
			R_flag=0;
			L_catch=0;
			R_catch=0;
		}
		
		if(B_flag==1){//到达拐角点 下面发送信号转会左右臂舵机
			move_mg3(mg3,1780);//左边
			mg3=1780;
			move_mg4(mg4,0x0719	);//右边
			mg4=0x0719;
			move_mg6(mg6,0x0770	);//右边
			mg6=0x0770;
			move_mg5(mg5,0x0770	);//右边
			mg5=0x0770;
		B_flag=2;
		}
		
		if(F_flag==1){
			int i;

			F_flag=2;
			b_flag=0;
			//抓住  电缸前进后退
		
			backward();//上面
			for(i=0;i<adapter1[0];i++)
			delay_ms(adapter1[1]);
			stop();
			
				EN_motor();
			
				end_flag=0;
			Fd_flag=0;
			while(!Fd_flag){
				USART_SendData(USART1, 'F');
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 'd');
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0d);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
				USART_SendData(USART1, 0x0a);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
			}
		
			while(!end_flag){
			}
			
		}
		if(b_flag==1){
			//抓住
			int i;
			DIS_motor();
			b_flag=2;
			F_flag=0;
			forward();
			for(i=0;i<adapter1[0];i++)
			delay_ms(adapter1[1]);
			stop();
					
		}
		if(s_flag){
			F_flag=0;
			b_flag=0;
			s_flag=0;
			stop_motor();
				}
		if(F_flag==2){//脱机上升
			TIM_Cmd(TIM4,ENABLE);
				motor1=motornum;
				motor2=motornum;
				motor3=motornum;
				motor4=motornum;
				
				F_flag=3;
		}
		if(motor1==1){
			stop_motor();
			P_flag=1;
		}
			
	}
	
 }
