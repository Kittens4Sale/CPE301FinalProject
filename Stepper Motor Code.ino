
//Includes the Arduino Stepper Library
#include <Stepper.h>
// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;
// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

const int button1 = 2;
const int button2 = 3;
int buttonState1 = 0;
int buttonState2 = 0;

void setup() {
// Nothing to do (Stepper Library sets pins as outputs)
  // Initialize the push button pins as inputs
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
}
void loop() {
    buttonState1 = digitalRead(button1);
    buttonState2 = digitalRead(button2);    
    int stepValue = 0;
    myStepper.setSpeed(5);
    if (buttonState1 == HIGH){
      stepValue = 200;
    } else if (buttonState2 == HIGH){
      stepValue = -200;
    } else {
      stepValue = 0;
    }

    myStepper.step(stepValue);
}
