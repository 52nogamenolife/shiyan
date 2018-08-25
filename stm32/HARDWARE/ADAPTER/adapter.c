#include "adapter.h"
//#include "led.h"

void stop(void)
{
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	GPIO_ResetBits(GPIOA,GPIO_Pin_12);
	GPIO_ResetBits(GPIOE,GPIO_Pin_13);
	GPIO_ResetBits(GPIOE,GPIO_Pin_14);

}
void forward(int n){
	switch(n){
		case 1:
			GPIO_SetBits(GPIOA,GPIO_Pin_0);
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);
			break;
		case 2:
			GPIO_SetBits(GPIOA,GPIO_Pin_4);
			GPIO_ResetBits(GPIOA,GPIO_Pin_5);
			break;
		case 3:
			GPIO_SetBits(GPIOA,GPIO_Pin_11);
			GPIO_ResetBits(GPIOA,GPIO_Pin_12);
			break;
		case 4:
			GPIO_SetBits(GPIOE,GPIO_Pin_13);
			GPIO_ResetBits(GPIOE,GPIO_Pin_14);
			break;
		default:
			break;
	}
	
}

void backward(int n){
	switch(n){
		case 1:
			GPIO_ResetBits(GPIOA,GPIO_Pin_0);
			GPIO_SetBits(GPIOA,GPIO_Pin_1);
			break;
		case 2:
			GPIO_ResetBits(GPIOA,GPIO_Pin_4);
			GPIO_SetBits(GPIOA,GPIO_Pin_5);
			break;
		case 3:
			GPIO_ResetBits(GPIOA,GPIO_Pin_11);
			GPIO_SetBits(GPIOA,GPIO_Pin_12);
			break;
		case 4:
			GPIO_ResetBits(GPIOE,GPIO_Pin_13);
			GPIO_SetBits(GPIOE,GPIO_Pin_14);
			break;
		default:
			break;
	}
	
}

