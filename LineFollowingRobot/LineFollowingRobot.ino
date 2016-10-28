#include <PololuWheelEncoders.h>

/* TO DO:
 *  Define pins
 *    - Left photoresistor
 *    - Right photoresistor
 *    - Encoder
 *    - Motor control
 *    - 
 */
#define L_PR        A0  //Left Photoresistor
#define R_PR        A1  //Right PR
#define ENC_A       A2  //Encoder in A
#define ENC_B       A3  //Encoder in B
#define ENC_EMPTY1  A4  //Empty for encoder initializer
#define ENC_EMPTY2  A5  //Empty for encoder initializer
#define L_MOTOR     5   //Left Motor
#define R_MOTOR     6   //Right Motor

/* IMPORT LIBRARIES:
 *  pololu c library (for encoder, controller)
 */

/* DEFINE PINS FOR:
 *  encoder
 *  motor controller
 *  light sensors
 *  speaker
 */

/*Encoder.h - Encoder handling library
Copyright (c) 2009 SMA.  All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

The latest version of this library can always be found at

*/

#include "pins_arduino.h"
/*
* an extension to the interrupt support for arduino.
* add pin change interrupts to the external interrupts, giving a way
* for users to have interrupts drive off of any pin.
* Refer to avr-gcc header files, arduino source and atmega datasheet.
*/

/*
* Theory: all IO pins on Atmega168 are covered by Pin Change Interrupts.
* The PCINT corresponding to the pin must be enabled and masked, and
* an ISR routine provided.  Since PCINTs are per port, not per pin, the ISR
* must use some logic to actually implement a per-pin interrupt service.
*/

/* Pin to interrupt map:
* D0-D7 = PCINT 16-23 = PCIR2 = PD = PCIE2 = pcmsk2
* D8-D13 = PCINT 0-5 = PCIR0 = PB = PCIE0 = pcmsk0
* A0-A5 (D14-D19) = PCINT 8-13 = PCIR1 = PC = PCIE1 = pcmsk1
*/

volatile uint8_t *port_to_pcmask[] = {
 &PCMSK0,
 &PCMSK1,
 &PCMSK2
};

typedef void (*voidFuncPtr)(void);

volatile static voidFuncPtr PCintFunc[24] = { 
 NULL };

volatile static uint8_t PCintLast[3];

/*
* attach an interrupt to a specific pin using pin change interrupts.
* First version only supports CHANGE mode.
*/
void PCattachInterrupt(uint8_t pin, void (*userFunc)(void), int mode) {
 uint8_t bit = digitalPinToBitMask(pin);
 uint8_t port = digitalPinToPort(pin);
 uint8_t slot;
 volatile uint8_t *pcmask;

 if (mode != CHANGE) {
   return;
 }
 // map pin to PCIR register
 if (port == NOT_A_PORT) {
   return;
 } 
 else {
   port -= 2;
   pcmask = port_to_pcmask[port];
 }
 slot = port * 8 + (pin % 8);
 PCintFunc[slot] = userFunc;
 // set the mask
 *pcmask |= bit;
 // enable the interrupt
 PCICR |= 0x01 << port;
}

void PCdetachInterrupt(uint8_t pin) {
 uint8_t bit = digitalPinToBitMask(pin);
 uint8_t port = digitalPinToPort(pin);
 volatile uint8_t *pcmask;

 // map pin to PCIR register
 if (port == NOT_A_PORT) {
   return;
 } 
 else {
   port -= 2;
   pcmask = port_to_pcmask[port];
 }

 // disable the mask.
 *pcmask &= ~bit;
 // if that's the last one, disable the interrupt.
 if (*pcmask == 0) {
   PCICR &= ~(0x01 << port);
 }
}

// common code for isr handler. "port" is the PCINT number.
// there isn't really a good way to back-map ports and masks to pins.
static void PCint(uint8_t port) {
 uint8_t bit;
 uint8_t curr;
 uint8_t mask;
 uint8_t pin;

 // get the pin states for the indicated port.
 curr = *portInputRegister(port+2);
 mask = curr ^ PCintLast[port];
 PCintLast[port] = curr;
 // mask is pins that have changed. screen out non pcint pins.
 if ((mask &= *port_to_pcmask[port]) == 0) {
   return;
 }
 // mask is pcint pins that have changed.
 for (uint8_t i=0; i < 8; i++) {
   bit = 0x01 << i;
   if (bit & mask) {
     pin = port * 8 + i;
     if (PCintFunc[pin] != NULL) {
       PCintFunc[pin]();
     }
   }
 }
}

class Encoder {
private:
 static uint8_t _pA, _pB;
 static int32_t _position;
 static void service(void);
public:
 // public methods
 Encoder(uint8_t, uint8_t);
 void start();
 void stop();
 void write(int32_t);
 int32_t read(void);
};

uint8_t Encoder::_pA = 0;
uint8_t Encoder::_pB = 0;
int32_t Encoder::_position = 0;

//
// Constructor
//
Encoder::Encoder(uint8_t pA, uint8_t pB) {
 // initialisation of class attributes
 _pA = pA;
 _pB = pB;
 _position = 0;

 // initialisation of I/O
 pinMode( _pA, INPUT );
 pinMode( _pB, INPUT );
 digitalWrite( _pA, HIGH );  // pull-up
 digitalWrite( _pB, HIGH );  // pull-up
}

void Encoder::start() {
 PCattachInterrupt( _pA, service, CHANGE );
}

void Encoder::stop() {
PCdetachInterrupt(_pA);
}

void Encoder::write(int32_t position) {
 _position = position;
}

int32_t Encoder::read(void) {
 return _position; 
}

void Encoder::service(void) {
 if ((digitalRead(_pA) == LOW) ^ (digitalRead(_pB) == HIGH)) _position++;
 else _position--;
}

Encoder enc(ENC_A, ENC_B);

void setup() {
  /* TO DO:  
   *  set output pins (controllers, speaker)
   *  set input pins (light sensors, encoder)
   *  initialize encoder
   */

  enc.write(0);
  enc.start();
  
  cli(); // stop interrupts
  
  //set timer0 interrupt at 2kHz for light sensors
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
    
  //set timer1 interrupt at 1Hz for encoder
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  Serial.begin(9600); // set up communication
}

ISR(TIMER0_COMPA_vect){  // Light Sensor Interrupt
  
}

ISR(TIMER1_COMPA_vect){  // Encoder Interrupt
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(enc.read()); // print encoder position for testing purposes.
}



