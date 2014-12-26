#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     HTIRS2,         sensorI2CCustom)
#pragma config(Motor,  motorA,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  motorB,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  motorC,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_1,     LeftWheels,    tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     Spindle,       tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     RightWheels,   tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_2,     Lift,          tmotorTetrix, openLoop, encoder)
#pragma config(Servo,  srvo_S1_C3_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    Gate,                 tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Unknowns (For Now)
#define FOO 0

/* Robot and Field Definitions */

//IR Frequencies
#define IR600 DSP_600
#define IR1200 DSP_1200

//Buttons
#define LEFT_BUTTON 2
#define RIGHT_BUTTON 1

//Measurements
#define DIAMETER 6.985
#define TRACK_DISTANCE 19.304
#define RAMP_DISTANCE 190

//Directions
#define FORWARD true
#define BACKWARD false
#define CLOCKWISE true
#define COUNTER_CLOCKWISE false

//Lift
#define LIFT_MAX 22300
#define LIFT_TOP 22250
#define LIFT_MIDDLE 14350
#define LIFT_LOWER 7150
#define LIFT_BASE 0

//Gate
#define GATE_CLOSED 50
#define GATE_OPEN 150


#include "hitechnic-irseeker-v2.h"

enum StartingPositionEnum {ParkingZone, Ramp};

StartingPositionEnum StartingPosition;

//Functions
void chooseProgram();
void ramp();
void rampDelay();
void parkingZone();
void parkingZoneDelay();
void initializeRobot();
void calcMove(float centimeters, float power, bool direction);
void dualMotorTurn(float robotDegrees, float power, bool direction);
void kickstand();
void grabGoalBase();
void moveLift(int encoderCounts);

tHTIRS2DSPMode irFrequency;
bool isDelay;

enum LiftStateEnum {Running, Stopped};

LiftStateEnum LiftState;

task main()
{
	initializeRobot();

	nMotorEncoder[LeftWheels] = 0;
	nMotorEncoder[RightWheels] = 0;

	chooseProgram();

	//wait for start

	if((StartingPosition== Ramp) && isDelay)
	{
	rampDelay();
	}//ramp and delay
	else if((StartingPosition== Ramp) && !isDelay)
	{
	ramp();
	}//ramp and no delay
	else if((StartingPosition== ParkingZone) && isDelay)
	{
	parkingZoneDelay();
	}//parking zone and delay
	else if((StartingPosition== ParkingZone) && !isDelay)
	{
	parkingZone();
	}//parking zone and no delay
}

/* Functions */

void ramp() //ramp, goals
{
	calcMove(RAMP_DISTANCE, 50, FORWARD);		//goes down ramp
	calcMove(FOO, 50, FORWARD);			//goes to nearest goal (middle)
	grabGoalBase();
	moveLift(LIFT_MIDDLE);			//puts two balls in the middle goal
	servo[Gate]=GATE_OPEN;
	wait1Msec(3000);
	servo[Gate]=GATE_CLOSED;
	moveLift(LIFT_BASE);
}

void rampDelay() //ramp, kickstand
{
	wait1Msec(FOO);
	calcMove(RAMP_DISTANCE, 50, FORWARD);
	dualMotorTurn(FOO, 40, COUNTER_CLOCKWISE);
	kickstand();
}

void parkingZone() //parking zone, goals
{
	calcMove(FOO, 50, FORWARD);
	grabGoalBase();
	moveLift(LIFT_MIDDLE);			//puts two balls in the middle goal
	servo[Gate]=GATE_OPEN;
	wait1Msec(3000);
	servo[Gate]=GATE_CLOSED;
	moveLift(LIFT_BASE);
}

void parkingZoneDelay() //parking zone, kickstand
{
	wait1Msec(FOO);
	calcMove(FOO, 50, FORWARD);
	kickstand();
}

void grabGoalBase()
{
	//stub
}

void kickstand()
{
	tHTIRS2 irSeeker;
	initSensor(&irSeeker, S2);
	irSeeker.mode = (irFrequency == IR600 ? DSP_600: DSP_1200);

	dualMotorTurn(100, 40, CLOCKWISE);
	calcMove(40, 40, FORWARD);
	dualMotorTurn(100, 40, CLOCKWISE);
	calcMove(100, 40, FORWARD);

	while(irSeeker.acDirection != 5)
	{
		readSensor(&irSeeker);
		motor[LeftWheels] = -20;
		motor[RightWheels] = 20;
	}
}

void moveLift(int encoderCounts)
{
	if(nMotorEncoder[Lift]>encoderCounts)
	{
		LiftState= Running;
		motor[Lift]= -65;
		while((nMotorEncoder[Lift]>encoderCounts)&&(nMotorEncoder[Lift]>LIFT_LOWER))
		{
			//idle loop
		}

		if(nMotorEncoder[Lift]>encoderCounts)
		{
			motor[Lift]=-30;
			while(nMotorEncoder[Lift]>encoderCounts)
			{
				//idle
			}
		}
	}
	else if(nMotorEncoder[Lift]<encoderCounts)
	{
		motor[Lift]= 80;
		LiftState= Running;
		while(nMotorEncoder[Lift]<encoderCounts)
		{
			//idle
		}
	}

	motor[Lift]=0;
	LiftState=Stopped;
}

void calcMove(float centimeters, float power, bool direction)
{
	float encoder_counts;

	encoder_counts = (centimeters/31.9024) * 1440; // converts centimeters to motor encoder counts

	while(nMotorEncoder[LeftWheels] < abs(encoder_counts) || nMotorEncoder[RightWheels] < abs(encoder_counts))
	{
		if (direction) // robot will move forward
		{
			motor[LeftWheels] = power;
			motor[RightWheels] = power;
		}
		else // robot will move backward
		{
			motor[LeftWheels] = -1 * power;
			motor[RightWheels] = -1 * power;
		}
	}
}


void dualMotorTurn(float robotDegrees, float power, bool direction) //robot turns using both motors
{
	int encoderCounts = (TRACK_DISTANCE/DIAMETER*(robotDegrees*4))*2;
	nMotorEncoder[LeftWheels] = 0;
	nMotorEncoder[RightWheels] = 0;
	if (direction == CLOCKWISE)	//turns robot clockwise or counter-clockwise, depending on user input
	{
		motor[LeftWheels] = power;
		motor[RightWheels] = -1 * power;
	}
	else
	{
		motor[LeftWheels] = -1* power;
		motor[RightWheels] = power;
	}
	while((nMotorEncoder[LeftWheels]<abs(encoderCounts))&&(nMotorEncoder[RightWheels]<abs(encoderCounts)))
	{
		//
	}
	motor[LeftWheels] = 0;
	motor[RightWheels] = 0;
}

void initializeRobot()
{
	return;
}

void chooseProgram()
{
	int irChoice;
	string startingPositionChoice;
	string delayChoice;
	bool choicesConfirmed = false;
	while(!choicesConfirmed)
	{
		wait1Msec(500); //need a half second delay to suppress multiple reads of the same key
		eraseDisplay();
		//choose frequency
		//left button for 600, right button for 1200
		displayTextLine(1, "Choose frequency:");
		displayTextLine(2,"Left== 600");
		displayTextLine(3,"Right==1200");

		//ignore everything until left or right arrow pressed.
		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON)){
			//intentional
		}

		if(nNxtButtonPressed == LEFT_BUTTON){

			irChoice = 600;
			irFrequency = IR600;
		}else if(nNxtButtonPressed == RIGHT_BUTTON)
		{
			irChoice = 1200;
			irFrequency = IR1200;
		}

    wait1Msec(500);
		eraseDisplay();


		//choose starting position
		//left button for parking zone, right button for ramp
		displayTextLine(1, "Starting Pos:");
		displayTextLine(2,"Left==PZ");
		displayTextLine(3,"Right==Ramp");

		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON)){
			//intentional
		}

		if(nNxtButtonPressed == LEFT_BUTTON)
		{
			startingPositionChoice = "PZ";
			StartingPosition = ParkingZone;
		}else if (nNxtButtonPressed == RIGHT_BUTTON)
		{
			startingPositionChoice = "Ramp";
			StartingPosition = Ramp;
		}
    wait1Msec(500);
		eraseDisplay();


		//choose delay
		//left button for yes, right button for no
		displayTextLine(1, "Need delay?");
		displayTextLine(2, "Left == Yes");
		displayTextLine(3, "Right== No");


		//ignore everything except left or right arrow.
		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON)){
			//intentional
		}
		if(nNxtButtonPressed == LEFT_BUTTON)
		{
			delayChoice = "Yes";
			isDelay = true;
		}//if delay
		else if (nNxtButtonPressed == RIGHT_BUTTON)
		{
			delayChoice = "None";
			isDelay = false;
		}
		wait1Msec(500);
		eraseDisplay();


		//confirmation
		displayTextLine(1, "Freq: %d", irChoice);
		displayTextLine(2, "Start pos: %s", startingPositionChoice);
		displayTextLine(3, "Delay: %s", delayChoice);
		displayTextLine(4, "Left == Correct");
		displayTextLine(5, "Right == Redo");

		//ignore everything except left and right arrows.

		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON)){
			//intentional
		}

		if(nNxtButtonPressed == LEFT_BUTTON)
		{
				//confirmed
				choicesConfirmed = true;
		}
	}//!choices confirmed
}//end chooseProgram
