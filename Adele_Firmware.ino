#include <Servo.h>
#include <Adafruit_CircuitPlayground.h>
#include "Adele_Robot.h"
#include "PharmacodeRead.h"

#define  PIN   6
#define  NBARS 4
#define SERIALPORT Serial1
#define TURNTIME 510

const char startOfNumberDelimiter = '[';
const char endOfNumberDelimiter   = ']';

pharmacodeRead pharmacode(NBARS, PIN);//creaes reader for NBARS

String cmd; //stores commands from serial as string

byte mode = 0;

int luzAntes; //variable for follow light behaviour

void setup() {
  // put your setup code here, to run once:
  CircuitPlayground.begin();
  Serial.begin(9600);
  pixelsOff();
  adeleBegin();
  stopServos();

  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(25 * i));
    CircuitPlayground.playTone(500 + i * 500, 150);
  }
  CircuitPlayground.clearPixels();
  CircuitPlayground.setPixelColor(0, CircuitPlayground.strip.Color(128, 0, 128));
  CircuitPlayground.setPixelColor(9, CircuitPlayground.strip.Color(128, 0, 128));
  openGarra();
  stopGarra();
  SERIALPORT.begin(9600); //Set up Bluetooth
  SERIALPORT.println("Starting...");

  if (CircuitPlayground.rightButton()) {
    CircuitPlayground.playTone(440, 100);
    CircuitPlayground.setPixelColor(0, 255, 0 , 0);
    delay(300);
    CircuitPlayground.setPixelColor(1, 255, 0 , 0);
    CircuitPlayground.playTone(440, 100);
    mode = 2;
  }
  if (CircuitPlayground.leftButton()) {
    CircuitPlayground.playTone(440, 100);
    CircuitPlayground.setPixelColor(0, 255, 0 , 0);
    mode = 1;
  }
  if (CircuitPlayground.slideSwitch() == 0 && !CircuitPlayground.leftButton() && !CircuitPlayground.rightButton() ) {
    CircuitPlayground.playTone(440, 100);
    CircuitPlayground.setPixelColor(0, 255, 0 , 0);
    delay(300);
    CircuitPlayground.playTone(440, 100);
    CircuitPlayground.setPixelColor(1, 255, 0 , 0);
    delay(300);
    CircuitPlayground.playTone(440, 100);
    CircuitPlayground.setPixelColor(2, 255, 0 , 0);
    mode = 3;
  }
  delay(1000);
  CircuitPlayground.setPixelColor(0, 0, 0 , 0);
  CircuitPlayground.setPixelColor(1, 0, 0 , 0);
  CircuitPlayground.setPixelColor(2, 0, 0 , 0);
  CircuitPlayground.setPixelColor(0, CircuitPlayground.strip.Color(128, 0, 128));
  CircuitPlayground.setPixelColor(9, CircuitPlayground.strip.Color(128, 0, 128));
  CircuitPlayground.playTone(440, 250);
  CircuitPlayground.playTone(349, 125);
  CircuitPlayground.playTone(523, 250);
}

void loop() {
  switch (mode) {
    case 0:
      executeSerial(); //Adelebot app
      break;
    case 1:
      mode1();//Follow instructions
      break;
    case 2:
      mode2();//Follow behavior 
      break;
    case 3:
      executeTitibotSerial(); //Titibot mode
  }
}

/**************** modes and functions **********/

void mode1() {
  setPixelRules();

  //if switch to RIGHT then read codes
  if (CircuitPlayground.slideSwitch()) {

    //read code when left button mainteined pressed

    if (CircuitPlayground.leftButton() && !pharmacode.stopState) {
      if (pharmacode.currentRes < 10) { //no more than 10 rules
        pharmacode.readCode();
      }
    }

    if (pharmacode.stopState) {//entire code was read
      CircuitPlayground.playTone(440, 100);
      pharmacode.resetCode();
    }

    //erase one rule with right button
    if (CircuitPlayground.rightButton()) {
      pharmacode.eraseLast();
      pharmacode.resetCode();
      CircuitPlayground.playTone(880, 100);
      delay(200);//to avoid bouncing
    }
    // when switch RIGHT it executes the rules
  } else {

    //waits 1 sec before start
    delay(1000);
    executeCode();
    pharmacode.resetCode();
    pharmacode.resetLog();
    stopServos();
    stopGarra();
    pixelsOff();
  }
}



//Code execution and removes rules after applied
void executeCode() {
  for (int i = 0; i < pharmacode.currentRes; i++) {
    switch (pharmacode.resultLog[i]) {
      case 16://move forward
        forward();
        delay(1000);
        break;
      case 17://move backward
        backward();
        delay(1000);
        break;
      case 18://left turn
        left();
        delay(600);
        break;
      case 19://right turn
        right();
        delay(600);
        break;
      case 20://openClaw
        closeGarra();
        delay(300);
        break;
      case 21://closeClaw
        openGarra();
        delay(300);
        break;
      case 22://play tune
        CircuitPlayground.playTone(440, 250);
        CircuitPlayground.playTone(349, 125);
        CircuitPlayground.playTone(523, 250);
        break;
      case 23://color wheel
        for (int i = 0; i < 10; i++) {
          CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(25 * i));
          delay(150);
        }
        break;
    }
    stopServos();
    stopGarra();
    delay(300);//waits so it each command are seen as discrete moves
    pharmacode.resultLog[i] = 0;
    setPixelRules();
  }
}


void mode2() {

  //read code when left button pressed

  if (CircuitPlayground.leftButton() && !pharmacode.stopState) {
    pharmacode.readCode();
    if (pharmacode.stopState) {
      CircuitPlayground.playTone(440, 100);
      allPixels(0, 255, 0);
      delay(500);
      pixelsOff();
      delay(300);
    }
  }

  if (pharmacode.stopState) {//entire code was read
    //CircuitPlayground.playTone(440, 100);
    CircuitPlayground.setPixelColor(9, 255, 100 , 50);
    CircuitPlayground.setPixelColor(0, 255, 100 , 50);
    if (!CircuitPlayground.slideSwitch()) {
      executeCode2();
    }else{
        stopServos();
      stopGarra();
    }
  }

  //erase one rule with right button
  if (CircuitPlayground.rightButton()) {
    pharmacode.resetCode();
    stopServos();
    stopGarra();
    CircuitPlayground.playTone(880, 100);
    CircuitPlayground.setPixelColor(9, 0, 0 , 0);
    CircuitPlayground.setPixelColor(0, 0, 0 , 0);
    delay(200);//to avoid bouncing
  }
  // when siwtch LEFT it executes the rules

}

void executeCode2() {

  switch (pharmacode.result) {
    case 16://stay inside black line
      stayInside();
      break;
    case 17://move when sound
      if (CircuitPlayground.soundSensor() > 900) {
        CircuitPlayground.setPixelColor(9, 255, 100 , 0);
        CircuitPlayground.setPixelColor(0, 255, 100 , 0);
        randomMove(500);
        randomMove(500);
        randomMove(500);
        stopServos();
        stopGarra();
        CircuitPlayground.setPixelColor(9, 0, 0 , 0);
        CircuitPlayground.setPixelColor(0, 0, 0 , 0);
      }
      break;
    case 18://simple line follower
      followLine();
      break;
    case 19://randomwalk
      randomMove(300);
      delay(100);
      break;
    case 20://just move forward
      forward();
      delay(200);
      break;
    case 21:
      int luzAhora = CircuitPlayground.lightSensor();
      if (luzAhora >= luzAntes) {
        int t = random(600);
        left();
        delay(t);
      }
      forward();
      delay(100);
      luzAntes = luzAhora;
      break;
  }

}


//turns off all pixels (to avoid using clearPixles)
void pixelsOff() {
  for (int i = 0; i < 10; i++)
    CircuitPlayground.setPixelColor(i, 0, 0, 0);
}

void allPixels(byte red, byte green, byte blue) {
  for (int i = 0; i < 10; i++)
    CircuitPlayground.setPixelColor(i, red, green, blue);
}

//set pixel colors depending on code
void setPixelRules() {
  for (int i = 0; i < 10; i++) {
    Serial.println(pharmacode.currentRes);
    switch (pharmacode.resultLog[i]) {
      case 0:
        CircuitPlayground.setPixelColor(i, 0, 0, 0);
        break;
      case 16:
        CircuitPlayground.setPixelColor(i, 0, 255, 0);
        break;
      case 17:
        CircuitPlayground.setPixelColor(i, 255, 0, 0);
        break;
      case 18:
        CircuitPlayground.setPixelColor(i, 0, 0, 255);
        break;
      case 19:
        CircuitPlayground.setPixelColor(i, 255, 255, 0);
        break;
      case 20:
        CircuitPlayground.setPixelColor(i, 255, 0, 255);
        break;
      case 21:
        CircuitPlayground.setPixelColor(i, 56, 149, 255);
        break;
      case 22:
        CircuitPlayground.setPixelColor(i, 244, 143, 66);
        break;
      case 23:
        CircuitPlayground.setPixelColor(i, 255, 102, 178);
        break;
      default:
        CircuitPlayground.setPixelColor(i, 255, 255, 255);
    }
  }
}

void executeSerial() {
  if (SERIALPORT.available() > 0) {
    digitalWrite(13, HIGH);
    cmd = SERIALPORT.readStringUntil('\n');//
    SERIALPORT.println(cmd + " recibido...");
    SERIALPORT.flush();
    digitalWrite(13, LOW);
  }
  char inChar;// variable for current command
  for (int i = 0; i < cmd.length(); i++) {
    inChar = cmd[i];//reads command from string
    SERIALPORT.println(inChar);
    switch (inChar) {
      case 'F'://move forward
        forward();
        //  CircuitPlayground.setPixelColor(0, CircuitPlayground.strip.Color(100, 200, 0));
        //  CircuitPlayground.setPixelColor(9, CircuitPlayground.strip.Color(100, 200, 0));
        delay(1000);
        break;
      case 'B'://move backward
        backward();
        delay(1000);
        break;
      case 'L'://move left
        left();
        delay(TURNTIME);
        break;
      case 'R'://move right
        right();
        delay(TURNTIME);
        break;
      case 'O'://open claw
        openGarra();
        delay(750);
        break;
      case 'C'://close claw
        closeGarra();
        delay(750);
        break;
      case 'N'://play tune
        CircuitPlayground.playTone(440, 250);
        CircuitPlayground.playTone(349, 125);
        CircuitPlayground.playTone(523, 250);
        break;
      case 'P':
        for (int i = 0; i < 10; i++) {
          CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(25 * i));
          delay(150);
        }
        CircuitPlayground.clearPixels();
        CircuitPlayground.setPixelColor(0, CircuitPlayground.strip.Color(128, 0, 128));
        CircuitPlayground.setPixelColor(9, CircuitPlayground.strip.Color(128, 0, 128));
        break;
    }
    stopServos();
    stopGarra();
    delay(300);//waits so it each command are seen as discrete moves
  }
  stopServos();
  stopGarra();
  SERIALPORT.flush();
  cmd = "";//clears command string to get ready for next
}

void executeTitibotSerial() {
    static int receivedNumber = 0;
    byte c = Serial1.read();
    switch (c)
    {
      case endOfNumberDelimiter:
        Serial.println(receivedNumber);
        processTitiCommand(receivedNumber);
        digitalWrite(13, LOW);
        break;
      case startOfNumberDelimiter:
        receivedNumber = 0;
        digitalWrite(13, HIGH);
        break;
      case '0' ... '9':
        receivedNumber *= 10;
        receivedNumber += c - '0';
        break;
    }
}

void processTitiCommand(int command) {
  switch (command) {
    case 2:
      stopGarra();
      break;
    case 3:
      forward();
      delay(750);
      break;
    case 4:
      backward();
      delay(750);
      break;
    case 5:
      left();
      delay(350);
      break;
    case 6:
      right();
      delay(350);
      break;
    case 7:
      openGarra();
      delay(750);
      break;
    case 8:
      closeGarra();
      delay(750);
      break;
    case 9:
      for (int i = 1; i < 9; i++) {
        CircuitPlayground.setPixelColor(i, CircuitPlayground.strip.Color(200, 200, 0));
      }
      delay(750);
      break;
    case 10:
      for (int i = 1; i < 9; i++) {
        CircuitPlayground.setPixelColor(i, CircuitPlayground.strip.Color(0, 0, 0));
      }
      delay(750);
      break;
  }
  stopServos();
  delay(300);
}




