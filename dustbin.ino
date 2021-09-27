//========= Defining instants and pins==========//

const int door_trigPin = 9;                
const int door_echoPin = 10;
const int level_trigPin = 5;
const int level_echoPin = 6;

const int door_open_duration = 10; // cm
const int open_switch_pin = 13;  // pin
int open_switch_read;

const int max_limit_LED = 3; // pin
const int stink_LED = 8; // pin
const int door_open_LED = 11; // pin

long duration_door;
int distance_door;
long duration_level;
int distance_level;

int gas_sensor_value;

const int gas_sensor_read = A0;  // pin

#include <Servo.h>
Servo servo1;
const int servo1_pin = 2;  // pin
Servo servo2;
const int servo2_pin = 4;  // pin
int angle = 0;

void setup() {
  pinMode(door_trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(door_echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(level_trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(level_echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(max_limit_LED , OUTPUT);
  digitalWrite(max_limit_LED, LOW);
  pinMode(stink_LED , OUTPUT);
  digitalWrite(stink_LED, LOW);
  pinMode(door_open_LED , OUTPUT);
  digitalWrite(door_open_LED, LOW);
  
  Serial.begin(9600); // Starts the serial communication
  pinMode(open_switch_pin, INPUT);
}

void loop() {
  open_switch_read = digitalRead(open_switch_pin);    // check the button read
  
  digitalWrite(door_trigPin, LOW);    // Clears the trigPin
  delayMicroseconds(2);
  digitalWrite(door_trigPin, HIGH);  // Sets the trigPin on HIGH state for 10 micro seconds
  delayMicroseconds(10);
  digitalWrite(door_trigPin, LOW);
  duration_door = pulseIn(door_echoPin, HIGH);   // Reads the echoPin, returns the sound wave travel time in microseconds

  digitalWrite(level_trigPin, LOW);    // Clears the trigPin
  delayMicroseconds(2);
  digitalWrite(level_trigPin, HIGH);  // Sets the trigPin on HIGH state for 10 micro seconds
  delayMicroseconds(10);
  digitalWrite(level_trigPin, LOW);
  duration_level = pulseIn(level_echoPin, HIGH);   // Reads the echoPin, returns the sound wave travel time in microseconds
  
  distance_door= duration_door*0.034/2;   // Calculating the distance
  distance_level= duration_level*0.034/2;   // Calculating the distance
  
  gas_sensor_value = analogRead(gas_sensor_read);
  
  Serial.print("Distance_door: ");  // Prints the distance on the Serial Monitor
  Serial.println(distance_door);
  Serial.print("Distance_level: ");  // Prints the distance on the Serial Monitor
  Serial.println(distance_level);
  Serial.print("PPM value: ");     // prints the PPM value from the MQ4 gas sensor
  Serial.println(gas_sensor_value);
  Serial.print("Button: ");     // prints the button read
  Serial.println(open_switch_read);
  Serial.print("\n");

  delay(500);  //--------- This delay is for controlling the sensitivity of the sensors...*

  if ( gas_sensor_value > 400){  // stink LED    (the normal methane PPM of the atmosphere is considered as about 400 PPM)
    digitalWrite(stink_LED, HIGH);
  } else{
    digitalWrite(stink_LED, LOW);
  }

  if ( distance_level < 10){  // max limit LED
    digitalWrite(max_limit_LED, HIGH);
  } else{
    digitalWrite(max_limit_LED, LOW);
  }

  if (angle < 120 && ( (distance_door < door_open_duration && distance_level > 10) || open_switch_read == HIGH)) {  // Door opening 
    digitalWrite(door_open_LED, HIGH);
    
    servo1.attach(servo1_pin);
    servo2.attach(servo2_pin);
    
    for (angle = 0; angle < 120; angle++) {
      servo1.write(angle);
      servo2.write(angle);
      delay(5);
    }

    servo1.detach();
    servo2.detach();
  }

  if (distance_level < 10 && distance_door < door_open_duration){   // blink the max limit LED 
    digitalWrite(max_limit_LED, LOW);
    delay(250);
    digitalWrite(max_limit_LED, HIGH);
    delay(250);
    digitalWrite(max_limit_LED, LOW);
    delay(250);
    digitalWrite(max_limit_LED, HIGH);
    delay(250);
    digitalWrite(max_limit_LED, LOW);
    delay(250);
    digitalWrite(max_limit_LED, HIGH);
  }

  if (distance_door > door_open_duration && angle == 120 && open_switch_read == LOW){     // door closing
    delay(5000);             // wait for 5 seconds befor closing the door
    
    Serial.println(angle);
    
    servo1.attach(servo1_pin);
    servo2.attach(servo2_pin);
    
    for (angle = 120; angle > 0; angle--) {
      servo1.write(angle);
      servo2.write(angle);
      delay(15);
    }
    servo1.detach();
    servo2.detach();
    
    digitalWrite(door_open_LED, LOW);
  }
}
