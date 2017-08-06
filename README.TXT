实验器材:
	战舰STM32F103开发板V3版本
	
实验目的:
	学习STM32驱动OV7670等摄像头.
	
硬件资源:
	ALIENTEK 2.8/3.5/4.3/7寸TFTLCD模块(通过FSMC驱动,FSMC_NE4接LCD片选/A10接RS)
	外部中断8(PA8,用于检测OV7670的帧信号)
	ALIENTEK OV7670摄像头模块,连接关系为:
		OV7670模块 ----------- STM32开发板
		 OV_D0~D7  -----------   PC0~7
		 OV_SCL    ------------  PD3
		 OV_SDA    ------------  PG13
		 OV_VSYNC  ------------  PA8
		 FIFO_RRST -----------   PG14
		 FIFO_OE   -----------   PG15
		 FIFO_WRST ------------  PD6
		 FIFO_WEN  ------------  PB3
		 FIFO_RCLK ------------  PB4	  
	
实验现象:
	本实验开机后，先初始化摄像头模块（OV7670）
	然后会在中心区域识别黑色小球，并用红色方框圈出
	
----elve960520@163.com































