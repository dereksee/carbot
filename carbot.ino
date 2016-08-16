// comment out below line if you don't use HC_SR04 sensor
#define __HC_SR04__

#define TIME_INTERVAL 1000

// defines arduino i/o pins to use
#define START_PIN 2
#define MOTOR1_ENABLE_PIN1  (START_PIN+0)
#define MOTOR1_ENABLE_PIN2 (START_PIN+1)
#define MOTOR2_ENABLE_PIN1  (START_PIN+2)
#define MOTOR2_ENABLE_PIN2 (START_PIN+3)
#ifdef __HC_SR04__
#define HC_SR04_TRIGGER (START_PIN+4)
#define HC_SR04_ECHO (START_PIN+5)
#define MIN_DISTANCE 20
#define MAX_DISTANCE MIN_DISTANCE + 20
#endif

#define CONTROL_UP 'a'
#define CONTROL_LEFT 'b'
#define CONTROL_STOP 'c'
#define CONTROL_RIGHT 'd'
#define CONTROL_DOWN 'e'
#define CONTROL_OFF 'g'
#define CONTROL_ON  'f'



unsigned long cur_time, prev_time;
boolean auto_mode = true;
char cmd = CONTROL_STOP, prev_cmd = -1;

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR1_ENABLE_PIN1, OUTPUT);
  pinMode(MOTOR1_ENABLE_PIN2, OUTPUT);
  pinMode(MOTOR2_ENABLE_PIN1, OUTPUT);
  pinMode(MOTOR2_ENABLE_PIN2, OUTPUT);
#ifdef __HC_SR04__
  pinMode(HC_SR04_TRIGGER, OUTPUT);
  pinMode(HC_SR04_ECHO, INPUT);
#endif
  stop();
  cur_time = prev_time = millis();
  analogWrite(A0, 0);
}

void loop() {
  //char cmd = -1;

  //forward(); delay(1000); left(); delay(500); forward(); delay(1000); right(); delay(500); return;
  //forward();return;
#ifdef __HC_SR04__
  if (auto_mode) {
    long cm = distance_measure();
    if (cm < MIN_DISTANCE) {
      stop(); cmd = CONTROL_STOP;
      while (Serial.available()) Serial.read();
      return;
    }

    if (cm >= MIN_DISTANCE && cm <= MAX_DISTANCE) {
      backward(); delay(1000); left();
      forward(); cmd = CONTROL_UP;
      while (Serial.available())  Serial.read();
      return;
    }
  }
#endif

  if (Serial.available () > 0) { // read the bluetooth and stored in state
    cmd = Serial.read ();
    //Serial.println(cmd);
    switch (cmd) {
      case CONTROL_ON: auto_mode = false; cmd = CONTROL_STOP;
        analogWrite(A0, 128);
        break;
      case CONTROL_OFF: auto_mode = true; cmd = CONTROL_STOP;
        analogWrite(A0, 0);
        //Serial.println("control off");
        break;
    }
  }

  if (auto_mode) {
    cur_time = millis();
    if (cur_time - prev_time > TIME_INTERVAL) {
      prev_time = cur_time;
      do {
        cmd = (char)random(CONTROL_UP, CONTROL_DOWN + 1);
        //Serial.println(cmd);
      } while (cmd == CONTROL_STOP || cmd == prev_cmd);
    }
  }

  //Serial.println(cmd);
  switch (cmd) {
    case CONTROL_UP : forward(); prev_cmd = cmd;
      break;
    case CONTROL_DOWN: if (!auto_mode) backward(); prev_cmd = cmd;
      break;
    case CONTROL_LEFT : stop(); left(); cmd = prev_cmd;
      break;
    case CONTROL_RIGHT: stop(); right(); cmd = prev_cmd;
      break;
    case CONTROL_STOP: stop();
      break;
  }

}


void stop() {
  digitalWrite(MOTOR1_ENABLE_PIN1, LOW);
  digitalWrite(MOTOR1_ENABLE_PIN2, LOW);
  digitalWrite(MOTOR2_ENABLE_PIN1, LOW);
  digitalWrite(MOTOR2_ENABLE_PIN2, LOW);
  if (auto_mode) delay(500);
}

void right() {
  motor2_forward();
  motor1_backward();
  if (auto_mode) delay(100);
  else
    delay(150);
}

void left() {
  motor1_forward();
  motor2_backward();
  if (auto_mode) delay(100);
  else
    delay(150);
}


void forward() {
  motor1_forward();
  motor2_forward();
}

void backward() {
  motor1_backward();
  motor2_backward();
}

void motor1_forward() {
  digitalWrite(MOTOR1_ENABLE_PIN1, HIGH);
  digitalWrite(MOTOR1_ENABLE_PIN2, LOW);
}

void motor1_backward() {
  digitalWrite(MOTOR1_ENABLE_PIN1, LOW);
  digitalWrite(MOTOR1_ENABLE_PIN2, HIGH);
}

void motor2_forward() {
  digitalWrite(MOTOR2_ENABLE_PIN1, HIGH);
  digitalWrite(MOTOR2_ENABLE_PIN2, LOW);
}

void motor2_backward() {
  digitalWrite(MOTOR2_ENABLE_PIN1, LOW);
  digitalWrite(MOTOR2_ENABLE_PIN2, HIGH);
}


#ifdef __HC_SR04__
long distance_measure()
{
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:

  digitalWrite(HC_SR04_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(HC_SR04_TRIGGER, HIGH);
  delayMicroseconds(5);
  digitalWrite(HC_SR04_TRIGGER, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration = pulseIn(HC_SR04_ECHO, HIGH);

  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  cm =  duration / 29 / 2;
  //Serial.print(cm);
  //Serial.println("cm");
  // delay(100);
  return cm;
}
#endif

