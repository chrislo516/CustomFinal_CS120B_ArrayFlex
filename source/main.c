/*      Author: Chris
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 */
#include <avr/io.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include <avr/interrupt.h>
#include <../header/io.h>    // Library is from UCR-IEEE
#include <../header/timer.h> // Library is from UCR-IEEE
#include <../header/io.c>    // Library is from UCR-IEEE
#include <../header/bit.h>   // Library is from UCR-IEEE
#include <../header/scheduler.h>  // Library is from UCR-IEEE
#include <../header/nokia5110.h> //Library is from https://github.com/LittleBuster/avr-nokia5110
#include <../header/nokia5110.c> //Library is from https://github.com/LittleBuster/avr-nokia5110
#include <../header/usart_ATmega1284.h> // Library is from UCR-IEEE
#include <../arrowFlex.h>
/*
#define E5 659.26
#define D_5 622.25
#define B4 493.88
#define D5 587.33
#define C5 523.25
#define A4 440.00
#define C4 261.63
#define E4 329.63
#define G_4 415.30
#define buttom (~PINA&0x01)
void set_PWM(double frequency);
void PWM_off();
void PWM_on();
#define TRUE = 0x01;
#define FALSE = 0x00;

void set_PWM(double frequency){
        static double current_frequency;
        if (frequency != current_frequency){
          if (!frequency) { TCCR3B &= 0x08;}
          else { TCCR3B |= 0x03;}
          if(frequency<0.954){ OCR3A=0xFFFF;}
          else if (frequency>31250){ OCR3A=0x0000;}
          else { OCR3A = (short)(8000000/(128*frequency))-1;}
          TCNT3 = 0;
          current_frequency = frequency;
        }
}

void PWM_on(){
        TCCR3A = (1<<COM3A0);
        TCCR3B = (1<<WGM32) | (1<<CS31) | (1<<CS30);
        set_PWM(0);
}

void PWM_off(){
        TCCR3A = 0x00;
        TCCR3B = 0x00;
}

*/
unsigned char GetKeypadKey() {
       PORTA = 0xEF;
        asm("nop");
        if (GetBit(PINA,0)==0) { return('1'); }
        if (GetBit(PINA,1)==0) { return('4'); }
        if (GetBit(PINA,2)==0) { return('7'); }
        if (GetBit(PINA,3)==0) { return('*'); }
       PORTA = 0xDF;
        asm("nop");
        if (GetBit(PINA,0)==0) { return('2'); }
        if (GetBit(PINA,1)==0) { return('5'); }
        if (GetBit(PINA,2)==0) { return('8'); }
        if (GetBit(PINA,3)==0) { return('0'); }
       PORTA = 0xBF;
        asm("nop");
        if (GetBit(PINA,0)==0) { return('3'); }
        if (GetBit(PINA,1)==0) { return('6'); }
        if (GetBit(PINA,2)==0) { return('9'); }
        if (GetBit(PINA,3)==0) { return('#'); }
        PORTA = 0x7F;
        asm("nop");
        if (GetBit(PINA,0)==0) { return('A'); }
        if (GetBit(PINA,1)==0) { return('B'); }
        if (GetBit(PINA,2)==0) { return('C'); }
        if (GetBit(PINA,3)==0) { return('D'); }

        return('\0'); // default value
}

//double notes[] = {E5, D_5, E5, D_5, E5, B4, D5, C5, A4, C4, E4, A4, B4, E4, G_4, B4, C5 ,0};
//unsigned short soundPlay[] = {200, 200, 200, 200, 200, 300, 300, 200, 200, 200, 250, 200, 200, 250 , 250 , 200 , 300, 400};
//unsigned char rest[] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 ,10 ,10};
const unsigned char MAX = 18;


enum LCD {start, checkPress, message, btnPress, waitRelease, waitRestart};
enum NOKIA {Nokstart, Nokmessage};
//enum Sound {START, PROCESS, SOUND, DEC, WAIT, DONE};
enum DString {startDisplay, play};
enum Play {playstart, playArrow};

unsigned long timePeriod = 50;
int temp = 0;
unsigned char startPlay = 0;
unsigned char gameStart, restart = 0;
unsigned char gameTime,check,x,k,score,ii,yy = 0;
unsigned char genArrow[4];
const char *countDown[5] = {"5","4","3","2","1"};

task makeTask(int state, unsigned long period, int(*Tick)(int)){
    task curr;
    curr.state = state;
    curr.period = period;
    curr.elapsedTime = 0;
    curr.TickFct = Tick;
    return curr;
}

task tasks[4];
int displayString(int state);
int Sound_Tick(int state);
int Nok_Tick(int state);
int LCD_Tick(int state);

int Nok_Tick(int state){
	switch(state){
		case Nokstart:
		  if(startPlay){
		  state = Nokmessage;
		  }else
		  state = Nokstart;
		  break;
 
		case Nokmessage:
		  if(!restart)
		  state = start;
		  else 
		  state = Nokstart;
		  break;

		default:
		  state = start;
		  x = 0;
		break;
	}
	
	switch(state){
            case start:
                break;

	    case Nokmessage:
		
	        if(ii<=5){
		 LCD_ClearScreen();
		 nokia_lcd_clear();
		 nokia_lcd_set_cursor(34, 15);
		 nokia_lcd_write_string(countDown[ii],3);
		 ii++;
		 nokia_lcd_render();
		 }else if(ii==6){
		 nokia_lcd_set_cursor(18, 15);
                 nokia_lcd_write_string("START",2);
		 ii++;
		 nokia_lcd_render();
		 }else{
		  gameStart = 1;
		 }
		 if(restart){
		  restart = 0;
		  ii = 0;
		 }
		break;


	   default:
	    break;
	}
	return state;
}


int LCD_Tick(int state){
	switch(state){
		case start:
 		  state = checkPress;
   	        break;

		case checkPress:
		 if( x == '\0'){
                  state = checkPress;
                 }else if( x != '\0' ){
                  state = message;
                  startPlay = 1;
                 }
		break;
 
		case message:
		 if(GetKeypadKey() != '\0')
		   state = btnPress;
		 else
		   state = checkPress;
		 if(restart){
		   restart = 0;
		   state = start;
		 }
		break;
		
		case btnPress:
		 if(GetKeypadKey() != '\0')
                   state = btnPress;
                 else
                   state = waitRelease;
		break;
	
		case waitRelease:
		  if(GetKeypadKey() == '\0')
		   state = checkPress;
		  else{
		   state = btnPress;
		  }
		  break;
		 
		default:
		  state = start;
		  x = 0;
		break;
	}
	
	switch(state){
            case start:
	    break;

	    case checkPress:
               x = GetKeypadKey();
            break;

	    case message:
	     if(gameTime <= 30){
  	        if (x == '4') //left
                {
                   if(genArrow[check]==0x22){
			nokia_lcd_set_cursor(6+(check*21), 25);
		        nokia_lcd_write_arrow(genArrow[check],1,1);
		        nokia_lcd_render();
			score += 1;
		   }else{
			nokia_lcd_set_cursor(6+(check*21), 25);
                        nokia_lcd_write_arrow(genArrow[check],1,0);
                        nokia_lcd_render();
		   }
		   if(check<=4)
		   check++;
		   else 
		   check = 0;
		}
                else if (x == '5') //Down
                {
		  if(genArrow[check]==0x20){
                        nokia_lcd_set_cursor(6+(check*21), 25);
                        nokia_lcd_write_arrow(genArrow[check],1,1);
                        nokia_lcd_render();
			score += 1;
                  }else{
                        nokia_lcd_set_cursor(6+(check*21), 25);
                        nokia_lcd_write_arrow(genArrow[check],1,0);
                        nokia_lcd_render();
                  }
		   if(check<=4)
                   check++;
                   else 
                   check = 0;
                }
                else if (x == '6') //right
                {
		 if(genArrow[check]==0x21){
                        nokia_lcd_set_cursor(6+(check*21), 25);
                        nokia_lcd_write_arrow(genArrow[check],1,1);
                        nokia_lcd_render();
			score += 1;
                 }else{
                        nokia_lcd_set_cursor(6+(check*21), 25);
                        nokia_lcd_write_arrow(genArrow[check],1,0);
                        nokia_lcd_render();
                  }
		   if(check<=4)
                   check++;
                   else 
                   check = 0;
	        }
	        LCD_DisplayString( 1 , "Score: ");
                LCD_Cursor(8);
		if(score<=9)
                LCD_WriteData(score+'0');
		else if (score<=99){
		LCD_Cursor(8);
		LCD_WriteData(score/10+'0');
		LCD_Cursor(9);
		LCD_WriteData(score%10+'0');
		}
		gameTime++;
	}else{
		LCD_DisplayString( 17 , "Game Over");
		if(GetKeypadKey()=='2'){
                restart = 1;
                gameTime = 0;
                startPlay = 0;
                gameStart = 0;
                score = 0;
                LCD_ClearScreen();
               }
	}  
	break;
	
       default:
	    break;
	}
	return state;
}

int displayString(int state){
  switch(state){
	case startDisplay:
	  if(gameStart){
	   nokia_lcd_clear();	      
	   state = play;
	  }else
	   state = startDisplay;
	break;
	
	case play:
	  if(gameStart)
	   state = play;
	  else 
	   state = startDisplay;
	break;

	default: break;	

  }
  switch(state){ 
	case startDisplay: 
    break;

	case play:
    if(gameTime <= 30){
    if(yy==0){ 
    for(int i=0 ; i<4 ; i++){
        temp = printRandoms(20,22);
	if(temp == 20)
        genArrow[i] = 0x20;
	else if(temp == 21)
	genArrow[i] = 0x21;
	else
	genArrow[i] = 0x22;
    }
    for(int i = 0 ; i < 4 ; i++){
        nokia_lcd_set_cursor(6+(i*21), 37);
        nokia_lcd_write_arrow(genArrow[i],1,1);
        nokia_lcd_render();
    }
    }
    if(yy<=5){
    	nokia_lcd_set_cursor(0,0);
        nokia_lcd_write_char((0x35)-yy,2);
        nokia_lcd_render();
	yy++;
    }else{
	yy = 0;	
	check = 0;
	nokia_lcd_clear();
    }
    }else{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(15,13);
        nokia_lcd_write_string("Press Enter",1);
	nokia_lcd_set_cursor(39,22);
	nokia_lcd_write_string("to",1);
	nokia_lcd_set_cursor(26,32);
        nokia_lcd_write_string("Restart",1);
	nokia_lcd_render();
	gameStart = 0;
    }
    
	break;          

        default: break;
  
  }
  return state;
}

int printRandoms( int lower, int  upper)
{
        int num = (rand() % (upper - lower + 1)) + lower;
        return num;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0xF0; PORTA = 0x0F;
    DDRC = 0xFF; PORTC = 0x00; //LCD data lines
    DDRD = 0xFF; PORTD = 0x00; //LCD control lines
    unsigned char p = 0;
     
    tasks[p++] = makeTask(start,25,&LCD_Tick); 
    tasks[p++] = makeTask(Nokstart,1000,&Nok_Tick);
    tasks[p] = makeTask(startDisplay,1000,&displayString);

    TimerSet(25);
    TimerOn();
    LCD_init();
    nokia_lcd_init();
    nokia_lcd_clear();
    
    N5110_image(arrowFlex);

    LCD_DisplayString( 1 , "Welcome!!");
    LCD_DisplayString( 17 , "Click To Start!");
  /* Insert your solution below */
    while (1) {
	 for (unsigned char i = 0; i < 3; i++) {
           if (tasks[i].elapsedTime >= tasks[i].period) {
              tasks[i].state = tasks[i].TickFct(tasks[i].state);
              tasks[i].elapsedTime = 0;
           }
           tasks[i].elapsedTime += timePeriod;
        }
	while (!TimerFlag);
        TimerFlag = 0; 
    } 
    return 0;
}
