//Ball and Plate PID Experiment
//Adam Hendel
//University of Wisconsin - River Falls, Physics Department 2016
//Program acquires position of an object on a resistive touch panel
//and makes appropriate response to return object to a setpoint by
//actuating two digital servos

//14-Oct-16: added comments, moved parameter tuning to a function
//20-Oct-16: added controller functionalty (moving setpoints)
//20-Oct-16: cleaned up comments, consolidated x/y pid gains
//27-Oct-16: removed acceleration term from PID
//09-Nov-16: added flag to reset programSelect to 0 after incrementing setpoint
            // stops in run away effect of incrementing in x or y
//21-Nov-16: added time to pyprint outut
//Current TODOs:
// 1) Optimize PID gain parameters
// 2) Move self dev velocity/accel to function, or;
// 3) Make own PID library?
// 4) Use z to adjust plate if ball is gonna fly off?
//      bigger z = less pressure
// 5) rename gain parameters, x and y are now the same
// 6) update map range of x and y (to more accurate?)


#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include "Adafruit_STMPE610.h"
#include <PID_v1.h>
#define dt 80     //cycle time in milliseconds
#define xPin 10   //x axis to pin 10
#define yPin 9    //y axis to pin 9

// Wiring Instructions for STMPE610 (touch screen controller)
// SCL to I2C clock (#A5 on Arduinio Uno)
// SDA to I2C data (#A4 on Uno)
// tie MODE to GND and POWER CYCLE (there is no reset pin)
Adafruit_STMPE610 touchPanel = Adafruit_STMPE610();

//declare servos
Servo servoX, servoY;

//some variables for calculations
boolean inActive = true; //ball off plate by default

double minPos        = -50, //constrain on servo output
       maxPos        = 50,  //constrain on servo output
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

       Kp           = 0.029,   //proportional gain [.027]
       Ki           = 0.00,    //integral gain [.03]
       Kd           = -0.175,  //velocity gain [-0.195]

       outputX,               //output from xPID class
       outputY;               //output from yPID class

unsigned long t =         0,  //for fixing loop time
              touched =   0,  //counting time last touched
              circTime =  0,  //counting time since last circle move
              boxTime =   0;  //counting time since last box move

int stableX = 10,
    stableY = 10,
    dTermX,                 //x velocity response output
    dTermY,                 //y velocity response output
    servoNeutralX  = 53,    //x axis horizontal, servo command
    servoNeutralY  = 59,    //y axis horizontal, servo command
    servoRequest_x = 0,     //x servo, sum of PIDD responses
    servoRequest_y = 0,     //y servo, sum of PIDD responses
    i =              0,     //inactive plate counter
    angle =          0,     //calculate circle movement
    j =              0,     //box position counter
    programSelect =  1;     //program selection (center by default)
    
uint16_t x, y;              // for touchscreen variables
uint8_t z;                  // touchscreen variables

//PID class only used for Proportional and Integral
PID PIDx(&currX, &outputX, &setPointX, Kp, Ki, 0, DIRECT);//MKI direct, MKII reverse
PID PIDy(&currY, &outputY, &setPointY, Kp, Ki, 0, DIRECT);//MKI direct, MKII reverse

void setup() {
  Serial.begin(115200);      //baud rate
  
for(int a = 2; a < 8; a++){  //set pints 2-7 as input
  pinMode(a, INPUT);         //for program selection controller
}
  
  //init touchscreen
  Serial.flush();
  if (! touchPanel.begin()) {
    //Serial.println("ERROR: STMPE controller not found");
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

void loop() {
  t = millis();     //set time when loop starts


  programSelect = getProgram();     //acquire ball action program from controller
  runProgram(programSelect);        //set program from previous command
  PIDx.SetTunings(Kp, Ki, 0);
  PIDy.SetTunings(Kp, Ki, 0);

  //acquire touchpanel information
  getTouchPanel();
  
  PIDx.Compute();               //x calculate P and I terms
  PIDy.Compute();               //y calculate P and I terms
  
  errorX  = currX - setPointX;
  errorY  = currY - setPointY;

  dx      = (currX - lastX);   //x calculate velocity
  dy      = (currY - lastY);   //y calculate velocity

//TODO METHOD 
//second if statement to else section of 
//the first if statement? 
  if(millis() - touched > 150){
    inActive = true;
    //Serial.println("inactive");
    //Serial.println(x);
    currX = setPointX;
    currY = setPointY;
    i = 0;
  }
  else {
    inActive = false;
    //Serial.println("active");
    }    
if(!inActive){
  //if plate active, wait 2 cycles to calculate
  //derivative terms to avoid the buffer carryover
  if(i < 2){
  //Serial.println("set dTerms to 0");
  dx = 0;
  dy = 0;
  i++;
  }
}
///END TODO METHOD

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

  //call print diagnostcs function
  //Serial.println(millis()-t);
  pyPrint();

  while ((millis() - t) < dt) { // Making sure the cycle time is equal to dt
    //do nothing
  }
  //Serial.println(millis() - t);
}//end main loop

void pyPrint(){
  //printing syntax for python visualization
  Serial.print(setPointX);
  Serial.print(",");
  Serial.print(currX);
  Serial.print(",");
  Serial.print(setPointY);
  Serial.print(",");
  Serial.print(currY); 
  Serial.print(",");
  Serial.println(t);
}

void getTouchPanel(){
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

void doCircle(){
  if((millis() - circTime) > 50){
      circTime = millis();
      angle = angle + 10; //increment angle by
      if (angle > 359){//reset angle to 0 after reaching 360
        angle = 0;
      }
  }
      setPointX = 450 + 200*sin(radians(angle));
      setPointY = 500 + 200*cos(radians(angle));
}//end do circle

void doBox(){
  if(millis() - boxTime > 2000){  
    boxTime = millis();
    switch (j){
      case 0: //first step
        setPointX = 700;
        setPointY = 250;
        break;
      case 1:
        setPointX = 700; //700
        setPointY = 750; //750
        break;
      case 2:
        setPointX = 200; //200
        setPointY = 750; //750
        break;
      case 3:
        setPointX = 200; //200
        setPointY = 250; //250
        break;         
    }//end switch
    j++;
  }//end if time passed
  if(j >3){//reset counter after completing box
    j = 0;}
}//end box

int getProgram(){
  if(digitalRead(2) == 1)
    {
    return 1;          //reset setpoint
    }
  else if(digitalRead(3) == 1)
    {
    return 2;          //run box pattern
    }
  else if(digitalRead(4) == 1)
    {
    return 3;          //run circle pattern
    }
   else if(digitalRead(5) == 1)
    {
    return 4;          //up in Y direction
    }
    else if(digitalRead(6) == 1)
    {
     return 5;         //down in Y direction
    }
    else if(digitalRead(7) == 1)
    {
      return 6;        //right in X direction
    }
    else if(digitalRead(8) == 1)
    {
      return 7;        //left in X direction
    }
    else if(programSelect > 3)
    {
      return 0;
    }
}//end getProgram

void runProgram(int program){
    //sets which ball pattern to run
    switch(program){
    case 1:
      setPointX = 500;
      setPointY = 500;
      break;
    case 2:
      doBox();
      break;
    case 3:
      doCircle();
      break;
    case 4:
      setPointY += 25;
      break;
    case 5:
      setPointY -= 25;
      break;
    case 6:
      setPointX += 25;
      break;
    case 7:
      setPointX -= 25;
      break; 
  }//end case
}//end runProgram

void printDiagnostics() {
  //comment out unnecessary lines
  //Serial.print(",Time:,");
  //Serial.print(millis() / 1000);\
//  Serial.print(,"Z,");
//  Serial.print(z);
//  Serial.print("Z:");  Serial.print("\t");
//  Serial.print(z);  Serial.print("\t");
//  Serial.print("LastTouched"); Serial.print("\t");
//  Serial.print(touched); Serial.print("\t");
  Serial.print("X_Position"); Serial.print("\t");
  Serial.print(currX); Serial.print("\t");
  Serial.print("PIDoutX");  Serial.print("\t");
  Serial.print(outputX);  Serial.print("\t");
  Serial.print("dTermX");  Serial.print("\t");
  Serial.print(dTermX);  Serial.print("\t");
  Serial.print("dx");  Serial.print("\t");
  Serial.print(dx);   Serial.print("\t");
    Serial.print("Y_Position");  Serial.print("\t");
    Serial.print(currY);  Serial.print("\t");
    Serial.print("PIDoutY");  Serial.print("\t");
    Serial.print(outputY);  Serial.print("\t");
    Serial.print("dTermY");  Serial.print("\t");
    Serial.print(dTermY);  Serial.print("\t");
  //  Serial.print("dy");  Serial.print("\t");
  //  Serial.print(dy);  Serial.print("\t");
Serial.println();
}//end printDiagnostics
