#include "avr/pgmspace.h"

// table of 256 sine values / one sine period / stored in flash memory
PROGMEM  const unsigned char sine256[]  = {
    127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,233,234,236,238,239,240,
    242,243,244,245,247,248,249,249,250,251,252,252,253,253,253,254,254,254,254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,244,243,242,240,239,238,236,234,233,231,229,227,225,223,
    221,219,217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178,176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130,127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81,78,
    76,73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,23,21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,27,29,31,
    33,35,37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124    
};
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define C_PR_VAL 660
#define G_PR_VAL 750

#define  R    0
#define  C3   130.813
#define  CS3  123.471
#define  DB3  123.471
#define  D3   146.832
#define  DS3  155.563
#define  EB3  155.563
#define  E3   164.814
#define  F3   174.614
#define  FS3  184.997
#define  GB3  184.997
#define  G3   195.998
#define  GS3  207.652
#define  A3   220.00
#define  AS3  233.082
#define  BB3  233.082
#define  B3   246.942
#define  C4   261.626
#define  CS4  277.183
#define  DB4  277.183
#define  D4   293.665
#define  DS4  311.127
#define  EB4  311.127
#define  E4   329.628
#define  F4   349.228
#define  FS4  369.994
#define  GB4  369.994
#define  G4   391.995
#define  GS4  415.305
#define  AB4  415.305
#define  A4   440.0
#define  AS4  466.164
#define  BB4  466.164
#define  B4   493.883
#define  C5   523.251 
#define  CS5  554.365
#define  DB5  554.365
#define  D5   587.330
#define  DS5  622.254
#define  EB5  622.254
#define  E5   659.255
#define  F5   698.456
#define  FS5  739.989
#define  GB5  739.989
#define  G5   783.991
#define  GS5  830.609
#define  AB5  830.609
#define  A5   880.00
#define  AS5  932.328
#define  BB5  932.328
#define  B5   987.767
#define  C6   1046.50

#define  PR   A0

int noteLength;
int count = 0;
double reference = 0;
double curNote = 0;
double lastNote;
bool pausing = false;
bool calibrating = true;
int pauseCount = 0;

double dfreq;
const double refclk=31376.6;      // measured

// variables used inside interrupt service declared as voilatile
volatile byte icnt;              // var inside interrupt
volatile byte icnt1;             // var inside interrupt
volatile byte c4ms;              // counter incremented all 4ms
volatile unsigned long phaccu;   // pahse accumulator
volatile unsigned long tword_m;  // dds tuning word m

void setup()
{
      // set prescale to 16
    sbi(ADCSRA,ADPS2) ;
    cbi(ADCSRA,ADPS1) ;
    cbi(ADCSRA,ADPS0) ;
    Serial.begin(9600);        // connect to the serial port
    Serial.println("DDS Test");
    
    pinMode(6, OUTPUT);      // sets the digital pin as output
    pinMode(7, OUTPUT);      // sets the digital pin as output
    pinMode(11, OUTPUT);     // pin11= PWM  output / frequency output

    pinMode(2, OUTPUT);
    
    Setup_timer2();
    Setup_timer1();
    
    // disable interrupts to avoid timing distortion
    cbi (TIMSK0,TOIE0);              // disable Timer0 !!! delay() is now not available
    sbi (TIMSK2,TOIE2);              // enable Timer2 Interrupt

    dfreq=261.63;                    // initial output frequency = 1000.o Hz
    tword_m=pow(2,32)*dfreq/refclk;
}
void loop()
{
    int shade = analogRead(A0);

    if (reference == 0) reference = shade / 10 * 10; // set reference shade, round to nearest 10
    
    // SET NOTE
    if (reference - 90  <= shade && shade < reference - 54){
      curNote = C4;
    }
    else if (reference - 54 <= shade && shade < reference - 18) {
      curNote = D4;
    }
    else if (reference - 18 <= shade && shade < reference + 18) {
      curNote = E4;
    }
    else if (reference + 18 <= shade && shade < reference + 54) {
      curNote = F4;
    }
    else if (reference + 54 <= shade && shade < reference + 90) {
      curNote = G4;
    }
    else if (reference + 90 <= shade && shade < reference + 126) {
      curNote = A4;
    }
    else if (reference + 126 <= shade && shade < reference + 162) {
      curNote = B5;
    }
    else if (reference + 162 <= shade && shade < reference + 198) {
      curNote = C6;
    }

//    if (curNote != lastNote) {
//      if (abs(count - noteLength) > 10
//    }

    dfreq = curNote;

//    Serial.write("PR Reading: ");
//    Serial.print(shade);
//    Serial.write("\tReference: ");
//    Serial.println(reference);
}
//******************************************************************
// timer2 setup
// set prscaler to 1, PWM mode to phase correct PWM,  16000000/510 = 31372.55 Hz clock
void Setup_timer2() {
    
    // Timer2 Clock Prescaler to : 1
    sbi (TCCR2B, CS20);
    cbi (TCCR2B, CS21);
    cbi (TCCR2B, CS22);
    
    // Timer2 PWM Mode set to Phase Correct PWM
    cbi (TCCR2A, COM2A0);  // clear Compare Match
    sbi (TCCR2A, COM2A1);
    
    sbi (TCCR2A, WGM20);  // Mode 1  / Phase Correct PWM
    cbi (TCCR2A, WGM21);
    cbi (TCCR2B, WGM22);
}

void Setup_timer1() {
    // 10 Hz
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    
    // Prescaler to 1024
    sbi (TCCR1B, CS10);
    cbi (TCCR1B, CS11);
    sbi (TCCR1B, CS12);

    // set compare register
    OCR1A = 6009;

    // turn on CTC mode
    sbi(TCCR1B, WGM12);

    // enable interrupts
    sbi(TIMSK1, OCIE1A);
}
//******************************************************************
// Timer2 Interrupt Service at 31372,550 KHz = 32uSec
// this is the timebase REFCLOCK for the DDS generator
// FOUT = (M (REFCLK)) / (2 exp 32)
// runtime : 8 microseconds ( inclusive push and pop)
ISR(TIMER2_OVF_vect) {
    
    sbi(PORTD,7);          // Test / set PORTD,7 high to observe timing with a oscope
    
    phaccu=phaccu+tword_m; // soft DDS, phase accu with 32 bits
    icnt=phaccu >> 24;     // use upper 8 bits for phase accu as frequency information
    // read value fron ROM sine table and send to PWM DAC
    OCR2A=pgm_read_byte_near(sine256 + icnt);
    
    if(icnt1++ == 125) {  // increment variable c4ms all 4 milliseconds
        c4ms++;
        icnt1=0;
    }   
    
    cbi(PORTD,7);            // reset PORTD,7
}

ISR(TIMER1_COMPA_vect) {
  tword_m=pow(2,32)*dfreq/refclk;
  int aaa = 0;
  while(aaa < 10000){
    __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"); 
    aaa++;
  }
}
