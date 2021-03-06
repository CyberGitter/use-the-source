/*********************************************************/
/** 单片机小系统测试程序 **/
/** 作者：关永峰 **/
/** 时间：20058/11/19 **/
/** National University of Defence Technology **/
/** 0731-4573493 **/
/*********************************************************/
#include <absacc.h>
#include <reg51.h>
#include<intrins.h>
#define uchar unsigned char
#define uint8 unsigned char
#define uint16 unsigned int
#define LED1 XBYTE [0xA000] //数码管地址
#define LED2 XBYTE [0xA001]
#define LED3 XBYTE [0xA002]
#define LED4 XBYTE [0xA003]
#define LED5 XBYTE [0xA004]
#define LED6 XBYTE [0xA005]
#define LED7 XBYTE [0xA006]
#define LED8 XBYTE [0xA007]
#define KEY XBYTE [0xA100] //键盘地址
#define lcdDl XBYTE [0xA201] //左半边液晶数据地址
#define lcdDr XBYTE [0xA301] //右半边液晶数据地址
#define lcdlCommand XBYTE [0xA200]//左半边液晶命令地址
#define lcdrCommand XBYTE [0xA300]//右半边液晶命令地址
/*液晶显示使用的常量*/
#define strat_yaddr 0x40
#define strat_xaddr 0xb8
#define strat_dispaddr 0xc0
#define libleng 0x5F //字模库容量
#define dataleng 16*8 //全屏8*8点阵容量
uint16 curr=50;//数控电流值临时寄存器
/*TLC1549使用的变量*/
sbit adclk=P1^0; //时钟线
sbit addata=P1^1; //数据线
sbit adcs=P1^2; //CS
/*TLC5615使用的变量*/
sbit SPI_DATA=P1^5;
sbit SPI_CLK=P1^6;
sbit CS_DA=P1^7;
xdata uchar RAM[32768] _at_ 0x0000;//外部RAM地址
/*扫描键盘使用的变量 */
sbit first_row = P1^4; //键盘第一行控制
sbit second_row = P1^3; //键盘第二行控制
bit first_getkey = 0,control_readkey = 0; //读键盘过程中的标志位
bit getkey = 0; //获得有效键值标志位 等于1时代表得到一个有效键值
bit keyon = 0; //防止按键冲突标志位
uchar keynum = 0; //获得的有效按键值寄存器
/*数码管显示使用的变量和常量*/
uchar lednum = 0; //数码管显示位控制寄存器
uchar led[8] = {0,0,0,0,0,0,0,0}; //数码管显示内容寄存器
uchar code segtab[18] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0x8c,0xff}; //七段码段码表
// "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "P" ,"black"
/*液晶显示使用的变量*/
uchar code worddata[dataleng] = "Author: Song Wenjie Yuan Hui Cheng Haoliang 2006/09/23";//自定义要显示的ASCII字符串
//8*8点阵ASCII全部字符表
uchar code ASCII[libleng][8] = {
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*SPACE*/
	{0x00,0x00,0x7e,0x21,0x21,0x21,0x7e,0x00},/*A*/
	{0x00,0x00,0x7f,0x49,0x49,0x49,0x36,0x00},/*B*/
	{0x00,0x00,0x3e,0x41,0x41,0x41,0x22,0x00},/*C*/
	{0x00,0x00,0x7f,0x41,0x41,0x22,0x1c,0x00},/*D*/
	{0x00,0x00,0x7f,0x49,0x49,0x49,0x49,0x00},/*E*/
	{0x00,0x00,0x7f,0x09,0x09,0x09,0x01,0x00},/*F*/
	{0x00,0x00,0x3e,0x41,0x49,0x49,0x7a,0x00},/*G*/
	{0x00,0x00,0x7f,0x08,0x08,0x08,0x7f,0x00},/*H*/
	{0x00,0x00,0x41,0x7f,0x41,0x00,0x00,0x00},/*I*/
	{0x00,0x00,0x20,0x40,0x41,0x3f,0x01,0x00},/*J*/
	{0x00,0x00,0x7f,0x08,0x14,0x22,0x41,0x00},/*K*/
	{0x00,0x00,0x7f,0x40,0x40,0x40,0x40,0x00},/*L*/
	{0x00,0x00,0x7f,0x02,0x0c,0x02,0x7f,0x00},/*M*/
	{0x00,0x00,0x7f,0x04,0x08,0x10,0x7f,0x00},/*N*/
	{0x00,0x00,0x3e,0x41,0x41,0x41,0x3e,0x00},/*O*/
	{0x00,0x00,0x7f,0x09,0x09,0x09,0x06,0x00},/*P*/
	{0x00,0x00,0x3e,0x41,0x41,0x41,0x3e,0x00},/*Q*/
	{0x00,0x00,0x7f,0x09,0x19,0x29,0x46,0x00},/*R*/
	{0x00,0x00,0x46,0x49,0x49,0x49,0x31,0x00},/*S*/
	{0x00,0x00,0x01,0x01,0x7f,0x01,0x01,0x00},/*T*/
	{0x00,0x00,0x3f,0x40,0x40,0x40,0x3f,0x00},/*U*/
	{0x00,0x00,0x1f,0x20,0x40,0x20,0x1f,0x00},/*V*/
	{0x00,0x00,0x3f,0x40,0x38,0x40,0x3f,0x00},/*W*/
	{0x00,0x00,0x63,0x14,0x08,0x14,0x63,0x00},/*X*/
	{0x00,0x00,0x07,0x08,0x70,0x08,0x07,0x00},/*Y*/
	{0x00,0x00,0x61,0x51,0x49,0x45,0x43,0x00},/*Z*/
	{0x00,0x00,0x20,0x54,0x54,0x54,0x7c,0x00},/*a*/
	{0x00,0x00,0x7f,0x48,0x44,0x44,0x38,0x00},/*b*/
	{0x00,0x00,0x38,0x44,0x44,0x44,0x20,0x00},/*c*/
	{0x00,0x00,0x38,0x44,0x44,0x48,0x7f,0x00},/*d*/
	{0x00,0x00,0x38,0x54,0x54,0x54,0x18,0x00},/*e*/
	{0x00,0x00,0x08,0x7e,0x09,0x01,0x02,0x00},/*f*/
	{0x00,0x00,0x0c,0x52,0x52,0x52,0x3e,0x00},/*g*/
	{0x00,0x00,0x7f,0x08,0x04,0x04,0x78,0x00},/*h*/
	{0x00,0x00,0x00,0x44,0x7d,0x40,0x00,0x00},/*i*/
	{0x00,0x00,0x40,0x40,0x44,0x3d,0x00,0x00},/*j*/
	{0x00,0x00,0x7f,0x10,0x28,0x44,0x00,0x00},/*k*/
	{0x00,0x00,0x00,0x41,0x7e,0x40,0x00,0x00},/*l*/
	{0x00,0x00,0x7c,0x04,0x18,0x04,0x78,0x00},/*m*/
	{0x00,0x00,0x7c,0x08,0x04,0x04,0x78,0x00},/*n*/
	{0x00,0x00,0x38,0x44,0x44,0x44,0x38,0x00},/*o*/
	{0x00,0x00,0x7c,0x14,0x14,0x14,0x08,0x00},/*p*/
	{0x00,0x00,0x08,0x14,0x14,0x14,0x7c,0x00},/*q*/
	{0x00,0x00,0x7c,0x08,0x04,0x04,0x08,0x00},/*r*/
	{0x00,0x00,0x48,0x54,0x54,0x54,0x20,0x00},/*s*/
	{0x00,0x00,0x04,0x3f,0x44,0x44,0x24,0x00},/*t*/
	{0x00,0x00,0x3c,0x40,0x40,0x20,0x7e,0x00},/*u*/
	{0x00,0x00,0x1c,0x20,0x40,0x20,0x1c,0x00},/*v*/
	{0x00,0x00,0x3c,0x40,0x30,0x40,0x3c,0x00},/*w*/
	{0x00,0x00,0x44,0x28,0x10,0x28,0x44,0x00},/*x*/
	{0x00,0x00,0x06,0x48,0x48,0x48,0x3e,0x00},/*y*/
	{0x00,0x00,0x44,0x64,0x54,0x4c,0x44,0x00},/*z*/
	{0x00,0x00,0x00,0x01,0x02,0x04,0x00,0x00},/*`*/
	{0x00,0x00,0x00,0x42,0x7f,0x40,0x40,0x00},/*1*/
	{0x00,0x00,0x62,0x51,0x51,0x49,0x46,0x00},/*2*/
	{0x00,0x00,0x21,0x41,0x45,0x4b,0x31,0x00},/*3*/
	{0x00,0x00,0x18,0x14,0x12,0x7f,0x10,0x00},/*4*/
	{0x00,0x00,0x27,0x45,0x45,0x45,0x39,0x00},/*5*/
	{0x00,0x00,0x3c,0x4a,0x49,0x49,0x30,0x00},/*6*/
	{0x00,0x00,0x01,0x71,0x09,0x05,0x03,0x00},/*7*/
	{0x00,0x00,0x36,0x49,0x49,0x49,0x36,0x00},/*8*/
	{0x00,0x00,0x06,0x49,0x49,0x29,0x1e,0x00},/*9*/
	{0x00,0x00,0x3e,0x51,0x49,0x45,0x3e,0x00},/*0*/
	{0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x00},/*-*/
	{0x00,0x00,0x14,0x14,0x14,0x14,0x14,0x00},/*=*/
	{0x00,0x00,0x01,0x02,0x08,0x10,0x20,0x00},/*\\*/
	{0x00,0x00,0x04,0x08,0x04,0x08,0x04,0x00},/*~*/
	{0x00,0x00,0x00,0x00,0x4f,0x00,0x00,0x00},/*!*/
	{0x00,0x00,0x3e,0x59,0x55,0x79,0x3e,0x00},/*@*/
	{0x00,0x00,0x14,0x7f,0x14,0x7f,0x14,0x00},/*#*/
	{0x00,0x00,0x24,0x2a,0x7f,0x2a,0x12,0x00},/*$*/
	{0x00,0x00,0x23,0x13,0x08,0x62,0x61,0x00},/*%*/
	{0x00,0x00,0x04,0x02,0x01,0x02,0x04,0x00},/*^*/
	{0x00,0x00,0x36,0x49,0x55,0x22,0x50,0x00},/*&*/
	{0x00,0x00,0x14,0x08,0x3e,0x08,0x14,0x00},/***/
	{0x00,0x00,0x00,0x1c,0x22,0x41,0x00,0x00},/*(*/
	{0x00,0x00,0x00,0x41,0x22,0x1c,0x00,0x00},/*)*/
	{0x00,0x00,0x40,0x40,0x40,0x40,0x40,0x00},/*_*/
	{0x00,0x00,0x08,0x08,0x3e,0x08,0x08,0x00},/*+*/
	{0x00,0x00,0x00,0x00,0x7e,0x00,0x00,0x00},/*|*/
	{0x00,0x00,0x00,0x7f,0x41,0x41,0x00,0x00},/*[*/
	{0x00,0x00,0x00,0x41,0x41,0x7f,0x00,0x00},/*]*/
	{0x00,0x00,0x00,0x56,0x36,0x00,0x00,0x00},/*;*/
	{0x00,0x00,0x00,0x00,0x05,0x03,0x00,0x00},/*'*/
	{0x00,0x00,0x00,0x00,0x28,0x18,0x00,0x00},/*,*/
	{0x00,0x00,0x00,0x00,0x60,0x60,0x00,0x00},/*.*/
	{0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x00},/*//*/
						       {0x00,0x00,0x00,0x08,0x36,0x41,0x00,0x00},/*{*/
	{0x00,0x00,0x00,0x41,0x36,0x08,0x00,0x00},/*}*/
	{0x00,0x00,0x00,0x36,0x36,0x00,0x00,0x00},/*:*/
	{0x00,0x00,0x00,0x00,0x07,0x00,0x07,0x00},/*"*/
	{0x00,0x00,0x08,0x14,0x22,0x41,0x00,0x00},/*<*/
	{0x00,0x00,0x00,0x41,0x22,0x14,0x08,0x00},/*>*/
	{0x00,0x00,0x02,0x01,0x51,0x09,0x06,0x00}/*?*/
};
void leddisp(void); //数码管显示函数
void readkey(void); //读键盘函数
void check_ram(void);//检测ram函数
/**********************************
  液晶驱动函数
 ***********************************/
void lcdlon_off(uchar D);
void setlstartx(uchar D);
void setlstarty(uchar D);
void setlstartline(uchar D);
void lcdron_off(uchar D);
void setrstartx(uchar D);
void setrstarty(uchar D);
void setrstartline(uchar D);
void lcdlwrite(uchar D);
void lcdrwrite(uchar D);
uchar GetIndex(uchar szText);
void writelcd(void);
void initial_lcd(void);
void clear_lcd(void);
void full_lcd(void);
/**********************************
  24c16驱动函数
 **********************************/
void delay(uint8 time_nop);
void Start_I2c(void);
void Stop_I2c(void);
uint8 SendByte(uint8 c);
uint8 RcvByte(void);
void Ack_I2c(uint8 a);
uint8 ISendByte(uint8 sla,uint8 c);
uint8 ISendStr(uint8 sla,uint8 suba,uint8 *s,uint8 no);
uint8 IRcvByte(uint8 sla,uint8 *c);
uint8 IRcvStr(uint8 sla,uint8 suba,uint8 *s,uint8 no);
void write_24lc16b(uint16 adrress, uint8 *wdata, uint8 number);
void read_24lc16b(uint16 adrress, uint8 *rdata, uint8 number);void check_24c16(void);//检测24c16函数
//TLC1549驱动函数
unsigned int ad_read(void)
{
	unsigned char i;
	unsigned int adtemp;
	adcs=1; //禁止I/O CLOCK
	adcs=0; //开启控制电路，使能DATA OUT和I/O CLOCK
	adtemp=0; //清转换变量
	for(i=0;i<10;i++) //采集10次 ，即10bit
	{
		adclk=0;
		adtemp*=2;
		if(addata) adtemp++;
		adclk=1;
	}
	adcs=1;
	return(adtemp);
}
//TLC5615驱动函数
void da5615(uint16 da)
{
	uchar i;
	da<<=6;
	CS_DA=0;
	SPI_CLK=0;
	for (i=0;i<12;i++)
	{
		SPI_DATA=(bit)(da&0x8000);
		SPI_CLK=1;
		da<<=1;
		SPI_CLK=0;
	}
	CS_DA=1;
	SPI_CLK=0;
	for (i=0;i<12;i++);
}
void intT0() interrupt 1 //T0 定时中断处理函数
{
	TH0 = -2720/256; //定时器中断时间间隔 4ms
	TL0 = -2720%256;
	leddisp(); //每次定时中断显示更新一次
	if(control_readkey == 1) //每两次定时中断扫描一次键盘
	{
		readkey();
	}
	control_readkey = !control_readkey;
}
void main(void)
{
	TMOD = 0x01;
	TH0 = -2720/256; //定时器中断时间间隔 4ms
	TL0 = -2720%256;
	TCON = 0x10;
	ET0 = 1;
	EA = 1;
	initial_lcd();//初始化液晶
	while(1)
	{
		if(getkey == 1) //判断是否获得有效按键
		{
			getkey = 0;
			switch(keynum) //判断键值，对不同键值采取相应的用户定义处理方式
			{
				case 0x0f:
					if (curr<500)
					{
						curr=curr+5;
						led[7]=curr/100;
						led[6]=(curr%100)/10;
						led[5]=(curr%100)%10;
						da5615(2*curr);
					}
					break;
				case 0x0e:
					if (curr>50)
					{
						curr=curr-5;
						led[7]=curr/100;
						led[6]=(curr%100)/10;
						led[5]=(curr%100)%10;
						da5615(2*curr);
					}
					break;
				case 0x0d: //当按下第二行倒数第三列按键时 ，液晶屏被全部填充为黑
					full_lcd();
					break;
				case 0x0c:
					clear_lcd(); //当按下第二行倒数第四列按键时，液晶屏被全部清空
					break;
				default:
					led[0] = keynum; //当按下其他按键时，全部数码管显示按键值
					led[1] = keynum;
					led[2] = keynum;
					led[3] = keynum;
					led[4] = keynum;
					led[5] = keynum;
					led[6] = keynum;
					led[7] = keynum;
					break;
			}
		}
		led[2]=(int) ((((float) 500)*ad_read()/1023)/100);
		led[1]=(((int) ((((float) 500)*ad_read())/1023))%100)/10;
		led[0]=(((int) ((((float) 500)*ad_read())/1023))%100)%10;
	}
}
/***************************************************
  键盘扫描函数
  原型: void readkey(void);
  功能: 当获得有效按键时,令getkey=1，keynum为按键值
 ****************************************************/
void readkey(void)
{
	uchar M_key = 0;
	first_row = 0;
	second_row = 0;
	M_key = KEY;
	if(M_key != 0xff) //如果有连续两次按键按下，认为有有效按键按下。消除按键抖动
	{
		if(first_getkey == 0)
		{
			first_getkey = 1;
		}
		else //当有有效按键按下时，进一步识别是哪一个按键
		{
			if(keyon == 0) //防止按键冲突，当还有未释放的按键时不对其它按键动作响应
			{
				first_row = 0; //扫描第一行按键
				second_row = 1;
				M_key = KEY;
				if(M_key != 0xff)
				{
					switch(M_key)
					{
						case 0xfe:
							keynum = 0x00;
							break;
						case 0xfd:
							keynum = 0x01;
							break;
						case 0xfb:
							keynum = 0x02;
							break;
						case 0xf7:
							keynum = 0x03;
							break;
						case 0xef:
							keynum = 0x04;
							break;
						case 0xdf:
							keynum = 0x05;
							break;
						case 0xbf:
							keynum = 0x06;
							break;
						case 0x7f:
							keynum = 0x07;
							break;
					}
				}
				else
				{
					second_row = 0; //扫描第二行按键
					first_row = 1;
					M_key = KEY;
					switch(M_key)
					{
						case 0xfe:
							keynum = 0x08;
							break;
						case 0xfd:
							keynum = 0x09;
							break;
						case 0xfb:
							keynum = 0x0a;
							break;
						case 0xf7:
							keynum = 0x0b;
							break;
						case 0xef:
							keynum = 0x0c;
							break;
						case 0xdf:
							keynum = 0x0d;
							break;
						case 0xbf:
							keynum = 0x0e;
							break;
						case 0x7f:
							keynum = 0x0f;
							break;
					}
				}
				getkey = 1; //获得有效按键数值
				keyon = 1; //防止按键冲突，当获得有效按键时将其置1
			}
		}
	}
	else
	{
		first_getkey = 0;
		keyon = 0; //防止按键冲突，当所有的按键都释放时将其清0
	}
}
/***************************************************
  数码管显示函数
  原型: void leddisp(void);
  功能: 每次调用轮流显示一位数码管
 ****************************************************/
void leddisp(void)
{
	switch(lednum) //选择需要显示的数码位
	{
		case 0:
			LED1 = segtab[led[0]];
			break;
		case 1:
			LED2 = segtab[led[1]];
			break;
		case 2:
			LED3 = segtab[led[2]];
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			LED6 = segtab[led[5]];
			break;
		case 6:
			LED7 = segtab[led[6]];
			break;
		case 7:
			LED8 = segtab[led[7]];
			break;
	}
	if(lednum == 0) //更新需要现实的数码位
	{
		lednum = 7;
	}
	else
	{
		lednum = lednum-1;
	}
}
void lcdlon_off(uchar D)
{
	lcdlCommand = D;
	_nop_();
}
void setlstartx(uchar D)
{
	lcdlCommand = D;
	_nop_();
}
void setlstarty(uchar D)
{
	lcdlCommand = D;
	_nop_();
}
void setlstartline(uchar D)
{
	lcdlCommand = D;
	_nop_();
}
void lcdron_off(uchar D)
{
	lcdrCommand = D;
	_nop_();
}
void setrstartx(uchar D)
{
	lcdrCommand = D;
	_nop_();
}
void setrstarty(uchar D)
{
	lcdrCommand = D;
	_nop_();
}
void setrstartline(uchar D)
{
	lcdrCommand = D;
	_nop_();
}
uchar GetIndex(uchar szText)
{
	uchar code TAB[libleng] = {' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q'
		,'R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q'
			,'r','s','t','u','v','w','x','y','z','`','1','2','3','4','5','6','7','8','9','0','-','=','\\','~','!','@','#','$','%','^','&','*','(',')','_'
			,'+','|','[',']',';','\'',',','.','/','{','}',':','"','<','>','?'};
	uchar i;
	uchar j = 0;
	for(i=0;i<=libleng;i++)
	{
		if(szText==TAB[i])
		{
			j = 1;
			return i;
			break;
		}
	}
	if(j==0)
	{
		return 0x00;
	}
}
void lcdlwrite(uchar D)
{
	lcdDl = D;
	_nop_();
}
void lcdrwrite(uchar D)
{
	lcdDr = D;
	_nop_();
}
void writelcd(void) //对LCD进行字符串写入
{
	uchar i = 0;//8*8字符数
	uchar j = 0;//8位数据中第几位
	uchar k = 0;//行数
	uchar index = 0;
	uchar z = 0;
	for(i=0;i<dataleng;i++)
	{
		index = GetIndex(worddata[i]);
		z = i%8;
		k = i/16;
		if(k>7)
		{
			k = 0;
		}
		if(i%16<8)
		{
			setlstartx(strat_xaddr+k);
			for(j=0;j<8;j++)
			{
				_nop_();
				setlstarty(strat_yaddr+8*z+j);
				lcdlwrite(ASCII[index][j]);
			}
			_nop_();
		}
		if(i%16>=8)
		{
			setrstartx(strat_xaddr+k);
			for(j=0;j<8;j++)
			{
				_nop_();
				setrstarty(strat_yaddr+8*z+j);
				lcdrwrite(ASCII[index][j]);
			}
			_nop_();
		}
	}
}
/**********************************************************
  LCD清屏
  原型: void clear_lcd(void);
  功能： 将LCD清屏
 **********************************************************/
void clear_lcd(void)
{
	uchar i,j;
	for(i=0;i<8;i++)
	{
		setlstartx(strat_xaddr + i);
		for(j=0;j<64;j++)
		{
			setlstarty(strat_yaddr+j);
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			lcdlwrite(0x00);
		}
		_nop_();
	}
	for(i=0;i<8;i++)
	{
		setrstartx(strat_xaddr + i);
		for(j=0;j<64;j++)
		{
			setrstarty(strat_yaddr+j);
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			lcdrwrite(0x00);
		}
		_nop_();
	}
}
/**********************************************************
  LCD填充
  原型: void full_lcd(void);
  功能： 将LCD填充
 **********************************************************/
void full_lcd(void)
{
	uchar i,j;
	for(i=0;i<8;i++)
	{
		setlstartx(strat_xaddr + i);
		for(j=0;j<64;j++)
		{
			setlstarty(strat_yaddr+j);
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			lcdlwrite(0xff);
		}
		_nop_();
	}
	for(i=0;i<8;i++)
	{
		setrstartx(strat_xaddr + i);
		for(j=0;j<64;j++)
		{
			setrstarty(strat_yaddr+j);
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			lcdrwrite(0xff);
		}
		_nop_();
	}
}
/************************************************************************************
  LCD初始化
  原型: void initial_lcd(void);
  功能： 将LCD初始化显示"National University of defence technology's ASIC R&D Center"
 ************************************************************************************/
void initial_lcd(void)
{
	setlstartline(strat_dispaddr);//设置显示初始行
	setrstartline(strat_dispaddr);
	lcdlon_off(0x3F);//LCD两屏都打开
	lcdron_off(0x3F);
	writelcd();//写数据
}
/**************************************************************************************
  测试RAM函数
  函数原型: void check_ram(void);
  功能: 向61256的全部存储单元写0x00、0xff并读回，出错单元的数量使用数码管的后五位显示
 ***************************************************************************************/
void check_ram(void)
{
	uint16 ram_count = 0;
	uint16 ram_error = 0;
	uint8 ram_data1 = 0;
	uint8 ram_data2 = 0;
	for(ram_count = 0;ram_count < 32768;ram_count++)
	{
		RAM[ram_count] = 0x00;
		ram_data1 = RAM[ram_count];
		RAM[ram_count] = 0xff;
		ram_data2 = RAM[ram_count];
		if((ram_data1 != 0x00) || (ram_data2 != 0xff))
		{
			ram_error = ram_error + 1;
		}
	}
	if(ram_error != 0)
	{
		for(ram_data1 = 0;ram_data1 < 5;ram_data1++)
		{
			if(ram_error != 0)
			{
				led[ram_data1] = ram_error % 10;
				ram_error = ram_error / 10;
			}
			else
			{
				led[ram_data1] = 0xff;
			}
		}
	}
	else
	{
		led[0] = 0x00;
		led[1] = 0x11;
		led[2] = 0x11;
		led[3] = 0x11;
		led[4] = 0x11;
	}
	led[5] = 0x0e;
	led[6] = 0x11;
	led[7] = 0x11;
}
/*******************************************************************
  延时函数
  函数原型: void delay(uint8 time_nop);
  功能: 延时time_nop个nop
 ********************************************************************/
void delay(uint8 time_nop)
{
	uint8 i;
	for(i=0;i<time_nop;i++)
	{
		_nop_();
	}
}
