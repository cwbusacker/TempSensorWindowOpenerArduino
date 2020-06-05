#include "DHT.h"

#define DHTTYPE DHT11
#define NOTE_G4  392
#define NOTE_C5  523
#define NOTE_G5  784
#define NOTE_C6  1047

/*SETTINGS*/
const int preferredTemperature = 69;

//pins
const int TempInPin = 2;
const int TempOutPin = 3;
const int magnetPin = 4;
const int alarmPin = 5;

//minutes between temperature readings
const int mins = 15;
/**********/

void playSoundOpen()
{
  int Sound[] = {NOTE_G4, NOTE_C5, NOTE_G5, NOTE_C6};
  int SoundNoteDurations[] = {4, 6, 8, 12};
  playMelody(Sound, SoundNoteDurations, 4);
}

void playSoundClose()
{
  int Sound[] = {NOTE_C6, NOTE_G5, NOTE_C5, NOTE_G4};
  int SoundNoteDurations[] = {12, 8, 6, 4};
  playMelody(Sound, SoundNoteDurations, 4);
}


DHT dhtIndoor(TempInPin, DHTTYPE);
DHT dhtOutdoor(TempOutPin, DHTTYPE);


/**********************************************************************
 * return 0 if tempA is closer. otherwise return 1. (ties go to B)
 ***********************************************************************/
int findClosestToPreferredTemperature(int tempA, int tempB)
{
   //find distances
   int distanceFromA = preferredTemperature - tempA;
   if (distanceFromA < 0)
   {
      distanceFromA *= -1;
   }
   int distanceFromB = preferredTemperature - tempB;
   if (distanceFromB < 0)
   {
      distanceFromB *= -1;
   }
   //make decision based off distances
   if (distanceFromA < distanceFromB)
   {
      return 0;
   }
   else
   {
      return 1;
   }
}

/**********************************************************************
 * returns if the window should open or not.
 * it returns false if the window is already open so that if it is 
 * later compatable with something to open the window, we dont want to
 * break anything by forcing it more open than it is.
 ***********************************************************************/
bool ShouldWindowOpen(int tempInside, int tempOutside)
{
   //if open... done.. why we asking?
   if (digitalRead(magnetPin) == LOW)
   {
      return false;
   }
   //if outside is closest to the preferred temp... then we should open it.
   if (findClosestToPreferredTemperature(tempInside,tempOutside) == 1)
   {
      return true;
   }
   else
   {
      return false;
   }
}

/**********************************************************************
 * returns if the window should close or not.
 * it returns false if the window is already closed so that if it is 
 * later compatable with something to close the window, we dont want to
 * break anything by forcing it closed more than it is.
 ***********************************************************************/
bool ShouldWindowClose(int tempInside, int tempOutside)
{
   //if closed.... done.. why we asking?
   if (digitalRead(magnetPin) == HIGH)
   {
      return false;
   }
   //if inside is closest to the preferred temp... then we should close it.
   if (findClosestToPreferredTemperature(tempInside,tempOutside) == 0)
   {
      return true;
   }
   else
   {
      return false;
   }
}

void setup() {
  // put your setup code here, to run once:
  dhtIndoor.begin();
  dhtOutdoor.begin();
  pinMode(alarmPin, OUTPUT);
  pinMode(magnetPin, INPUT);

}

void loop() {
  float indoorH = dhtIndoor.readHumidity();
  //true means fahrenheit
  float indoorTempF = dhtIndoor.readTemperature(true);

  float outdoorH = dhtOutdoor.readHumidity();
  //true means fahrenheit
  float outdoorTempF = dhtOutdoor.readTemperature(true); 

  float indoorHI = dhtIndoor.computeHeatIndex(indoorTempF, indoorH);
  float outdoorHI = dhtOutdoor.computeHeatIndex(outdoorTempF, outdoorH);

  if(ShouldWindowClose(indoorHI, outdoorHI))
  {
    playSoundOpen();
  }
  else if(ShouldWindowOpen(indoorHI, outdoorHI))
  {
    playSoundClose();
  }

 delay(minutes*60000); //wait a little while between reads
  
}

void playMelody(int *melody, int *noteDurations, int notesLength)
{

  for (int thisNote = 0; thisNote < notesLength; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(alarmPin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(alarmPin);
  }
}
