// CPE 301: Embedded System Design
// Authors: Matthew Guild, Guilherme Cassiano, Megan Kershaw and Lucas Adams
// Date: 5/4/2023
// Project: Final Project

#include <Stepper.h>
#include <LiquidCrystal.h>
#include <dht11.h>
#include "Arduino.h"
#include "uRTCLib.h"

/* initialize the library by associating any needed LCD interface pin
with the arduino pin number it is connected to*/
const int rs = 47, en = 43, d4 = 33, d5 = 31, d6 = 29, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// declare variables for stepper motor
const int stepsPerRevolution = 2038;
int stepValue = 0;
Stepper myStepper = Stepper(stepsPerRevolutions, 8, 9, 10, 11);


const int button1 = 2;   //Stepper motor rotate
int buttonState1 = 0;
const int button2 = 3;   //Stepper motor rotate the other way
int buttonState2 = 0;
const int onOffButton = 4;  //Enable and disable swamp cooler

//Four LEDs variable declaration
int YellowLED = ;
int RedLED = ;
int GreenLED = ;
int BlueLED = ;

//Humidity and temperature monitor variable declaration
#define DHT11PIN 5;
dht11 DHT11;

//Establish clock variable
uRTCLib rtc(0x68);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Set variables for motor
int enMotor = 17;
int in1Motor = 15;
int in2Motor = 16;

void setup() {
  // Initialize LCD pins as output and back light on
  pinMode(23,OUTPUT);
  pinMode(25,OUTPUT);
  pinMode(45,OUTPUT);
  pinMode(53,OUTPUT);
  pinMode(49,OUTPUT);
  pinMode(51,OUTPUT);
  digitalWrite(23,LOW); 
  digitalWrite(25,HIGH); 
  digitalWrite(45,LOW); 
  digitalWrite(53,LOW);
  digitalWrite(49,LOW);
  digitalWrite(51,HIGH);
  lcd.begin(16, 2);

   // Initialize the push button pins as inputs
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(onOffButton, INPUT);

  // Initialize LED pins as outputs
  pinMode(YellowLED, OUTPUT);
  pinMode(RedLED, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  pinMode(BlueLED, OUTPUT);

  // Set all the motor control pins to outputs
  pinMode(enMotor, OUTPUT);
  pinMode(in1Motor, OUTPUT);
  pinMode(in2Motor, OUTPUT);
  //Turn off motor - Initial state
  digitalWrite (in1Motor, LOW);
  digitalWrite (in2Motor, LOW);
  
  // Initialize serial communication
  Serial.begin(9600);

  //Set date and time for clock
  URTCLIB_WIRE.begin();
  rtc.set(0, 10, 1, 5, 4, 5,23)
}

void loop() {
  int onOffValue = 0;
  int onOffLastValue = 0;
  int chk = DHT11.read(DHT11PIN);

  // Refresh Clock
  rtc.refresh();  

  // Vent movement using buttons
  buttonState1 = digitalRead(button1);
  buttonState2 = digitalRead(button2);    
  int stepValue = 0;
  myStepper.setSpeed(5);
  if (buttonState1 == HIGH){
    stepValue = 200;
    Serial.print ();            //display change in vent in serial monitor
  } else if (buttonState2 == HIGH){
    stepValue = -200;
    Serial.print ();            //display change in vent in serial monitor
  } else {
    stepValue = 0;
  }
  myStepper.step(stepValue);

  // if on off button pressed, add to value variable
  if (onOffButton == HIGH){
    onOffValue = onOffValue + 1;
  }
  // if on off button pressed, change state between disbaled and enabled
  if (onOffValue != onOffLastValue){
    humTempMonitor();
    // Check water level
    if () {                        //Insert boolean expression for if water level is LESS than a certain number
      // System in error
      led.setCursor (0, 0);
      led.print ("ERROR");
      // Turn on red LED
      digitalWrite (YellowLED, LOW);
      digitalWrite (RedLED, HIGH);
      digitalWrite (GreenLED, LOW);
      digitalWrite (BlueLED, LOW);
       // Turn off motor
      digitalWrite(in1Motor, LOW);
      digitalWrite(in2Motor,LOW); 
                                            //Add code to move to idle if button pressed         
    } else {
      if () {                      //Insert boolean expression for if tempature is GREATER than a certain number
        // System in running
        led.setCursor (0, 0);
        led.print ("Running");
        // Turn on green LED
        digitalWrite (YellowLED, LOW);
        digitalWrite (RedLED, LOW);
        digitalWrite (GreenLED, HIGH);
        digitalWrite (BlueLED, LOW);
        // Run Motor at fastest speed
        analogWrite (enMotor, 255);
        digitalWrite(in1Motor, LOW);
        digitalWrite(in2Motor,HIGH);
                                            //     
      } else {
        // System in idle
        led.setCursor (0, 0);        
        led.print ("Idle");
        // Turn on blue LED
        digitalWrite (YellowLED, LOW);
        digitalWrite (RedLED, LOW);
        digitalWrite (GreenLED, LOW);
        digitalWrite (BlueLED, HIGH);
        // Turn off motor
        digitalWrite(in1Motor, LOW);
        digitalWrite(in2Motor,LOW);
      }
    }
    onOffLastValue = onOffValue;
  } else {
    // System Disabled
    lcd.setCursor (0, 0);
    lcd.print ("Disabled")
    // Turn on yellow LED
    digitalWrite (YellowLED, HIGH);
    digitalWrite (RedLED, LOW);
    digitalWrite (GreenLED, LOW);
    digitalWrite (BlueLED, LOW);
    // Turn off motor
    digitalWrite(in1Motor, LOW);
    digitalWrite(in2Motor,LOW);
    // Print state and time to serial monitor
    Serial.print ("DISABLED");
    Serial.print ();                        //Insert code to display time to serial monitor
  }

}

// Function for using humidity and temp monitor
void humTempMonitor()
 {
  lcd.setCursor(0, 1);  
  lcd.print((float)DHT11.humidity, 2);
  lcd.print("%");
  lcd.setCursor(5, 1);
  lcd.print((float)DHT11.temperature, 2);
  lcd.print("C"); 
}