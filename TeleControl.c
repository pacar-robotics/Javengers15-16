#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     irseeker,       sensorHiTechnicIRSeeker1200)
#pragma config(Sensor, S3,     LiftLimitTouch, sensorTouch)
#pragma config(Motor,  mtr_S1_C1_1,     LeftWheels,    tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     Spindle,       tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     RightWheels,   tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_2,     Lift,          tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Servo,  srvo_S1_C3_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    GoalClaw,             tServoStandard)
#pragma config(Servo,  srvo_S1_C3_6,    Gate,                 tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//definitions that help readability of the program

#define GATE_CLOSED 70
#define GATE_OPEN 150

//Goal Finger Positions
#define GOAL_CLAW_OPEN 10
#define GOAL_CLAW_CLOSED 180

//lift related
#define LIFT_LIMIT 10800
#define LIFT_TOP 10750
#define LIFT_MIDDLE 7100
#define LIFT_LOWER 3600
#define LIFT_BASE 0
#define LIFT_HOLD_POSITION_POWER 5

//controller button definitions

#define BTN_LIFT_UP joy1Btn(11)
#define BTN_LIFT_DOWN joy1Btn(12)
#define BTN_LIFT_TOP_GOAL joy1Btn(3)
#define BTN_LIFT_MIDDLE_GOAL joy1Btn(4)
#define BTN_LIFT_LOWER_GOAL joy1Btn(1)
#define BTN_LIFT_BASE joy1Btn(2)
#define BTN_SPINDLE_ROTATE_FORWARD joy1Btn(7)
//#define BTN_SPINDLE_ROTATE_BACKWARD joy1Btn(8)
#define BTN_GATE_OPEN joy1Btn(6)
#define BTN_GATE_CLOSED joy1Btn(5)
#define BTN_GOAL_CLAW_TOGGLE joy1Btn(8)



enum SpindleStateEnumeration {Running,Stopped};
enum LiftStateEnumeration  {Running, Stopped};


int TargetPosition =0;
int CurrentPosition=0;


SpindleStateEnumeration SpindleState;
LiftStateEnumeration LiftState;



/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                           Tele-Operation Mode Code Template
//
// This file contains a template for simplified creation of an tele-op program for an FTC
// competition.
//
// You need to customize two functions with code unique to your specific robot.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    initializeRobot
//
// Prior to the start of tele-op mode, you may want to perform some initialization on your robot
// and the variables within your program.
//
// In most cases, you may not have to add any code to this function and it will remain "empty".
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

void initializeRobot()
{
  // Place code here to initialize servos to starting positions.
  // Sensors are automatically configured and setup by ROBOTC. They may need a brief time to stabilize.

  servo[Gate]=GATE_CLOSED;
  servo[GoalClaw]=GOAL_CLAW_OPEN;

  SpindleState=Stopped;
  LiftState=Stopped;

  nMotorEncoder[Lift]=0;

  return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                         Main Task
//
// The following is the main code for the tele-op robot operation. Customize as appropriate for
// your specific robot.
//
// Game controller / joystick information is sent periodically (about every 50 milliseconds) from
// the FMS (Field Management System) to the robot. Most tele-op programs will follow the following
// logic:
//   1. Loop forever repeating the following actions:
//   2. Get the latest game controller / joystick settings that have been received from the PC.
//   3. Perform appropriate actions based on the joystick + buttons settings. This is usually a
//      simple action:
//      *  Joystick values are usually directly translated into power levels for a motor or
//         position of a servo.
//      *  Buttons are usually used to start/stop a motor or cause a servo to move to a specific
//         position.
//   4. Repeat the loop.
//
// Your program needs to continuously loop because you need to continuously respond to changes in
// the game controller settings.
//
// At the end of the tele-op period, the FMS will autonmatically abort (stop) execution of the program.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////


task LiftSafetyUpperLimitWatch();
task LiftSafetyLowerLimitWatch();
task LiftSafetyLimitTouchWatch();
task HoldPosition();

void MoveLiftToPosition(int EncoderValue);


task main()
{
  initializeRobot();

  waitForStart();   // wait for start of tele-op phase

  clearTimer(T1);//timer for spindle functions
  clearTimer(T2);//timer for lift functions
  clearTimer(T3); //timer for Goal Claw functions

	int threshold = 10;             /* Int 'threshold' will allow us to ignore low       */
                                  /* readings that keep our robot in perpetual motion. */


  //launch the LiftSafetyWatch tasks


  startTask(LiftSafetyUpperLimitWatch);
  startTask(LiftSafetyLowerLimitWatch);
  startTask(LiftSafetyLimitTouchWatch);
  //startTask(HoldPosition);

  while(true)                            // Infinite loop:
  {
    getJoystickSettings(joystick);

    //lift up

    if(BTN_LIFT_UP){
    	stopTask(HoldPosition);
    	//check to see if button is not being read too fast
    	if(time1[T2]>500){
    		if((LiftState==Stopped)){
    			//check to see if the lift is already at limit
    			if(nMotorEncoder[Lift]<LIFT_LIMIT){
    				//start the lift
    				motor[Lift]=75;
    				LiftState=Running;
    				//restart timer for button press for Lift
    				clearTimer(T2);
    			}
    		}else{
    		//stop the Lift
    			motor[Lift]=0;
    			LiftState=Stopped;
    			startTask(HoldPosition);
    		//restart timer for button press for Lift
    			clearTimer(T2);
    		}

    	}
  	}


  	//lift down

    if(BTN_LIFT_DOWN){
    	//check to see if button is not being read too fast
    	stopTask(HoldPosition);
    	if(time1[T2]>500){
    		if((LiftState==Stopped)){
    			//check to see if the lift is already at limit
    			if(nMotorEncoder[Lift]>0){
    				//start the lift
    				motor[Lift]=-30;
    				LiftState=Running;
    				//restart timer for button press for Lift
    				clearTimer(T2);
    			}
    		}else{
    		//stop the Lift
    			motor[Lift]=0;
    			LiftState=Stopped;
    			startTask(HoldPosition);
    		//restart timer for button press for Lift
    			clearTimer(T2);
    		}

    	}
  	}


  	//spindle rotate forward

    if(BTN_SPINDLE_ROTATE_FORWARD){
    	//check to see if button is not being read too fast
    	if(time1[T1]>500){
    		if((SpindleState==Stopped)){
    		//start the spindle
    			motor[Spindle]=100;
    			SpindleState=Running;
    		}else{
    		//stop the spindle
    			motor[Spindle]=0;
    			SpindleState=Stopped;
    		}
    		//restart timer for button press for Spindle
    		clearTimer(T1);
    	}
  	}

  	if(BTN_GOAL_CLAW_TOGGLE){
    	//check to see if button is not being read too fast
    	if(time1[T3]>500){
    		if((servo[GoalClaw]==GOAL_CLAW_CLOSED)){
    		//open the claw
    			servo[GoalClaw]=GOAL_CLAW_OPEN;
    		}else{
    		//close the claw
    			servo[GoalClaw]=GOAL_CLAW_CLOSED;
    		}
    		//restart timer for button press for Spindle
    		clearTimer(T3);
    	}
  	}

  	 //spindle rotate backward
		/*
     if(BTN_SPINDLE_ROTATE_BACKWARD){
       //check to see if the button is not being pressed too fast.
    	if(time1[T1]>500){
    		if((SpindleState==Stopped)){
    		//start the spindle
    			motor[Spindle]=-100;
    			SpindleState=Running;
    		}else{
    		//stop the spindle
    			motor[Spindle]=0;
    			SpindleState=Stopped;
    		}
    		//restart timer for button press for Spindle
    		clearTimer(T1);
    	}
  	}
  */

  	//gate open

  	if(BTN_GATE_OPEN){
  		servo[Gate]=GATE_OPEN;
  	}

  	//gate close

   	if(BTN_GATE_CLOSED){
  		servo[Gate]=GATE_CLOSED;
  	}

  	if(BTN_LIFT_BASE){
  		MoveLiftToPosition(LIFT_BASE);
  	}

  	if(BTN_LIFT_LOWER_GOAL){
  		MoveLiftToPosition(LIFT_LOWER);
  	}

  	if(BTN_LIFT_MIDDLE_GOAL){
  		MoveLiftToPosition(LIFT_MIDDLE);
  	}

  	if(BTN_LIFT_TOP_GOAL){
  		MoveLiftToPosition(LIFT_TOP);
  	}


    //Wheel Control

    if(abs(joystick.joy1_y1) > threshold)   // If the right analog stick's Y-axis readings are either above or below the threshold:
    {

    			motor[RightWheels] = 0.25*joystick.joy1_y1;
    			//fine motor control (1/4th the speed)

              // Motor RightWheels is assigned a power level equal to the right analog stick's Y-axis reading.
    }
    else                                    // Else if the readings are within the threshold:
    {
      motor[RightWheels] = 0;                        // Motor RightWheels is stopped with a power level of 0.
    }


	if(abs(joystick.joy1_y2) > threshold)   // If the left analog stick's Y-axis readings are either above or below the threshold:
    {

    			motor[LeftWheels] = 0.25*joystick.joy1_y2;
    			//fine motor control

              // Motor LeftWheels is assigned a power level equal to the left analog stick's Y-axis reading.
    }
    else                                    // Else if the readings are within the threshold:
    {
      motor[LeftWheels] = 0;                        // Motor LeftWheels is stopped with a power level of 0.
    }


  }
}

task LiftSafetyUpperLimitWatch(){
	while(1){
		if(nMotorEncoder[Lift]>LIFT_LIMIT){
			//stop the lift
			motor[Lift]=0;
			//move the lift back down to the limit.
			while(nMotorEncoder[Lift]>LIFT_LIMIT){
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
		if(SensorValue[LiftLimitTouch]!=0){
			playTone(5000,5);
			//stop the lift
			motor[Lift]=0;
			//move the lift back down to the limit.
			while(nMotorEncoder[Lift]>LIFT_LIMIT){
				motor[Lift]=-10;
				LiftState=Running;
			}
			//now stop the motor again.
			motor[Lift]=0;
			LiftState=Stopped;
		}
	}

}


void MoveLiftToPosition(int EncoderValue)
{
	 nMotorPIDSpeedCtrl[Lift]=mtrSpeedReg;
	 CurrentPosition=nMotorEncoder[Lift];

	 //release the Hold Position task, to allow movement
   stopTask(HoldPosition);

   //close the gate, to ensure the safety of motion of the lift

   servo[Gate]=GATE_CLOSED;

	//Now check if we need to travel up or down.
	if(nMotorEncoder[Lift]>EncoderValue){
		//we need to move down


	  //check if we are travelling below the lower goal, special handling, until counterweight deployed.
		LiftState=Running;
		nMotorEncoderTarget[Lift]=(CurrentPosition-EncoderValue);
		motor[Lift]=-75;



		while((nMotorRunState[Lift]!=runStateIdle)&&(nMotorEncoder[Lift]>=LIFT_LOWER)){
			//let the motor lift run until motor becomes idle on reaching target or we reach the last segment down
		  //if we reach the last segment down we want to slow down.
		}

		if(nMotorEncoder[Lift]>EncoderValue){
			CurrentPosition=nMotorEncoder[Lift];
			//we are now below the height of the lower base and need to slow down to let the last segment fall slowly
			//This should only execute when the target is the base of the lift
			nMotorEncoderTarget[Lift]=(CurrentPosition-EncoderValue);
			motor[Lift]=-20;
			while(nMotorRunState[Lift]!=runStateIdle){
					//let the motor reach the target
			}
		}

	}else if(nMotorEncoder[Lift]<EncoderValue){
		//we need to move up

		nMotorEncoderTarget[Lift]=(EncoderValue-CurrentPosition);
		motor[Lift]=95;
		LiftState=Running;
		while(nMotorRunState[Lift]!=runStateIdle){
			//let the motor run till it reaches target
		}

	}
	//If we are already at the EncoderValue target we dont have to do anything

	//stop the motor
	motor[Lift]=0;
	LiftState=Stopped;
	TargetPosition=EncoderValue;
	if(EncoderValue!=LIFT_BASE){
		//if it is at lift base we dont need to hold position as it will be supported already
	  //otherwise we lock on to the position
		startTask(HoldPosition);
	}else{
		//not needed, just caution
		stopTask(HoldPosition);
	}
}

task HoldPosition()
{
	while(1){
		if(nMotorEncoder[Lift]>TargetPosition){
			motor[Lift]=-1*LIFT_HOLD_POSITION_POWER;
		}
		if(nMotorEncoder[Lift]<TargetPosition){
			motor[Lift]=LIFT_HOLD_POSITION_POWER;
		}
	}
}
