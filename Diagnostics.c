#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S2,     irseeker,       sensorHiTechnicIRSeeker1200)
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

// hitechnic-sensormux.h is already opened by hitechnic-irseeker-v2.h
#include "hitechnic-irseeker-v2.h"
#include "lego-touch.h"

#define GATE_CLOSED 180
#define GATE_OPEN 10

//Goal Hooks
#define GOAL_HOOKS_OPEN 10
#define GOAL_HOOKS_CLOSED 185

//NXT Buttons
#define LEFT_BUTTON 2
#define RIGHT_BUTTON 1

tHTIRS2 irSeeker;

//errors
bool hasErrors = false;
bool NXTBatteryError = false;
bool EXTBatteryError = false;
bool leftWheelsError = false;
bool rightWheelsError = false;
bool liftError = false;
bool spindleError = false;
bool hooksError = false;
bool gateError = false;
bool irBeaconError = false;
bool liftTouchSensorError = false;
bool goalTouchError1 = false;
bool goalTouchError2 = false;
bool colorSensorError = false;

//Functions
void testNXTBattery();
void testEXTBattery();
void testWheelMotors();
void testSpindleMotor();
void testLiftMotor();
void testGateServo();
void testHookServo();
void testIRBeacon();
void testSMUXOn();
void testLiftLimitSensor();
void testGoalTouchSensors();
void testGoalTouchSensor1(); // Secondary to testGoalTouchSensors()
void testGoalTouchSensor2(); // Also secondary
void testColorSensor();
void waitForButtonPress();

bool confirmWorking();

//for multiplexer
const tMUXSensor GoalBaseTouch1 = msensor_S4_1;
const tMUXSensor GoalBaseTouch2 = msensor_S4_2;
const tMUXSensor LiftLimitTouch = msensor_S4_3;

task main()
{
	SensorType[ColorSensor] = sensorColorNxtNONE;	// turns off color sensor
	testNXTBattery();
	testEXTBattery();
	testWheelMotors();
	testSpindleMotor();
	testLiftMotor();
	testGateServo();
	testHookServo();
	testIRBeacon();
	testSMUXOn();
	testLiftLimitSensor();
	testGoalTouchSensors();
	testColorSensor();

	eraseDisplay();
	if(hasErrors)
	{
		displayTextLine(1,"Has Errors!!!");
		playTone(6000, 15);
	}
	else
	{
		displayTextLine(1, "No errors.");
		playTone(3000, 5);
	}

	waitForButtonPress();

	int displayLineNumber = 1;

	//Displays individual errors
	if(NXTBatteryError)
	{
		displayTextLine(displayLineNumber++,"NXT Battery Error");
	}

	if(EXTBatteryError)
	{
		displayTextLine(displayLineNumber++,"EXT Battery Error");
	}

	if(leftWheelsError)
	{
		displayTextLine(displayLineNumber++,"Left Wheels Error");
	}

	if(rightWheelsError)
	{
		displayTextLine(displayLineNumber++,"Right Wheels Error");
	}

	if(liftError)
	{
		displayTextLine(displayLineNumber++, "Lift Error");
	}

	if(irBeaconError)
	{
		displayTextLine(displayLineNumber++, "IR Beacon Error");
	}

	if(liftTouchSensorError)
	{
		displayTextLine(displayLineNumber++, "LiftLimitTouch Sensor Error");
	}

	if(goalTouchError1)
	{
		displayTextLine(displayLineNumber++, "GoalTouch Sensor 1 Error");
	}

	if(goalTouchError2)
	{
		displayTextLine(displayLineNumber++, "GoalTouch Sensor 2 Error");
	}

	wait1Msec(3000);
}

// Functions

void testNXTBattery()
{
	eraseDisplay();
	//test NXT Battery
	displayTextLine(1, "Testing NXT Batt..");
	if(nAvgBatteryLevel <8000) //if battery level is too low
	{
		hasErrors=true;		//turns error on
		NXTBatteryError = true;
		displayTextLine(3, "NXT Batt Low !!!...");
		displayTextLine(5,"NXT Batt:%d",nAvgBatteryLevel);
		playTone(5000, 5);

		//wait for acknowledgement
		waitForButtonPress();
		}
	else
	{
		displayTextLine(3, "NXT Batt Ok !!!...");
		displayTextLine(5,"NXT Batt: %d",nAvgBatteryLevel);
		wait1Msec(1000);
	}
}
void testEXTBattery()
{
	eraseDisplay();
	//test External Battery
	displayTextLine(1, "Testing Ext Batt..");
	if(externalBatteryAvg <13000)
	{
		hasErrors=true;	//turn error on
		EXTBatteryError = true;
		displayTextLine(3, "EXT Batt Low !!!...");
		displayTextLine(4,"EXT Batt Lev:%d",externalBatteryAvg);
		if(externalBatteryAvg<0)
		{
			displayTextLine(5,"EXT Batt Off ?");
			wait1Msec(2000);
		}
		playTone(8000, 5);

		//wait for acknowledgement
		waitForButtonPress();
	}
	if(externalBatteryAvg >13000)
	{
		eraseDisplay();
		displayTextLine(3, "EXT Batt Ok !!!...");
		displayTextLine(5,"EXT Batt Lev:%d",externalBatteryAvg);
		wait1Msec(3000);
	}
}
void testWheelMotors()
{
	eraseDisplay();
	//Get wheel motors to be at rest positions
	motor[LeftWheels]=0;
	motor[RightWheels]=0;
	//reset all encoders
	nMotorEncoder[LeftWheels] = 0;
	nMotorEncoder[RightWheels] = 0;
	// test motors
	// test LeftWheels
	displayTextLine(1, "Testing Left Wheels ...");
	wait1Msec(1000);
	motor[LeftWheels] = 20;
	wait1Msec(500);
	motor[LeftWheels] = 0;
	if (nMotorEncoder[LeftWheels] == 0)
	{
		// either motor or encoder is faulty
		hasErrors = true;
		leftWheelsError = true;
		displayTextLine(3, "Left wheels error !!!...");
		playTone(100, 5); //wait for acknowledgement
		waitForButtonPress();
	}
	eraseDisplay();
	// test RightWheels
	displayTextLine(1, "Testing Right Wheels...");
	wait1Msec(1000);
	motor[RightWheels] = 20;
	wait1Msec(500);
	motor[RightWheels] = 0;
	if (nMotorEncoder[RightWheels] == 0)
	{
		// either motor or encoder is faulty
		hasErrors = true;
		rightWheelsError = true;
		displayTextLine(3, "Right wheels error....");
		playTone(500, 5);
		//wait for acknowledgement
		waitForButtonPress();
	}
}

//tests lift
void testLiftMotor()
{
	eraseDisplay();
	motor[Lift]=0;
	nMotorEncoder[Lift] = 0;
	//set an encoder target for the lift
	displayTextLine(1, "Testing Lift...");
	wait1Msec(1000);
	nMotorEncoderTarget[Lift]=500;
	motor[Lift] = 20;
	wait1Msec(500);
	motor[Lift] = 0;
	if (nMotorEncoder[Lift] == 0)
	{
		// either motor or encoder is faulty
		hasErrors = true;
		liftError = true;
		displayTextLine(3, "Lift error....");
		playTone(1000, 5);
		//wait for acknowledgement
		waitForButtonPress();
	}
	else
	{
		nMotorEncoderTarget[Lift]=0;
		motor[Lift] = -20;
		wait1Msec(500);
		motor[Lift] = 0;
	}
}

//tests spindle
void testSpindleMotor()
{
	eraseDisplay();
	// test Spindle
	motor[Spindle]=0;
	displayTextLine(1, "Testing Spindle...");
	wait1Msec(1000);
	displayTextLine(2, "Conf Spindle Turn..");
	motor[Spindle] = 20;
	wait1Msec(500);
	motor[Spindle] = 0;
	//wait for acknowledgement
	if(!confirmWorking())
	{
		spindleError = true;
	}

}

//tests goal hooks
void testHookServo()
{
	eraseDisplay();
	//reset encoder
	servo[Hooks]=GOAL_HOOKS_OPEN;
	// test Hooks
	displayTextLine(1, "Testing Goal Hooks...");
	wait1Msec(1000);
	displayTextLine(2, "Conf GoalHook Close..");
	servo[Hooks] = GOAL_HOOKS_CLOSED;
	wait1Msec(500);
	//wait for acknowledgement
	if(!confirmWorking())
	{
		hooksError = true;
	}
	servo[Hooks]=GOAL_HOOKS_OPEN;
}

//test gate servo
void testGateServo()
{
	eraseDisplay();
	// test Gate
	//set servos to starting position
	servo[Gate]=GATE_CLOSED;
	displayTextLine(1, "Testing Gate Servo...");
	wait1Msec(1000);
	displayTextLine(2, "Conf Gate Open..");
	servo[Gate]= GATE_OPEN;
	wait1Msec(500);
	//wait for acknowledgement
	if(!confirmWorking())
	{
		gateError = true;
	}
	servo[Gate]= GATE_CLOSED;
}

//tests ir beacon
void testIRBeacon()
{
	eraseDisplay();
	//Test IR Beacon
	displayTextLine(1, "Testing IR Becon...");
	wait1Msec(1000);
	//initialize Beacon
	initSensor(&irSeeker, S2);
	wait1Msec(1000);
	irSeeker.mode=DSP_1200;
	displayTextLine(2, "Set IRBeacon=1200");
	displayTextLine(3, "Hold IRB in front");
	//Testing at 1200
	irSeeker.acDirection=0;
	displayTextLine(4,"Reading IR Sensor..");
	clearTimer(T1);
	while((irSeeker.acDirection==0)&&(time1[T1]<10000)&&(nNxtButtonPressed<0))
	{
		readSensor(&irSeeker);
	}
	if((time1[T1]>=10000)||(nNxtButtonPressed>=0))
	{
		//timer ran out or interrupted
		hasErrors=true;
		irBeaconError = true;
		displayTextLine(5,"Could not find IR Beacon");
		//wait for confirmation
		wait1Msec(1000);
		waitForButtonPress();
		}
	else
	{
		//found IR Beacon
		displayTextLine(5,"Found IR, Dir:%d",irSeeker.acDirection);
		wait1Msec(2000);
	}
}

void testSMUXOn()
{
	if((TSreadState(GoalBaseTouch1) != 0)&&(TSreadState(GoalBaseTouch2) != 0)&&(TSreadState(LiftLimitTouch) != 0))
	{ // If all are touched at the same time, then SMUX is most likely off
		eraseDisplay();
		displayBigTextLine(2, "MUX OFF");
		displayTextLine(4, "Turn MUX on");
		while((TSreadState(GoalBaseTouch1) == 0)&&(TSreadState(GoalBaseTouch2) == 0)&&(TSreadState(LiftLimitTouch) == 0));
	}
}

//test LiftLimitTouch
void testLiftLimitSensor()
{
	eraseDisplay();
	//test for Touch Sensor
	displayTextLine(1, "Testing LiftLimit Sensor...");
	wait1Msec(1000);
	displayTextLine(2, "Press LiftLimit Sensor..");
	clearTimer(T1);
	while((TSreadState(LiftLimitTouch) == 0)&&(time1[T1]<10000)&&(nNxtButtonPressed<0)){
		//intentional
	}
	if((time1[T1]>=10000)||(nNxtButtonPressed>=0)){
		//timer ran out or we were interrupted
		hasErrors=true;
		liftTouchSensorError = true;

		displayTextLine(5,"No LiftLimit Sensed");
		//wait for confirmation
		wait1Msec(1000);
		waitForButtonPress();
		}else{
		displayTextLine(5,"Detected LiftLimit Press");
		wait1Msec(2000);
	}
}

void testGoalTouchSensors()
{
	testGoalTouchSensor1();
	testGoalTouchSensor2();
}

void testGoalTouchSensor1()
{
	eraseDisplay();
	//test for GoalBaseTouch Sensor
	displayTextLine(1, "Testing GoalTouch Sensor...");
	wait1Msec(1000);
	displayTextLine(2, "Press GoalTouch Sensor...");
	clearTimer(T1);
	while((TSreadState(GoalBaseTouch1) == 0)&&(time1[T1] < 10000)&&(nNxtButtonPressed < 0))
	{
		//intentional
	}
	if((time1[T1]>=10000)||(nNxtButtonPressed>=0)){
		//timer ran out or we were interrupted
		hasErrors=true;
		goalTouchError1 = true;
		displayTextLine(5,"No GoalTouch Sensed");
		//wait for confirmation
		wait1Msec(1000);
		waitForButtonPress();
		}else{
		displayTextLine(5,"Detected GoalTouch Press");
		wait1Msec(2000);
	}
}

void testGoalTouchSensor2()
{
	eraseDisplay();
	//test for GoalBaseTouch Sensor
	displayTextLine(1, "Testing GoalTouch Sensor...");
	wait1Msec(1000);
	displayTextLine(2, "Press GoalTouch Sensor...");
	clearTimer(T1);
	while((TSreadState(GoalBaseTouch2) == 0)&&(time1[T1] < 10000)&&(nNxtButtonPressed < 0))
	{
		//intentional
	}
	if((time1[T1]>=10000)||(nNxtButtonPressed>=0)){
		//timer ran out or we were interrupted
		hasErrors=true;
		goalTouchError2 = true;
		displayTextLine(5,"No GoalTouch Sensed");
		//wait for confirmation
		wait1Msec(1000);
		waitForButtonPress();
		}else{
		displayTextLine(5,"Detected GoalTouch Press");
		wait1Msec(2000);
	}
}

void testColorSensor()
{
	eraseDisplay();
	//testing for Color Sensor
	displayTextLine(1, "Testing Color Sensor...");
	wait1Msec(1000);
	SensorType[ColorSensor] = sensorColorNxtRED;
	wait1Msec(1000);
	SensorType[ColorSensor] = sensorColorNxtGREEN;
	wait1Msec(1000);
	SensorType[ColorSensor] = sensorColorNxtBLUE;
	wait1Msec(1000);
	SensorType[ColorSensor] = sensorColorNxtNONE;
	wait1Msec(1000);
	if(!confirmWorking())
	{
		colorSensorError = true;
	}
}

void waitForButtonPress()
{
	displayTextLine(6,"Press any btn..");
	while(nNxtButtonPressed<0){
		//intentional. will run till a button is pressed
	}
	//absorbing button press buffer persistence
	wait1Msec(1000);
}

bool confirmWorking()
{
	eraseDisplay();

		displayTextLine(1, "Did it work?");
		displayTextLine(2, "Left = Yes");
		displayTextLine(3, "Right = No");


		// Ignore everything except left or right arrow.
		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON))
		{
			//intentional
		}

		if(nNxtButtonPressed == LEFT_BUTTON)
		{
			return true;
		}
		else
		{
			return false;
		}
}
