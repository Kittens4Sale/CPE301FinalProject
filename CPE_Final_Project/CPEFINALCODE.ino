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
Stepper myStepper = Stepper(stepsPerRevolution, 8, 9, 10, 11);
const int button1 = 2;   //Stepper motor rotate
int buttonState1 = 0;
const int button2 = 3;   //Stepper motor rotate the other way
int buttonState2 = 0;
const int onOffButton = 4;  //Enable and disable swamp cooler
int onOffButtonState = 0; 
const int resetButton = 6;  //Reset button to set to idle
int resetButtonState = 0;

//Four LEDs variable declaration
int BlueLED = 40;
int RedLED = 42;
int YellowLED = 44;
int GreenLED = 46;


//Humidity and temperature monitor variable declaration
#define DHT11PIN 7
dht11 DHT11;

//Establish clock variable
uRTCLib rtc(0x68);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Set variables for motor
int enMotor = 17;
int in1Motor = 15;
int in2Motor = 16;

//Set water sensor variables
#define RDA 0x80
#define TBE 0x20  
//UART POINTERS
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
//Define Port B to OUTPUT
volatile unsigned char* port_b = (unsigned char*) 0x25; 
volatile unsigned char* ddr_b  = (unsigned char*) 0x24; 
volatile unsigned char* pin_b  = (unsigned char*) 0x23; 


volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;
volatile unsigned int  *myTCNT1  = (unsigned  int *) 0x84;
volatile unsigned char *myTIFR1 =  (unsigned char *) 0x36;




volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
// Value for storing water level
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;
int stateNum;
int onOffValue = 1;
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
  pinMode(resetButton, INPUT);
  // Initialize LED pins as outputs
  DDRG = 0b00000010; //BLUE
  DDRC = 0b00100000; //RED
  DDRA = 0b00000001; //YELLOW
  DDRL = 0b00001000; //GREEN

  // Set all the motor control pins to outputs
  pinMode(enMotor, OUTPUT);
  pinMode(in1Motor, OUTPUT);
  pinMode(in2Motor, OUTPUT);
  // Turn off motor - Initial state
  digitalWrite (in1Motor, LOW);
  digitalWrite (in2Motor, LOW);

  // Initialize water sensor pins
  // Set Port B7 (pin 13) as an OUTPUT
  *ddr_b |= 0x80;
  // turn the sensor OFF
  *ddr_b |= 0x7F;
  // setup the UART
  U0init(9600);
  // setup the ADC
  adc_init();
  
  // Initialize serial communication
  Serial.begin(9600);

  //Set date and time for clock
  URTCLIB_WIRE.begin();
  rtc.set(0, 24, 6, 3, 9, 5,23);
  stateNum = 0;
}

void loop() {
  int chk = DHT11.read(DHT11PIN);
  Serial.print('\n');
  // Refresh Clock
  rtc.refresh();  

  // Vent movement using buttons
  buttonState1 = digitalRead(button1);
  buttonState2 = digitalRead(button2); 
  onOffButtonState = digitalRead(onOffButton);   
  resetButtonState = digitalRead(resetButton);
  int stepValue = 0;
  myStepper.setSpeed(5);
  if (buttonState1 == HIGH){
    stepValue = 200;
  } else if (buttonState2 == HIGH){
    stepValue = -200;
  } else{
        stepValue = 0;
  }
  myStepper.step(stepValue);
  if(resetButtonState == HIGH){
    stateNum = 2;
    displayTime(stateNum);
    stateAction(stateNum);
    myDelay(50000);
  }
  // if on off button pressed, add to value variable
  if (onOffButtonState == HIGH){
    onOffValue = onOffValue + 1;
  }
  // if on off button pressed, change state between disbaled and enabled
if (onOffValue % 2 == 0){

    // Humidity and temp reading and displaying
    int temp = humTempMonitor();

    // Check water level
    *port_b |= 0x80;  // turn the sensor ON
    unsigned int adc_reading = adc_read(0); // read the analog value from sensor
    *port_b &= 0x7F; // turn the sensor OFF

    if (adc_reading <= 159) {                  //Insert boolean expression for if water level is LESS than a certain number
      // System in error
      stateNum = 3;
      displayTime(stateNum);
                                            //Add code to move to idle if button pressed         
    } else {
      if (temp > 40) {                      //Insert boolean expression for if tempature is GREATER than a certain number //placeholder number
        // System Running
        stateNum = 1;  
        displayTime(stateNum);

      } else {
        // System in idle
        stateNum = 2;
        displayTime(stateNum);
      }
    }
  } else {
    // System Disabled
    stateNum = 0;
    displayTime(stateNum);
  }
  stateAction(stateNum);
  myDelay(1000);
}

// Function for deciding which state the cooler is in and doing the actions for that stage
void stateAction(int stateNum) {
  if (stateNum == 1) {             //Running
    // Display "Running" on LCD
    lcd.clear();
    lcd.setCursor (0, 0);
    // Turn on green LED
  PORTG = 0b00000000; //BLUE
  PORTC = 0b00000000; //RED
  PORTA = 0b00000000; //YELLOW
  PORTL = 0b00001000; //GREEN
    // Run Motor at fastest speed
    analogWrite (enMotor, 255);
    digitalWrite(in1Motor, LOW);
    digitalWrite(in2Motor,HIGH);
    displayTemp();
    
  } else if (stateNum == 2) {      //Idle
    // Display "Idle" in LCD
    lcd.clear();
    lcd.setCursor (0, 0);        
    // Turn on blue LED
   PORTG = 0b00000000; //BLUE
   PORTC = 0b00000000; //RED
   PORTA = 0b00000000; //YELLOW
   PORTL = 0b00001000; //GREEN
    // Turn off motor
    digitalWrite(in1Motor, LOW);
    digitalWrite(in2Motor,LOW);
    displayTemp();
  } else if (stateNum == 3) {      //Error
    // Display "Error" on LCD
    lcd.clear();
    lcd.setCursor (0, 0);
    lcd.print ("Water level is");
    lcd.setCursor (0, 1);
    lcd.print ("too low");
    // Turn on red LED
    PORTG = 0b00000000; //BLUE
    PORTC = 0b00100000; //RED
    PORTA = 0b00000000; //YELLOW
    PORTL = 0b00000000; //GREEN
     // Turn off motor
    digitalWrite(in1Motor, LOW);
    digitalWrite(in2Motor,LOW); 

                                            //Add code to move to idle if button pressed
  } else {                         //Disabled
    //Display "Disabled" on LCD
    lcd.clear();
    lcd.setCursor (0, 0);
    lcd.print ("Disabled");
    // Turn on yellow LED
   PORTG = 0b00000000; //BLUE
   PORTC = 0b00000000; //RED
   PORTA = 0b00000001; //YELLOW
   PORTL = 0b00000000; //GREEN
    // Turn off motor
    digitalWrite(in1Motor, LOW);
    digitalWrite(in2Motor,LOW);
    // Print state and time to serial monitor
  }
}

// Function for using humidity and temp monitor
int humTempMonitor()
 {
  int chk = DHT11.read(DHT11PIN);
  int temp = DHT11.temperature;
  return temp;
}

// Functions for water sensor:
void adc_init()
{
  // setup the A register
  *my_ADCSRA |=  0x80;// set bit   7 to 1 to enable the ADC
  *my_ADCSRA &=  0xDF;// clear bit 5 to 0 to disable the ADC trigger mode
  *my_ADCSRA &=  0xDF;// clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &=  0xDF;// clear bit 5 to 0 to set prescaler selection to slow reading
  // setup the B register
  *my_ADCSRB &=  0xF7;// clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &=  0xF8;// clear bit 2-0 to 0 to set free running mode
  // setup the MUX Register
  *my_ADMUX  &=  0x7F;// clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |=  0x40;// set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &=  0xDF;// clear bit 5 to 0 for right adjust result
  *my_ADMUX  &=  0xDF;// clear bit 5 to 0 for right adjust result
  *my_ADMUX  &=  0xE0;// clear bit 4-0 to 0 to reset the channel and gain bits
}
unsigned int adc_read(unsigned char adc_channel_num)
{
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX  &= 0xE0;
  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= 0xF7;
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    // set MUX bit 5
    *my_ADCSRB |= 0x20;
  }
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register
  return *my_ADC_DATA;
}
void print_int(unsigned int out_num)
{
  // clear a flag (for printing 0's in the middle of numbers)
  unsigned char print_flag = 0;
  // if its greater than 1000
  if(out_num >= 1000)
  {
    // get the 1000's digit, add to '0' 
    U0putchar(out_num / 1000 + '0');
    // set the print flag
    print_flag = 1;
    // mod the out num by 1000
    out_num = out_num % 1000;
  }
  // if its greater than 100 or we've already printed the 1000's
  if(out_num >= 100 || print_flag)
  {
    // get the 100's digit, add to '0'
    U0putchar(out_num / 100 + '0');
    // set the print flag
    print_flag = 1;
    // mod the output num by 100
    out_num = out_num % 100;
  } 
  // if its greater than 10, or we've already printed the 10's
  if(out_num >= 10 || print_flag)
  {
    U0putchar(out_num / 10 + '0');
    print_flag = 1;
    out_num = out_num % 10;
  } 
  // always print the last digit (in case it's 0)
  U0putchar(out_num + '0');
  // print a newline
  U0putchar('\n');
}
void U0init(int U0baud)
{
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}
unsigned char U0kbhit()
{
  return *myUCSR0A & RDA;
}
unsigned char U0getchar()
{
  return *myUDR0;
}
void U0putchar(unsigned char U0pdata)
{
  while((*myUCSR0A & TBE)==0);
  *myUDR0 = U0pdata;
}
void displayTime(int mode){
    rtc.refresh();

  switch(mode){
    case 0:
      Serial.print("Mode changed to Disabled at time: ");
      break;
    case 1:
      Serial.print("Mode changed to Running at time: ");
      break;
    case 2:
      Serial.print("Mode changed to Idle at time: ");
      break;
    case 3:
      Serial.print("Mode changed to Error at time: ");
      break;
  }
  Serial.print(rtc.year());
  Serial.print('/');
  Serial.print(rtc.month());
  Serial.print('/');
  Serial.print(rtc.day());

  Serial.print(" (");
  Serial.print(daysOfTheWeek[rtc.dayOfWeek()-1]);
   Serial.print(") ");

  Serial.print(rtc.hour());
  Serial.print(':');
  Serial.print(rtc.minute());
  Serial.print(':');
  Serial.println(rtc.second());
  
  myDelay(1000);
}
void displayTemp()
{
  int chk = DHT11.read(DHT11PIN);
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(DHT11.temperature);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(DHT11.humidity);
  lcd.print("%");
  myDelay(1000);
}
void myDelay(unsigned int freq) {
  double period = 1.0/double(freq);
  double half_period = period/ 2.0f;
  double clk_period = 0.0000000625;
  unsigned int ticks = half_period / clk_period;

  *myTCCR1B &= 0xF8;
  *myTCNT1 = (unsigned int) (65536 - ticks);
  * myTCCR1B |= 0b00000001;

  while((*myTIFR1 & 0x01)==0); // 0b 0000 0000

  *myTCCR1B &= 0xF8;   // 0b 0000 0000
  *myTIFR1 |= 0x01;
}