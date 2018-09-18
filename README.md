# RoboGame
！一定要把 exit.c文件导入到Fwlib中！！！！！
已经使用的io口：PA2 PA3 PA6 PA7;PA11 PA9 PA10 PA12  PB0 PB1 PB5 PB6 PB7 PB3 PB9 PB12 PB13 PB14 PB15 PE7 PE8 PE9 PE10 PE11 PE13 PE14
#被玩坏的家伙 PB4(PWM) PA13(pp) PA14(pp) PB8(蜂鸣器)  PA0 PA1;PA4 PA5
#四个舵机使用TIM3的四个通道使用引脚为PA6 PA7 PB0 PB1
#四个步进电机使用的TIM4的定时器中断使用的引脚是PB6 PB7 PB5 PB9

#通信协议：Lr 左手抓 Rr右手抓 Pr放下 Fr脱机 Gr读取rfidx信息 Br左右臂舵机回转 使用的是uart1 蓝牙引脚为PA9输出(TX) PA10输入(RX)

#rfid的读取使用的是uart2 引脚为PA2输出(TX) PA3输入(RX)
#函数Read_flag的返回值 0为失败 1 2 3 4 5 6 7 8分别“表示红专并进历史交融”


#电缸的前进与后退的引脚是PA12 PA11;PE14 PE13;
上面和下面抓2.7s 松2.7s

#超声波模块的引脚 一号：trig PE5 echo PE9  700=3cm    二号：trig PE4 echo PE11  7300 1.2m  5730 0.974m

#开关触发 PE7被置高

#右边 1B红 1A蓝 2A黑 2B绿   左边 1B蓝 1A红 2A黑 2B绿 
左上 0f


rfid  高17.8cm 宽12.6  cm


舵机参数
右边舵机 04 初始位置  077f   取旗 0719 外接规避077f  放旗077f  集合0730

左边舵机 03   初始位置0717            取旗  077a 外接规避0717 放旗0717  集合0760

（12）舵机 01    夹紧06F0   全松073a

（）舵机 02        加紧06f8  全松0780



电钢（78 ）抓1A+ 松1A-


电钢（78 ）抓1D+ 松1D-

