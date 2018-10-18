#include "sys.h"

void stop(void); //停止
void forward(void);//电缸向前推进 爪子张开
void backward(void);//电感向后缩退 爪子合并抓住
void stop(void);//电缸全部停止
void adapter_GPIO_init(void);
