/* TO DO:
 *  Define pins
 *    - Left photoresistor
 *    - Right photoresistor
 *    - Encoder
 *    - Motor control
 *    - 
 */
#define RIGHT       0
#define LEFT        1
#define epsilon     150

int PR[] = {A0, A1};
int motor[] = { 10, 9 };

int lastShade[] = {0, 0};

#include "encoder.h"

Encoder enc(ENC_A, ENC_B);

void drive (int left, int right)  {
  analogWrite(L_MOTOR, left);
  analogWrite(R_MOTOR, right);   
}

void followLine() {
  Serial.write("encoder: ");
  Serial.println(enc.read()); // print encoder position for testing purposes.

  int rShade = analogRead(PR[RIGHT]);
  int lShade = analogRead(PR[LEFT]);

  if (abs(lShade - rShade) < epsilon) {
    drive(200, 200);
    if (correcting) {
      correcting = false;
      Serial.write("back to equal speed");
    }
  }
  else if(lShade - rShade > epsilon) {
    drive(200, 255);
    if (!correcting) {
      Serial.write("R Full Speed");
      correcting = true;
    }
  }
  else if(rShade - lShade > epsilon) {
    drive(255, 200);
    if (!correcting) {
      Serial.write("L Full Speed");
      correcting = true;
    }
  }

  Serial.write("L: ");
  Serial.print(lShade);
  Serial.write("\tR: ");
  Serial.print(rShade);
  Serial.write("\tEncoder: ");
  Serial.println(enc.read());

}

void setup() {
  enc.write(0);
  enc.start();

  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH)
  
  Serial.begin(9600); // set up communication
  pinMode(motor[RIGHT], OUTPUT);
  pinMode(motor[LEFT], OUTPUT);  

  drive(200, 200);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  followLine();
}

