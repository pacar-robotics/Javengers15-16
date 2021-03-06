#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     irseeker,       sensorHiTechnicIRSeeker1200)
#pragma config(Motor,  mtr_S1_C1_1,     LeftWheels,    tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     Spindle,       tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     RightWheels,   tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_2,     Lift,          tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Servo,  srvo_S1_C3_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    Gate,                 tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "hitechnic-irseeker-v2.h"

#define WHEEL_RADIUS 1.5


//Addressing 4 wheel drive ?

#define TRACK_DISTANCE 10.1
//4 wheel calculations are shifted from actual

#define CLOCK_WISE true
#define COUNTER_CLOCKWISE false
#define FORWARD true
#define BACKWARD false

// Create struct to hold sensor data
 tHTIRS2 irSeeker;

 int Direction=0;
 int Strength=0;

task getIRBeaconDirection();
void calcMove(int Power, int Distance, bool Direction);
void dualTurn(int Power, int Degrees, bool Direction);
float encoderDistanceEquivalent(int Distance);
float encoderDegreesEquivalent(int Degrees);

void initializeRobot()
{
// Initialise and configure struct and port
  initSensor(&irSeeker, S2);
  wait1Msec(1000);
  irSeeker.mode=DSP_1200;
  return;
}

int Position=0;
bool BeaconFound=false;

void displayIRBeaconValues();

task main()
{


  initializeRobot();
  startTask(getIRBeaconDirection);
  //displayIRBeaconValues();

  calcMove(25,55,FORWARD);
  dualTurn(25,90,COUNTER_CLOCKWISE);
  calcMove(25,18,FORWARD);
  dualTurn(25,90,COUNTER_CLOCKWISE);
  calcMove(25,50,FORWARD);
  dualTurn(25,90,CLOCK_WISE);
  displayIRBeaconValues();

  /*
  if(BeaconFound){
  	//we have found the Beacon
  	motor[LeftWheels]=0;
		motor[RightWheels]=0;

  	if(Strength>9){
  		//execute Position 3 Routine
  		displayTextLine(4,"Position 3");
  		wait1Msec(1000);
  		dualTurn(30,95,CLOCK_WISE);
  		calcMove(30,40,FORWARD);
  	}
	}else{
  // we have not found the beacon yet
	//continue to move forward

		calcMove(30,24,FORWARD);
		dualTurn(30,30,CLOCK_WISE);
		calcMove(30,24,FORWARD);
		if(BeaconFound){
			motor[LeftWheels]=0;
			motor[RightWheels]=0;
  		//we have found the Beacon
  		if(Strength>9){
  			//execute Position 2 Routine
  			displayTextLine(4,"Position 2");
  			wait1Msec(5000);

  		}
		}else{
      //we have not found the beacon yet
		  //continue to move forward
			dualTurn(30,30,CLOCK_WISE);

			calcMove(30,24,FORWARD);
			if(BeaconFound){
				//we have found the Beacon
				motor[LeftWheels]=0;
				motor[RightWheels]=0;

				if(Strength>9){
					//execute Position 1 Routine
					displayTextLine(4,"Position 1");
  				wait1Msec(5000);

				}
			}

		}

	}
	*/

  wait1Msec(3000);



  stopTask(getIRBeaconDirection);
  waitForStart();


  while (true)
  {}
}

task getIRBeaconDirection()
{
	while(1){
		readSensor(&irSeeker);
		Direction=irSeeker.acDirection;
		Strength=irSeeker.enhStrength;

		if(!Direction){
			playTone(250,2);
		}
		if(Direction){
			BeaconFound=true;
			playTone(1000,5);
		}



	}
}

 void calcMove(int Power, int Distance, bool Direction)
{
	int EncoderValue=encoderDistanceEquivalent(Distance);
	nMotorEncoder[LeftWheels]=0;
	nMotorEncoder[RightWheels]=0;

	if(Direction){


		nMotorEncoderTarget[LeftWheels]=EncoderValue;
		nMotorEncoderTarget[RightWheels]=EncoderValue;
		motor[LeftWheels]=Power;
		motor[RightWheels]=Power;

	}else{

		nMotorEncoderTarget[LeftWheels]=EncoderValue;
		nMotorEncoderTarget[RightWheels]=EncoderValue;
		motor[LeftWheels]=-Power;
		motor[RightWheels]=-Power;


	}
	while(nMotorEncoder[LeftWheels]<abs(EncoderValue)){
		//do nothing
	}

	//stop the motors
	motor[LeftWheels]=0;
	motor[RightWheels]=0;



}

void dualTurn(int Power, int Degrees, bool Direction)
{
	int leftPower=0;
	int rightPower=0;
	int encoderValue=0;
	nMotorEncoder[LeftWheels]=0;
	nMotorEncoder[RightWheels]=0;

	encoderValue=encoderDegreesEquivalent(Degrees);

	if(Direction==CLOCK_WISE){
		//clockwise
		rightPower=Power;
		leftPower=-Power;
		//nMotorEncoderTarget[RightWheels]=encoderValue;
		//nMotorEncoderTarget[LeftWheels]=-encoderValue;


	}else{
		//Anti-clockwise
		rightPower=-Power;
		leftPower=Power;
		//nMotorEncoderTarget[RightWheels]=-encoderValue;
		//nMotorEncoderTarget[LeftWheels]=encoderValue;


	}
		motor[LeftWheels]=leftPower;
		motor[RightWheels]=rightPower;


		while((nMotorEncoder[LeftWheels]<abs(encoderValue))&&(nMotorEncoder[RightWheels]<abs(encoderValue))){
			//do nothing until turn
			displayTextLine(2,"%d --> %d",nMotorEncoder[RightWheels], encoderValue);
		}
		//stop the motor
		motor[LeftWheels]=0;
		motor[RightWheels]=0;



}


float encoderDistanceEquivalent(int Distance)
{
	return (1440*(Distance/(2*PI*WHEEL_RADIUS)));
}

float encoderDegreesEquivalent(int Degrees)
{
	return (1440*((TRACK_DISTANCE/WHEEL_RADIUS)/360)*Degrees);
}

void displayIRBeaconValues()
{
	while(1){
			eraseDisplay();
      readSensor(&irSeeker);
      displayTextLine(1, "Sensors");
      displayTextLine(2, "Dir-%d EnhDir-%d",irSeeker.acDirection,irSeeker.enhDirection);
      displayTextLine(3, "1-%d 2-%d 3-%d",
      		irSeeker.acValues[0],
      		irSeeker.acValues[1],
      		irSeeker.acValues[2],
					);

      displayTextLine(4, "4-%d 5-%d",
      		irSeeker.acValues[3],
      		irSeeker.acValues[4]);

      displayTextLine(5, "Strength:%d", irSeeker.enhStrength);

      if(irSeeker.enhStrength>9){
      	Position=3;
      }else if((irSeeker.enhStrength>5)&(irSeeker.acValues[1]>5)){
      	Position=2;
    	}else{
    		Position=1;
    	}

    	displayTextLine(6,"Position: %d", Position);
      wait1Msec(500);
   }
 }
