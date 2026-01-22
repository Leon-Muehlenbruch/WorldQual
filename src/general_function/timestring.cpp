/***********************************************************************
*
*  Attention!!!
*  This file should only be edited with the RCS revision control system
*
*  $RCSfile: timestring.c,v $
*
*  Description:
*  return a string which contains the current date and time
*
*  $Log: timestring.c,v $
*  Revision 1.3  1999/10/25 15:46:37  kaspar
*  3 new functions for date/time according to ISO format
*
*  Revision 1.2  1999/10/11 22:43:36  kaspar
*  some simplifications
*
*  Revision 1.1  1999/10/11 22:41:05  kaspar
*  Initial revision
*
***********************************************************************/       
#include <time.h>
#include <stdio.h>
#include <string.h>

char* getTimeString() {
 
  time_t timeDate;
  struct tm *systemTime;
  
  timeDate = time(NULL);
  /* with the NULL pointer the function */
  /* stores the current calender time   */
  
  systemTime = localtime(&timeDate);
  /* the function stores an encoding of  */
  /* the calender time and returns the   */
  /* address of that structure           */ 
  
  return (asctime(systemTime));
  /* asctime stores a 26-character representation of */ 
  /* the time encoded in systemTime and returns the  */
  /* the address of the string.                      */
} 

void getISOdate(char *dateString, short maxLength) {
  time_t timeDate;
  struct tm *systemTime;
  char year[5];
  char month[3];
  char day[3];
  char string[11];
  //short length;
  size_t length;

  timeDate = time(NULL);
  systemTime = localtime(&timeDate);
  
  length=strftime(year, sizeof(year), "%Y", systemTime);
  length=strftime(month, sizeof(month), "%m", systemTime);
  length=strftime(day, sizeof(day), "%d", systemTime);
  
  sprintf(string, "%s-%s-%s",year,month,day);
  strncpy(dateString, string, maxLength);
}

void getISOtime(char *timeString, short maxLength) {
  time_t timeDate;
  struct tm *systemTime;
  char timeString1[9];
  //short length;
  size_t length;

  timeDate = time(NULL);
  systemTime = localtime(&timeDate);
  
  length=strftime(timeString1, sizeof(timeString1), "%X", systemTime);
  strncpy(timeString, timeString1, maxLength);
}

void getISOdateTime(char *dateTimeString, short maxLength){
  char dateString[11];
  char timeString[9];

  char string[20];
  getISOdate(dateString, sizeof(dateString));
  getISOtime(timeString, sizeof(timeString));

  sprintf(string, "%s %s", dateString, timeString);
  strncpy(dateTimeString, string, maxLength);
}
  
