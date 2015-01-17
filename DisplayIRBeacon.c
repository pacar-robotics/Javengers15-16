#pragma config(Sensor, S2,     irseeker,       sensorHiTechnicIRSeeker1200)

/*
			Display IR Beacon Values
																*/

#include "hitechnic-irseeker-v2.h"	//for ir seeker


void displayIRBeaconValues();

tHTIRS2 irSeeker;

task main()
{
	initSensor(&irSeeker, S2);
	wait1Msec(500);
	//for sensor to settle.
	irSeeker.mode=DSP_1200;




	displayIRBeaconValues();
}

void displayIRBeaconValues()
{

	while(true)
	{
		eraseDisplay();
		readSensor(&irSeeker);

		displayTextLine(1, "Sensors");

		//displays directional value
		displayTextLine(2, "Dir-%d EnhDir-%d",irSeeker.acDirection,irSeeker.enhDirection);
		displayTextLine(3, "1-%d 2-%d 3-%d", irSeeker.acValues[0], irSeeker.acValues[1], irSeeker.acValues[2]);
		displayTextLine(4, "4-%d 5-%d",
		irSeeker.acValues[3],
		irSeeker.acValues[4]);

		displayTextLine(5, "Strength:%d", irSeeker.enhStrength);


		//tells what position center field goal is in

		switch(irSeeker.acDirection){
			case 0:	// for Position 1
				displayTextLine(6,"Position: 1");

			break;

		case 3:	// for Position 2
				displayTextLine(6,"Position: 2");
			break;

		case 5:	// for Position 3
				displayTextLine(6,"Position: 3");
			break;
		}

		wait1Msec(500);
	} //while(true)
}
