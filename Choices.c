#define DATA_FILE_NAME "choices.txt" //file we are going to write on


//Buttons
#define LEFT_BUTTON 2
#define RIGHT_BUTTON 1
#define CENTER_BUTTON 3

#define RAMP_START 1
#define PARKING_ZONE 0
#define BLOCK 3


task main()
{
	TFileIOResult nIoResult;
	TFileHandle myFileHandle;
	string startingPositionChoice;
	short myFileSize = 10;
	short startingPositionShort;
	short delayTime = 0;
	bool choicesConfirmed = false;

	Delete(DATA_FILE_NAME, nIoResult); // Deleting old file
	if(nIoResult) // Could not delete file
	{
		playTone(1000, 5);
	}

	while(!choicesConfirmed)
	{
		wait1Msec(500); // Need a half second delay to suppress multiple reads of the same key
		eraseDisplay();

		// Choose starting position
		displayTextLine(1, "Starting Pos:");
		displayTextLine(3,"Left = PZ");
		displayTextLine(4,"Right = Ramp");
		displayTextLine(5, "Center = Block");

		while ((nNxtButtonPressed != LEFT_BUTTON)&&(nNxtButtonPressed != RIGHT_BUTTON)&& (nNxtButtonPressed != CENTER_BUTTON))
		{ // Ignore everything except left, right, or center.
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
		else if (nNxtButtonPressed == CENTER_BUTTON)
		{
			startingPositionChoice = "Block";
			startingPositionShort = BLOCK;
		}

		wait1Msec(500);
		eraseDisplay();

		displayTextLine(1, "Need delay?");
		displayTextLine(3, "Left = Yes");
		displayTextLine(4, "Right = No");

		while ((nNxtButtonPressed != LEFT_BUTTON)&&(nNxtButtonPressed != RIGHT_BUTTON))
		{ // Ignore everything except left or right arrow.
			//intentional
		}

		if(nNxtButtonPressed == LEFT_BUTTON)
		{
			eraseDisplay();
			while(nNxtButtonPressed != CENTER_BUTTON)
			{
				displayTextLine(1, "Choose Delay Time:");
				displayTextLine(2, "Press center key to submit");
				displayTextLine(4, "%d", delayTime);

				if(nNxtButtonPressed == RIGHT_BUTTON)
				{
					if(time1[T1] > 500)		//checks to see if button isn't pressed too fast
					{
						++delayTime;
						eraseDisplay();
						clearTimer(T1);
					}
				}
				else if(nNxtButtonPressed == LEFT_BUTTON)
				{
					if(time1[T1] > 500)		//checks to see if button isn't pressed too fast
					{
						++delayTime;
						eraseDisplay();
						clearTimer(T1);
					}
				}
			}
			eraseDisplay();
		}
		else if (nNxtButtonPressed == RIGHT_BUTTON)
		{
			delayTime = 0;
		}

		wait1Msec(500);
		eraseDisplay();

		//confirmation
		displayTextLine(2, "Start pos: %s", startingPositionChoice);
		displayTextLine(3, "Delay Time: %d", delayTime);
		displayTextLine(4, "Left = Correct");
		displayTextLine(5, "Right = Redo");

		while ((nNxtButtonPressed != LEFT_BUTTON)&&(nNxtButtonPressed != RIGHT_BUTTON))
		{ // Ignore everything except left and right arrows.
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
	WriteShort(myFileHandle, nIoResult, delayTime);
	Close(myFileHandle, nIoResult);

	if(nIoResult) // Error to writing or closing file
	{
		playTone(10000, 5);
	}
}
