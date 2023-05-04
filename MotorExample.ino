int enMotor = 17;
int in1Motor = 15;
int in2Motor = 16;



void setup() {
  // Set all th emotor control pins to outputs
  pinMode(enMotor, OUTPUT);
  pinMode(in1Motor, OUTPUT);
  pinMode(in2Motor, OUTPUT);
  //Turn off motor - Initial state
  digitalWrite (in1Motor, LOW);
  digitalWrite (in2Motor, LOW);
}

void loop() {
  // Run Motor at fastest speed
  analogWrite (enMotor, 255);
  digitalWrite(in1Motor, LOW);
  digitalWrite(in2Motor,HIGH);
  delay(2000);
  
  // Turn off motor
  digitalWrite(in1Motor, LOW);
  digitalWrite(in2Motor,LOW);
  delay(2000);
}
