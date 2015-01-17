#pragma config(Sensor, S2,     irseeker,       sensorHiTechnicIRSeeker1200)

/*
			Display IR Beacon Values
																*/

#include "hitechnic-irseeker-v2.h"	//for ir seeker

//ir wave values
#define IR600 DSP_600
#define IR1200 DSP_1200

//file used in Choice.c
#define DATA_FILE_NAME "choices.txt"


void displayIRBeaconValues();

tHTIRS2 irSeeker;

task main()
{
	initSensor(&irSeeker, S2);
	irSeeker.mode = IR600;	//mode set to 600

	displayIRBeaconValues();
}

void displayIRBeaconValues()
{
	int Position;
	//tHTIRS2DSPMode irFrequency;
	TFileIOResult nIoResult;
	TFileHandle myFileHandle;
	short myFileSize = 10;
	short irChoice;

	OpenRead(myFileHandle, nIoResult, DATA_FILE_NAME, myFileSize);

	if(nIoResult) // Error in opening file for read
	{
		// NEED A BACK-UP PLAN
		playTone(5000, 5);
	  stopAllTasks();
	}

	//reads choices
	ReadShort(myFileHandle, nIoResult, irChoice);

	//sets values depending on choices
	if(irChoice == 1200)
	{
		irSeeker.mode = IR1200;
	}
	else
	{
		irSeeker.mode = IR600;
	}

	while(true)
	{
		eraseDisplay();
		readSensor(&irSeeker);

		displayTextLine(1, "Sensors");

		//displays directional value
		displayTextLine(2, "Dir-%d EnhDir-%d",irSeeker.acDirection,irSeeker.enhDirection);
		displayTextLine(3, "1-f%d 2-%d 3-%d", irSeeker.acValues[0], irSeeker.acValues[1], irSeeker.acValues[2]);
		displayTextLine(4, "4-%d 5-%d",
		irSeeker.acValues[3],
		irSeeker.acValues[4]);

		displayTextLine(5, "Strength:%d", irSeeker.enhStrength);


		//tells what position center field goal is in
		if(irSeeker.enhStrength>9)
		{
			Position = 3;
		}
		else if((irSeeker.enhStrength > 5) & (irSeeker.acValues[1] > 5))
		{
			Position = 2;
		}
		else
		{
			Position=1;
		}

		displayTextLine(6,"Position: %d", Position);
		wait1Msec(500);
	} //while(true)
}
