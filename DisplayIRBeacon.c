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

		//displayTextLine(1, "Sensors");

		//displays directional value
		displayTextLine(1, "Dir - %d",irSeeker.acDirection);
		displayTextLine(2, "EnhDir - %d",irSeeker.enhDirection);
		displayTextLine(3, "1-%d 2-%d 3-%d", irSeeker.acValues[0], irSeeker.acValues[1], irSeeker.acValues[2]);
		displayTextLine(4, "4-%d 5-%d",	irSeeker.acValues[3],	irSeeker.acValues[4]);

		displayTextLine(5, "Strength: %d", irSeeker.enhStrength);


		switch(irSeeker.acDirection)
		{
			case 0: // for Position 1
				displayTextLine(6,"Position: 1");
				break;

			case 3:	// for Position 2
				displayTextLine(6,"Position: 2");
				break;

			case 5:
				if(irSeeker.enhStrength > 65)
				{
					displayTextLine(6,"Position: 3");
				}
				else if((irSeeker.enhStrength > 30) && (irSeeker.enhStrength < 65)) // for Position 2
				{
					displayTextLine(6,"Position: 2");
				}
				else if(irSeeker.enhStrength < 30) // for Position 1
				{
					displayTextLine(6,"Position: 1");
				}
				break;
			default:
				displayTextLine(6, "Position: ?");
		} //while(true)
		wait1Msec(500);
	}
}
