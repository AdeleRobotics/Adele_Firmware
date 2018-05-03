#define CENTERL 75
#define CENTERR 90
#define SOFFSET 50

#define LEFTIR 9
#define RIGHTIR 6

int garraPin = 2;
int motorLpin = 10;
int motorRpin = 12;
int lineLpin = 9;
int lineRpin = 6;

Servo garra;
Servo motorL;
Servo motorR;

void stopServos() {
  motorL.detach();
  motorR.detach();
  //  garra.detach();
  //  motorL.write(CENTERL);
  //  motorR.write(CENTERR);//calibrate stop
}

void stopGarra() {
  //  motorL.detach();
  //  motorR.detach();
  garra.detach();
  //  motorL.write(CENTERL);
  //  motorR.write(CENTERR);//calibrate stop
}

void adeleBegin() {
  pinMode(INPUT, lineLpin);
  pinMode(INPUT, lineRpin);
  garra.attach(garraPin);
  motorL.attach(motorLpin);
  motorR.attach(motorRpin);
  garra.write(150);
  stopServos();
  stopGarra();
}

void forward() {
  motorL.attach(motorLpin);
  motorR.attach(motorRpin);
  motorL.write(180 - SOFFSET);
  motorR.write(0 + SOFFSET); //calibrate stop
}

void backward() {
  motorL.attach(motorLpin);
  motorR.attach(motorRpin);
  motorL.write(0 + SOFFSET);
  motorR.write(180 - SOFFSET); //calibrate stop
}

void left() {
  motorL.attach(motorLpin);
  motorR.attach(motorRpin);
  motorL.write(0 + SOFFSET);
  motorR.write(0 + SOFFSET); //calibrate stop
}

void right() {
  motorL.attach(motorLpin);
  motorR.attach(motorRpin);
  motorL.write(180 - SOFFSET);
  motorR.write(180 - SOFFSET); //calibrate stop
}

void closeGarra() {
  garra.attach(garraPin);
  garra.write(150);
  delay(500);
//  garra.detach();
}

void openGarra() {
  garra.attach(garraPin);
  garra.write(90);
  delay(500);
//  garra.detach();
}

void stayInside() {//Stays inside line
  if (digitalRead(RIGHTIR) || digitalRead(LEFTIR)) {
    backward();
    delay(300);
    //chooses left or right at random
    if (random(2) == 0) left();
    else right();
    delay(500);
  } else {
    forward();
    delay(10);
  }
}

void followLine() {//simple line follower
  if (!digitalRead(RIGHTIR)) {
    right();
    delay(20);
  }
  if (!digitalRead(LEFTIR)) {
    left();
    delay(20);
  }
  forward();
  delay(10);
}

void randomMove(int t) {
  int choice = random(4);
  switch (choice) {
    case 1:
      forward();
      break;
    case 2:
      left();
      break;
    case 3:
      right();
      break;
  }
  delay(t);
}

