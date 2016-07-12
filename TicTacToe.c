#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

#define g1 PORTA
#define g2 PORTA
#define g3 PORTA
#define g4 PORTA
#define g5 PORTA
#define g6 PORTA
#define g7 PORTA
#define g8 PORTA
#define g9 PORTC
#define gr1 PA0
#define gr2 PA1
#define gr3 PA2
#define gr4 PA3
#define gr5 PA4
#define gr6 PA5
#define gr7 PA6
#define gr8 PA7
#define gr9 PC0
#define r1 PORTB
#define r2 PORTB
#define r3 PORTB
#define r4 PORTB
#define r5 PORTB
#define r6 PORTB
#define r7 PORTB
#define r8 PORTB
#define r9 PORTD
#define re1 PB0
#define re2 PB1
#define re3 PB2
#define re4 PB3
#define re5 PB4
#define re6 PB5
#define re7 PB6
#define re8 PB7
#define re9 PD7

void ProcessPressedButton(int ButtonPressed);
void ProcessReleasedButton(int ButtonReleased);
void StraightButton();
void ReverseButton();
void SelectButton();
void ResetButton();
void LEDon();
void ResetCurr();
void CheckGame();
void WinBlink(int a,char c,int b);

const uint8_t flowLevel[] ={0b00000001,0b01101011,0b00010010,0b01000010,0b01101000,0b01000100,0b00000100,0b01100011,0b00000000,0b01000000};//0b00000000,0b00000000/*0b11111010,0b10100100,0b10100001,0b11110111,0b11101111,0b11011111,0b10111111,0b01111111*/};
int Pressed_Confidence_Level[4] = {0,0,0,0};
int Released_Confidence_Level[4]= {0,0,0,0};
int Pressed[4] = {0,0,0,0};
volatile int gon[9]={0,0,0,0,0,0,0,0,0};
volatile int ron[9]={0,0,0,0,0,0,0,0,0};

char ch='g';
int f=0,m=0;
volatile int n=8;
volatile int curr=0;
volatile int pcurr=0;
volatile int cnt=0;

void main()
{
	sei();

	TCCR1B |= 1<<CS12;
	OCR1A = 3125;
	TIMSK |= 1 << OCIE1A;
	
	DDRA =  0b11111111;
	DDRB =  0b11111111;
	DDRD =  0b11111111;
	DDRC = 0b00000101;
	
	PORTB=0b00000000;
	PORTA=0b00000000;
	PORTC=0b00000000;
	PORTD=0b11111111;
	PORTD=flowLevel[9];
	
	while(1)
	{
		if (bit_is_clear(PINC , 5))    //reset
		ProcessPressedButton(3);
		else
		ProcessReleasedButton(3);
		
		
		if(f==0)
		{
			LEDon();
			
			
			if (bit_is_clear(PINC , 7))    //up- down button
			
			ProcessPressedButton(0);
			
			else
			ProcessReleasedButton(0);
			
			
			if (bit_is_clear(PINC , 6))    //select
			
			ProcessPressedButton(1);
			
			else
			ProcessReleasedButton(1);
			
			
			if (bit_is_clear(PINC , 4))    //across
			ProcessPressedButton(2);
			else
			ProcessReleasedButton(2);
			
		}
	}
}

void ProcessPressedButton(int ButtonPressed)
{
	Released_Confidence_Level[ButtonPressed] =0;
	Pressed_Confidence_Level[ButtonPressed]++;
	if(Pressed_Confidence_Level[ButtonPressed]>300)
	{
		if(Pressed[ButtonPressed] == 0)
		{
			Pressed[ButtonPressed] = 1;
			if(Pressed[ButtonPressed] == 1)
			{
				if(ButtonPressed == 0)
				StraightButton();
				else if(ButtonPressed == 1)
				ReverseButton();
				else if(ButtonPressed == 2)
				SelectButton();
				else if(ButtonPressed == 3)
				ResetButton();
			}
		}
		Pressed_Confidence_Level[ButtonPressed] =0;
	}
}

void ProcessReleasedButton(int ButtonReleased)
{
	Pressed_Confidence_Level[ButtonReleased] = 0;
	Released_Confidence_Level[ButtonReleased]++;
	if(Released_Confidence_Level[ButtonReleased] >300 )
	{
		Pressed[ButtonReleased] =0;
		Released_Confidence_Level[ButtonReleased] =0;
	}
}

void LEDon()
{ int i;
	for(i=0;i<=8;i++)
	{
		if(gon[i]==1 && i!=8)
		PORTA |=1<<i;
		else
		PORTA &=~(1<<i);
		
		if(gon[i]==1 && i==8)
		PORTC |=1<<PC0;
		else
		PORTC &=~(1<<PC0);
		
		if(ron[i]==1 && i!=8)
		PORTB |=1<<i;
		else
		PORTB &=~(1<<i);
		
		if(ron[i]==1 && i==8)
		PORTD |=1<<PD7;
		else
		PORTD &=~(1<<PD7);
		
		
	}
}


void StraightButton()      //function to move to the next position
{    	int i=curr;
	while(1)
	{
		if(i!=8)
		i++;
		else
		i=0;
		if(gon[i]==0 && ron[i]==0)
		{
			curr=i;
			break;
		}
		
	}
	if(ch=='g')
	gon[pcurr] =0;
	if(ch=='r')
	ron[pcurr] =0;
}

void ReverseButton()     //function to move to the previous position
{
	int i=curr;
	while(1)
	{
		if(i!=0)
		i--;
		else
		i=8;
		
		if(gon[i]==0 && ron[i]==0)
		{
			curr=i;
			break;
		}
		
		
	}
	
	if(ch=='g')
	gon[pcurr] =0;
	if(ch=='r')
	ron[pcurr] =0;
}

void ResetCurr()
{   int i;

	for(i=0;i<=8;i++)
	{
		if(gon[i]==0 && ron[i]==0)
		{
			curr=i;
			break;
		}
	}

	if(curr==pcurr && f==0 )
	{
		_delay_ms(4000);
		g9&=~(1<<gr9);
		r9&=~(1<<re9);
		_delay_ms(300);
		g8&=~(1<<gr8);
		r8&=~(1<<re8);
		_delay_ms(300);
		g7&=~(1<<gr7);
		r7&=~(1<<re7);
		_delay_ms(300);
		g6&=~(1<<gr6);
		r6&=~(1<<re6);
		_delay_ms(300);
		g5&=~(1<<gr5);
		r5&=~(1<<re5);
		_delay_ms(300);
		g4&=~(1<<gr4);
		r4&=~(1<<re4);
		_delay_ms(300);
		g3&=~(1<<gr3);
		r3&=~(1<<re3);
		_delay_ms(300);
		g2&=~(1<<gr2);
		r2&=~(1<<re2);
		_delay_ms(300);
		g1&=~(1<<gr1);
		r1&=~(1<<re1);
		_delay_ms(300);
		ch='g';
		ResetButton();

		ch='g';
		
	}
}

void SelectButton()
{
	

	if(ch=='g')
	gon[curr]=1;
	else if(ch=='r')
	ron[curr]=1;
	
	CheckGame();
	ResetCurr();
	n=8;
	PORTD &= ~(0x7F);
	PORTD|=flowLevel[9];
	cnt=0;
	
	if(ch=='r')
	ch='g';
	else
	ch='r';
	
}

void ResetButton()
{  int i;
	for(i=0;i<=8;i++)
	{
		gon[i]=0;
		ron[i]=0;
		
	}
	curr=0;
	pcurr=0;
	ch='g';
	n=8;
	cnt=0;
	ch='g';
	f=0;
	PORTD &= ~(0x7F);
	PORTD|=flowLevel[9];
}
ISR(TIMER1_COMPA_vect)
{

	pcurr=curr;
	TCNT1=0;
	
	if(ch=='g')
	gon[curr] =!(gon[curr]);
	if(ch=='r')
	ron[curr] =!(ron[curr]);
	cnt++;
	if(cnt==20 && f==0)
	{
		PORTD &= ~(0x7F);
		PORTD|=flowLevel[n];
		cnt=0;
		n--;
		
		if(n==-1)
		SelectButton();
	}
	else if(f==1)
	{//PORTD|=1<<PD0;
		PORTD &= ~(0x7F);
		PORTD |= 0b00000001;
	}
	
	
}

void CheckGame()
{
	int i;
	if(gon[0]==1 && gon[4]==1 && gon[8]==1 && ron[0]==0 && ron[4]==0 && ron[8]==0)
	{	WinBlink(3,'g',0);
	return;}
	else if(ron[0]==1 && ron[4]==1 && ron[8]==1)
	{	WinBlink(3,'r',0);
	return;}
	else if(ron[2]==1 && ron[4]==1 && ron[6]==1)
	{	WinBlink(4,'r',1);
	return;}
	else if(gon[2]==1 && gon[4]==1 && gon[6]==1)
	{	WinBlink(4,'g',1);
	return;}
	else
	{
		
		for(i=0;i<=6;i+=3)
		{
			if(gon[i]==1 && gon[i+1]==1 && gon[i+2]==1 && ron[i]==0 && ron[i+1]==0 && ron[i+2]==0)
			{
				WinBlink(1,'g',i);
				return;
			}
			else if(ron[i]==1 && ron[i+1]==1 && ron[i+2]==1 && gon[i]==0 && gon[i+1]==0 && gon[i+2]==0)
			
			{
				WinBlink(1,'r',i);
				return;
			}
		}
		
		for(i=0;i<=2;i++)
		{
			if(gon[i]==1 && gon[i+3]==1 && gon[i+6]==1 && ron[i]==0 && ron[i+3]==0 && ron[i+6]==0)
			{
				WinBlink(2,'g',i);
				return;
			}
			else if(ron[i]==1 && ron[i+3]==1 && ron[i+6]==1 && gon[i]==0 && gon[i+3]==0 && gon[i+6]==0)
			{
				WinBlink(2,'r',i);
				return;
			}
			
		}
	}
	

}

void WinBlink(int a,char c,int b)
{
	f=1;
	m=0;
	
	int i;
	
	for(i=1;i<=8;i++)
	{
		if(c=='g')
		{
			switch(a)
			{
				case 1:
				if(b==0)
				{
					if(m==0)
					{
						g1|=1<<gr1;
						g2|=1<<gr2;
						g3|=1<<gr3;
					}
					m++;
					g1^=1<<gr1;
					g2^=1<<gr2;
					g3^=1<<gr3;
				}
				
				else if(b==3)
				{
					if(m==0)
					{
						g4|=1<<gr4;
						g5|=1<<gr5;
						g6|=1<<gr6;
					}
					m++;
					g4^=1<<gr4;
					g5^=1<<gr5;
					g6^=1<<gr6;
				}
				else if(b==6)
				{
					if(m==0)
					{
						g7|=1<<gr7;
						g8|=1<<gr8;
						g9|=1<<gr9;
					}
					m++;
					g7^=1<<gr7;
					g8^=1<<gr8;
					g9^=1<<gr9;
				}
				
				break;
				
				case 2:
				if(b==0)
				{
					if(m==0)
					{
						g1|=1<<gr1;
						g4|=1<<gr4;
						g7|=1<<gr7;
					}
					m++;
					g1^=1<<gr1;
					g4^=1<<gr4;
					g7^=1<<gr7;
				}
				else if(b==1)
				
				{
					if(m==0)
					{
						g2|=1<<gr2;
						g5|=1<<gr5;
						g8|=1<<gr8;
					}
					m++;
					g2^=1<<gr2;
					g5^=1<<gr5;
					g8^=1<<gr8;
				}
				else if(b==2)
				{  if(m==0)
					{
						g3|=1<<gr3;
						g6|=1<<gr6;
						g9|=1<<gr9;
					}
					m++;
					g3^=1<<gr3;
					g6^=1<<gr6;
					g9^=1<<gr9;
				}
				break;
				case 3:
				if(m==0)
				{
					g1|=1<<gr1;
					g5|=1<<gr5;
					g9|=1<<gr9;
				}
				m++;
				g1^=1<<gr1;
				g5^=1<<gr5;
				g9^=1<<gr9;
				
				break;
				case 4:
				if(m==0)
				{
					g3|=1<<gr3;
					g5|=1<<gr5;
					g7|=1<<gr7;
				}
				m++;
				g3^=1<<gr3;
				g5^=1<<gr5;
				g7^=1<<gr7;
				
				break;
			}
			
		}
		else
		{
			switch(a)
			{
				case 1: if(b==0)
				{
					if(m==0)
					{
						r1|=1<<re1;
						r2|=1<<re2;
						r3|=1<<re3;
					}
					m++;
					r1^=1<<re1;
					r2^=1<<re2;
					r3^=1<<re3;
				}
				else	if(b==3)
				{
					if(m==0)
					{
						r4|=1<<re4;
						r5|=1<<re5;
						r6|=1<<re6;
					}
					m++;
					r4^=1<<re4;
					r5^=1<<re5;
					r6^=1<<re6;
				}
				else	if(b==6)
				{    if(m==0)
					{
						r7|=1<<re7;
						r8|=1<<re8;
						r9|=1<<re9;
					}
					m++;
					r7^=1<<re7;
					r8^=1<<re8;
					r9^=1<<re9;
				}
				
				break;
				case 2: if(b==0)
				{ if(m==0)
					{
						r1|=1<<re1;
						r4|=1<<re4;
						r7|=1<<re7;
					}
					m++;
					r1^=1<<re1;
					r4^=1<<re4;
				r7^=1<<re7;}
				else	if(b==1)
				{
					if(m==0)
					{
						r2|=1<<re2;
						r5|=1<<re5;
						r8|=1<<re8;
					}
					m++;
					r2^=1<<re2;
					r5^=1<<re5;
				r8^=1<<re8;}
				else	if(b==2)
				{   if(m==0)
					{
						r3|=1<<re3;
						r6|=1<<re6;
						r9|=1<<re9;
					}
					m++;
					r3^=1<<re3;
					r6^=1<<re6;
				r9^=1<<re9;}
				break;
				case 3:
				if(m==0)
				{
					r1|=1<<re1;
					r5|=1<<re5;
					r9|=1<<re9;
				}
				m++;
				r1^=1<<re1;
				r5^=1<<re5;
				r9^=1<<re9;
				
				break;
				case 4:
				if(m==0)
				{
					r3|=1<<re3;
					r5|=1<<re5;
					r7|=1<<re7;
				}
				m++;
				r3^=1<<re3;
				r5^=1<<re5;
				r7^=1<<re7;
				
				break;
			}
			
		}
		_delay_ms(500);
	}
}


