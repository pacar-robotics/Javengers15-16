#define DATA_FILE_NAME "choices.txt"

//IR Frequencies
#define IR600 DSP_600
#define IR1200 DSP_1200

//Buttons
#define LEFT_BUTTON 2
#define RIGHT_BUTTON 1

#define RAMP_START 1
#define PARKING_ZONE 0

#define NEED_DELAY 1
#define NO_DELAY 0

task main()
{
	TFileIOResult nIoResult;
	TFileHandle myFileHandle;
	string fileLine;
	short myFileSize = 10;
	short irChoice;
	string startingPositionChoice;
	short startingPositionShort;
	string delayChoice;
	short delayShort;
	bool choicesConfirmed = false;
	//deleting old file
	Delete(DATA_FILE_NAME, nIoResult);
	if(nIoResult)
	{
		//error - could not delete file
		playTone(1000, 5);
	}
	while(!choicesConfirmed)
	{
		wait1Msec(500); //need a half second delay to suppress multiple reads of the same key
		eraseDisplay();
		//choose frequency
		//left button for 600, right button for 1200
		displayTextLine(1, "Choose frequency:");
		displayTextLine(2,"Left== 600");
		displayTextLine(3,"Right==1200");

		//ignore everything until left or right arrow pressed.
		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON)){
			//intentional
		}

		if(nNxtButtonPressed == LEFT_BUTTON)
		{

			irChoice = 600;
		}
		else if(nNxtButtonPressed == RIGHT_BUTTON)
		{
			irChoice = 1200;
		}

		wait1Msec(500);
		eraseDisplay();


		//choose starting position
		//left button for parking zone, right button for ramp
		displayTextLine(1, "Starting Pos:");
		displayTextLine(2,"Left==PZ");
		displayTextLine(3,"Right==Ramp");

		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON)){
			//intentional
		}

		if(nNxtButtonPressed == LEFT_BUTTON)
		{
			startingPositionChoice = "PZ";
			startingPositionShort = PARKING_ZONE;
		}else if (nNxtButtonPressed == RIGHT_BUTTON)
		{
			startingPositionChoice = "Ramp";
			startingPositionShort = RAMP_START;
		}
		wait1Msec(500);
		eraseDisplay();


		//choose delay
		//left button for yes, right button for no
		displayTextLine(1, "Need delay?");
		displayTextLine(2, "Left == Yes");
		displayTextLine(3, "Right== No");


		//ignore everything except left or right arrow.
		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON)){
			//intentional
		}
		if(nNxtButtonPressed == LEFT_BUTTON)
		{
			delayChoice = "Yes";
			delayShort = NEED_DELAY;
		}//if delay
		else if (nNxtButtonPressed == RIGHT_BUTTON)
		{
			delayChoice = "None";
			delayShort = NO_DELAY;
		}
		wait1Msec(500);
		eraseDisplay();


		//confirmation
		displayTextLine(1, "Freq: %d", irChoice);
		displayTextLine(2, "Start pos: %s", startingPositionChoice);
		displayTextLine(3, "Delay: %s", delayChoice);
		displayTextLine(4, "Left == Correct");
		displayTextLine(5, "Right == Redo");

		//ignore everything except left and right arrows.

		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON)){
			//intentional
		}

		if(nNxtButtonPressed == LEFT_BUTTON)
		{
			//confirmed
			choicesConfirmed = true;
		}
	}//!choices confirmed
	OpenWrite(myFileHandle, nIoResult, DATA_FILE_NAME, myFileSize);
	if(nIoResult)
	{
		//error in opening file for write
		playTone(5000, 5);
		stopAllTasks();
	}
	WriteShort(myFileHandle, nIoResult, irChoice);
	WriteShort(myFileHandle, nIoResult, startingPositionShort);
	WriteShort(myFileHandle, nIoResult, delayShort);
	Close(myFileHandle, nIoResult);
	if(nIoResult)
	{
		playTone(10000, 5);
	}
}
