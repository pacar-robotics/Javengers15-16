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

#define RAMP_DISTANCE 190
#define FORWARD true
#define BACKWARD false
#define DIAMETER 6.985
#define TRACK_DISTANCE 19.304
#define CLOCKWISE true
#define COUNTER_CLOCKWISE false

#include "JoystickDriver.c"
#include "hitechnic-irseeker-v2.h"


void initializeRobot();
void calcMove(float centimeters, float power, bool direction);
void rampMove();
void kickstand();
void dualMotorTurn(float robotDegrees, float power, bool direction);

task main()
{
  initializeRobot();

	nMotorEncoder[LeftWheels] = 0;
	nMotorEncoder[RightWheels] = 0;

	//waitForStart();

	//rampMove();
 	//kickstand();

	dualMotorTurn(90, 40, COUNTER_CLOCKWISE);
	wait1Msec(10000);
	calcMove(50, 40, FORWARD);
}


/*	Functions:	*/

void rampMove()
{
	calcMove(RAMP_DISTANCE, 50, FORWARD);
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


void kickstand()
{
	tHTIRS2 irSeeker;
	initSensor(&irSeeker, S2);
	irSeeker.mode=DSP_600;

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

void dualMotorTurn(float robotDegrees, float power, bool direction) //robot turns using both motors
{
	int encoderCounts = (TRACK_DISTANCE/DIAMETER*(robotDegrees*4))*2;
	nMotorEncoder[LeftWheels] = 0;
	nMotorEncoder[RightWheels] = 0;
	if (direction == CLOCKWISE)	//turns robot clockwise or counter-clockwise, depending on user input
	{
		nMotorEncoderTarget[LeftWheels] = encoderCounts;
		nMotorEncoderTarget[RightWheels] = -1*encoderCounts;
		motor[LeftWheels] = power;
		motor[RightWheels] = -1 * power;
		while((nMotorEncoder[LeftWheels] < encoderCounts) || (nMotorEncoder[RightWheels] > -1*encoderCounts))
		{
			// idle loop
		}
	}
	else
	{
		nMotorEncoderTarget[LeftWheels] = -1*encoderCounts;
		nMotorEncoderTarget[RightWheels] = encoderCounts;
		motor[LeftWheels] = -1* power;
		motor[RightWheels] = power;
		while((nMotorEncoder[LeftWheels] > -1*encoderCounts) || (nMotorEncoder[RightWheels] < encoderCounts))
		{
			// idle loop
		}
	}
	motor[LeftWheels] = 0;
	motor[RightWheels] = 0;
}

void initializeRobot()
{
  return;
}
