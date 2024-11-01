/********************8051和12864液晶接口电路********************

8051            LCD

数据口：
P0.0            DB0
P0.1            DB1     
P0.2			DB2       
P0.3			DB3       
P0.4			DB4     
P0.5			DB5     
P0.6			DB6     
P0.7			DB7
控制字：     
P2.4			CSA      //低电平有效
P2.3			CSB      //低电平有效 
P2.2			RS(DI)	 //H：Data ， L：Instruction
P2.1			RW     	 //H：Read ， L：Write
P2.0			E        //高电平有效 
****************************************************************/


#include<reg51.h>

#define uchar unsigned char 
#define uint unsigned int

sbit cs1=P2^3;
sbit cs2=P2^4;
sbit di=P2^2;
sbit rw=P2^1;
sbit e=P2^0;
sbit d1=P1^0;
sbit d2=P1^1;
sbit d3=P1^2;
sbit d4=P1^3;
sbit d5=P1^4;
sbit d6=P1^5;
sbit buzzer=P3^7;
/*********************全局变量定义********************/
uchar room[6]={0,0,0,0,0,0};
uchar bed[6]={0,0,0,0,0,0};
uchar receivedata;
uchar Btime;
uchar Btime1;
/**********************************字符取模********************************************/
/*--  文字:  房  --*/
/*--  Times New Roman12;  此字体下对应的点阵为：宽x高=16x16   --*/
uchar code fang[32]={0x00,0x00,0xFC,0x94,0x94,0x94,0x94,0x95,0xB6,0xD4,0x94,0x94,0x94,0x9C,0x80,0x00,
							0x40,0x30,0x0F,0x80,0x40,0x30,0x0F,0x04,0x04,0x44,0x84,0x44,0x3C,0x00,0x00,0x00};
/*--  文字:  间  --*/
/*--  Times New Roman12;  此字体下对应的点阵为：宽x高=16x16   --*/
uchar code jian[32]={0x00,0xF8,0x01,0x06,0x00,0xF0,0x92,0x92,0x92,0x92,0xF2,0x02,0x02,0xFE,0x00,0x00,
							0x00,0xFF,0x00,0x00,0x00,0x07,0x04,0x04,0x04,0x04,0x07,0x40,0x80,0x7F,0x00,0x00};
/*--  文字:  床  --*/
/*--  Times New Roman12;  此字体下对应的点阵为：宽x高=16x16   --*/
uchar code chuang[32]={0x00,0x00,0xFC,0x44,0x44,0x44,0x44,0xC5,0xFE,0x44,0x44,0x44,0x44,0x44,0x04,0x00,
							0x80,0x60,0x1F,0x40,0x20,0x18,0x06,0x01,0xFF,0x01,0x02,0x04,0x08,0x30,0x10,0x00};
/*--  文字:  号  --*/
/*--  Times New Roman12;  此字体下对应的点阵为：宽x高=16x16   --*/
uchar code hao[32]={0x40,0x40,0x40,0x5F,0xD1,0x51,0x51,0x51,0x51,0x51,0x51,0x5F,0x40,0x40,0x40,0x00,
							0x00,0x00,0x00,0x02,0x07,0x02,0x02,0x22,0x42,0x82,0x42,0x3E,0x00,0x00,0x00,0x00};
/*--0--9数字--*/
/*--  Times New Roman12;  此字体下对应的点阵为：宽x高=8x16   --*/
uchar code shu[10][16]={{0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00},
								{0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},
								{0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00},
								{0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00},
								{0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00},
								{0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00},
								{0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00},
								{0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00},
								{0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00},
								{0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00}};

/**********************延时函数************************/
void delay(uint num)
{
	while(num--) ;
}
/*************************写LCD函数********************************/
void lw(uchar x,uchar y,uchar dd) 
{
		if (x<64) 
		{
			cs1=cs2=0;
			di=rw=0;cs1=1;cs2=0;P0=0xb8|y;e=1;delay(2);e=0;delay(2);
			di=rw=0;cs1=1;cs2=0;P0=0x40|x;e=1;delay(2);e=0;delay(2);
			di=1;rw=0;cs1=1;cs2=0;P0=dd;e=1;delay(2);e=0;delay(2);	
			
		}
		else 
		{
			cs1=cs2=0;x=x-64;
			di=rw=0;cs1=0;cs2=1;P0=0xb8|y;e=1;delay(2);e=0;delay(2);			
			di=rw=0;cs1=0;cs2=1;P0=0x40|x;e=1;delay(2);e=0;delay(2);
			di=1;rw=0;cs1=0;cs2=1;P0=dd;e=1;delay(2);e=0;delay(2);
   			cs1=cs2=1;
		}
}

/************************LCD初始化*************************/
void lcd_init(void) {
	uchar x,y;	
	e=di=rw=0;
	cs1=cs2=1;
	cs1=0;cs2=1;
	di=rw=0;P0=0x3f;e=1;delay(2);e=0;delay(2);
	cs1=0;cs2=1;
	di=rw=0;P0=0xc0;e=1;delay(2);e=0;delay(2);
	cs1=cs2=0;

	cs1=1;cs2=0;
	di=rw=0;P0=0x3f;e=1;delay(2);e=0;delay(2);
	cs1=1;cs2=0;
	di=rw=0;P0=0xc0;e=1;delay(2);e=0;delay(2);
	cs1=cs2=0;

      for (y=0;y<8;y++) {
		for (x=0;x<128;x++) lw(x,y,0); 
	}
}


/***************写16*16分辨率汉字************/
void write_chinese(x,y,array)
uchar x,y;
uchar array[];
{
	uchar j,dd;			
	for(j=0;j<32;j++)
	{
		dd=array[j];
		if(j<16) lw(x+j,y,dd);
		 else  lw(x+j-16,y+1,dd);
	}	
}
/***************写8*16分辨率字母，数字**********/
void write_english(x,y,array)
uchar x,y;
uchar array[];
{
	uchar j,dd;			
	for(j=0;j<16;j++)
	{
		dd=array[j];
		if(j<8) lw(x+j,y,dd);
		 else  lw(x+j-8,y+1,dd);
	}	
}			

/*******************显示函数****************/
void display0(void)					//显示汉字“房间床号”
{
	 write_chinese(0,0,fang);
	 write_chinese(16,0,jian);
	 write_chinese(32,0,chuang);
	 write_chinese(48,0,hao);
	 write_chinese(64,0,fang);
	 write_chinese(80,0,jian);
	 write_chinese(96,0,chuang);
	 write_chinese(112,0,hao);
}

void display1(void)	 				//显示具体的房间号和床号
{
	 write_english(8,2,shu[room[0]/10]);write_english(16,2,shu[room[0]%10]);write_english(40,2,shu[bed[0]/10]);write_english(48,2,shu[bed[0]%10]);
	 write_english(72,2,shu[room[1]/10]);write_english(80,2,shu[room[1]%10]);write_english(104,2,shu[bed[1]/10]);write_english(112,2,shu[bed[1]%10]);
	 write_english(8,4,shu[room[2]/10]);write_english(16,4,shu[room[2]%10]);write_english(40,4,shu[bed[2]/10]);write_english(48,4,shu[bed[2]%10]);
	 write_english(72,4,shu[room[3]/10]);write_english(80,4,shu[room[3]%10]);write_english(104,4,shu[bed[3]/10]);write_english(112,4,shu[bed[3]%10]);
	 write_english(8,6,shu[room[4]/10]);write_english(16,6,shu[room[4]%10]);write_english(40,6,shu[bed[4]/10]);write_english(48,6,shu[bed[4]%10]);
	 write_english(72,6,shu[room[5]/10]);write_english(80,6,shu[room[5]%10]);write_english(104,6,shu[bed[5]/10]);write_english(112,6,shu[bed[5]%10]);
}

/*******************串口设置********************/
void UART_init()
{
    TMOD=0x21;//设置波特率为9600的定时器1方式和初始
    PCON=0x00;
	SCON=0x50;//设置串行口方式
	IP=0x10;
	TH1=0xfd;//定时器初值
	TL1=0xfd;
	TH0=(65536-8333)/256;
	TL0=(65536-8333)%256;
	TCON=0x40;
	IE=0x92;
	TR0=0;
}

/***************发送回执****************/
void send(void)
{
	SBUF=0x55;
	while(TI==0);
	TI=0;
}
/*****************数据处理保存********************/
void datadeal(void)
{
	uchar roomdata,beddata;
	roomdata=receivedata&0xf0;
	roomdata=roomdata>>4;
	beddata=receivedata&0x0f;
	if(room[0]==0){room[0]=roomdata;}
	else if(room[1]==0){room[1]=roomdata;}
		else if(room[2]==0){room[2]=roomdata;}
			else if(room[3]==0){room[3]=roomdata;}
				else if(room[4]==0){room[4]=roomdata;}
					else if(room[5]==0){room[5]=roomdata;}
	if(bed[0]==0){bed[0]=beddata;}
	else if(bed[1]==0){bed[1]=beddata;}
		else if(bed[2]==0){bed[2]=beddata;}	
			else if(bed[3]==0){bed[3]=beddata;}	
				else if(bed[4]==0){bed[4]=beddata;}	
					else if(bed[5]==0){bed[5]=beddata;}							
}			
/***************接受数据********************/ 
void receive() interrupt 4 using 1
{	 
	while(RI==0);
	RI=0;
	receivedata=SBUF;
	datadeal();
	send();
	TR0=1;
	buzzer=0;
} 
void time0() interrupt 1 using 2
{
	TH0=(65536-8333)/256;
	TL0=(65536-8333)%256;
	Btime1++;
	if(Btime1>100)
	{
		buzzer=!buzzer;
		Btime1=0;
		Btime++;
	}	
}
void key(void)
{	
	uchar i;
	P1=0xff;
	if(P1!=0xff)
	{
		delay(10);
		if(P1!=0xff)
		{
			 i=P1;
			 switch(i)
			 {
			 	case 0xfe:room[0]=0;bed[0]=0;break;
				case 0xfd:room[1]=0;bed[1]=0;break;
				case 0xfb:room[2]=0;bed[2]=0;break;
				case 0xf7:room[3]=0;bed[3]=0;break;
				case 0xef:room[4]=0;bed[4]=0;break;
				case 0xdf:room[5]=0;bed[5]=0;break;
				default: break;
			 }
			 while(P1!=0xff);
		}
	}
}
void main(void)
{
	UART_init();
	lcd_init();
	for(;;)
	{	
		key();
		display0();
		display1();
		if(Btime>4){Btime=0;TR0=0;}
	}
}
