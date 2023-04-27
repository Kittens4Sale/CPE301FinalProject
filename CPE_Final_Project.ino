// Define pins for the system
#define PUMP_PIN 4
#define FAN_PIN 5
#define VENT_PIN 2
#define WATER_LEVEL_PIN A0
#define TEMP_HUMIDITY_PIN A1
#define VENT_CONTROL_PIN A2
#define ON_OFF_PIN A3

// Define variables for the system
bool systemEnabled = false;
bool motorRunning = false;
bool lowWaterLevel = false;
int currentVentAngle = 0;
int temperature = 0;
int humidity = 0;
int fanSpeed = 0;
unsigned long lastMotorStartTime = 0;

void setup() {
  // Set the direction of each pin
  DDRC |= (1 << PUMP_PIN);
  DDRC |= (1 << FAN_PIN);
  DDRE |= (1 << VENT_PIN);
  DDRC &= ~(1 << WATER_LEVEL_PIN);
  DDRC &= ~(1 << TEMP_HUMIDITY_PIN);
  DDRC |= (1 << VENT_CONTROL_PIN);
  DDRC &= ~(1 << ON_OFF_PIN);
  
  // Enable pull-up resistor for on/off pin
  PORTC |= (1 << ON_OFF_PIN);
  
  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // Check if the system is enabled
  if (systemEnabled) {
    // Check the water level
    if (PINB & (1 << WATER_LEVEL_PIN)) {
      lowWaterLevel = false;
    } else {
      lowWaterLevel = true;
    }
    
    // Check the temperature and humidity
    // Assume the sensor returns the temperature and humidity as a string in the format "temp,humidity"
    if (Serial.available()) {
      String data = Serial.readStringUntil('\n');
      int commaIndex = data.indexOf(',');
      temperature = data.substring(0, commaIndex).toInt();
      humidity = data.substring(commaIndex + 1).toInt();
    }
    
    // Adjust the fan speed based on the temperature
    if (temperature > 28) {
      fanSpeed = 255;
    } else if (temperature > 25) {
      fanSpeed = map(temperature, 25, 28, 50, 255);
    } else {
      fanSpeed = 0;
    }
    
    // Start or stop the fan based on the fan speed
    if (fanSpeed > 0) {
      PORTE |= (1 << FAN_PIN);
    } else {
      PORTE &= ~(1 << FAN_PIN);
    }
    
    // Adjust the vent angle based on the user input
    int ventControlValue = (PINC >> VENT_CONTROL_PIN) & 0b00000011;
    if (ventControlValue == 0) {
      currentVentAngle = 0;
    } else if (ventControlValue == 1) {
      currentVentAngle = 45;
    } else if (ventControlValue == 2) {
      currentVentAngle = 90;
    } else {
      currentVentAngle = 135;
    }
    // Set the vent angle using PWM
    analogWrite(VENT_PIN, map(currentVentAngle, 0, 180, 0, 255));
    
    // Check the on/off button
    if ((PINC >> ON_OFF_PIN) & 0b00000001) {
      systemEnabled = false;
      // Stop the motor if it's running
      if (motorRunning) {
        PORTC &= ~(1 << PUMP_PIN);
        motorRunning = false;
        // Record the stop time
        unsigned long
        stopTime = millis();
        // Calculate the total running time
        unsigned long runningTime = stopTime - lastMotorStartTime;
        Serial.print("Motor stopped after running for ");
        Serial.print(runningTime);
        Serial.println(" ms.");
        }
      }
      // Check the motor running status
      if (motorRunning) {
        // Check if the motor has been running for more than 10 seconds
        if (millis() - lastMotorStartTime > 10000) {
        // Check the water level
          if (lowWaterLevel) {
          // Stop the motor
          PORTC &= ~(1 << PUMP_PIN);
          motorRunning = false;
          // Record the stop time
          unsigned long stopTime = millis();
         // Calculate the total running time
         unsigned long runningTime = stopTime - lastMotorStartTime;
         Serial.print("Motor stopped after running for ");
         Serial.print(runningTime);
         Serial.println(" ms due to low water level.");
        }
      }
    } 
    else {
      // Check if the water level is low
      if (lowWaterLevel) {
      // Start the motor
      PORTC |= (1 << PUMP_PIN);
      motorRunning = true;
     // Record the start time
        lastMotorStartTime = millis();
    Serial.println("Motor started due to low water level.");
    }
  }
}   
else {
// Check the on/off button
if (!((PINC >> ON_OFF_PIN) & 0b00000001)) {
systemEnabled = true;
Serial.println("System enabled.");
}
}
}