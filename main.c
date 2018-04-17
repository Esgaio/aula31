#include "C8051F340.h"

#define LCD P1
#define RS P3_1
#define RW P3_2
#define EN P3_3

/* unsigned char sentence1[]={'A','p','o','s','t','a','r',' ','e','m',' ','T','I'};
unsigned char sentence2[]={'P','A','U','L','O'}; */

unsigned char done[]={'D','O','N','E'};
int idx=0;
char sentence[35];
int counter=0;
int received =0, state =0;

void ConfigUART(void){
    SCON = 0b00010000;
    TMOD = 0b00100000;
    TH1 = 204;
    TL1 = 204;
    TR1 = 1;
    TR0 = 1;

}
void pulseEnable(void){
    unsigned int i;

    EN=1;
    for(i=0;i<10;++i);
    EN=0;
    for(i=0;i<2000;++i); //aprox 2.7 ms

}

void configureLCD(void){
    RS=0;
    RW=0;
    LCD=0b00111000;//N = duas linhas f = 5*7 dots
    pulseEnable();

    LCD=0b00001111;//display on, cursor on, blink on
    pulseEnable();

    LCD=0b00000001;
    pulseEnable();

    LCD=0b00000110;
    pulseEnable();

}

void changeLine(unsigned char line){
    RS=0;
    LCD=0b10000000|line<<6; //rodo 6 vezes
    pulseEnable();

}

void writeSentence(unsigned char *sentence, unsigned char size){
    unsigned char i;
    RS=1;
    RW=0;
    for(i=0; i<size; ++i){
        LCD=sentence[i];
        pulseEnable();
    }

}

 void RSI_Uart0(void)__interrupt(4){
 			 
 	if(TI==1){
 		TI=0;
 		
 	}
 	if(RI==1){
 		RI=0;
 		received = 1;
 			}
 	}



void main(void){
    P2MDOUT = 0x0C; 				 //pullup for LEDs
    XBR1 = 0x40;  
    XBR0 = 0x01;   				//exercises
    OSCICN = 0x83;     				//Internal H-F cock sysclock 12MHz
    PCA0MD &= ~0x40;				 //watchdog configures
    PCA0MD = 0x00;     				//watchdog disabl
    

    ConfigUART();
    configureLCD();
    IE=0b10010010;
    changeLine(0);
   // writeSentence(sentence1,13);

    //changeLine(1);
    //writeSentence(sentence2,6);
    while(1){

	if (received == 1){
		received = 0;
		switch (state){

		case 0:
			sentence[counter]=SBUF;

			if(sentence[counter]== '#'){
			counter++;
			state =1;
			}
		break;

		case 1:
			sentence[counter]=SBUF;
			if(sentence[counter]=='!'){
			writeSentence(sentence, counter);
			state = 2;
			}
			if(counter==16){
			changeLine(1);
			}
			counter++;
			break;

		 case 2:
			if(idx<4){
			SBUF = done[idx];
			idx++;
			}
			break;

			
			
		


		}
	}



    }
}

