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

/* IMPORT LIBRARIES:
 *  pololu c library (for encoder, controller)
 */

/* DEFINE PINS FOR:
 *  encoder
 *  motor controller
 *  light sensors
 *  speaker
 */
PololuWheelEncoders enc;

void setup() {
  /* TO DO:  
   *  set output pins (controllers, speaker)
   *  set input pins (light sensors, encoder)
   *  initialize encoder
   */


  cli(); // stop interrupts
  
  //set timer0 interrupt at 8kHz for light sensors
    TCCR0A = 0; // reset register
    TCCR0B = 0;
    TCNT0  = 0;//initialize counter value to 0
    // set compare match register for 8khz increments
    OCR0A = 249;
    // turn on CTC mode
    TCCR0A |= (1 << WGM01);
    // Set CS21 8 prescaler
    TCCR0B |= (1 << CS01); 
    // enable timer compare interrupt
    TIMSK0 |= (1 << OCIE0A);
    
  // set timer1 interrupt at 1Hz for encoder
    TCCR1A = 0;// set entire register to 0
    TCCR1B = 0;
    TCNT1  = 0;//initialize counter value to 0
    // set compare match register for 1hz increments
    OCR1A = 15624;
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler
    TCCR1B |= (1 << CS12) | (1 << CS10);  
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
  // allow interrupts
  sei();
  
  enc.init(ENC_A, ENC_B, ENC_EMPTY1, ENC_EMPTY2); // initialize encoders
  Serial.begin(9600); // set up communication
}

ISR(TIMER0_COMPA_vect){  // Light Sensor Interrupt
  
}

ISR(TIMER1_COMPA_vect){  // Encoder Interrupt
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
