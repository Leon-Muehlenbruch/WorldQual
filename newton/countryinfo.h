/***********************************************************************
*
*  Attention!!!
*  This file should only be edited with the RCS revision control system
*
*  $RCSfile: countryinfo.h,v $
*
*  Description:
*  - countryinfo.cpp reads 'COUNTRY_NAMES.DAT' and provides information
*  about max. number of countries, ISO numbers of countries and
*  names of countries.
*
*  $Log: countryinfo.h,v $
*  Revision 1.3  2000/05/21 23:03:32  kaspar
*  - namespace std, stl_vector instead of arrays
*
*  Revision 1.2  1999/12/18 18:23:12  kaspar
*  all the arrays are created dynamically
*
*  Revision 1.1  1999/12/18 18:22:04  kaspar
*  Initial revision
*
***********************************************************************/     
#if !defined (_countryinfo_h_)
#define _countryinfo_h_

#include <vector>

using namespace std;

class countryInfoClass {
 public:
  void  init(const char *input_dir, 
	     const unsigned short maxLengthOfCountryName=15);
  short getIsoNumber(const short n);
  short getArrayPosition(const short IsoNumber);
  short getNumberOfCountries(void);
  char* getCountryName(const short n);

 private:  
  std::vector<short> countryIsoNumber;
  std::vector<char*> countryName;
  short numberOfCountries;
};
#endif

