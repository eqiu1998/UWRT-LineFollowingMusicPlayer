/* TO DO:
 *  Define pins
 *    - Left photoresistor
 *    - Right photoresistor
 *    - Encoder
 *    - Motor control
 *    - 
 */
#include "encoder.h"

#define ENC_A       A4
#define ENC_B       A5
#define RIGHT       0
#define LEFT        1
#define epsilon     50

int PR[] = {A0, A1};
int motor[] = { 10, 9 };
int lastShade[] = {0, 0};

bool correcting = false;

Encoder enc(ENC_A, ENC_B);

void drive (int left, int right)  {
  analogWrite(motor[LEFT], left);
  analogWrite(motor[RIGHT], right);   
}

void followLine() {
  int rShade = analogRead(PR[RIGHT]);
  int lShade = analogRead(PR[LEFT]);

  if (abs(lShade - rShade) < epsilon) {
    drive(255, 255);
    if (correcting) {
      correcting = false;
      Serial.write("back to equal speed");
    }
  }
  else if(lShade - rShade > epsilon) {
    drive(0, 255);
    if (!correcting) {
      Serial.write("R Full Speed");
      correcting = true;
    }
  }
  else if(rShade - lShade > epsilon) {
    drive(255, 0);
    if (!correcting) {
      Serial.write("L Full Speed");
      correcting = true;
    }
  }

  Serial.write("L: ");
  Serial.print(lShade);
  Serial.write("\tR: ");
  Serial.print(rShade);
}

void setup() {
  enc.write(0);
  enc.start();

  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  
  Serial.begin(9600); // set up communication
  pinMode(motor[RIGHT], OUTPUT);
  pinMode(motor[LEFT], OUTPUT);  
}

void loop() {
  // put your main code here, to run repeatedly:
  followLine();

  Serial.write("\tEncoder: ");
  Serial.println(enc.read());
}

