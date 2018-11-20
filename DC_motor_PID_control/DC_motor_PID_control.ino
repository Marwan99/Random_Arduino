/*Circuit connections:
-Motor red   --> OUT1 H-bridge
-Motor white --> OUT2 H-bridge
-Motor blue  --> 3.3V (encoder power)
-Motor black --> GND (encoder power)
-Motor yellow --> Arduino D2 (encoder phase A)
-Motor green  --> Arduino D3 (encoder phase B)
-H-bridge IN1 --> Arduino D10
-H-bridge IN2 --> Arduino D9*/

const byte encoderPinA = 2;
const byte encoderPinB = 3;

const byte motorN3 = 9;
const byte motorN4 = 10;

volatile long pulse_count;
long prev_pulse_count;

long prev_millis;
long mil2;

int set_point = 50;
double Kp = 1.6; //2.1
double ki = 5; //4
double kd = 0.01;
double error;
double prev_error;
double integral;
double derivative;
double speed_reading;
int actuation_signal;

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

  pinMode(motorN3, OUTPUT);
  pinMode(motorN4, OUTPUT);
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  attachInterrupt(0, encoderA_ISR, CHANGE);
  attachInterrupt(1, encoderB_ISR, CHANGE);
}

void loop()
{
  if (Serial.available())
  {
    set_point = Serial.parseInt();
    Serial.print("Setpoint: ");
    Serial.println(set_point);
    //integral = 0;
  }

  if (millis() - prev_millis > 20)
  {
    prev_millis = millis();

    speed_reading = ((prev_pulse_count - pulse_count) / 0.02) / 34.02; //motor shaft RPM
    Serial.println(speed_reading);
    prev_pulse_count = pulse_count;

    error = set_point - speed_reading;

    integral += (error) * 0.02;
    derivative = (error - prev_error) / 0.02;
    actuation_signal = (Kp * error) + (ki * integral) + (kd * derivative);

    prev_error = error;

    if (actuation_signal > 0)
    {
      analogWrite(motorN3, (actuation_signal < 255) ? actuation_signal : 255);
      digitalWrite(motorN4, LOW);
    }
    else if (actuation_signal < 0)
    {
      actuation_signal = abs(actuation_signal);
      analogWrite(motorN4, (actuation_signal < 255) ? actuation_signal : 255);
      digitalWrite(motorN3, LOW);
    }
    else
    {
      digitalWrite(motorN3, LOW);
      digitalWrite(motorN4, LOW);
    }
  }
  //digitalWrite(motorN3, LOW);
  //digitalWrite(motorN4, HIGH);

  /*if (millis() - mil2 > 100)
    {
    mil2 = millis();
    Serial.println(speed_reading);
    }*/
    if (millis() - mil2 > 5000)
    {
    mil2 = millis();
    set_point += 25;
    if (set_point > 300)
      set_point = 100;
    }
}
