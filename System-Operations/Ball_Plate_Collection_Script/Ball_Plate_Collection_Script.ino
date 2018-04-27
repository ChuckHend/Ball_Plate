//Ball and Plate PID Experiment
//Adam Hendel
//University of Wisconsin - River Falls, Physics Department 2016-2017
//Script used to collect a range of Kp and Kd gain parameters
//increases Kp and Kd indefinitely, in increments of
//Kpinc and Kdinc, defined on line 42-43
//data collected either through console or serial application

#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include "Adafruit_STMPE610.h"
#include <PID_v1.h>
#define dt 80     //force constant cycle time in milliseconds
#define xPin 10   //x axis to pin 10
#define yPin 9    //y axis to pin 9

Adafruit_STMPE610 touchPanel = Adafruit_STMPE610();

//declare servos
Servo servoX, servoY;

//some variables for calculations
boolean inActive = true; //ball off plate by default

double minPos        = -45, //constrain on servo output
       maxPos        = 45,  //constrain on servo output
       setPointX     = 500, //center of X plane
       setPointY     = 500, //center of Y plane
       errorX        = 0,   //current x error
       errorY        = 0,   //current y error
       currX         = 0,   //current X
       currY         = 0,   //current Y
       lastX         = 0,   //to store last X position
       lastY         = 0,   //to store last Y position
       dx            = 0,   //change in x position
       dy            = 0,   //change in y position

       Kp           = 0.000,   //proportional gain
       Ki           = 0.00,    //integral gain
       Kd           = -0.00,  //velocity gain
       Kpinc        = 0.005,  //increment for Kp
       Kdinc        = 0.05,   //increment for Kd
       outputX,               //output from xPID class
       outputY;               //output from yPID class

unsigned long t =         0,  //for fixing loop time
              touched =   0,  //counting time last touched
              circTime =  0,  //counting time since last circle move
              boxTime =   0,  //counting time since last box move
              resetTime =  0;  //counter for data colelction
int stableX = 5,            //set range for no response
    stableY = 5,            //set range for no response
    dTermX,                 //x velocity response output
    dTermY,                 //y velocity response output
    servoNeutralX  = 57,    //x axis horizontal, servo command
    servoNeutralY  = 47,    //y axis horizontal, servo command
    servoRequest_x = 0,     //x servo, sum of PIDD responses
    servoRequest_y = 0,     //y servo, sum of PIDD responses
    i =              0,     //inactive plate counter
    angle =          0,     //calculate circle movement
    j =              0,     //box position counter
    programSelect =  1;     //program selection (center by default)
    
uint16_t x, y;              // for touchscreen variables
uint8_t z;                  // touchscreen variables

//PID class only used for Proportional
PID PIDx(&currX, &outputX, &setPointX, Kp, Ki, 0, DIRECT);//MKI direct, MKII reverse
PID PIDy(&currY, &outputY, &setPointY, Kp, Ki, 0, DIRECT);//MKI direct, MKII reverse

void setup() {
  Serial.begin(115200);      //baud rate
  
for(int a = 2; a < 8; a++){  //set dig pins 2-7 as input
  pinMode(a, INPUT);         //for program selection controller
}
  //init touchscreen
  Serial.flush();
  if (! touchPanel.begin()) {
    while (1);
  }
  
  //initialize servos
  //pulse width default is 544-2400
  //hitec is  750-2250μsec per datasheet
  servoX.attach(xPin, 750, 2250);
  servoY.attach(yPin, 750, 2250);

  //set both servos to horizontal plate positions
  servoX.write(servoNeutralX);
  servoY.write(servoNeutralY);

  //set some parameters for PID class controller
  PIDx.SetMode(AUTOMATIC);
  PIDy.SetMode(AUTOMATIC);
  PIDx.SetOutputLimits(minPos, maxPos);
  PIDy.SetOutputLimits(minPos, maxPos);
  PIDx.SetSampleTime(10);
  PIDy.SetSampleTime(10);
  
}//end setup

void loop() {//main loop
  Kp += Kpinc; //increment Kp
  Kd = 0;      //reset Kd to starting point
  for(int d = 0; d<10; d++){//inner loop
    Kd -= Kdinc; 
    for(int q=0; q<2; q++){ //iteration loop twice, for each Kp/Kd pair
      servoX.write(30); //force ball to corner
      servoY.write(30); //force ball to corner
      delay(2500);      //wait 2.5 seconds, so ball can get there
      resetTime = millis();
      while(millis()-resetTime<2000){   
      t = millis();     //set time when loop starts       

      // update Kp in proportional controller
      PIDx.SetTunings(Kp, 0, 0);
      PIDy.SetTunings(Kp, 0, 0);
    
      //acquire touchpanel information
      getTouchPanel();
      
      PIDx.Compute();               //x calculate P term
      PIDy.Compute();               //y calculate P term

      // used for derivative term
      // earlier versions used 2nd order derivative
      // consider moving derivative term calc inside the
      // PID class
      errorX  = currX - setPointX;
      errorY  = currY - setPointY;
      
      dx      = (currX - lastX);   //x calculate velocity
      dy      = (currY - lastY);   //y calculate velocity

      // if system inactive
      // force system error to zero
      if(millis() - touched > 150){
        inActive = true;
        currX = setPointX;
        currY = setPointY;
        i = 0;
      }
      else {
        inActive = false;
        }    
    if(!inActive){
      //if plate active, wait 2 cycles to calculate
      //derivative terms to avoid the buffer carryover
      if(i < 2){
      dx = 0;
      dy = 0;
      i++;
      }
    }
      //consider using the pid package
      //calculate velocity and acceleration output terms
      //velocity and accel * respective gain values
      dTermX  = constrain(dx * Kd, minPos, maxPos);
      dTermY  = constrain(dy * Kd, minPos, maxPos);
    
      //remember variables for next iteration
      lastX = currX;
      lastY = currY;
      
      //determine the response output for both servos
      //constrain the response to minPos and maxPos   
      servoRequest_x = servoNeutralX +
                       constrain((outputX + dTermX), minPos, maxPos);
      servoRequest_y = servoNeutralY +
                       constrain((outputY + dTermY), minPos, maxPos);
      //write the responses to both servos
    if(millis()-touched > 2000){
      //if inactive, set neutral positions
      servoX.write(servoNeutralX);
      servoY.write(servoNeutralY);}
    else{
      if (errorX > stableX || errorX < -stableX) {//if outside stable range, do full responses
        servoX.write(servoRequest_x);
      }//if within the stable range, do a different response
      else {servoX.write(servoNeutralX + constrain((dTermX), minPos, maxPos));}
    
      if (errorY > stableY || errorY < -stableY) {//if outside stable range, do full responses
        servoY.write(servoRequest_y);
      }//if within the stable range, do a different response
      else {servoY.write(servoNeutralY + constrain((dTermY), minPos, maxPos));}
    }//end servo writing
    
      //send data to Serial
      printData();
      // make sure the cycle time is equal to dt
      while ((millis() - t) < dt) { 
        //do nothing
      }
        }//inner while loop
      }//end iteration loop
    }//end outer loop
 }//end main loop

void printData() {
  //prints position data to serial
  Serial.print(setPointX);
  Serial.print(",");
  Serial.print(currX);
  Serial.print(",");
  Serial.print(setPointY);
  Serial.print(",");
  Serial.print(currY);
  Serial.print(","); 
  Serial.print(Kp, 3);
  Serial.print(",");
  Serial.print(Kd, 3);
  Serial.print(",");
  Serial.println(t);
}
void getTouchPanel(){
  //acquire data from touchscreen
  if (touchPanel.touched()) {
    while (! touchPanel.bufferEmpty()) {
      //Serial.print(touchPanel.bufferSize());
      touchPanel.readData(&x, &y, &z);
      currX = map(x, 0, 3950, 0, 1000);//map positions 0 to 1000
      currY = map(y, 0, 3900, 0, 1000);
    }
    touchPanel.writeRegister8(STMPE_INT_STA, 0xFF); // reset all ints
    touched = millis();
  }//end touchpanel acquisition
}//end getTouchPanel
