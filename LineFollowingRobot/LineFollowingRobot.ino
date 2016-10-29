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

#include "encoder.h"

/* IMPORT LIBRARIES:
 *  pololu c library (for encoder, controller)
 */

/* DEFINE PINS FOR:
 *  encoder
 *  motor controller
 *  light sensors
 *  speaker
 */

Encoder enc(ENC_A, ENC_B);

void drive (int left, int right)  {
  analogWrite(L_MOTOR, left);
  analogWrite(R_MOTOR, right);   
}


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
  pinMode(L_MOTOR, OUTPUT);
  pinMode(R_MOTOR, OUTPUT);  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(enc.read()); // print encoder position for testing purposes.
}

