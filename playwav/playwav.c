//play WAV
//  Author: takuya matsubara

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "voice.h"

#ifndef F_CPU
	#define F_CPU 8000000	// CPUクロック周波数[Hz]
#endif
#define FREQ_H 16000	// frequency[Hz]

//top=8000000/16000=500
volatile unsigned char hsync=0;
volatile unsigned char hsyncbak=0;

SIGNAL (TIMER1_OVF_vect){  //
	hsync++;
}

#define CENTER ((F_CPU / FREQ_H)/2)		// TOP値の半分
//
void wave_init(void)
{
	TCNT1 = 0;
	ICR1 = (F_CPU / FREQ_H) - 1;	// TOP値
	TCCR1A = (3<<COM1A0)|(2<<COM1B0)|(2<<WGM10);
	TCCR1B = (3<<WGM12)|(1<<CS10);

	TIMSK1 |= (1<<TOIE1);	// タイマー１割込み
	DDRB |= (1<<2);		/// port out

	OCR1B = CENTER;
}

int main(void)
{
	#define PTRMAX 12000	//wav data size[bytes]
	unsigned int ptr;
	int vol;
	int speed=1;
	int i;

	wave_init();
	sei();
	
	ptr=0;
	while(1)
	{
		vol=(int)pgm_read_byte(&(wavdat[ptr]));
		vol -= 128;
		vol = (vol*19)/10;
		vol += CENTER;
		for(i=0;i<speed;i++){
			while(hsync==hsyncbak){
			}
			hsyncbak=hsync;
		}
		hsyncbak=hsync;
		OCR1B = (unsigned int)vol;	//Output Compare Register B
		ptr++;
		if(ptr>=PTRMAX){
			ptr=0;
			speed++;
			if(speed>3)speed=1;
		}
	}
}
