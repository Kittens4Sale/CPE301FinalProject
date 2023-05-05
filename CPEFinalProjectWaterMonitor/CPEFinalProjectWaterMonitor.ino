// Define the port registers for the power and signal pins
#define POWER_DDR DDRD
#define POWER_PORT PORTD
#define SIGNAL_DDR DDRC
#define SIGNAL_PORT PORTC
#define SIGNAL_PIN PC5

int value = 0; // variable to store the sensor value

void setup() {
  // Set the power pin as an output
  POWER_DDR |= (1 << POWER_PIN);
  
  // Turn the sensor OFF
  POWER_PORT &= ~(1 << POWER_PIN);
}

void loop() {
  // Turn the sensor ON
  POWER_PORT |= (1 << POWER_PIN);
  
  // Wait 10 milliseconds
  _delay_ms(10);
  
  // Read the analog value from the sensor
  SIGNAL_DDR &= ~(1 << SIGNAL_PIN); // Set the signal pin as an input
  SIGNAL_PORT &= ~(1 << SIGNAL_PIN); // Disable the internal pull-up resistor
  ADCSRA |= (1 << ADEN); // Enable the ADC module
  ADCSRA |= (1 << ADSC); // Start a conversion
  while (ADCSRA & (1 << ADSC)); // Wait for the conversion to complete
  value = ADC; // Read the ADC value
  ADCSRA &= ~(1 << ADEN); // Disable the ADC module
  
  // Turn the sensor OFF
  POWER_PORT &= ~(1 << POWER_PIN);
  
  // Wait for 1 second
  _delay_ms(1000);
}
