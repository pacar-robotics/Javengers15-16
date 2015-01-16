#pragma config(Sensor, S2,     irseeker,       sensorHiTechnicIRSeeker1200)

#include "hitechnic-irseeker-v2.h"

#define IR600 DSP_600
#define IR1200 DSP_1200

void displayIRBeaconValues();

tHTIRS2 irSeeker;

task main()
{
	initSensor(&irSeeker, S2);
	irSeeker.mode = IR600;

	displayIRBeaconValues();
}

void displayIRBeaconValues()
{
	int Position;
	while(1)
	{
		eraseDisplay();
		readSensor(&irSeeker);
		displayTextLine(1, "Sensors");
		displayTextLine(2, "Dir-%d EnhDir-%d",irSeeker.acDirection,irSeeker.enhDirection);
		displayTextLine(3, "1-f%d 2-%d 3-%d",
		irSeeker.acValues[0],
		irSeeker.acValues[1],
		irSeeker.acValues[2],
		);

		displayTextLine(4, "4-%d 5-%d",
		irSeeker.acValues[3],
		irSeeker.acValues[4]);

		displayTextLine(5, "Strength:%d", irSeeker.enhStrength);

		if(irSeeker.enhStrength>9)
		{
			Position=3;
		}
		else if((irSeeker.enhStrength>5)&(irSeeker.acValues[1]>5))
		{
			Position=2;
		}
		else
		{
			Position=1;
		}

		displayTextLine(6,"Position: %d", Position);
		wait1Msec(500);
	}
}
