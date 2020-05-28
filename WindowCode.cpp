#include <iostream>
using namespace std;

int preferredTemperature = 70;
bool isOpen = false;


void closeWindow()
{
   isOpen = false;
   // code to trigger mechanisms goes here...
}

void openWindow()
{
   isOpen = true;
   // code to trigger mechanisms goes here...
}

void changePreferredTemperature(int preferrence)
{
   preferredTemperature = preferrence;
}

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
   if (isOpen)
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
   if (!isOpen)
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


/**********************************************************************
 * Function: main
 * Purpose: This is the entry point and driver for the program.
 ***********************************************************************/
int main()
{
   //TESTS____________________________________________________________________________________
   isOpen = false;
   cout << findClosestToPreferredTemperature(65, 73) << endl; //outside is closer... so 1
   cout << ShouldWindowOpen(33, 72) << endl; //expected 1
   cout << ShouldWindowOpen(72, 33) << endl; //expected 0
   cout << ShouldWindowOpen(72, 75) << endl; //expected 0

   isOpen = true;
   cout << endl << ShouldWindowClose(33, 72) << endl; //expected 0
   cout << ShouldWindowClose(72, 33) << endl; //expected 1
   cout << ShouldWindowClose(72, 75) << endl; //expected 1

   return 0;
}