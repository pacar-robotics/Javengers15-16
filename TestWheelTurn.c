#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Motor,  mtr_S1_C1_1,     LeftWheels,    tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     RightWheels,   tmotorTetrix, openLoop, reversed, encoder)
//*!!Code automatically generated by K^3S^3T Hogwarts wizards               !!*//

#define TURN_TIME 5000
#define TURN_POWER 75

void doubleTurn();
void singleTurn();

task main()
{
	doubleTurn();
	wait1Msec(1000);
	singleTurn();
}

void doubleTurn()
{
	eraseDisplay();
	displayBigTextLine(2, "Double");
	motor[LeftWheels] = TURN_POWER;
	motor[RightWheels] = -1 * TURN_POWER;
	wait1Msec(TURN_TIME);
	motor[RightWheels] = TURN_POWER;
	motor[LeftWheels] = -1 * TURN_POWER;
	wait1Msec(TURN_TIME);
	motor[RightWheels] = 0;
	motor[LeftWheels] = 0;
}


void singleTurn()
{
	eraseDisplay();
	displayBigTextLine(2, "Single");
	motor[LeftWheels] = TURN_POWER;
	wait1Msec(TURN_TIME);
	motor[LeftWheels] = 0;
	wait1Msec(500);
	motor[RightWheels] = TURN_POWER;
	wait1Msec(TURN_TIME);
	motor[RightWheels] = 0;
}
