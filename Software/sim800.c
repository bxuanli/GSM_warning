#include <reg52.h> 
#include "sim800.h"	
/*****************************************************************************
UART��ʼ�� 11.0592
*****************************************************************************/
void Uart_Init(void)		
{
	EA=1;
//	ES=1;
	SCON = 0x50;			 
	TMOD = 0x20;
	PCON=0x00;	
	TH1 = 0xFD;	
	TL1 = 0xfd;
	TR1 = 1;		
	RI = 0;	
}

/*****************************************************************************
���ַ����ͺ���
/****************************************************************************/
void Send_Char (unsigned char UART_data)		
{ 		
	SBUF = UART_data;			
	while(TI == 0);			
	TI = 0;					  
}

/*****************************************************************************
�ַ������ͺ���
/****************************************************************************/
void Send_String(unsigned char *str)		
{
	while(*str != '\0')		
	{
		Send_Char(*str);			
		*str=*str++;		
	}
	*str = 0;				  
}
/*****************************************************************************
�ӳٺ���
/****************************************************************************/
void Delay_Ms (unsigned int a)
{
	unsigned int i;
	while( a-- != 0)
	{
	for(i = 0; i < 800; i++);	//	for(i = 0; i < 600; i++);
	}
} 
/*****����Ϊ���ն���ģʽ*************/
void Receive_message()
{
 	Send_String("AT+CMGF=1\r\n"); //����Ӣ��ģʽ
	Delay_Ms(500);	
	Send_String("AT+CSCS=\"GSM\"\r\n");     //�����ֽڼ�
	Delay_Ms(500);	
	Send_String("AT+CNMI=2,2,0,1\r\n"); //���óɣ����ܶ������ѣ�ͬʱ���ܶ������ݡ���������ʾģʽ ����Ϣָʾ��
	Delay_Ms(1000);
}
 /*****������*******
  *content:��������*/
void Send_message(unsigned char *content)	  //
{
 	Send_String("AT+CMGF=1\r\n");
	Delay_Ms(200);
	Send_String("AT+CSMP=17,167,2,25\r\n");//���ö����ı�ģʽ����
	Delay_Ms(200);
	Send_String("AT+CSCS=\"GSM\"\r\n");     //����Ӣ�ĸ�ʽ�ֽڼ�
	Delay_Ms(200);
	Send_String("AT+CMGS=\"15350874527\"\r\n");	//���ն��ŵĵ绰����   ����Ƭ������ĳ����Ѿ������ɿͻ����ֻ��ţ���������ͳһ���ͣ�������������ط�û�и�
	Delay_Ms(200);
	Send_String(content); // ���Ͷ������ݣ�
	Delay_Ms(200);
	Send_Char(0x1A);
	Delay_Ms(5000);
}	