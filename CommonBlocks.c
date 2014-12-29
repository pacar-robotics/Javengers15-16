//Measurements
#define DIAMETER 7.62  //corrected
#define TRACK_DISTANCE 48 //corrected


//Directions
#define FORWARD true
#define BACKWARD false
#define CLOCKWISE true
#define COUNTER_CLOCKWISE false




//These match #defined values in this file.
void calcMove(float centimeters, float power, bool direction)
{
	float encoder_counts;

	//set motors to PID control.

	nMotorPIDSpeedCtrl[LeftWheels]=mtrSpeedReg;
	nMotorPIDSpeedCtrl[RightWheels]=mtrSpeedReg;

	//confirm motors are not moving before we start.

	nMotorEncoder[LeftWheels] = 0;
	nMotorEncoder[RightWheels] = 0;

	//since robot c uses encoder counts we have to convert distance in centimeters to encoder counts.

	encoder_counts = (centimeters/(DIAMETER*PI)) * 1440; // converts centimeters to motor encoder counts //


	//check for direction.

	if (!direction){
			power = -1 * power;
			encoder_counts=-1*encoder_counts;
		}

	//set the target encoder values before we set power to start motors.

  nMotorEncoderTarget[LeftWheels]=encoder_counts;
  nMotorEncoderTarget[RightWheels]=encoder_counts;


  motor[LeftWheels]=power;
  motor[RightWheels]=power;

  while((nMotorRunState[LeftWheels]!=runStateIdle)&&(nMotorRunState[RightWheels]!=runStateIdle)){
  	//do nothing while we wait for motors to spin to correct distance.
  }

	//stop the motors

	motor[LeftWheels] = 0;
	motor[RightWheels] = 0;


	//set back to unregulated gmotors just so we dont have the wrong mode when exiting
	nMotorPIDSpeedCtrl[LeftWheels]=mtrNoReg;
	nMotorPIDSpeedCtrl[RightWheels]=mtrNoReg;

}

void dualMotorTurn(float robotDegrees, float power, bool direction) //robot turns using both motors
{
	//full calc of encoder count = (((TRACK_DISTANCE/DIAMETER)/360)*robotDegrees*1440) // no divide by two, because we are using
  //full diameter. The line below uses a reduced formula. The track distance has been adjusted for effects of using 4 traction wheels.


	float encoderCounts = (TRACK_DISTANCE/DIAMETER)*robotDegrees*4;

	//set motors to No PID control as differential turns have problems

	nMotorPIDSpeedCtrl[LeftWheels]=mtrNoReg;
	nMotorPIDSpeedCtrl[RightWheels]=mtrNoReg;

	//ensure motors are stopped before starting
	nMotorEncoder[LeftWheels] = 0;
	nMotorEncoder[RightWheels] = 0;

	//set the Encoder Targets before powering the turn.
	if (direction == COUNTER_CLOCKWISE)	//turns robot clockwise or counter-clockwise, depending on user input
	{
		nMotorEncoderTarget[LeftWheels]=encoderCounts;
		nMotorEncoderTarget[RightWheels]=-1*encoderCounts;
		motor[LeftWheels]=power;
		motor[RightWheels]=-1*power;
	}
	else
	{
		nMotorEncoderTarget[LeftWheels]=-1*encoderCounts;
		nMotorEncoderTarget[RightWheels]=encoderCounts;
		motor[LeftWheels]=-1*power;
		motor[RightWheels]=power;

	}



	while((nMotorRunState[LeftWheels]!=runStateIdle)&&(nMotorRunState[RightWheels]!=runStateIdle)){
  	//do nothing while we wait for motors to spin to correct angles.
  }

	//stop the motors

	motor[LeftWheels] = 0;
	motor[RightWheels] = 0;


	//set back to unregulated gmotors just so we dont have the wrong mode when exiting
	nMotorPIDSpeedCtrl[LeftWheels]=mtrNoReg;
	nMotorPIDSpeedCtrl[RightWheels]=mtrNoReg;

}
