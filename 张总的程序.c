/*********************************************************/
/** LC压控振荡器 **/
/** 作者：张龙飞 **/
/** 时间：2006 **/
/** National University of Defense Technology **/
/** **/
/*********************************************************/
#include <absacc.h>
#include <reg51.h>
#include<intrins.h>
#define uchar unsigned char
#define uint8 unsigned char
#define uint16 unsigned int
#define lint unsigned int
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
/*扫描键盘使用的变量 */
sbit first_row = P1^4; //键盘第一行控制
sbit second_row = P1^3; //键盘第二行控制
bit first_getkey = 0,control_readkey = 0; //读键盘过程中的标志位
bit getkey = 0; //获得有效键值标志位 等于1时代表得到一个有效键值
bit keyon = 0; //防止按键冲突标志位
uchar keynum = 0; //获得的有效按键值寄存器
uint16 x=0;
uint16 t=5;
uint16 s=0;
float n=0.0;
uint16 k=0;
uint16 y=125;
uint16 m=125;
uint16 t0count=0;
uint16 count_fm=0;
float u1=0;
uint16 result=0;
uint16 h=0;
sbit adout=P1^6; //ad输出
sbit adclk=P1^7; //ad时钟
sbit adcs=P1^5; //ad片选
sbit pn=P1^0; //N输出片选
sbit pa=P1^1; //A输出片选
/*数码管显示使用的变量和常量*/
uchar lednum = 0; //数码管显示位控制寄存器
uchar led[8] = {0,0,0,0,0,0,0,0}; //数码管显示内容寄存器
uchar code segtab[11] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff}; //七段码段码表
// "0", "1", "2", "3", "4", "5", "6", "7", "8", "9","black"
/*液晶显示使用的变量*/
uchar code worddata[dataleng] ="instructions: KEY1: BY STEP 10KEY2: BY STEP 5 KEY3: up KEY4: down MADE BY Zhang Longfei ";
uchar code worddata1[dataleng] ="instructions: KEY1: BY STEP 10KEY2: BY STEP 5 KEY3: up KEY4: down step 10 100k ";
uchar code worddata2[dataleng] ="instructions: KEY1: BY STEP 10KEY2: BY STEP 5 KEY3: up KEY4: down step 5 50k ";
uchar code worddata3[dataleng] ="instructions: KEY1: BY STEP 10KEY2: BY STEP 5 KEY3: up KEY4: down up ";
uchar code worddata4[dataleng] ="instructions: KEY1: BY STEP 10KEY2: BY STEP 5 KEY3: up KEY4: down down ";
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
void dac5615(uint16);
void delay(uint16);
uint16 ad1549(void);
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
void appearstep10(void);
void appearstep5(void);
void appearup(void);
void appeardown(void);
void clear_lcd(void);
void full_lcd(void);
intT0() interrupt 1 using 1 //T0 定时中断处理函数
{
	TH0 = 0xFC; //定时器中断时间间隔 1ms
	TL0 = 0x18;
	leddisp(); //每次定时中断显示更新一次 //数码管显示函数
	t0count=t0count+1;
	if(t0count==20)
	{
		count_fm=TH1*256;
		count_fm=TL1+count_fm;
		TH1=0x00;
		TL1=0x00;
		t0count=0;
	}
	if(control_readkey == 1) //每两次定时中断扫描一次键盘
	{
		readkey();
	}
	control_readkey = !control_readkey;
}
intT1() interrupt 3 using 2
{
	TH1=0x00;
	TL1=0x00;
}
void main(void)
{
	TMOD = 0x51; //
	TH0 = 0xFC; //定时器中断时间间隔 1ms
	TL0 = 0x18;
	ET0 = 1;
	EA = 1;
	TH1=0;
	TL1=0;
	t0count=0;
	EA=1;
	ET0=1;
	TR0=1;
	PT0=1;
	ET1=1;
	TR1=1;
	count_fm=0;
	initial_lcd();//初始化液晶
	while(1)
	{
		if(getkey == 1) //判断是否获得有效按键
		{
			getkey = 0;
			switch(keynum) //判断键值，对不同键值采取相应的用户定义处理方式
			{
				case 0x00:
					t=10;
					appearstep10();
					break;
				case 0x08:
					t=5;
					appearstep5();
					break;
				case 0x09:
					if (x>=t)
						x=x-t;
					else
					{
						x=63+x-t;
						if (y>125)
							y=y-1;
						else
							y=187;
					}
					appeardown();
					break;
				case 0x01:
					if (x<63-t)
						x=x+t;
					else
					{
						x=x+t-63;
						if(y<187)
							y=y+1;
						else
							y=125;
					}
					appearup();
					break;
			}
		}
		EA=0;
		P0=y;
		pn=0;
		pn=1;
		pn=0;
		P0=x;
		pa=0;
		pa=1;
		pa=0;
		EA=1;
		s=count_fm/100*126;
		u1=ad1549();
		result=(500*(u1))/(1024);
		h=result;
		led[7]=s/10000;
		s=s%10000;
		led[6]=s/1000;
		s=s%1000;
		led[5]=s/100;
		s=s%100;
		led[4]=s/10;
		led[3]=s%10 ;
		led[2]=h/100;
		h=h%100;
		led[1]=h/10;
		led[0]=h%10;
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
			LED4 = segtab[led[3]];
			break;
		case 4:
			LED5 = segtab[led[4]];
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
void appearstep10(void) //对LCD进行字符串写入
{
	uchar i = 0;//8*8字符数
	uchar j = 0;//8位数据中第几位
	uchar k = 0;//行数
	uchar index = 0;
	uchar z = 0;
	for(i=0;i<dataleng;i++)
	{
		index = GetIndex(worddata1[i]);
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
void appearstep5(void) //对LCD进行字符串写入
{
	uchar i = 0;//8*8字符数
	uchar j = 0;//8位数据中第几位
	uchar k = 0;//行数
	uchar index = 0;
	uchar z = 0;
	for(i=0;i<dataleng;i++)
	{
		index = GetIndex(worddata2[i]);
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
void appearup(void) //对LCD进行字符串写入
{
	uchar i = 0;//8*8字符数
	uchar j = 0;//8位数据中第几位
	uchar k = 0;//行数
	uchar index = 0;
	uchar z = 0;
	for(i=0;i<dataleng;i++)
	{
		index = GetIndex(worddata3[i]);
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
void appeardown(void) //对LCD进行字符串写入
{
	uchar i = 0;//8*8字符数
	uchar j = 0;//8位数据中第几位
	uchar k = 0;//行数
	uchar index = 0;
	uchar z = 0;
	for(i=0;i<dataleng;i++)
	{
		index = GetIndex(worddata4[i]);
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
/*********************************************************************************
  AD转换函数
  原型：uint16 ad1549(void)
  功能： 模/数转换
 ********************************************************************/
uint16 ad1549(void)
{
	uint16 u=0;
	uint16 i;
	adcs=1;
	_nop_();
	adcs=0;
	for(i=0;i<10;i++) //read data
	{
		adclk=0;
		u=(u<<1)|adout;
		adclk=1;
		_nop_();
	}
	delay(2); //delay 25us
	adcs=1;
	return(u); //return ad data
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
