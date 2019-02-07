#include <TimerOne.h>
#include <SingleChannelMotorDriver.h>


#define STOP_PIN 2 //e-stop pin
#define ENCA_PIN 3 //encoder pin
#define ENCB_PIN 4 //encoder pin
#define PWM_PIN 5 //pin to output PWM for motor driver
#define DIR_PIN 6 //pin to output direction for motor driver
#define FAST_PIN 8 //button to go faster
#define SLOW_PIN 9 //button to go slower

#define PWM_MAX 150 //maximum PWM allowed
#define RPM_MAX 30 //maximum RPM allowed

#define CONTROL_FREQUENCY 50
#define MOTOR_CPR 4741.44

volatile float desiredSpeed = 0.0; //desired speed in RPM
float speedIncrement = 10.0; //how much to increase/decrease speed on button press

volatile long signed int encoderCount = 0; //number of encoder clicks since last control loop iteration
volatile float encoderSpeed = 0.0; //speed as read by sensors
volatile float error = 0.0; //difference between desiredSpeed and encoderSpeed
volatile float esum = 0.0; //integral of error

const float kp = 1.0; //control gains (proportional)
const float ki = 0.4; //control gains (integral)
const float encGain = (float)(60.0*CONTROL_FREQUENCY)/(float)(MOTOR_CPR); //converts encoder clicks to RPM

SingleChannelMotorDriver driver(PWM_PIN, DIR_PIN, PWM_MAX); //object for our motor driver


void setup() {

  //setup inputs. constructor for motor driver automatically sets up outputs
  pinMode(STOP_PIN, INPUT);
  pinMode(ENCA_PIN, INPUT);
  pinMode(ENCB_PIN, INPUT);
  pinMode(FAST_PIN, INPUT);
  pinMode(SLOW_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(STOP_PIN), estop, RISING);
    //interrupt for our estop
    
  attachInterrupt(digitalPinToInterrupt(ENCA_PIN), encoder, CHANGE);
    //interrupt for our encoder

  Timer1.initialize(20000); //run 50 times a second 
  Timer1.attachInterrupt(control); //attach this interrupt to the "control" function

  Serial.begin(9600);

}

void loop() {

  //the buttons go LOW when they are pressed
  bool fast = digitalRead(FAST_PIN);
  bool slow = digitalRead(SLOW_PIN);

  Serial.print("fast: ");
  Serial.print(fast);
  Serial.print(", slow: ");
  Serial.println(slow);

  if (fast && !slow) {
    if (desiredSpeed < RPM_MAX) {
      desiredSpeed += speedIncrement;
    }
  }
  else if (slow && !fast) {
    if (desiredSpeed > -RPM_MAX) {
      desiredSpeed -= speedIncrement;
    }
  }

  Serial.print("Desired speed: ");
  Serial.print(desiredSpeed);
  Serial.print(", Encoder speed: ");
  Serial.println(encoderSpeed);

  delay(250); //poll 4 times a second
}


//stop the motor when the button is pressed, reset esum and desiredspeed
void estop() {
  driver.stopMotor();
  desiredSpeed = 0;
  esum = 0;
  Serial.println("Stopped");
}

//keeps track of encoder pulses
void encoder() {
  bool encA = digitalRead(ENCA_PIN);
  bool encB = digitalRead(ENCB_PIN);

  if (encA ^ encB) {
    encoderCount++;
  }
  else {
    encoderCount--;
  }
}

//controls PI control of the motor
void control() {
  
  encoderSpeed = (float)(encoderCount)*encGain;
  encoderCount = 0;

  error = desiredSpeed - encoderSpeed;
  esum += error;

  signed int input = (int)(error*kp + esum*ki);

  driver.driveMotor(input);

}
