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
//ALIENTEKս��STM32������ ʵ��35
//����ͷʵ�� ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
int minx,miny,maxx,maxy,pointx,pointy,r;
u16 picture[48][64];
u16 R,G,B;
const u8*LMODE_TBL[5]={"Auto","Sunny","Cloudy","Office","Home"};							//5�ֹ���ģʽ	    
//const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7����Ч 
extern u8 ov_sta;	//��exit.c�� �涨��
extern u8 ov_frame;	//��timer.c���涨��		 
//����LCD��ʾ
void camera_refresh(void)
{
	u32 i,j;
 	u16 color;
	minx=270;
	miny=230;
	maxx=50;
	maxy=10;	
	if(ov_sta)//��֡�жϸ��£�
	{
		LCD_Scan_Dir(U2D_L2R);		//���ϵ���,������  
		if(lcddev.id==0X1963)LCD_Set_Window((lcddev.width-240)/2,(lcddev.height-320)/2,240,320);//����ʾ�������õ���Ļ����
		else if(lcddev.id==0X5510||lcddev.id==0X5310)LCD_Set_Window((lcddev.width-320)/2,(lcddev.height-240)/2,320,240);//����ʾ�������õ���Ļ����
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
		OV7670_RRST=0;				//��ʼ��λ��ָ�� 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//��λ��ָ����� 
		OV7670_RCK_H;
		for(i=0;i<240;i++)
		{
			for(j=0;j<320;j++)
			{
				OV7670_RCK_L;
				color=GPIOC->IDR&0XFF;	//������
				OV7670_RCK_H; 
				color=color<<8;  
				OV7670_RCK_L;
				color|=GPIOC->IDR&0XFF;	//������
				OV7670_RCK_H;
				
				R = (color&0xf800)>>11;
				G = (color&0x07e0)>>5;
				B = (color&0x001f);
				
				if(R<=10&&G<=20&&B<=10)
				{
					color = 0x0000;
					if(i>=10&&i<=230&&j>=50&&j<=270)
					{
						minx = j<minx?j:minx;
						miny = i<miny?i:miny;
						maxx = j>maxx?j:maxx;
						maxy = i>maxy?i:maxy;
					}
				}
				else color = 0xffff;
				
				LCD->LCD_RAM=color; 
			}  
		}   							  
 		ov_sta=0;					//����֡�жϱ��
		ov_frame++; 
		pointx = (minx+maxx)/2;
		pointy = (miny+maxy)/2;
		r = ((maxx-minx)+(maxy-miny))/4;
		LCD_DrawRectangle(minx,miny,maxx,maxy);
		LCD_Draw_Circle(pointx,pointy,r);
		LCD_Scan_Dir(DFT_SCAN_DIR);	//�ָ�Ĭ��ɨ�跽�� 
//		LCD_DrawRectangle(miny,minx,maxy,maxx);
	} 
}	   

void tuxiangchuli()
{
	int i,j;	
	minx=2;
	miny=10;
	maxx=46;
	maxy=54;
	pointx = 0;
	pointy = 0;
//	R = (picture[32][24]&0xf800)>>11;
//	G = (picture[32][24]&0x07e0)>>5;
//	B = (picture[32][24]&0x001f);
//	LCD_ShowNum(30,50,R,3,16);
//	LCD_ShowNum(30,70,G,3,16);
//	LCD_ShowNum(30,90,B,3,16);
//	LCD_DrawPoint(24*5,32*5);
//	tt = (R*38 + G*75 + B*15) >> 7;

	for(i=2;i<46;i++)
	{
		for(j=10;j<54;j++)
		{
//			R = (picture[i][j]&0xf800)>>11;
//			G = (picture[i][j]&0x07e0)>>5;
//			B = (picture[i][j]&0x001f);
			if(picture[i][j] <=0x1111)
			{
//				R = (picture[i+1][j]&0xf800)>>11;
//				G = (picture[i+1][j]&0x07e0)>>5;
//				B = (picture[i+1][j]&0x001f);
//				if(R<=10&&G<=20&&B<=10)
//				{
					minx = i;
					pointy = j;
					i = 46;
					j = 54;

//					miny = j+1;
//				}
			}
		}
	}
	LCD_ShowNum(30,50,minx,3,16);
	LCD_ShowNum(30,70,pointy,3,16);
	LCD_Fast_DrawPoint(miny*5,pointy*5,RED);
//	for(i=minx;i<54;i++)
//	{
//			R = (picture[i][pointy]&0xf800)>>11;
//			G = (picture[i][pointy]&0x07e0)>>5;
//			B = (picture[i][pointy]&0x001f);
//		if(R>=15&&G>=25&&B>=15)
//		{
//			R = (picture[i+2][pointy]&0xf800)>>11;
//			G = (picture[i+2][pointy]&0x07e0)>>5;
//			B = (picture[i+2][pointy]&0x001f);
//			if(R>=15&&G>=25&&B>=15)
//			{
//				maxx = i;
//				i = 54;
//				break;
//			}
//		}
//	}
//	if(minx!=10&&maxx!=54&&pointy!=0)
//	{
//		pointx = (minx+maxx)/2;
//		r = pointx - minx;
//	}
//	LCD_Fast_DrawPoint(miny*5+maxy*5,j,RED);
//	LCD_Scan_Dir(U2D_L2R);
//	LCD_DrawRectangle(pointy*5,minx*5,pointy*5+10,minx*5+20);
//	LCD_Draw_Circle(pointy*5,minx*5,5);
//	LCD_Scan_Dir(DFT_SCAN_DIR);	
//	if(R>=15)
//	{
//	}
//	else
//	{
//		LCD_ShowString(30,50,200,16,16,"black");
//	}
}


int main(void)
{		
	u8 key;
	u8 lightmode=0,saturation=2,brightness=2,contrast=2;
	u8 effect=0;	 
 	u8 i=0;	    
	u8 msgbuf[15];				//��Ϣ������
	u8 tm=0; 
 	Stm32_Clock_Init(9);		//ϵͳʱ������
	uart_init(72,115200);		//���ڳ�ʼ��Ϊ115200
	delay_init(72);	   	 		//��ʱ��ʼ�� 
 	usmart_dev.init(72);		//��ʼ��USMART		
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	LCD_Init();			   		//��ʼ��LCD  
//	TPAD_Init(6);				//����������ʼ�� 
 	POINT_COLOR=RED;			//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"OV7670 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2015/1/18"); 
	LCD_ShowString(30,130,200,16,16,"KEY0:Light Mode");
	LCD_ShowString(30,150,200,16,16,"KEY1:Saturation");
	LCD_ShowString(30,170,200,16,16,"KEY2:Brightness");
	LCD_ShowString(30,190,200,16,16,"KEY_UP:Contrast");
	LCD_ShowString(30,210,200,16,16,"TPAD:Effects");	 
  	LCD_ShowString(30,230,200,16,16,"OV7670 Init...");	  
	while(OV7670_Init())//��ʼ��OV7670
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
//	TIM6_Int_Init(10000,7199);			//10Khz����Ƶ��,1�����ж�									  
	EXTI8_Init();						//ʹ���ⲿ�ж�8,����֡�ж�
	OV7670_Window_Set(12,176,240,320);	//���ô���	  
  	OV7670_CS=0;					
	LCD_Clear(BLACK);	
//	OV7670_Special_Effects(0);//������Ч
 	while(1)
	{	
//		key=KEY_Scan(0);//��֧������
//		if(key)
//		{
//			tm=20;
//			switch(key)
//			{				    
//				case KEY0_PRES:	//�ƹ�ģʽLight Mode
//					lightmode++;
//					if(lightmode>4)lightmode=0;
//					OV7670_Light_Mode(lightmode);
//					sprintf((char*)msgbuf,"%s",LMODE_TBL[lightmode]);
//					break;
//				case KEY1_PRES:	//���Ͷ�Saturation
//					saturation++;
//					if(saturation>4)saturation=0;
//					OV7670_Color_Saturation(saturation);
//					sprintf((char*)msgbuf,"Saturation:%d",(signed char)saturation-2);
//					break;
//				case KEY2_PRES:	//����Brightness				 
//					brightness++;
//					if(brightness>4)brightness=0;
//					OV7670_Brightness(brightness);
//					sprintf((char*)msgbuf,"Brightness:%d",(signed char)brightness-2);
//					break;
//				case WKUP_PRES:	//�Աȶ�Contrast			    
//					contrast++;
//					if(contrast>4)contrast=0;
//					OV7670_Contrast(contrast);
//					sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-2);
//					break;
//			}
//		}	 
//		if(TPAD_Scan(0))//��⵽�������� 
//		{
//			effect++;
//			if(effect>6)effect=0;

//	 		sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
//			tm=20;
//		} 
		OV7670_Special_Effects(0);
		camera_refresh();//������ʾ
//		tuxiangchuli();
// 		if(tm)
//		{
//			LCD_ShowString((lcddev.width-240)/2+30,(lcddev.height-320)/2+60,200,16,16,msgbuf);
//			tm--;
//		}
//		i++;
//		if(i==15)//DS0��˸.
//		{
//			i=0;
//			LED0=!LED0;
// 		}
	}	   
}










