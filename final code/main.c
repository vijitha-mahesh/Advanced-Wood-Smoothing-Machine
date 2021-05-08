

#define F_CPU 100000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "i2cmaster.h"
#include "i2c_lcd.h"

int TimerOverflow = 0;




char oilValue; // c oil or p oil
int functions; // 2 for Getting Oil , 3 for Price & Volume, 1 for Density
int includePrice,includeVolume;
int defaultPrice = 100;

char numberpressed();
void oilVolume();
void gettingOil();
void select();


void displayMenuA();


#define KEY_PRT 	PORTB
#define KEY_DDR		DDRB
#define KEY_PIN		PINB

unsigned char keypad[4][4] = {{'s','=','0','*'},{'b','9','8','7'},
{'c','6','5','4'},{'d','3','2','1'}};
// start *
// stop  =
// F1    -
// F2    a
// F3    /
// F4    +

unsigned char colloc, rowloc;


// function for keypad
char keyfind()
{
	while(1)
	{
		KEY_DDR = 0xF0;           /* set port direction as input-output */
		KEY_PRT = 0xFF;

		do
		{
			KEY_PRT &= 0x0F;      /* mask PORT for column read only */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F); /* read status of column */
		}while(colloc != 0x0F);
		
		do
		{
			do
			{
				_delay_ms(20);             /* 20ms key debounce time */
				colloc = (KEY_PIN & 0x0F); /* read status of column */
				}while(colloc == 0x0F);        /* check for any key press */
				
				_delay_ms (40);	            /* 20 ms key debounce time */
				colloc = (KEY_PIN & 0x0F);
			}while(colloc == 0x0F);

			/* now check for rows */
			KEY_PRT = 0xEF;            /* check for pressed key in 1st row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 0;
				break;
			}

			KEY_PRT = 0xDF;		/* check for pressed key in 2nd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 1;
				break;
			}
			
			KEY_PRT = 0xBF;		/* check for pressed key in 3rd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 2;
				break;
			}

			KEY_PRT = 0x7F;		/* check for pressed key in 4th row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 3;
				break;
			}
		}

		if(colloc == 0x0E)
		return(keypad[rowloc][0]);
		else if(colloc == 0x0D)
		return(keypad[rowloc][1]);
		else if(colloc == 0x0B)
		return(keypad[rowloc][2]);
		else
		return(keypad[rowloc][3]);
	}


	int main(void)
	{
		DDRB = 0xF0;
	
		
		int16_t count_a = 0;
		char show_a[16];
		char c;
		int p = 0,q;
		char *msg = "Smart Oil supplier";
		
		int l = strlen(msg);
		
		lcd_init(LCD_BACKLIGHT_ON);
		DDRD = 0b11110011;
		
		PORTA=0xF0;
		_delay_ms(50);

		lcd_init(LCD_BACKLIGHT_ON);

		GICR |= 1<<INT0;
		MCUCR |= 1<<ISC00;
		
		sei();
		
		lcd_clear();
		lcd_goto_xy(2,0);
		lcd_puts("  Welcome");
		lcd_goto_xy(-1,2);
		lcd_puts("Group No.04");
		lcd_goto_xy(-4,3);
		lcd_puts("Press A to Start");
		
		while(1){
			
			c = numberpressed();
			
			if(c == '*'){
				displayMenuA();
				break;
			}
			
			
			if((p + 16) < l){
				for(q=0;q<16;q++){
					lcd_goto_xy(q,1);
					lcd_putc(msg[q+p]);
				}
				++p;
				}else{
				p = 0;
			}
			_delay_ms(20);
		}
		
	} // end main function




	void oilVolume(){
		
		char show_a[16];
		int16_t count_a = 0;
		char c;
		char *str = "Press Stop button to Back";
		int l = strlen(str);
		int i = 0,j;
		
		
		while(1){
			
			c = numberpressed();
			
			if(c == 's'){
				
			}
			
			PORTD |= 1<<PIND0;
			_delay_us(50);
			PORTD &= ~(1<<PIND0);
			count_a = pulse/58;
			
			itoa(count_a,show_a,10);
			
			lcd_goto_xy(1,0);
			lcd_puts("oil volume");
			lcd_goto_xy(4,1);
			itoa((36-count_a)*600,show_a,10);//r=14cm
			lcd_puts(show_a);
			lcd_puts(" ");
			lcd_goto_xy(10,1);
			lcd_puts("ml");
			lcd_goto_xy(1,1);
			
			if((i + 16) < l){
				for(j=0;j<16;j++){
					lcd_goto_xy(j-3,3);
					lcd_putc(str[j+i]);
				}
				++i;
				}else{
				i = 0;
			}
		}
	}





	// menu
	void displayMenuA(){
		char c;
		int flag = 1;
		char *msg1 = "Select option and press Start button";
		int l1 = strlen(msg1);
		int i=0,j;
		lcd_clear();
		lcd_goto_xy(1,0);
		lcd_puts("   Select ");
		lcd_goto_xy(1,1);
		lcd_puts(" C Oil ");
		lcd_goto_xy(-3,2);
		lcd_puts(" P Oil");
		lcd_goto_xy(13,1);
		lcd_putc('<');
		
		oilValue = 'C';
		
		while(1){
			c = keyfind();
			if(c == '2'){
				flag = 1;
				oilValue = 'C';
				lcd_goto_xy(13,1);
				lcd_putc('<');
				lcd_goto_xy(9,2);
				lcd_putc(' ');
				}else if(c == '8'){
				flag = 2;
				oilValue = 'P';
				lcd_goto_xy(9,2);
				lcd_putc('<');
				lcd_goto_xy(13,1);
				lcd_putc(' ');
				}else if(c == '*'){
				;
			}
			
			if((i + 16) < l1){
				for(j=0;j<16;j++){
					lcd_goto_xy(j-4,3);
					lcd_putc(msg1[j+i]);
				}
				++i;
				}else{
				i = 0;
			}
			_delay_ms(20);
		}
	}

	

	char numberpressed()
	{
		PORTB = 0b10000000;
		if(PINB & (1<<PB0))
		{
			return 'a';
		}
		if(PINB & (1<<PB1))
		{
			return '3';
		}
		if(PINB & (1<<PB2))
		{
			return '2';
		}
		if(PINB & (1<<PB3))
		{
			return '1';
		}
		
		PORTB = 0b01000000;
		if(PINB & (1<<PB0))
		{
			return 'b';
		}
		if(PINB & (1<<PB1))
		{
			return '6';
		}
		if(PINB & (1<<PB2))
		{
			return '5';
		}
		if(PINB & (1<<PB3))
		{
			return '4';
		}
		PORTB = 0b00100000;
		if(PINB & (1<<PB0))
		{
			return 'c';
		}
		if(PINB & (1<<PB1))
		{
			return '9';
		}
		if(PINB & (1<<PB2))
		{
			return '8';
		}
		if(PINB & (1<<PB3))
		{
			return '7';
		}
		PORTB = 0b00010000;
		if(PINB & (1<<PB0))
		{
			return 'd';
		}
		if(PINB & (1<<PB1))
		{
			return '#';
		}
		if(PINB & (1<<PB2))
		{
			return '0';
		}
		if(PINB & (1<<PB3))
		{
			return '*';
		}
	}




	



