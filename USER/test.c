#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"   
//#include "tpad.h"  
#include "exti.h"   
#include "usmart.h" 
#include "ov7670.h" 
//ALIENTEK战舰STM32开发板 实验35
//摄像头实验 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
int minx,miny,maxx,maxy,pointx,pointy,r;
u16 picture[48][64];
u16 R,G,B;
u16 i,j,k;
u16 color;
const u8*LMODE_TBL[5]={"Auto","Sunny","Cloudy","Office","Home"};							//5种光照模式	    
//const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效 
extern u8 ov_sta;	//在exit.c里 面定义
extern u8 ov_frame;	//在timer.c里面定义		 
//更新LCD显示
void camera_refresh(void)
{

	minx=269;
	miny=229;
	maxx=49;
	maxy=9;	
	if(ov_sta)//有帧中断更新？
	{
		LCD_Scan_Dir(U2D_L2R);		//从上到下,从左到右  
		if(lcddev.id==0X1963)LCD_Set_Window((lcddev.width-240)/2,(lcddev.height-320)/2,240,320);//将显示区域设置到屏幕中央
		else if(lcddev.id==0X5510||lcddev.id==0X5310)LCD_Set_Window((lcddev.width-320)/2,(lcddev.height-240)/2,320,240);//将显示区域设置到屏幕中央
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
		OV7670_RRST=0;				//开始复位读指针 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//复位读指针结束 
		OV7670_RCK_H;
		for(j=0;j<240;j++)
		{
			for(i=0;i<320;i++)
			{
				OV7670_RCK_L;
				color=GPIOC->IDR&0XFF;	//读数据
				OV7670_RCK_H; 
				color=color<<8;  
				OV7670_RCK_L;
				color|=GPIOC->IDR&0XFF;	//读数据
				OV7670_RCK_H;
				
				R = (color&0xf800)>>11;
				G = (color&0x07e0)>>5;
				B = (color&0x001f);
				
				if(R<=18&&G<=27&&B<=18)//二值化
				{
					color = 0x0000;
					if(i>=49&&i<=269&&j>=9&&j<=229)
					{
						minx = i<minx?i:minx;
						miny = j<miny?j:miny;
						maxx = i>maxx?i:maxx;
						maxy = j>maxy?j:maxy;
					}
				}
				else color = 0xffff;
				
				if(i==49||i==269||j==9||j==229)
					color = 0xF800;//标注识别范围
				
				LCD->LCD_RAM=color; 
			}  
		}   				

 		ov_sta=0;					//清零帧中断标记
		pointx = (minx+maxx)/2;
		pointy = (miny+maxy)/2;
		r = ((maxx-minx)+(maxy-miny))/4;
		LCD_DrawRectangle(minx,miny,maxx,maxy);	
		
		USART1->DR = (u8) (pointx-49);      	//写DR,串口1将发送数据
		while((USART1->SR&0X40)==0);//等待上一次串口数据发送完成  
		USART1->DR = (u8) (pointy-9);      	//写DR,串口1将发送数�
		while((USART1->SR&0X40)==0);
		USART1->DR = (u8) 0x0d; 		
		while((USART1->SR&0X40)==0); //等待上一次串口数据发送完成  
		USART1->DR = (u8) 0x0a; 			
		while((USART1->SR&0X40)==0); //等待上一次串口数据发送完成 
		
//		printf("%d%d\r\n",pointx-49,pointy-9);
//		LCD_Draw_Circle(pointx,pointy,r);
//		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 

//		USART1->DR = (u8) (pointx-49);      	//写DR,串口1将发送数据
//		while((USART1->SR&0X40)==0);//等待上一次串口数据发送完成  
//		USART1->DR = (u8) (pointy-9);      	//写DR,串口1将发送数�
//		while((USART1->SR&0X40)==0);
//		USART1->DR = (u8) 0x0d; 		
//		while((USART1->SR&0X40)==0); //等待上一次串口数据发送完成  
//		USART1->DR = (u8) 0x0a; 			
//		while((USART1->SR&0X40)==0); //等待上一次串口数据发送完成  

	} 
}	   

int main(void)
{		
//	u8 key;
	u8 lightmode=0,saturation=2,brightness=2,contrast=2;
	u8 effect=0;	 
// 	u8 i=0;	    
//	u8 msgbuf[15];				//消息缓存区
//	u8 tm=0; 
 	Stm32_Clock_Init(9);		//系统时钟设置
	uart_init(72,115200);		//串口初始化为115200
	delay_init(72);	   	 		//延时初始化 
// 	usmart_dev.init(72);		//初始化USMART		
// 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD  
//	TPAD_Init(6);				//触摸按键初始化 
 	POINT_COLOR=RED;			//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"OV7670 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2015/1/18"); 
	LCD_ShowString(30,130,200,16,16,"KEY0:Light Mode");
	LCD_ShowString(30,150,200,16,16,"KEY1:Saturation");
	LCD_ShowString(30,170,200,16,16,"KEY2:Brightness");
	LCD_ShowString(30,190,200,16,16,"KEY_UP:Contrast");
//	LCD_ShowString(30,210,200,16,16,"TPAD:Effects");	 
  	LCD_ShowString(30,230,200,16,16,"OV7670 Init...");	  
	while(OV7670_Init())//初始化OV7670
	{
		LCD_ShowString(30,230,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	    LCD_Fill(30,230,239,246,WHITE);
		delay_ms(200);
	}
 	LCD_ShowString(30,230,200,16,16,"OV7670 Init OK");
	delay_ms(1500);	 	   
	OV7670_Light_Mode(lightmode);
	OV7670_Color_Saturation(saturation);
	OV7670_Brightness(brightness);
	OV7670_Contrast(contrast);
 	OV7670_Special_Effects(effect);	 
//	TIM6_Int_Init(10000,7199);			//10Khz计数频率,1秒钟中断									  
	EXTI8_Init();						//使能外部中断8,捕获帧中断
	OV7670_Window_Set(12,176,240,320);	//设置窗口	  
  	OV7670_CS=0;					
	LCD_Clear(BLACK);	
	OV7670_Special_Effects(0);//设置特效
 	while(1)
	{	
//		OV7670_Special_Effects(0);
		camera_refresh();//更新显示

	}	   
}










