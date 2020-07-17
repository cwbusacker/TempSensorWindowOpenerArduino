#include "DHT.h"
#include <LiquidCrystal.h>

#define DHTTYPE DHT11
#define NOTE_G4  392
#define NOTE_C5  523
#define NOTE_G5  784
#define NOTE_C6  1047

const int rs = A4, en = A5, d4 = A0, d5 = A1, d6 = A2, d7 = A3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 250;

/*SETTINGS*/
int preferredTemperature = 68;
bool justChangedPrefTemp = false;

//pins
const int TempInPin = 52;
const int TempOutPin = 22;
const int magnetPin = 13;
const int alarmPin = 50;

//minutes between temperature readings
const int mins = 5;


//use an array of ints and play melody of ascending notes
void playSoundOpen()
{
  int Sound[] = {NOTE_G4, NOTE_C5, NOTE_G5, NOTE_C6};
  int SoundNoteDurations[] = {12, 8, 6, 4};
  playMelody(Sound, SoundNoteDurations, 4, 2);

}

void playSoundChangePref()
{
  int Sound[] = {NOTE_G4,NOTE_G4};
  int SoundNoteDurations[] = {20, 20};
  playMelody(Sound, SoundNoteDurations, 2, 1);
}

//use an array of ints and play melody of descending notes
void playSoundClose()
{
  int Sound[] = {NOTE_C6, NOTE_G5, NOTE_C5, NOTE_G4};
  int SoundNoteDurations[] = {12, 8, 6, 4};
  playMelody(Sound, SoundNoteDurations, 4, 2);
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
   //if open... done.. 
   if (digitalRead(magnetPin) == HIGH)
   {
      return false;
   }
   else if(tempInside > preferredTemperature && tempOutside < preferredTemperature || 
      tempInside < preferredTemperature && tempOutside > preferredTemperature)
   {
    //the window needs opened because the outdoor temperature will help us reach preferred temp
    return true;
   }
   
   //if outside is closest to the preferred temp... then we should open it.
   else if (findClosestToPreferredTemperature(tempInside,tempOutside) == 1)
   {
   //   Serial.println("We should open");
      return true;
   }
   else
   {
  //    Serial.println("We should not open");
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
   //if closed.... done.
   if (digitalRead(magnetPin) == LOW)
   {
      return false;
   }
   else if(tempInside > preferredTemperature && tempOutside < preferredTemperature || 
           tempInside < preferredTemperature && tempOutside > preferredTemperature)
   {
    //the outside temperature can help reach the preferred temperature because it's either
      //a. warmer inside than the preffered temp AND colder outside than the preffered temp 
      //OR
      //b. colder inside than the preferred temp AND warmer outside than the preferred temp
      //therefore, we should not close!
    return false;
   }
   
   //if inside is closest to the preferred temp... then we should close it.
   else if (findClosestToPreferredTemperature(tempInside,tempOutside) == 0)
   {
      //we should close
      return true;
   }
   else
   {
      //do not close
      return false;
   }
}

void setup()
{
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), increaseTemp, RISING);
  attachInterrupt(digitalPinToInterrupt(3), decreaseTemp, RISING);
    // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  // Print a message to the LCD.
  lcd.print("Temperature Project");
  
  //setup the dht sensors
  dhtIndoor.begin();
  dhtOutdoor.begin();
  
  //setup the alarm and the magnetPin which checks to see if window is open or closed
  pinMode(alarmPin, OUTPUT);
  pinMode(magnetPin, INPUT_PULLUP);
}

void loop() {

  //Dht SENSOR: https://learn.adafruit.com/dht
  
  //Read indoor temperature
  float indoorH = dhtIndoor.readHumidity();
  float indoorTempF = dhtIndoor.readTemperature(true);

  //Read outdoor temperature
  float outdoorH = dhtOutdoor.readHumidity();
  float outdoorTempF = dhtOutdoor.readTemperature(true); 

  //Converted everything to Heat indexes using the dht library
  float indoorHI = dhtIndoor.computeHeatIndex(indoorTempF, indoorH);
  float outdoorHI = dhtOutdoor.computeHeatIndex(outdoorTempF, outdoorH);

  
   lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print("In Temp:  ");
  lcd.print(indoorTempF);
  lcd.print((char)223);
  lcd.print("F");
  
  lcd.setCursor(0, 2);
  lcd.print("Out Temp: ");
  lcd.print(outdoorTempF);
  lcd.print((char)223);
  lcd.print("F");
  
  lcd.setCursor(0, 3);
  lcd.print("Our Temp: ");
  lcd.print(preferredTemperature);
  lcd.print((char)223);
  lcd.print("F");
  if(ShouldWindowClose(indoorHI, outdoorHI))
  {
    //notify the user to close the window
    playSoundClose();
  }
  else if(ShouldWindowOpen(indoorHI, outdoorHI))
  {
    //notify the user to open the window
    playSoundOpen();
  }


for(int i = 0; i < mins*60 * 250; i++)
{
   delay(30);
   if(justChangedPrefTemp)
  {
    justChangedPrefTemp = false;
    lcd.setCursor(10, 3);
    lcd.print(preferredTemperature);
    playSoundChangePref();
  }
}

 
  
}

void playMelody(int *melody, int *noteDurations, int notesLength, int reps)
{
  for(int num = 0; num < reps; num++)
  {
    for (int thisNote = 0; thisNote < notesLength; thisNote++) {
      int noteDuration = 2000 / noteDurations[thisNote];
      tone(alarmPin, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(alarmPin);
    }
  }
}

void increaseTemp()
{
  if((millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis();
    preferredTemperature++;
    justChangedPrefTemp = true;
  }
}

void decreaseTemp()
{
  if((millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis();
    preferredTemperature--;
    justChangedPrefTemp = true;
  }
}
