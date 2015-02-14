#pragma platform(FTC)         // Only works for "FTC" platform. Generate error on other platforms.
#pragma autoStartTasks        // Automatically start this task when the main user program starts.

typedef struct
{
  int nLine;
  int nSizeOfMessage;
  string message;  // 20 byte char
} TSamanathaStatusMessage;
TSamanathaStatusMessage samanthaStatusCopy;

const TMailboxIDs kQueueID = mailbox2;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// "Macro" to get a non-volatile copy of the last joystick settings so
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

#define readSamanthaStatus(dest) 	memcpy(dest, samanthaStatusCopy, sizeof(dest))

task getSamanthaStatusTask()
{
  int nSizeOfMessage;
  int nNumbOfMessages;
  int i;

  TFileIOResult nBTCmdRdErrorStatus;
  const int kMaxSizeOfMessage = 50;
  ubyte tempBuffer[kMaxSizeOfMessage];

  samanthaStatusCopy.nLine = 0;
  samanthaStatusCopy.message = "WiFi Stats";

  while (true)
  {
    // Check to see if a message is available.
    nNumbOfMessages = 0;
    while (true)
    {
      nSizeOfMessage = cCmdMessageGetSize(kQueueID);

      if (nSizeOfMessage > 0)
      {
        break;
      }
      wait1Msec(4);    // Give other tasks a chance to run
    }
    //
    // We got a message, so read it in
    //
    if (nSizeOfMessage > sizeof(tempBuffer))
      nSizeOfMessage = sizeof(tempBuffer);
    nBTCmdRdErrorStatus = cCmdMessageRead(tempBuffer, nSizeOfMessage, kQueueID);
    nBTCmdRdErrorStatus = nBTCmdRdErrorStatus; //Get rid of info message
    // Once we've reached here, a valid message is available

    hogCPU();   // grab CPU for duration of critical section

    samanthaStatusCopy.nLine = (int)tempBuffer[0];
    samanthaStatusCopy.nSizeOfMessage = (int)nSizeOfMessage;

    //    StringFromChars(samanthaStatusCopy.message,tempBuffer);
    samanthaStatusCopy.message ="";
    for(i=0; i<20; i++)
      samanthaStatusCopy.message += tempBuffer[i+1];

    releaseCPU(); // end of critical section
  }
}
task main()
{
  TSamanathaStatusMessage status;

  while (true)
  {
    readSamanthaStatus(status);
    nxtDisplayTextLine(status.nLine, "%s", status.message);
  }
}
