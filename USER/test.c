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
//ALIENTEKÕ½½¢STM32¿ª·¢°å ÊµÑé35
//ÉãÏñÍ·ÊµÑé ÊµÑé 
//¼¼ÊõÖ§³Ö£ºwww.openedv.com
//¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 
int minx,miny,maxx,maxy,pointx,pointy,r;
u16 picture[48][64];
u16 R,G,B;
u16 i,j,k;
u16 color;
const u8*LMODE_TBL[5]={"Auto","Sunny","Cloudy","Office","Home"};							//5ÖÖ¹âÕÕÄ£Ê½	    
//const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7ÖÖÌØĞ§ 
extern u8 ov_sta;	//ÔÚexit.cÀï Ãæ¶¨Òå
extern u8 ov_frame;	//ÔÚtimer.cÀïÃæ¶¨Òå		 
//¸üĞÂLCDÏÔÊ¾
void camera_refresh(void)
{

	minx=269;
	miny=229;
	maxx=49;
	maxy=9;	
	if(ov_sta)//ÓĞÖ¡ÖĞ¶Ï¸üĞÂ£¿
	{
		LCD_Scan_Dir(U2D_L2R);		//´ÓÉÏµ½ÏÂ,´Ó×óµ½ÓÒ  
		if(lcddev.id==0X1963)LCD_Set_Window((lcddev.width-240)/2,(lcddev.height-320)/2,240,320);//½«ÏÔÊ¾ÇøÓòÉèÖÃµ½ÆÁÄ»ÖĞÑë
		else if(lcddev.id==0X5510||lcddev.id==0X5310)LCD_Set_Window((lcddev.width-320)/2,(lcddev.height-240)/2,320,240);//½«ÏÔÊ¾ÇøÓòÉèÖÃµ½ÆÁÄ»ÖĞÑë
		LCD_WriteRAM_Prepare();     //¿ªÊ¼Ğ´ÈëGRAM	
		OV7670_RRST=0;				//¿ªÊ¼¸´Î»¶ÁÖ¸Õë 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//¸´Î»¶ÁÖ¸Õë½áÊø 
		OV7670_RCK_H;
		for(j=0;j<240;j++)
		{
			for(i=0;i<320;i++)
			{
				OV7670_RCK_L;
				color=GPIOC->IDR&0XFF;	//¶ÁÊı¾İ
				OV7670_RCK_H; 
				color=color<<8;  
				OV7670_RCK_L;
				color|=GPIOC->IDR&0XFF;	//¶ÁÊı¾İ
				OV7670_RCK_H;
				
				R = (color&0xf800)>>11;
				G = (color&0x07e0)>>5;
				B = (color&0x001f);
				
				if(R<=18&&G<=27&&B<=18)//¶şÖµ»¯
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
					color = 0xF800;//±ê×¢Ê¶±ğ·¶Î§
				
				LCD->LCD_RAM=color; 
			}  
		}   				

 		ov_sta=0;					//ÇåÁãÖ¡ÖĞ¶Ï±ê¼Ç
		pointx = (minx+maxx)/2;
		pointy = (miny+maxy)/2;
		r = ((maxx-minx)+(maxy-miny))/4;
		LCD_DrawRectangle(minx,miny,maxx,maxy);	
		
		USART1->DR = (u8) (pointx-49);      	//Ğ´DR,´®¿Ú1½«·¢ËÍÊı¾İ
		while((USART1->SR&0X40)==0);//µÈ´ıÉÏÒ»´Î´®¿ÚÊı¾İ·¢ËÍÍê³É  
		USART1->DR = (u8) (pointy-9);      	//Ğ´DR,´®¿Ú1½«·¢ËÍÊı¾
		while((USART1->SR&0X40)==0);
		USART1->DR = (u8) 0x0d; 		
		while((USART1->SR&0X40)==0); //µÈ´ıÉÏÒ»´Î´®¿ÚÊı¾İ·¢ËÍÍê³É  
		USART1->DR = (u8) 0x0a; 			
		while((USART1->SR&0X40)==0); //µÈ´ıÉÏÒ»´Î´®¿ÚÊı¾İ·¢ËÍÍê³É 
		
//		printf("%d%d\r\n",pointx-49,pointy-9);
//		LCD_Draw_Circle(pointx,pointy,r);
//		LCD_Scan_Dir(DFT_SCAN_DIR);	//»Ö¸´Ä¬ÈÏÉ¨Ãè·½Ïò 

//		USART1->DR = (u8) (pointx-49);      	//Ğ´DR,´®¿Ú1½«·¢ËÍÊı¾İ
//		while((USART1->SR&0X40)==0);//µÈ´ıÉÏÒ»´Î´®¿ÚÊı¾İ·¢ËÍÍê³É  
//		USART1->DR = (u8) (pointy-9);      	//Ğ´DR,´®¿Ú1½«·¢ËÍÊı¾
//		while((USART1->SR&0X40)==0);
//		USART1->DR = (u8) 0x0d; 		
//		while((USART1->SR&0X40)==0); //µÈ´ıÉÏÒ»´Î´®¿ÚÊı¾İ·¢ËÍÍê³É  
//		USART1->DR = (u8) 0x0a; 			
//		while((USART1->SR&0X40)==0); //µÈ´ıÉÏÒ»´Î´®¿ÚÊı¾İ·¢ËÍÍê³É  

	} 
}	   

int main(void)
{		
//	u8 key;
	u8 lightmode=0,saturation=2,brightness=2,contrast=2;
	u8 effect=0;	 
// 	u8 i=0;	    
//	u8 msgbuf[15];				//ÏûÏ¢»º´æÇø
//	u8 tm=0; 
 	Stm32_Clock_Init(9);		//ÏµÍ³Ê±ÖÓÉèÖÃ
	uart_init(72,115200);		//´®¿Ú³õÊ¼»¯Îª115200
	delay_init(72);	   	 		//ÑÓÊ±³õÊ¼»¯ 
// 	usmart_dev.init(72);		//³õÊ¼»¯USMART		
// 	LED_Init();		  			//³õÊ¼»¯ÓëLEDÁ¬½ÓµÄÓ²¼ş½Ó¿Ú
	KEY_Init();					//³õÊ¼»¯°´¼ü
	LCD_Init();			   		//³õÊ¼»¯LCD  
//	TPAD_Init(6);				//´¥Ãş°´¼ü³õÊ¼»¯ 
 	POINT_COLOR=RED;			//ÉèÖÃ×ÖÌåÎªºìÉ« 
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
	while(OV7670_Init())//³õÊ¼»¯OV7670
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
//	TIM6_Int_Init(10000,7199);			//10Khz¼ÆÊıÆµÂÊ,1ÃëÖÓÖĞ¶Ï									  
	EXTI8_Init();						//Ê¹ÄÜÍâ²¿ÖĞ¶Ï8,²¶»ñÖ¡ÖĞ¶Ï
	OV7670_Window_Set(12,176,240,320);	//ÉèÖÃ´°¿Ú	  
  	OV7670_CS=0;					
	LCD_Clear(BLACK);	
	OV7670_Special_Effects(0);//ÉèÖÃÌØĞ§
 	while(1)
	{	
//		OV7670_Special_Effects(0);
		camera_refresh();//¸üĞÂÏÔÊ¾

	}	   
}










