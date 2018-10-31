const byte encoderPinA = 2;
const byte encoderPinB = 3;

const byte motorN3 = 9;
const byte motorN4 = 10;


volatile long cntr;
long prev_count;
long prev_millis;
int m2;
int error;
int set_point = 1200;
int prev_error;
int integral;
int drevative;
int speed_reading;
double Kp = 0.4; //0.16
double ki = 0.03;
double actuation_signal;

void encoderA_ISR()
{

  if (digitalRead(encoderPinA) == HIGH)
    (digitalRead(encoderPinB) == LOW) ? cntr++ : cntr--;
  else
    (digitalRead(encoderPinB) == HIGH) ? cntr++ : cntr--;
}

void encoderB_ISR()
{
  if (digitalRead(encoderPinB) == HIGH)
    (digitalRead(encoderPinA) == HIGH) ? cntr++ : cntr--;
  else
    (digitalRead(encoderPinA) == LOW) ? cntr++ : cntr--;
}

void setup()
{
  Serial.begin(115200);

  delay(1500);

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

  if (millis() - prev_millis > 100)
  {
    prev_millis = millis();
    speed_reading = prev_count - cntr;
    Serial.println(speed_reading);
    prev_count = cntr;

    error = set_point - speed_reading;
    integral += error;
    //prev_error = error;

    //dervative = prev_error - error;

    actuation_signal = (Kp * error) + (ki * integral);// + (kd * dervative);
    //Serial.print("actuation signal: ");
    //Serial.println(actuation_signal);

    if (actuation_signal > 0)
    {
      analogWrite(motorN3, (int(actuation_signal) < 255) ? int(actuation_signal) : 255);
      digitalWrite(motorN4, LOW);
    }
    else if (actuation_signal < 0)
    {
      actuation_signal = abs(actuation_signal);
      analogWrite(motorN4, (int(actuation_signal) < 255) ? int(actuation_signal) : 255);
      digitalWrite(motorN3, LOW);
    }
    else
    {
      digitalWrite(motorN3, LOW);
      digitalWrite(motorN4, LOW);
    }
  }
}


