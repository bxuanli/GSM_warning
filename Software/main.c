#include<12C5A60S2.h>
#include<intrins.h>
#include<ADC.h>
#include "uart2.h"	
#include "sim800.h"	
/********甲醛串口接收数据缓存**********/
unsigned char idata ZE08_receive[30];
/********甲醛串口接收计数器**********/
unsigned char  ZE08_DATA_count; 
bit kg_flag = 0;
unsigned char str_ze08[5];//甲醛值
unsigned char str_co[5];//值
unsigned char P_baojing = 100;	//甲醛报警值
unsigned int  C_baojing = 100;
unsigned char P_buff[4];
unsigned char C_buff[4];
unsigned char moshi=0;
unsigned char time;
int ZE08,Value1,Value2;
 //unsigned char dat = 0x00;      //AD值
bit qx_flag = 1;      //报警开关 默认开
bit flag_1s ;      //报警开关 默认开
sbit  shezhi = P3^7;
sbit  jia    = P3^6;
sbit  jian   = P3^5;
sbit  quxiao = P3^4;
sbit  BUZZER = P3^2;
sbit  LED_P  = P1^4;//   甲醛报警指示灯
sbit  LED_C  = P1^5;//  烟雾报警指示灯
sbit  HW  = P1^6;//  红外感应
unsigned int sum=0;
unsigned char IntToString(unsigned char *str, int dat);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdWriteCmd(unsigned char cmd);
unsigned char *content1 =  "Warning: Vehicle anomaly!\r\n";

void Key_set_scan()
{
  	if(quxiao==0)
	{
	   Delay_Ms(10);

		if(quxiao==0)
		{
		   while(!quxiao);
		  qx_flag =~ qx_flag;
		}
	}  
	if(shezhi==0)
	{
	   Delay_Ms(10);
		if(shezhi==0)
		{  
		   while(!shezhi);
		   LcdWriteCmd(0x01);  //清屏
		   moshi++;
		   if(moshi >= 3)moshi = 0;
		   if(moshi == 0)
		   { 
              LcdShowStr(0, 0,"HCHO:   ppm T:  ");
              LcdShowStr(0, 1,"Smoke:    ppm");
		   }
		
		   else if(moshi == 1)
		   {    LcdWriteCmd(0x01);  //清屏
		        Delay_Ms(10);
		       LcdShowStr(0, 0,"                ");
		   	   LcdShowStr(0, 1,"Set_H:    ppm   ");
		   }
		   else if(moshi == 2)
		   {   LcdWriteCmd(0x01);  //清屏
		       Delay_Ms(10);
		       LcdShowStr(0, 0,"                ");
		   	   LcdShowStr(0, 1,"Set_S:    ppm   ");
		   }		   		   	
		}
	}
	
	if(jia==0)
	{
	   Delay_Ms(80);
		if(jia==0)
		{ 				
			if(moshi==1)
			{
			 P_baojing++ ;
			 if( P_baojing>=999 )P_baojing =999;
			}

			if(moshi==2)
			{
			 C_baojing++ ;
			 if( C_baojing>=999 )C_baojing =999;
			}		
		 }
	 }
	 if(jian == 0)
	 {
	    Delay_Ms(80);
	   if(jian == 0)
	   {
	       
			if(moshi==1)
			{
			 P_baojing-- ;
			 if( P_baojing<=0 )P_baojing =0;
			}
			if(moshi==2)
			{
			 C_baojing-- ;
			 if( C_baojing<=0 )C_baojing =0;
			}
	   }
	 }
		 
}

/*------------------------------------------------
                    定时器初始化子程序
------------------------------------------------*/
void Init_Timer0(void)
{
 TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
 TH0=(65536-5000)/256;		  //重新赋值5ms
 TL0=(65536-5000)%256;
 EA=1;            //总中断打开
 ET0=1;           //定时器中断打开
 TR0=1;           //定时器开关打开
 PT0=1;           //优先级打开
}
/************主函数****************/
void main()
{  	  
   char len,a,i;
   char buff[3];
//   U8 m;
  InitLcd1602();     //初始化液晶
  LcdShowStr(0,0,"  System init   ");
  LcdShowStr(0,1,"  please wait   ");
  for(i = 0;i < 15;i++)//等待网络稳定
 {
		Delay_Ms(1000);
  }
   InitLcd1602();   
  LcdShowStr(0, 0,"HCHO:   ppm T:  ");
  LcdShowStr(0, 1,"Smoke:   ppm    ");

  Init_Timer0();	//定时器初始化
  Uart_Init();   //配置波特率为9600
  Uart_Init_2(); 
  ADC_Init(ADC_PORT0);
 
  while(1)
 {
   
 Key_set_scan();//按键扫描
 if(moshi == 0)	  //设置模式切换
 {  
    len = IntToString(str_ze08,ZE08); //转换成字符串
    while (len < 3)                  //用空格补齐到5个字符长度
    {
        str_ze08[len++] = ' ';
    }
    str_ze08[len] = '\0';              //添加字符串结束符
    LcdShowStr(5, 0, str_ze08); //甲醛显示到液晶屏上 
	  	 
	Value2 = GetADCResult(ADC_CH0);	  //yanwu 检�
    Value2 =  (float)(Value2/2);//浓度校准
    len = IntToString(str_co,Value2); //转换成字符串
    while (len < 3)               //用空格补齐到3个字符长度
    {
        str_co[len++] = ' ';
    }
     str_co[len] = '\0';              //添加字符串结束符
 
   LcdShowStr(6, 1, str_co); //烟雾浓度显示到液晶屏上  
   if(qx_flag == 1)
   {
   if((ZE08>=P_baojing)||(Value2>=C_baojing)) BUZZER = 0; else BUZZER = 1;
   }
   else BUZZER = 1;
 
   if(ZE08>=P_baojing)      LED_P = 0;   else  LED_P = 1;	
   if(Value2>=C_baojing)     LED_C = 0;   else  LED_C = 1;

    if(HW==0)
	{	
	  if(flag_1s==1)
	  {
	   flag_1s=0;
	   time++; if(time>99) time=99;
	   buff[0]=time/10+0x30;
	   buff[1]=time%10+0x30;
	   buff[2]=	'\0';
	   LcdShowStr(14, 0,buff);
	   if(time>=5)
	   {
	   	  if(a==0)
		  {  
		   a=1;
		   Send_message(content1);	  //发报警短信
		   //time=0;		   
		  }
	   }
	  }
	}
	else 
	{
	   time=0;
	   buff[0]=time/10+0x30;
	   buff[1]=time%10+0x30;
	   buff[2]=	'\0';
	   LcdShowStr(14, 0,buff);
	}
   	
  }

   else if(moshi == 1)	 //设置甲醛浓度上限模式
   {
   P_buff[0] = P_baojing/100+0x30;
   P_buff[1] = P_baojing%100/10+0x30;
   P_buff[2] = P_baojing%10+0x30;
   P_buff[4] = '\0';
   LcdShowStr(6, 1,P_buff);
   }
   else if(moshi == 2)	 //设置烟雾浓度上限模式
   {
   C_buff[0] = C_baojing/100+0x30;
   C_buff[1] = C_baojing%100/10+0x30;
   C_buff[2] = C_baojing%10+0x30;
   C_buff[4] = '\0';
   LcdShowStr(6, 1,C_buff);
   }				                                                                           
 }
}
/* 整型数转换为字符串，str-字符串指针，dat-待转换数，返回值-字符串长度 */
unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
    if (dat < 0)  //如果为负数，首先
	
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //先转换为低位在前的十进制数组
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i最后的值就是有效字符的个数
    while (i-- > 0)   //将数组值转换为ASCII码反向拷贝到接收指针上
    {
        *str++ = buf[i] + '0';
    }

    *str = '\0';  //添加字符串结束符
    
    return len;   //返回字符串长度
}

/************串行口2中断处理函数*************/
void UART_2Interrupt(void) interrupt 8
{
	unsigned char UART_data;
	if(S2CON&S2RI)
	{
	  UART_data=S2BUF;
	  if(UART_data==0xFF)
		{
			if((ZE08_receive[0]==0x17)&&(ZE08_receive[1]==0x04))
			{
			   ZE08 = ZE08_receive[3]*256+ZE08_receive[4];			   
			}
		
			ZE08_DATA_count=0;
		}
		else
		{
			ZE08_receive[ZE08_DATA_count]=UART_data;
			ZE08_DATA_count++;
		}
	
	}
	S2CON&=~S2RI;
}
/*------------------------------------------------
    定时器中断子程序（定时1ms）
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{
   static unsigned int s1j;
   TH0=(65536-5000)/256;	 //重新赋值 5ms
   TL0=(65536-5000)%256;
    
    s1j++;   
	if(s1j>=1300)
   {
     s1j = 0;
	 flag_1s = 1;
	 	
    }

}
