#define DATA_FILE_NAME "choices.txt" //file we are going to write on

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
	string startingPositionChoice;
	short startingPositionShort;
	string delayChoice;
	short delayShort;
	bool choicesConfirmed = false;

	Delete(DATA_FILE_NAME, nIoResult); // Deleting old file

	if(nIoResult) // Error - could not delete file
	{
		playTone(1000, 5);
	}

	while(!choicesConfirmed)
	{
		wait1Msec(500); // Need a half second delay to suppress multiple reads of the same key
		eraseDisplay();

		// Choose starting position
		displayTextLine(1, "Starting Pos:");
		displayTextLine(2,"Left = PZ");
		displayTextLine(3,"Right = Ramp");

		// Ignore everything except left or right arrow.
		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON))
		{
			//intentional
		}

		if(nNxtButtonPressed == LEFT_BUTTON)
		{
			startingPositionChoice = "PZ";
			startingPositionShort = PARKING_ZONE;
		}
		else if (nNxtButtonPressed == RIGHT_BUTTON)
		{
			startingPositionChoice = "Ramp";
			startingPositionShort = RAMP_START;
		}

		wait1Msec(500);
		eraseDisplay();

		displayTextLine(1, "Need delay?");
		displayTextLine(2, "Left = Yes");
		displayTextLine(3, "Right = No");


		// Ignore everything except left or right arrow.
		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON))
		{
			//intentional
		}

		if(nNxtButtonPressed == LEFT_BUTTON)
		{
			delayChoice = "Yes";
			delayShort = NEED_DELAY;
		}
		else if (nNxtButtonPressed == RIGHT_BUTTON)
		{
			delayChoice = "None";
			delayShort = NO_DELAY;
		}

		wait1Msec(500);
		eraseDisplay();


		//confirmation
		displayTextLine(2, "Start pos: %s", startingPositionChoice);
		displayTextLine(3, "Delay: %s", delayChoice);
		displayTextLine(4, "Left =  Correct");
		displayTextLine(5, "Right = Redo");

		// Ignore everything except left and right arrows.
		while ((nNxtButtonPressed != LEFT_BUTTON) && (nNxtButtonPressed != RIGHT_BUTTON))
		{
			//intentional
		}

		if(nNxtButtonPressed == LEFT_BUTTON) // Confirmed
		{
			choicesConfirmed = true;
		}
	}// while(!choices)

	OpenWrite(myFileHandle, nIoResult, DATA_FILE_NAME, myFileSize);

	if(nIoResult)
	{
		//error in opening file for write
		playTone(5000, 5);
		stopAllTasks();
	}

	//writing on to file
	WriteShort(myFileHandle, nIoResult, startingPositionShort);
	WriteShort(myFileHandle, nIoResult, delayShort);
	Close(myFileHandle, nIoResult);

	if(nIoResult)
	{
		playTone(10000, 5);
	}
}
