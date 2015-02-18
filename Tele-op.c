#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S3,     ColorSensor,    sensorColorNxtFULL)
#pragma config(Sensor, S4,     HTSMUX,         sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     LeftWheels,    tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     Spindle,       tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     RightWheels,   tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_2,     Lift,          tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Servo,  srvo_S1_C3_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    Hooks,                tServoStandard)
#pragma config(Servo,  srvo_S1_C3_6,    Gate,                 tServoStandard)
//*!!Code automatically generated by K^3S^3T Hogwarts wizards               !!*//

#include "JoystickDriver.c" //Include file to "handle" the Bluetooth messages.
#include "hitechnic-sensormux.h" //for Multiplexer
#include "lego-touch.h"

// Controller 1 Buttons
#define BTN_ROTATESPINDLE_FORWARD joy1Btn(7)
#define BTN_ROTATESPINDLE_BACKWARD joy1Btn(8)
#define BTN_GRAB_GOAL	joy1Btn(6)

// Controller 2 Buttons
#define BTN_LIFT_LOWERGOAL joy2Btn(1)
#define BTN_LIFT_BASE joy2Btn(2)
#define BTN_LIFT_TOPGOAL joy2Btn(3)
#define BTN_LIFT_MIDDLEGOAL joy2Btn(4)
#define BTN_GATE_CTRL joy2Btn(6)
#define BTN_LIFT_UP joy2Btn(11)
#define BTN_LIFT_DOWN joy2Btn(12)

// Controller 1 Joysticks
#define CTRL1_JOY_LEFT_Y joystick.joy1_y1
#define CTRL1_JOY_RIGHT_Y joystick.joy1_y2

// Controller 2 Joysticks
#define CTRL2_JOY_LEFT_Y joystick.joy2_y1
#define CTRL2_JOY_RIGHT_Y	joystick.joy2_y2

// Controllers' D-Pads.
#define CTRL1_DPAD joystick.joy1_TopHat // Controller 1 Directional Pad
#define CTRL2_DPAD joystick.joy2_TopHat // Controller 2 D-Pad
// D-Pad possible positions
#define DPAD_RIGHT 2
#define DPAD_LEFT 6

// Lift
#define LIFT_MAX 11400
#define LIFT_TOP 11350
#define LIFT_MIDDLE 7500
#define LIFT_LOWER 3600
#define LIFT_BASE 0
#define LIFT_HOLD_POSITION_POWER 5

//Gate
#define GATE_CLOSED 180
#define GATE_OPEN 10

//Goal Hooks
#define GOAL_HOOKS_OPEN 10
#define GOAL_HOOKS_CLOSED 185

//Threshold for Joysticks, so it doesn't move when thumbs are slightly touching it
#define JOYSTICK_THRESHOLD 10

// Used for dualMotorTurn
#define CLOCKWISE true
#define COUNTER_CLOCKWISE false
#define DIAMETER 7.62
#define TRACK_DISTANCE 48
#define RAMP_DISTANCE 150

enum SpindleStateEnum {Running, Stopped}; // State of Spindle
enum LiftStateEnum {Running, Stopped}; // State of Lift
enum GateStateEnum {Open, Closed}; // State of gate
enum ChooseDriverEnum {MainDriver, Scorer}; // Shows which driver is running the robot

SpindleStateEnum SpindleState;
LiftStateEnum LiftState;
GateStateEnum GateState;
ChooseDriverEnum ChooseDriver;

// Tasks
task holdPosition(); // Holds position of lift because there is too much weight to hold with no power
task liftMoveCheck();	//turns off motor if lift is running
task liftCheckMAX(); // Checks if lift is too high
task liftCheckMIN(); // Checks if lift is too low
task MUXTasks();

// Functions for MUXTasks
void checkLiftTouch(); // Checks if touch sensor on lift gets hit. Used with liftCheckMAX for safety
void colorSignal();	//turns on color sensor if hooks is down, goaltouch has been detected, or both

// Primary Functions
void initializeRobot(); // Gets robot ready for Tele-op
void clrTimers(); // Clears timers needed for buttons
void processControls(); // Has uses for buttons

// Secondary Functions
void moveLift(int encoderCounts); // Moves lift to specified position

const tMUXSensor GoalBaseTouch1 = msensor_S4_1;	//for use of multiplexer
const tMUXSensor GoalBaseTouch2 = msensor_S4_2;
const tMUXSensor LiftLimitTouch = msensor_S4_3;

int TargetPosition = 0; // Used in holdPosition
int CurrentPosition = 0; // Used in moveLift
float powerFactor; // Used in driving, when lift is up, powerFactor goes down

// For smoothing out the wheel movements
int prev_Joy1Y1;
int prev_Joy1Y2;

task main()
{
	initializeRobot();
	waitForStart();

	clrTimers();
	startTask (liftMoveCheck);
	startTask (MUXTasks);
	startTask (liftCheckMAX);
	startTask (liftCheckMIN);

	while(true) // Infinite loop, will end when match ends
	{
		getJoystickSettings(joystick); // Gets current joystick settings
		processControls();
	}
}

void initializeRobot ()
{
	servo[Gate] = GATE_CLOSED;
	servo[Hooks] = GOAL_HOOKS_OPEN;
	LiftState = Stopped;
	GateState = Closed;
	SpindleState = Stopped;
	ChooseDriver = MainDriver;
}

void clrTimers()
{
	clearTimer (T1);	// Timer used for Spindle
	clearTimer (T2);	// Lift
	clearTimer (T3);	// Gate
	clearTimer (T4);	// Hooks
}

task MUXTasks()
{
	while(true)
	{
		checkLiftTouch();
		colorSignal();
	}
}

task liftCheckMAX ()
{
	while(true)
	{
		if (nMotorEncoder[Lift] > LIFT_MAX) // Checks if lift is higher than it is supposed to be
		{
			LiftState = Running;

			while (nMotorEncoder[Lift] > LIFT_MAX) // Lowers lift until it is in the right position
			{
				motor[Lift] = -20;
			}

			motor[Lift] = 0; // After lift is where it is supposed to be, stop it
			LiftState = Stopped;
		} // if (nMotorEncoder[Lift] > LIFT_MAX)
	}
} // task liftCheckMAX()

task liftCheckMIN()
{
	while(true)
	{
		if (nMotorEncoder[Lift] < LIFT_BASE - 1) // Checks if lift is lower than it is supposed to be
		{
			LiftState = Running;

			while (nMotorEncoder[Lift] < LIFT_BASE - 1) // Raises lift until it is in the right position
			{
				motor[Lift] = 20;
			}

			motor[Lift] = 0;// After lift is where it is supposed to be, stop it
			LiftState = Stopped;
		} // if (nMotorEncoder[Lift] < LIFT_BASE)
	}
} // task liftCheckMIN()


void checkLiftTouch()
{
	if(TSreadState(LiftLimitTouch) != 0) // Checks if touch sensor is touched
	{
		LiftState = Running;

		while(nMotorEncoder[Lift] > LIFT_MAX) //move the lift back down to the limit.
		{
			motor[Lift]=-10;
		}

		motor[Lift]=0;
		LiftState=Stopped;
	} // if (SensorValue[LiftLimitTouch]!=0)
} // task checkLiftTouch()

//for use to hold lift up in position
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

// turns off the motors if the lift is running
task liftMoveCheck()
{
	if(LiftState == Running)
	{
		motor[LeftWheels] = 0;
		motor[RightWheels] = 0;
	}
}

void colorSignal()
{
	if((TSreadState(GoalBaseTouch1) != 0)||(TSreadState(GoalBaseTouch2)))
	{
		if(ServoValue[Hooks] > 100)
		{
			//turns green if hooks is down and goal base touch is detected
			SensorType[ColorSensor] = sensorColorNxtGREEN;
		}
		else
		{
			//turns blue if only goal base touch is detected
			SensorType[ColorSensor] = sensorColorNxtBLUE;
		}
	}
	else if(ServoValue[Hooks] > 100)
	{
		//turns red if hooks is down
		SensorType[ColorSensor] = sensorColorNxtRED;
	}
	else
	{
		SensorType[ColorSensor] = sensorColorNxtNONE; //emits no light
	}
}

//check controls
void processControls()
{
	if(BTN_LIFT_UP)
	{
		stopTask(holdPosition);

		if(time1[T2] > 500) 	//checks to see if button isn't pressed to fast
		{
			if(LiftState == Stopped)
			{
				if(nMotorEncoder[Lift] < LIFT_MAX)
				{
					motor[Lift] = 70;	//raises lift
					LiftState = Running;
				} // if(nMotorEncoder[Lift] < LIFT_MAX)
			} // if(LiftState == Stopped)
			else
			{
				motor[Lift] = 0;
				LiftState = Stopped;
			}
			clearTimer(T2);
		} // if(time1[T2]>500)
	} // if(BTN_LIFT_UP)

	if(BTN_LIFT_DOWN)
	{
		stopTask(holdPosition);

		if(time1[T2] > 500) 	//checks to see if button isn't pressed to fast
		{
			if(LiftState == Stopped)
			{
				if(nMotorEncoder[Lift] > 0)
				{
					motor[Lift] = -70;	//lowers lift
					LiftState = Running;
				} // if(nMotorEncoder[Lift] > 0)
			} // if(LiftState == Stopped)
			else
			{
				motor[Lift] = 0;
				LiftState = Stopped;
			}
			clearTimer(T2);
		} // if(time1[T2]>500)
	} // if(BTN_LIFT_DOWN)

	if(BTN_ROTATESPINDLE_FORWARD)
	{
		if(time1[T1] > 500)		//checks to see if button isn't pressed too fast
		{
			if(SpindleState == Stopped)
			{
				motor[Spindle] = 100;		//starts the Spindle
				SpindleState = Running;
			}
			else
			{
				motor[Spindle] = 0;		//stops the Spindle
				SpindleState = Stopped;
			}
			clearTimer(T1);
		} // if(time1[T1]>500)
	} // if(BTN_ROTATESPINDLE_FORWARD)

	if(BTN_ROTATESPINDLE_BACKWARD)
	{
		if(time1[T1] > 500)	//checks to see if button isn't pressed to fast
		{
			if(SpindleState == Stopped)
			{
				motor[Spindle] = -100;		//starts the spindle (backwards)
				SpindleState = Running;
			}
			else
			{
				motor[Spindle] = 0;		//stops the spindle
				SpindleState = Stopped;
			}
			clearTimer(T1);
		} // if(time1[T1]>500)
	} // if(BTN_ROTATESPINDLE_BACKWARD)

	if(BTN_GATE_CTRL)
	{
		if(time1[T3] > 500)	//checks to see if button is pressed too fast
		{
			if(GateState == Closed)
			{
				servo[Gate] = GATE_OPEN;	//opens gate
				GateState = Open;
			}
			else
			{
				servo[Gate] = GATE_CLOSED;	//closes gate
				GateState = Closed;
			}
			clearTimer(T3);
		} // if(time1[T3]>500)
	} // if(BTN_GATE_CTRL)

	if(BTN_LIFT_BASE)
	{
		moveLift(LIFT_BASE);	//moves lift to base
	}
	if(BTN_LIFT_LOWERGOAL)
	{
		moveLift(LIFT_LOWER);	//moves lift to lower goal height
	}
	if(BTN_LIFT_MIDDLEGOAL)
	{
		moveLift(LIFT_MIDDLE);	//moves lift to middle goal height
	}
	if(BTN_LIFT_TOPGOAL)
	{
		moveLift(LIFT_TOP);	//moves lift to top goal height
	}

	if(BTN_GRAB_GOAL)
	{
		if(time1[T4]>500)	//checks to see if button is pressed too fast
		{
			if((servo[Hooks] == GOAL_HOOKS_CLOSED))
			{
				servo[Hooks] = GOAL_HOOKS_OPEN;	//opens goal hook
			}
			else
			{
				servo[Hooks] = GOAL_HOOKS_CLOSED;	//closes goal hook
			}
			clearTimer(T4);
		} // if(time1[T4]>500)
	} // if(BTN_GRAB_GOAL)

	//  Wheels
	if(nMotorEncoder[Lift] >= LIFT_LOWER - 10)	//assigns driver depending on height of lift
	{
		ChooseDriver = Scorer;
	}
	else
	{
		ChooseDriver = MainDriver;
	}

	if(ChooseDriver == MainDriver) // For Controller 1
	{
		nMotorPIDSpeedCtrl[LeftWheels] = mtrSpeedReg;
		nMotorPIDSpeedCtrl[RightWheels] = mtrSpeedReg;
		powerFactor = 1;

		if(abs(CTRL1_JOY_LEFT_Y) > JOYSTICK_THRESHOLD)
		{
			//multiplied by power factor to become slower (less jerking)
			motor[RightWheels] = (int)(((CTRL1_JOY_LEFT_Y + prev_Joy1Y1)/2) * powerFactor);
		}
		else
		{
			motor[RightWheels] = 0;
		}

		prev_Joy1Y1 = CTRL1_JOY_LEFT_Y;

		if(abs(CTRL1_JOY_RIGHT_Y) > JOYSTICK_THRESHOLD)
		{
			motor[LeftWheels] = (int)(((CTRL1_JOY_RIGHT_Y + prev_Joy1Y2)/2) * powerFactor);
		}
		else
		{
			motor[LeftWheels] = 0;
		}

		prev_Joy1Y2 = CTRL1_JOY_RIGHT_Y;
	} // if(ChooseDriver == MainDriver)

	else if(ChooseDriver == Scorer)
	{
		nMotorPIDSpeedCtrl[LeftWheels] = mtrNoReg;
		nMotorPIDSpeedCtrl[RightWheels] = mtrNoReg;
		powerFactor = 0.125;	//for fine movements to adjust for scoring

		if(abs(CTRL2_JOY_LEFT_Y) > JOYSTICK_THRESHOLD)
		{
			motor[RightWheels] = (int)(CTRL2_JOY_LEFT_Y * powerFactor);
		}
		else
		{
			motor[RightWheels] = 0;
		}

		if(abs(CTRL2_JOY_RIGHT_Y) > JOYSTICK_THRESHOLD)
		{
			motor[LeftWheels] = (int)(CTRL2_JOY_RIGHT_Y * powerFactor);
		}
		else
		{
			motor[LeftWheels] = 0;
		}
	} // else if(ChooseDriver == Scorer)
} // void processControls()

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
