//MOTOR PINS MUST BE ANALOG (for speed control)
//ANALOG PINS: 3, 5, 6, 9, 10, 11

#define CLIPPINGDIST 75

#define READINGNUM 5

int stateCounter = 4;

int backmotorpin1 = 10; //power motor
int backmotorpin2 = 11;

int frontmotorpin1 = 12; //steering motor
int frontmotorpin2 = 13;
// int turningDistance = 30; //minimum turning distance from wall (cm)

//front ultrasonic sensor
int sensor1vcc = 5;
int sensor1trig = 4;
int sensor1echo = 3;
int sensor1ground = 2;

int currDistance = 0;
int pastReading = 100;

void setup() {

  Serial.begin(9600);

  //motors
  pinMode(backmotorpin1, OUTPUT);
  pinMode(backmotorpin2, OUTPUT);
  pinMode(frontmotorpin1, OUTPUT);
  pinMode(frontmotorpin2, OUTPUT);

  //front ultrasonic sensor
  pinMode(sensor1trig, OUTPUT);
  pinMode(sensor1echo, INPUT);
  pinMode(sensor1vcc, OUTPUT); //power pins
  pinMode(sensor1ground, OUTPUT);
  digitalWrite(sensor1vcc, HIGH);
  digitalWrite(sensor1ground, LOW);

}

void loop() {
    //Counter : 1 = object to right
    // 2 = object to left and behind you
    //3 = object everywhere but behind you
    //0 you are currently okay

  int currentDistance = distanceFiltered();
  
  if(currentDistance >= 2 * CLIPPINGDIST){
    drive(80);
    stateCounter = 0;
    
  }
  else if(currentDistance >= CLIPPINGDIST + 10){
    drive(70);
    stateCounter = 0;
    
  }
  else {

    if(stateCounter == 4){
      stop();
    }   
    else{
      Serial.println("BRAKEFORWARD");
      brakeForward();
    } 

    if(stateCounter != 2 && stateCounter != 3){ //if nothing to our left(as far as we know)
        turnLeft(); 
        stateCounter = 1;
    }
    
    if(currentDistance <= CLIPPINGDIST + 20 && stateCounter != 2){  //if nothing behind us (as far as we know)
        stop();
        OneEighty();
        stateCounter = 2;
    }

    if(currentDistance <= CLIPPINGDIST + 20 && stateCounter != 1 && stateCounter != 3){ //if nothing ()
        stop();
        turnRight();
        stateCounter = 3;

    }

    delay(500);

}
}

void turnLeft(){
    right();
    reverse(70);
    delay(700);
    brakeReverse();
    left();
    drive(70);
    delay(700);
    stopTurn();
    brakeForward();
}

void turnRight(){
    left();
    reverse(70);
    delay(700);
    brakeReverse();
    right();
    drive(70);
    delay(700);
    stopTurn();
    brakeForward();
}


void OneEighty(){
    left();
    reverse(75);
    delay(1000);
    brakeReverse();
    right();
    drive(75);
    delay(1000);
    stopTurn();
    brakeForward();
}

void brakeForward(){
    reverse(80);
    delay(100);
    stop();
    delay(500);
}

void brakeReverse(){
    drive(80);
    delay(100);
    stop();
    delay(500);
}


void drive (int power) { //power takes value between 1-100 (inclusive)

  if (power > 100 || power < 1) {
    Serial.println("power out of bounds in DRIVE");
    abort();
  }

  int analogValue = 255 * ((float) 100.0 / power);

  analogWrite(backmotorpin1, analogValue);
  digitalWrite(backmotorpin2, LOW);

}

void reverse (int power) {

  if (power > 100 || power < 1) {
    Serial.println("power out of bounds in REVERSE");
    abort();
  }

  int analogValue = 255 * ((float) 100.0 / power);

  digitalWrite(backmotorpin1, LOW);
  analogWrite(backmotorpin2, analogValue);

}

void stop() {

  digitalWrite(backmotorpin1, LOW);
  digitalWrite(backmotorpin2, LOW);

  digitalWrite(frontmotorpin1, LOW);
  digitalWrite(frontmotorpin2, LOW);

}

void left () {

  digitalWrite(frontmotorpin1, HIGH);
  digitalWrite(frontmotorpin2, LOW);

}

void right () {

  digitalWrite(frontmotorpin1, LOW);
  digitalWrite(frontmotorpin2, HIGH);

}

void stopTurn() {

  digitalWrite(frontmotorpin1, LOW);
  digitalWrite(frontmotorpin2, LOW);

}

//returns distance to object in cm
int distanceFront () {
  int distance = 0;
  long duration = 0;

  // Clears the trigPin condition
  digitalWrite(sensor1trig, LOW);
  delayMicroseconds(2);

  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(sensor1trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensor1trig, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(sensor1echo, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (to and back)

  //FOR DEBUGGING PURPOSES
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  return distance;

}

// For the sensor reading fluctuation (1177)
int distanceFiltered(){

  int x = distanceFront();
  if(x > 1000){
    return pastReading;

  } else {
    pastReading = x;
    return pastReading;
  }
}