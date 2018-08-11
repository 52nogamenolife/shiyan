#include "adapter.h"
//#include "led.h"
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
			GPIO_SetBits(GPIOA,GPIO_Pin_6);
			GPIO_ResetBits(GPIOA,GPIO_Pin_7);
			break;
		case 4:
			GPIO_SetBits(GPIOA,GPIO_Pin_8);
			GPIO_ResetBits(GPIOA,GPIO_Pin_11);
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
			GPIO_ResetBits(GPIOA,GPIO_Pin_6);
			GPIO_SetBits(GPIOA,GPIO_Pin_7);
			break;
		case 4:
			GPIO_ResetBits(GPIOA,GPIO_Pin_8);
			GPIO_SetBits(GPIOA,GPIO_Pin_11);
			break;
		default:
			break;
	}
	
}

