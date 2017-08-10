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
u16 i,j,k;
u16 color;
const u8*LMODE_TBL[5]={"Auto","Sunny","Cloudy","Office","Home"};							//5�ֹ���ģʽ	    
//const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7����Ч 
extern u8 ov_sta;	//��exit.c�� �涨��
extern u8 ov_frame;	//��timer.c���涨��		 
//����LCD��ʾ
void camera_refresh(void)
{

	minx=269;
	miny=229;
	maxx=49;
	maxy=9;	
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
		for(j=0;j<240;j++)
		{
			for(i=0;i<320;i++)
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
				
				if(R<=18&&G<=27&&B<=18)//��ֵ��
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
					color = 0xF800;//��עʶ��Χ
				
				LCD->LCD_RAM=color; 
			}  
		}   				

 		ov_sta=0;					//����֡�жϱ��
		pointx = (minx+maxx)/2;
		pointy = (miny+maxy)/2;
		r = ((maxx-minx)+(maxy-miny))/4;
		LCD_DrawRectangle(minx,miny,maxx,maxy);	
		
		USART1->DR = (u8) (pointx-49);      	//дDR,����1����������
		while((USART1->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
		USART1->DR = (u8) (pointy-9);      	//дDR,����1���������
		while((USART1->SR&0X40)==0);
		USART1->DR = (u8) 0x0d; 		
		while((USART1->SR&0X40)==0); //�ȴ���һ�δ������ݷ������  
		USART1->DR = (u8) 0x0a; 			
		while((USART1->SR&0X40)==0); //�ȴ���һ�δ������ݷ������ 
		
//		printf("%d%d\r\n",pointx-49,pointy-9);
//		LCD_Draw_Circle(pointx,pointy,r);
//		LCD_Scan_Dir(DFT_SCAN_DIR);	//�ָ�Ĭ��ɨ�跽�� 

//		USART1->DR = (u8) (pointx-49);      	//дDR,����1����������
//		while((USART1->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
//		USART1->DR = (u8) (pointy-9);      	//дDR,����1���������
//		while((USART1->SR&0X40)==0);
//		USART1->DR = (u8) 0x0d; 		
//		while((USART1->SR&0X40)==0); //�ȴ���һ�δ������ݷ������  
//		USART1->DR = (u8) 0x0a; 			
//		while((USART1->SR&0X40)==0); //�ȴ���һ�δ������ݷ������  

	} 
}	   

int main(void)
{		
//	u8 key;
	u8 lightmode=0,saturation=2,brightness=2,contrast=2;
	u8 effect=0;	 
// 	u8 i=0;	    
//	u8 msgbuf[15];				//��Ϣ������
//	u8 tm=0; 
 	Stm32_Clock_Init(9);		//ϵͳʱ������
	uart_init(72,115200);		//���ڳ�ʼ��Ϊ115200
	delay_init(72);	   	 		//��ʱ��ʼ�� 
// 	usmart_dev.init(72);		//��ʼ��USMART		
// 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
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
//	LCD_ShowString(30,210,200,16,16,"TPAD:Effects");	 
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
	OV7670_Special_Effects(0);//������Ч
 	while(1)
	{	
//		OV7670_Special_Effects(0);
		camera_refresh();//������ʾ

	}	   
}










