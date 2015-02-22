#define PARAMETER_FILE_NAME "Powerfactors.txt" //file we are going to write on

// Buttons
#define LEFT_BUTTON 2
#define RIGHT_BUTTON 1
#define CENTER_BUTTON 3

void readOldValues();
float decidePowerfactor(const string parameterName, float defaultValue);
void writeParameterFile();
bool confirmValues();
void deleteParameterFile();

float mainPowerFactor;
float scorerPowerFactor;
float mainPowerFactorTurn;
float scorerPowerFactorTurn;


TFileHandle myFileHandle;
TFileIOResult nIoResult;
short myFileSize = 100;

task main()
{
	readOldValues();

	do
	{
		mainPowerFactor = decidePowerfactor("Main Straight", mainPowerFactor);
		wait1Msec(500);

		scorerPowerFactor = decidePowerfactor("Scorer Straight", scorerPowerFactor);
		wait1Msec(500);

		mainPowerFactorTurn = decidePowerfactor("Main Turn", mainPowerFactorTurn);
		wait1Msec(500);

		scorerPowerFactorTurn = decidePowerfactor("Scorer Turn", scorerPowerFactorTurn);
		wait1Msec(500);
	} while(!confirmValues());

	writeParameterFile();
}

void deleteParameterFile()
{
	Delete(PARAMETER_FILE_NAME, nIoResult); // Deleting old file

	if(nIoResult) // Error occurred
	{
		playTone(1000, 5);
	}
}

float decidePowerfactor(const string parameterName, float defaultValue)
{
	float powerFactor = defaultValue;

	while(nNxtButtonPressed != CENTER_BUTTON)
	{
		eraseDisplay();
		displayTextLine(1, parameterName);
		displayBigTextLine(3, "%3.2f", powerFactor);
		displayTextLine(6, "Press center key to submit");
		wait1Msec(250);

		if(nNxtButtonPressed == RIGHT_BUTTON)
		{
			powerFactor += 0.05;
		}
		else if(nNxtButtonPressed == LEFT_BUTTON)
		{
			powerFactor -= 0.05;
		}
	}

	return powerFactor;
}

void readOldValues()
{
	OpenRead(myFileHandle, nIoResult, PARAMETER_FILE_NAME, myFileSize);

	if(nIoResult)
	{ // No file exists. Set default values
		playTone(5000, 1);
		mainPowerFactor = 1;
		mainPowerFactorTurn = 1;
		scorerPowerFactor = 0.5;
		scorerPowerFactorTurn = 0.5;
	}
	else
	{
		ReadFloat(myFileHandle, nIoResult, mainPowerFactor);
		ReadFloat(myFileHandle, nIoResult, scorerPowerFactor);
		ReadFloat(myFileHandle, nIoResult, mainPowerFactorTurn);
		ReadFloat(myFileHandle, nIoResult, scorerPowerFactorTurn);
	}

	Close(myFileHandle, nIoResult);
}

bool confirmValues()
{
	displayTextLine(1, "Main Str: %3.2f", mainPowerFactor);
	displayTextLine(2, "Score Str: %3.2f", scorerPowerFactor);
	displayTextLine(3, "Main Turn: %3.2f", mainPowerFactorTurn);
	displayTextLine(4, "Score Turn: %3.2f", scorerPowerFactorTurn);
	displayTextLine(5, "Left = Correct");
	displayTextLine(6, "Right = Redo");

	while ((nNxtButtonPressed != LEFT_BUTTON)&&(nNxtButtonPressed != RIGHT_BUTTON))
	{ // Ignore everything except left and right arrows.
		//intentional
	}

	if(nNxtButtonPressed == LEFT_BUTTON) // Confirmed
	{
		return true;
	}
	else
	{
		return false;
	}
}

void writeParameterFile()
{
	deleteParameterFile();

	OpenWrite(myFileHandle, nIoResult, PARAMETER_FILE_NAME, myFileSize);
	if(nIoResult) // Error in opening file for write
	{
		playTone(4000, 50);
		return;
	}

	WriteFloat(myFileHandle, nIoResult, mainPowerFactor);
	WriteFloat(myFileHandle, nIoResult, scorerPowerFactor);
	WriteFloat(myFileHandle, nIoResult, mainPowerFactorTurn);
	WriteFloat(myFileHandle, nIoResult, scorerPowerFactorTurn);
	Close(myFileHandle, nIoResult);

	if(nIoResult)
	{
		playTone(4000, 50);
	}

}
