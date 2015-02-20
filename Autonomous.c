#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S2,     irseeker,       sensorHiTechnicIRSeeker1200)
#pragma config(Sensor, S4,     HTSMUX,         sensorI2CCustom)
#pragma config(Motor,  motorA,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  motorB,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  motorC,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_1,     LeftWheels,    tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     Spindle,       tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     RightWheels,   tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_2,     Lift,          tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Servo,  srvo_S1_C3_1,    KickClaw,             tServoContinuousRotation)
#pragma config(Servo,  srvo_S1_C3_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    Hooks,                tServoStandard)
#pragma config(Servo,  srvo_S1_C3_6,    Gate,                 tServoStandard)
//*!!Code automatically generated by K^3S^3T Hogwarts wizards               !!*//

#include "JoystickDriver.c"
#include "hitechnic-irseeker-v2.h"	//used for IR Seeker
#include "lego-touch.h"

// Robot and Field Definitions

// Measurements
#define DIAMETER 7.62
#define TRACK_DISTANCE 48
#define RAMP_DISTANCE 155

// Directions
#define FORWARD true
#define BACKWARD false
#define CLOCKWISE true
#define COUNTER_CLOCKWISE false

// Lift
#define LIFT_MAX 11400
#define LIFT_TOP 11350
#define LIFT_MIDDLE 7550
#define LIFT_LOWER 3600
#define LIFT_BASE 0
#define LIFT_HOLD_POSITION_POWER 5

// Gate
#define GATE_CLOSED 180
#define GATE_OPEN 10

// Goal Finger Positions
#define GOAL_HOOKS_OPEN 10
#define GOAL_HOOKS_CLOSED 185

// Kickstand Servo
#define KICKCLAW_REVERSE_START 0
#define KICKCLAW_FORWARD_START 255
#define KICKCLAW_STOP 127
#define KICKCLAW_MOVE_TIME 750 //in msec

//delay
#define DELAY_TIME 10000

//used to regulate motors
#define REGULATED true
#define UNREGULATED false

//file used in Choice.c
#define DATA_FILE_NAME "choices.txt"

#define RAMP_START 1
#define PARKING_ZONE 0
#define BLOCK 3

#define NEED_DELAY 1
#define NO_DELAY 0


//Functions
void readChoices();	//reads choices made in Choices.c
void rampFunction();	//goes down the ramp to collect goals
void kickstand();		//kicks kickstand
void blockFunction();	//blocks opponent from scoring balls
void initializeRobot();
void calcMove(float centimeters, float power, bool direction, bool isRegulated);
void dualMotorTurn(float robotDegrees, float power, bool direction);
void moveLift(int encoderCounts);
void extendKickClaw();
void retractKickClaw();

//for irSeeker
tHTIRS2 irSeeker;

bool isDelay;
int CurrentPosition;
int TargetPosition;

//const tMUXSensor GoalBaseTouch1 = msensor_S4_1;
//const tMUXSensor GoalBaseTouch2 = msensor_S4_2;
const tMUXSensor LiftLimitTouch = msensor_S4_3;


enum StartingPositionEnum {ParkingZone, Ramp, Block};
StartingPositionEnum StartingPosition;

enum LiftStateEnum {Running, Stopped};
LiftStateEnum LiftState;

task holdPosition();
task LiftSafetyUpperLimitWatch();
task LiftSafetyLowerLimitWatch();
task LiftSafetyLimitTouchWatch();

task main()
{
	initializeRobot();
	startTask (holdPosition);
	startTask (LiftSafetyUpperLimitWatch);
	startTask (LiftSafetyLowerLimitWatch);
	startTask (LiftSafetyLimitTouchWatch);

	waitForStart();

	// Robot specific code
	if(isDelay)	// If there is a delay wait
	{
		wait1Msec(DELAY_TIME);
	}

	if(StartingPosition == Ramp)
	{
		rampFunction();
	}
	else if(StartingPosition == ParkingZone)
	{
		kickstand();
	}
	else if(StartingPosition == Block)
	{
		blockFunction();
	}

	while(true)
	{

	}
}

// Tasks
task holdPosition()
{
	while(true) // Constantly checks
	{
		if(nMotorEncoder[Lift] > TargetPosition) // if higher than supposed to be, lower it
		{
			motor[Lift] = -1 * LIFT_HOLD_POSITION_POWER;
		}

		if(nMotorEncoder[Lift] < TargetPosition) // if lower than supposed to be, raise it
		{
			motor[Lift] = LIFT_HOLD_POSITION_POWER;
		}
	} // while(true)
} // task holdPosition()

task LiftSafetyUpperLimitWatch(){
	while(1){
		if(nMotorEncoder[Lift]>LIFT_MAX){
			//stop the lift
			motor[Lift]=0;
			//move the lift back down to the limit.
			while(nMotorEncoder[Lift]>LIFT_MAX){
				motor[Lift]=-10;
				LiftState=Running;
			}
			//now stop the motor again.
			motor[Lift]=0;
			LiftState=Stopped;
		}
	}
}

task LiftSafetyLowerLimitWatch(){
	while(1){
		if(nMotorEncoder[Lift]<LIFT_BASE){
			//stop the lift
			motor[Lift]=0;
			//rewind the cable to zero
			while(nMotorEncoder[Lift]<LIFT_BASE){
				motor[Lift]=10;
				LiftState=Running;
			}
			//stop the motor again
			motor[Lift]=0;
			LiftState=Stopped;
		}
	}
}

task LiftSafetyLimitTouchWatch(){
	while(1){
		if(TSreadState(LiftLimitTouch) != 0){
			playTone(1000,5);
			//stop the lift
			motor[Lift]=0;
			//move the lift back down to the limit.
			while(nMotorEncoder[Lift]>LIFT_MAX){
				motor[Lift]=-10;
				LiftState=Running;
			}
			//now stop the motor again.
			motor[Lift]=0;
			LiftState=Stopped;
		}
	}
}

// Functions

void initializeRobot()
{
	// Initializes servos
	servo[Gate] = GATE_CLOSED;
	servo[Hooks] = GOAL_HOOKS_OPEN;

	readChoices(); // Reads choices from a file that is written in from a program called Choices

	initSensor(&irSeeker, S2);
	wait1Msec(500);
	//wait for stability of sensor before setting the mode.
	irSeeker.mode = DSP_1200;
	wait1Msec(500);

	nMotorEncoder[LeftWheels] = 0;
	nMotorEncoder[RightWheels] = 0;

	nMotorEncoder[Lift] = 0;

	retractKickClaw();
}

void rampFunction() //ramp, goals
{
	calcMove(RAMP_DISTANCE, 50, BACKWARD, REGULATED);		//goes down ramp
	dualMotorTurn(6, 40, CLOCKWISE);
	calcMove(55, 90, BACKWARD, REGULATED);
	calcMove(10, 15, BACKWARD, REGULATED);
	moveLift(LIFT_MIDDLE);			//puts two balls in the middle goal
	servo[Gate] = GATE_OPEN;
	wait1Msec(2000);
	servo[Gate] = GATE_CLOSED;
	moveLift(LIFT_BASE);
	dualMotorTurn(5, 40, CLOCKWISE);
	calcMove(5, 15, BACKWARD, REGULATED);
	servo[Hooks] = GOAL_HOOKS_CLOSED; // Grabs the goal
	dualMotorTurn(25, 40, CLOCKWISE);
	wait1Msec(300); // Waits because the servo has time to move before the wheels start moving
	calcMove(237, 90, FORWARD, REGULATED);
	dualMotorTurn(180, 40, COUNTER_CLOCKWISE);
}

void kickstand()	//kicks kickstand depending on directional value of irseeker
{
	readSensor(&irSeeker); // Reads the IR sensor values

	switch(irSeeker.acDirection)
	{
		case 0: // for Position 1
			calcMove(74, 50, FORWARD, REGULATED);
			dualMotorTurn(45, 30, COUNTER_CLOCKWISE);
			calcMove(55, 50, FORWARD, REGULATED);
			extendKickClaw();
			calcMove(55, 50, BACKWARD, REGULATED);
			break;

		case 3:	// for Position 2
			calcMove(40, 40, FORWARD, REGULATED);
			dualMotorTurn(15, 40, CLOCKWISE);
			calcMove(82,40,FORWARD,REGULATED);
			dualMotorTurn(105, 40, COUNTER_CLOCKWISE);
			calcMove(28,40,FORWARD,REGULATED);
			extendKickClaw();
			calcMove(30,75,BACKWARD,REGULATED);
			break;

		case 5:
		if(irSeeker.enhStrength > 65) // for Position 3
		{
			calcMove(50, 75, FORWARD, REGULATED);
			dualMotorTurn(30, 60, CLOCKWISE);
			calcMove(137, 60, FORWARD, REGULATED);
			dualMotorTurn(145, 60, COUNTER_CLOCKWISE);
			calcMove(68, 50, FORWARD, REGULATED);
			extendKickClaw();
			calcMove(50, 50, BACKWARD, REGULATED);
		}
		else if((irSeeker.enhStrength > 30) && (irSeeker.enhStrength < 65)) // for Position 2
		{
			calcMove(40, 40, FORWARD, REGULATED);
			dualMotorTurn(15, 40, CLOCKWISE);
			calcMove(82,40,FORWARD,REGULATED);
			dualMotorTurn(105, 40, COUNTER_CLOCKWISE);
			calcMove(28,40,FORWARD,REGULATED);
			extendKickClaw();
			calcMove(30,75,BACKWARD,REGULATED);
		}
		else if(irSeeker.enhStrength < 30) // for Position 1
		{
			calcMove(74, 50, FORWARD, REGULATED);
			dualMotorTurn(45, 30, COUNTER_CLOCKWISE);
			calcMove(55, 50, FORWARD, REGULATED);
			extendKickClaw();
			calcMove(55, 50, BACKWARD, REGULATED);
		}
			break;
	}	//switch
	retractKickClaw();
}

void blockFunction()
{
	calcMove(130, 100, FORWARD, REGULATED);
}

// for moving forward and backword
void calcMove(float centimeters, float power, bool direction, bool isRegulated)
{
	float encoder_counts;

	if(isRegulated)	//turns on pid control (originally in pragma statements)
	{
		nMotorPIDSpeedCtrl[LeftWheels] = mtrSpeedReg;
		nMotorPIDSpeedCtrl[RightWheels] = mtrSpeedReg;
	}
	else
	{
		nMotorPIDSpeedCtrl[LeftWheels] = mtrNoReg;
		nMotorPIDSpeedCtrl[RightWheels] = mtrNoReg;
	}

	// Resets encoders
	nMotorEncoder[LeftWheels] = 0;
	nMotorEncoder[RightWheels] = 0;

	encoder_counts = (centimeters / (DIAMETER * PI)) * 1440; // converts centimeters to motor encoder counts

	if (!direction) //check for direction.
	{
		power = -1 * power;
		encoder_counts = -1 * encoder_counts;
	}

	//set the target encoder values before we set power to start motors.
	nMotorEncoderTarget[LeftWheels] = encoder_counts;
	nMotorEncoderTarget[RightWheels] = encoder_counts;

	motor[LeftWheels] = power;
	motor[RightWheels] = power;

	while((nMotorRunState[LeftWheels] != runStateIdle) && (nMotorRunState[RightWheels] != runStateIdle))
	{
		// Do nothing while we wait for motors to spin to correct distance.
	}

	//stop the motors
	motor[LeftWheels] = 0;
	motor[RightWheels] = 0;

	//set back to unregulated gmotors just so we dont have the wrong mode when exiting
	nMotorPIDSpeedCtrl[LeftWheels] = mtrNoReg;
	nMotorPIDSpeedCtrl[RightWheels] = mtrNoReg;
}

//for turning with two wheels
void dualMotorTurn(float robotDegrees, float power, bool direction) //robot turns using both motors
{
	int encoderCounts = ( TRACK_DISTANCE / DIAMETER) * robotDegrees * 4;

	// Set motors to No PID control as differential turns have problems
	nMotorPIDSpeedCtrl[LeftWheels] = mtrNoReg;
	nMotorPIDSpeedCtrl[RightWheels] = mtrNoReg;

	nMotorEncoder[LeftWheels] = 0;
	nMotorEncoder[RightWheels] = 0;

	if (direction)	// Turns robot clockwise or counter-clockwise, depending on user input
	{
		nMotorEncoderTarget[LeftWheels] = -1 * encoderCounts;
		nMotorEncoderTarget[RightWheels] = encoderCounts;
		motor[LeftWheels] = -1 * power;
		motor[RightWheels] = power;
	}
	else
	{
		nMotorEncoderTarget[LeftWheels] = encoderCounts;
		nMotorEncoderTarget[RightWheels] = -1 * encoderCounts;
		motor[LeftWheels] = power;
		motor[RightWheels] = -1 * power;
	}

	while((nMotorRunState[LeftWheels] != runStateIdle) && (nMotorRunState[RightWheels] != runStateIdle))
	{
		//do nothing while we wait for motors to spin to correct angles.
	}

	//stop the motors
	motor[LeftWheels] = 0;
	motor[RightWheels] = 0;

	//set back to unregulated gmotors just so we dont have the wrong mode when exiting
	nMotorPIDSpeedCtrl[LeftWheels] = mtrNoReg;
	nMotorPIDSpeedCtrl[RightWheels] = mtrNoReg;
}

void moveLift(int encoderCounts)
{
	nMotorPIDSpeedCtrl[Lift] = mtrSpeedReg;
	CurrentPosition = nMotorEncoder[Lift];

	stopTask(holdPosition); // Release the Hold Position task, to allow movement

	servo[Gate] = GATE_CLOSED; // Close the gate, to ensure the safety of motion of the lift

	// Now check if we need to travel up or down.
	if(nMotorEncoder[Lift] > encoderCounts) // Need to move down
	{
		// Check if we are travelling below the lower goal, special handling, until counterweight deployed.
		LiftState = Running;
		nMotorEncoderTarget[Lift] = CurrentPosition - encoderCounts;
		motor[Lift] = -75;

		while(nMotorRunState[Lift] != runStateIdle && nMotorEncoder[Lift] >= LIFT_LOWER)
		{
			// Let the motor lift run until motor becomes idle on reaching target or we reach the last segment down if we reach the last segment down we want to slow down.
		}

		if(nMotorEncoder[Lift] > encoderCounts)
		{
			CurrentPosition=nMotorEncoder[Lift];
			// We are now below the height of the lower base and need to slow down to let the last segment fall slowly
			// This should only execute when the target is the base of the lift
			nMotorEncoderTarget[Lift] = CurrentPosition - encoderCounts;
			motor[Lift] = -30;
			while(nMotorRunState[Lift] != runStateIdle)
			{
				//let the motor reach the target
			}
		} // if(nMotorEncoder[Lift] > encoderCounts)
	} // if(nMotorEncoder[Lift] > encoderCounts)
	else if(nMotorEncoder[Lift] < encoderCounts) // Need to move up
	{
		nMotorEncoderTarget[Lift] = encoderCounts - CurrentPosition;
		motor[Lift] = 95;
		LiftState = Running;
		while(nMotorRunState[Lift] != runStateIdle)
		{
			//let the motor run till it reaches target
		}
	} // else if(nMotorEncoder[Lift]<encoderCounts)

	// If we are already at the EncoderValue target we don't have to do anything

	// Stop the motor
	motor[Lift] = 0;
	LiftState = Stopped;
	TargetPosition = encoderCounts;
	if(encoderCounts != LIFT_BASE)
	{
		// If it is at lift base we don't need to hold position as it will be supported already otherwise we lock on to the position
		startTask(holdPosition);
	}
	else // Not needed, just caution
	{
		stopTask(holdPosition);
	}
}

void readChoices() // Reads choices made in Choices.c
{
	TFileIOResult nIoResult;
	TFileHandle myFileHandle;
	short myFileSize = 10;
	short startingPositionShort;
	short delayShort;

	OpenRead(myFileHandle, nIoResult, DATA_FILE_NAME, myFileSize);

	if(nIoResult) // Error in opening file for read
	{
		// NEED A BACK-UP PLAN
		playTone(5000, 5);
		stopAllTasks();
	}

	//reads choices
	ReadShort(myFileHandle, nIoResult, startingPositionShort);
	ReadShort(myFileHandle, nIoResult, delayShort);
	Close(myFileHandle, nIoResult);

	//sets values depending on choices

	if(startingPositionShort == RAMP_START)
	{
		StartingPosition = Ramp;
	}
	else if(startingPositionShort == PARKING_ZONE)
	{
		StartingPosition = ParkingZone;
	}
	else
	{
		StartingPosition = Block;
	}

	if(delayShort == NEED_DELAY)
	{
		isDelay = true;
	}
	else
	{
		isDelay = false;
	}
}

void retractKickClaw()
{
	servo[KickClaw]=KICKCLAW_REVERSE_START;
	wait1Msec(KICKCLAW_MOVE_TIME);
	servo[KickClaw]=KICKCLAW_STOP;
}

void extendKickClaw()
{
	servo[KickClaw]=KICKCLAW_FORWARD_START;
	wait1Msec(KICKCLAW_MOVE_TIME);
	servo[KickClaw]=KICKCLAW_STOP;
}
