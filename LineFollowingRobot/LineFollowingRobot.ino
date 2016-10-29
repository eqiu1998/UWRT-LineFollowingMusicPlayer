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
#define epsilon     150

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
  digitalWrite(8, HIGH);
  
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

