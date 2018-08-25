#include "MG.h"
#define TIME 100
#define internal 0x0008
void move_mg1(u16 a,u16 b){
	if(a>b){
		u16 Old=a,New=b;
		while(Old>New){
		TIM_SetCompare1(TIM3,Old);
			delay_ms(TIME);
			Old-=0x0002;
		}
	}
	else if(a<b){
		u16 Old=a,New=b;
		while(Old<New){
		TIM_SetCompare1(TIM3,Old);
			delay_ms(TIME);
			Old+=0x0002;
		}
	}
	TIM_SetCompare1(TIM3,b);
}

void move_mg2(u16 a,u16 b){
	if(a>b){
		u16 Old=a,New=b;
		while(Old>New){
		TIM_SetCompare2(TIM3,Old);
			delay_ms(TIME);
			Old-=0x0010;
		}
	}
	else if(a<b){
		u16 Old=a,New=b;
		while(Old<New){
		TIM_SetCompare2(TIM3,Old);
			delay_ms(TIME);
			Old+=0x0010;
		}
	}
	TIM_SetCompare2(TIM3,b);
}

void move_mg3(u16 a,u16 b){
	if(a>b){
		u16 Old=a,New=b;
		while(Old>New){
		TIM_SetCompare3(TIM3,Old);
			delay_ms(TIME);
			Old-=0x0010;
		}
	}
	else if(a<b){
		u16 Old=a,New=b;
		while(Old<New){
		TIM_SetCompare3(TIM3,Old);
			delay_ms(TIME);
			Old+=0x0010;
		}
	}
	TIM_SetCompare3(TIM3,b);
}

void move_mg4(u16 a,u16 b){
	if(a>b){
		u16 Old=a,New=b;
		while(Old>New){
		TIM_SetCompare4(TIM3,Old);
			delay_ms(TIME);
			Old-=0x0010;
		}
	}
	else if(a<b){
		u16 Old=a,New=b;
		while(Old<New){
		TIM_SetCompare4(TIM3,Old);
			delay_ms(TIME);
			Old+=0x0010;
		}
	}
	TIM_SetCompare4(TIM3,b);
}


