
#define ENC_A         A4
#define ENC_B         A5
#define RIGHT         0
#define LEFT          1
#define epsilon       50
#define N_SP_PRSETS   3

int PR[] = {A0, A1};
int motor[] = { 10, 9 };
int lastShade[] = {0, 0};

const int speedPresets[N_SP_PRSETS] = { 50, 150, 250 }; // arbitrary values for now
const int speedCtrlPins[N_SP_PRSETS] = { 2, 4, 7 };
int speed = 0;  // robot will not move if no pins are set

bool correcting = false;

void drive (int left, int right)  {
  analogWrite(motor[LEFT], left);
  analogWrite(motor[RIGHT], right);
}

void followLine() {
  int rShade = analogRead(PR[RIGHT]);
  int lShade = analogRead(PR[LEFT]);

  if (abs(lShade - rShade) < epsilon) {
    drive(speed, speed);
    if (correcting) {
      correcting = false;
      Serial.write("back to equal speed");
    }
  }
  else if(lShade - rShade > epsilon) {
    drive(0, speed);
    if (!correcting) {
      Serial.write("R Full Speed");
      correcting = true;
    }
  }
  else if(rShade - lShade > epsilon) {
    drive(speed, 0);
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
  int i;
  for (i = 0; i < N_SP_PRSETS; i++) {
    pinMode(speedCtrlPins[i], INPUT);
  }  
  for (i = 0; i < N_SP_PRSETS; i++) {
    if (digitalRead(speedCtrlPins[i]) == HIGH) {
      speed = speedPresets[i];
      break;
    }
  }
  
  enc.write(0);
  enc.start();

  Serial.begin(9600); // set up communication
  pinMode(motor[RIGHT], OUTPUT);
  pinMode(motor[LEFT], OUTPUT);  
}

void loop() {
  followLine();
  Serial.write("\tSpeed: ");
  Serial.println(speed);
}

