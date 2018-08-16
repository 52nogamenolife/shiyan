# RoboGame
已经使用的io口：PA2 PA3 PA0 PA1;PA4 PA5 PA6 PA7;PA14 PA11 PA9 PA10 PA12  PB0 PB1 PB6 PB7 PB3 PB9 PB13 PB14 PE7 PE8 PE9 PE10 PE11
#被玩坏的家伙 PB4(PWM) PA13(pp) PA14(pp) PB8(蜂鸣器)
#四个舵机使用TIM3的四个通道使用引脚为PA6 PA7 PB0 PB1
#四个步进电机使用的TIM4的定时器中断使用的引脚是PB6 PB7 PB8 PB9

#通信协议：Lr 左手抓 Rr右手抓 Pr放下 Fr脱机 Gr读取rfidx信息 Br左右臂舵机回转 使用的是uart1 蓝牙引脚为PA9输出(TX) PA10输入(RX)

#rfid的读取使用的是uart2 引脚为PA2输出(TX) PA3输入(RX)
#“红”、“专”、“并”、“进”、“理”、“实”、“交”、“融”八面旗帜颜色分别为 
RGB(190,64,64), RGB(190,160,64), RGB(127,190,64), RGB(64,190,97), RGB(64,190,190), RGB(64,94,190), RGB(130,64,190), RGB(190,64,157)；
对应底座的 RFID 标签 ID 分别为 0x01 0x00,	 0x02 0x00, 	0x03 0x00, 		0x04 0x00,		 0x05 0x00,		 0x06 0x00,		 0x07 0x00,		 0x08 0x00,
#函数Read_flag的返回值 0为失败 1 2 3 4 5 6 7 8分别“表示红专并进历史交融”


#电缸的前进与后退的引脚是PA0 PA1;PA4 PA5;PA12 PA11;PB14 PB13;


#stm与pc的通信 分为三个部分：
#第一部分 t/g 表示步进电机或者舵机
#第二部分 1 2 3 4表示哪一个电机或者舵机
#第三部分 一个数字（暂时应该可能是字符的形式读入所以ASCII码，日后应该调整改动）

#步进电机10ms一次中断 motor（可调）总的运行长度 speed（可调）另一个变量控制速度 u8
#想想超声波提前减速

#电缸参数的调整 以及安全急停
#电缸的前进和后退都要可控

#超声波模块的引脚 一号：trig PE8 echo PE9 二号：trig PE10 echo PE11

#开关触发 PE7被置高


