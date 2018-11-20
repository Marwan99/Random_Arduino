const byte encoderPinA = 2;
const byte encoderPinB = 3;

volatile long pulse_count;
long prev_millis;

void encoderA_ISR()
{

  if (digitalRead(encoderPinA) == HIGH)
    (digitalRead(encoderPinB) == LOW) ? pulse_count++ : pulse_count--;
  else
    (digitalRead(encoderPinB) == HIGH) ? pulse_count++ : pulse_count--;
}

void encoderB_ISR()
{
  if (digitalRead(encoderPinB) == HIGH)
    (digitalRead(encoderPinA) == HIGH) ? pulse_count++ : pulse_count--;
  else
    (digitalRead(encoderPinA) == LOW) ? pulse_count++ : pulse_count--;
}

void setup() 
{
  Serial.begin(115200);
  
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  attachInterrupt(0, encoderA_ISR, CHANGE);
  attachInterrupt(1, encoderB_ISR, CHANGE);

}

void loop() 
{
  if (millis() - prev_millis > 250)
    Serial.println(pulse_count);
}
