/***********************************************************************
*
*  Attention!!!
*  This file should only be edited with the RCS revision control system
*
*  $RCSfile: countryinfo.cpp,v $
*
*  Description:
*  - reads 'COUNTRY_NAMES.DAT' and provides information 
*  about max. number of countries, ISO numbers of countries and
*  names of countries.
* 
*  $Log: countryinfo.cpp,v $
*  Revision 1.4  2000/05/21 23:07:31  kaspar
*  - namespace std
*  - 'vector' and 'algorithm' instead of arrays
*    * test has shown that this works faster and does not need more memory
*    * iterators and find from algorithm has been used (fastest possibility)
*    * stack is not needed any more
*  - method to skip whitespace has been changed (during file reading)
*
*  Revision 1.3  1999/12/18 18:25:15  kaspar
*  - all arrays are created dynamically
*  - the stack class is used to read the information
*
*  Revision 1.2  1999/10/13 12:12:49  kaspar
*  - functions check the ranges of the parameters
*  - comment lines in COUNTRY_NAMES.DAT allowed
*  - variable length for country names allowed
*  - some minor improvements
*
*  Revision 1.1  1999/10/11 09:09:04  kaspar
*  Initial revision
*
*
***********************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>   
#include <fstream>  
#include <cctype>
#include <vector>
#include <algorithm>
#include "countryinfo.h"
 
#include <iostream>

void countryInfoClass::init(const char  *input_dir, 
			    const unsigned short maxLengthOfCountryName)
{
  // in the default case only the first 15 characters 
  // of the country names are used.
 
  char filename[250];   
  sprintf(filename,"%s/COUNTRY_NAMES.DAT",input_dir);
  ifstream countryFile(filename);
  if (!countryFile){
	  cerr << "Error while opening file " << filename << "." << endl; 
    exit(1);
  }
  else {     
	char *name;
    char string[250];   
    short length, j , i=0;  
   
    // read commentlines indicated by # 
    while (!countryFile.eof() && countryFile.peek()=='#')  {
      countryFile.getline(string,sizeof(string));
    }
    // set data type to 'decimal'
    // otherwise some compiler assume that numbers with leading 'zeros'
    // are octal 
    cin.setf(ios::dec, ios::basefield);

    while (countryFile >> j >> ws){ 
      // read iso number and following whitespaces
      
      countryIsoNumber.push_back(j);

      // read country name.
      // country names might include space characters 
      // therefore 'getline' is used.  
      countryFile.getline(string,sizeof(string));
      length = strlen(string);
      
      // allocate memory and copy string with the desired length into it
      if (length >= maxLengthOfCountryName) {
	name = new char[maxLengthOfCountryName+1];
	length = maxLengthOfCountryName;
	strncpy(name, string, length);
      }
      else {
	name = new char[length+1];
	strncpy(name, string, length);
	name[length]='\0';
      }
      countryName.push_back(name);     
      i++;
    }
    countryFile.close();  
    
    numberOfCountries = i;
    
    // avoid unused memory
    countryIsoNumber.resize(numberOfCountries);
    countryName.resize(numberOfCountries);
    
    for (int i=0; i<numberOfCountries; i++)
    	cout << i << " "<<countryIsoNumber[i]<<" "<<countryName[i]<<endl;
    
  } 
}

short countryInfoClass::getNumberOfCountries(void) 
{
  return numberOfCountries;
}

short countryInfoClass::getIsoNumber(const short n)
{
  if ((n >= 0) && (n <= numberOfCountries-1)) 
    return countryIsoNumber[n];
  else {
    cerr << "Country number out of range: " 
	 << n << " (0.." << numberOfCountries-1 << ")." << endl;
    exit(-1);
  }
}

short countryInfoClass::getArrayPosition(const short IsoNumber)
{
  std::vector<short>::iterator it 
    = std::find(countryIsoNumber.begin(), countryIsoNumber.end(), IsoNumber);
  if (it != countryIsoNumber.end()) {
    return (it-countryIsoNumber.begin());
    cout << "IsoNumber: "<<IsoNumber<<" : "<<it-countryIsoNumber.begin()<<endl;
  }
  cerr << "No country found with ISO number " << IsoNumber << endl;
  exit(-1);
}

char* countryInfoClass::getCountryName(const short n)
{
  if ((n >= 0) && (n <= numberOfCountries-1)) 
    return countryName[n];
  else {
    cerr << "Country number out of range: " 
	 << n << "(0.." << numberOfCountries-1 << ")." << endl;
    exit(-1);
  }
}










