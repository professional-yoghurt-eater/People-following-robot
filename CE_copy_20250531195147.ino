#include <Servo.h>
Servo sweepservo;
int PIR=2; //PIR sensor
int motor1 = 7;//right wheel motor
int backpins = 4;// backwards activation pins for both motors
int motor2 = 8;//left wheel motor
int motorcontrol1= 5;//speed right motor
int motorcontrol2= 6;//speed left motor
int trigpin=11; //ultrasonic range sensor trigger
int echopin=12; //ultrasonic range sensor echo
//
void setup() {
  Serial.begin(9600);
  sweepservo.attach(3); //servomotor
  pinMode(PIR,INPUT); //setting up PIR
  pinMode(motor1, OUTPUT); //setting up right motor
  pinMode(backpins, OUTPUT); //setting up backwards activation pins for both motors
  pinMode(motor2, OUTPUT); // setting up left_motor
  pinMode(motorcontrol1, OUTPUT); //setting up right motor speed control
  pinMode(motorcontrol2, OUTPUT); //setting up left motor speed control
  pinMode(trigpin,OUTPUT); //setting up trigger pin
  pinMode(echopin,INPUT); //setting up echo pin
  digitalWrite(backpins, LOW); //making sure that the wheels never go backwards
  analogWrite(motorcontrol1,200); //speed that makes both wheel go the same speed in real life
  analogWrite(motorcontrol2,160); //speed that makes both wheel go the same speed in real life
  delay(30000); // waiting for the PIR sensor to be warmed up
}


void forward() {//makes robot go forward
  digitalWrite(motor1,  HIGH);//rigt wheel moves
  digitalWrite(motor2, HIGH); //left wheel moves
}

void left_turn() {//makes robot turn left
  digitalWrite(motor1,  HIGH);//right wheel moves
  digitalWrite(motor2, LOW); //left wheel is stationary

}

void right_turn() {//makes robot turn right
  digitalWrite(motor1,  LOW);//right wheel is stationary
  digitalWrite(motor2, HIGH);//left wheel moves
 
}
void stop() {//makes robot stop
  digitalWrite(motor1,  LOW);//right wheel is stationary
  digitalWrite(motor2, LOW); //left wheel is stationary

}
void trig_pulse(){//send ultrasonic pulse
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
}

void loop() { 
  long duration;
  float distance;
  int left_sum=0;//sum of all detections on the left side
  int right_sum=0;//sum of all detections on the right side
  ////
  ///first sweep
  int AngleToState1[7][2]={{0,0},{30,0},{60,0},{90,0},{120,0},{150,0},{180,0}};// assigns a state to each tested angle(initially zero)
  for(int i=0;i<7;i++)
  {
    int angle=i*(180/6);//turns by 30 degrees each time
    sweepservo.write(angle);
    AngleToState1[i][1]=digitalRead(PIR);//assigns to the angle value in AngleToState the reading value(0 if not detection, 1 if detection)
    Serial.println(AngleToState1[i][1]);
    Serial.println(angle);
    delay(1300);// wait in order to reinitialize the 0 state of the sensor
  }
  
  for(int i=0;i<3;i++)
  {
    left_sum=left_sum+AngleToState1[i][1]; //adds the number of detections on the left side
  }

  for(int i=4;i<7;i++)
  {
    right_sum=right_sum+AngleToState1[i][1];//adds the number of detections on the right sie
  }
  
  ///second sweep
  //refer to first sweep for comments
  int AngleToState2[7][2]={{0,0},{30,0},{60,0},{90,0},{120,0},{150,0},{180,0}};//9?
  for(int i=0;i<7;i++)
  {
    int angle=i*(180/6);
    sweepservo.write(angle);
    AngleToState2[i][1]=digitalRead(PIR);
    Serial.println(AngleToState2[i][1]);
    Serial.println(angle);
    delay(1300);
  }

  for(int i=0;i<3;i++)
  {
    left_sum=left_sum+AngleToState2[i][1];
  }

  for(int i=4;i<7;i++)
  {
    right_sum=right_sum+AngleToState2[i][1];
  }
  /////
  delay(1300);
  sweepservo.write(90);

  if(AngleToState1[3][1]==1 && AngleToState2[3][1]==1 && right_sum==left_sum){// the pir sensor detected a body in front of the robot
    Serial.println("straight");
    forward();
    for(int i=0;i<200;i++){//go straight for 2 seconds or stop when 5 cm from an object
      delay(10);
      trig_pulse();//send pulse
      duration = pulseIn(echopin, HIGH);
      distance = duration * 0.0343 / 2; //use speed of sound equation to find the distance
      Serial.println(distance);
      if(distance<5){// check if distance is larger than 5 cm
        break;
      }
    }
    stop();
  }

  if(right_sum>left_sum){//the PIR sensor detected a body to the right of the robot
    Serial.println("go right");
    right_turn();//turn on yourself to the right 
    while(true){
      if (digitalRead(PIR)==1){//if the target body is detected
        forward();
        for(int i=0;i<200;i++){//go straight for 2 seconds or stop when 5 cm from an object
          delay(10);
          trig_pulse();//send pulse
          duration = pulseIn(echopin, HIGH);
          distance = duration * 0.0343 / 2;//use speed of sound equation to find the distance
          Serial.println(distance);
          if(distance<5){// check if distance is larger than 5 cm
            break;
          }
        }
        stop();
        break;
      }
    }
  }

  if(left_sum>right_sum){//the PIR sensor detected a body to the left of the robot
    Serial.println("go left");
    left_turn();//turn on yourself to the left 
    while(true){
      if (digitalRead(PIR)==1){//if the target body is detected
        forward();
        for(int i=0;i<200;i++){//go straight for 2 seconds or stop when 5 cm from an object
          delay(10);
          trig_pulse();//send pulse
          duration = pulseIn(echopin, HIGH);
          distance = duration * 0.0343 / 2;//use speed of sound equation to find the distance
          Serial.println(distance);
          if(distance<5){// check if distance is larger than 5 cm
            break;
          }
        }
        stop();
        break;
      }
    }
  }

  Serial.println("##############################");
  delay(1500);
}