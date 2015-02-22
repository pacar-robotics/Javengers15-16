#define FILE_NAME "ScorerPowerfactor.txt" //file we are going to write on

// Buttons
#define LEFT_BUTTON 2
#define RIGHT_BUTTON 1
#define CENTER_BUTTON 3

float decidePowerfactor();
void deleteFile();

TFileHandle myFileHandle;
TFileIOResult nIoResult;
short myFileSize = 10;

task main()
{
	float scorerPowerFactor = decidePowerfactor();
	OpenWrite(myFileHandle, nIoResult, FILE_NAME, myFileSize);

	if(nIoResult) // Error in opening file for write
	{
		playTone(5000, 5);
		return;
	}

	WriteFloat(myFileHandle, nIoResult, scorerPowerFactor);
	Close(myFileHandle, nIoResult);

	if(nIoResult)
		playTone(5000, 5);
}

void deleteFile()
{
	Delete(FILE_NAME, nIoResult); // Deleting old file

	if(nIoResult) // Error occurred
		playTone(1000, 5);
}

float decidePowerfactor()
{
	float scorerPowerFactor;
	OpenRead(myFileHandle, nIoResult, FILE_NAME, myFileSize);
	ReadFloat(myFileHandle, nIoResult, scorerPowerFactor);
	Close(myFileHandle, nIoResult);
	deleteFile();

	if(nIoResult) // If an error happened, then use the default value
		scorerPowerFactor = 0.125;

	displayBigTextLine(3, "%.3f", scorerPowerFactor);

	while(nNxtButtonPressed != CENTER_BUTTON)
	{
		if(nNxtButtonPressed == RIGHT_BUTTON)
			scorerPowerFactor =+ 0.005;
		else if(nNxtButtonPressed == LEFT_BUTTON)
			scorerPowerFactor =- 0.005;

		eraseDisplay();
		displayBigTextLine(3, "%.3f", scorerPowerFactor);
		displayTextLine(6, "Press center key to submit");
	}

	return scorerPowerFactor;
}
