/*	Author: lab
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency){
  static double current_frequency; //keeps track of the currently set current_frequency
  //will only update the registers when the frequency changes, otherwise allows
  //music to play uninterrupted.
  if(frequency != current_frequency){
    if(!frequency) {TCCR3B &= 0xFFFF;} //stops timer/counter
    else{TCCR3B |= 0x03;} //resumes/continues timer/counter

    //prevents OCR3A from overflowing, using prescalar 64
    //0.954 is smallest frequency that will not result in overflowing
    if(frequency < 0.954){OCR3A = 0xFFFF;}

    //prevents OCR3A from underflowing, using prescalar 64
    //31250 is largest frequency that will not result in underflow
    else if (frequency > 31250){ OCR3A = 0x0000;}

    else {OCR3A = (short)(8000000 / (128 * frequency)) - 1;}

    TCNT3 = 0; //resets counter
    current_frequency = frequency; //Updates the current frequency
  }
}

void PWM_on(){
  TCCR3A = (1 << COM3A0);
    //COM3A0: Toggle PB3 on compare match between counter and OCR3A
  TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    //WGM32: When counter (TCNT3) matches OCR3A, reset counter
    //CS31 & CS30: Set a prescalar of 64
  set_PWM(0);
}

void PWM_off(){
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}

unsigned char tmpA, button0, button1, button2;
double notes[8] = {261.63, 293.66, 329.63, 349.232, 392.00, 440.00, 493.88, 523.25};
enum States {INIT, ON, OFF, INC, DEC} State;
unsigned char i = 0;

void tick(){
  switch(State){
    case INIT:
      State = OFF; //off untill on button pressed
      break;
    case OFF:
      if(button0){
        i = 0; //reset
        set_PWM(array[i]);
        State = ON;
      }else{
        State = OFF;
      }
      break;
    case ON:
      if(button0){
        State = OFF;
      }else if(button1){
        State = UP;
      }else if(button2){
        State = DEC;
      }
      break;
    case INC:
      State = ON;
      break;
    case DEC:
      State = ON;
      break;
    default:
      break;
  }

  switch(State){
    case INIT:
      break;
    case OFF:
      set_PWM(0);
      break;
    case INC:
      if(i <= 7){
        i++;
      }
      set_PWM(notes[i]);
      break;
    case DEC:
      if(i >= 0){
        i--;
      }
      set_PWM(notes[i]);
      break;
    default:
      break;
  }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    PWM_on();
    TimerSet(100);
    TimerOn();
    State = INIT;
    /* Insert your solution below */
    while (1) {
      button0 = ~PINA & 0x01;
      button1 = ~PINA & 0x02;
      button2 = ~PINA & 0x04;
      tick();
      while(!TimerFlag){}
      TimerFlag = 0;
    }
    return 1;
}
