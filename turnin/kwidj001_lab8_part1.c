/*	Author: lab
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
Demo video: Hardware Issues unable to test on breadboard.
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

unsigned char tmpA;
unsigned char button0, button1, button2;
int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    unsigned double cNote = 261.63, dNote = 293.66, eNote = 329.63;
    PWM_on();
    TimerSet(50);
    TimerOn();
    /* Insert your solution below */
    while (1) {
      tmpA = ~PINA;
      button0 = ~PINA & 0x01;
      button1 = ~PINA & 0x02;
      button2 = ~PINA & 0x04;

      if(tmpA = 0x00){ //no button
        set_PWM(0); //turn off
      }else if(button0){
        set_PWM(cNote);
      }else if(button1){
        set_PWM(dNote);
      }else if(button2){
        set_PWM(eNote);
      }

      while(!TimerFlag){}
      TimerFlag = 0;
    }
    return 1;
}
