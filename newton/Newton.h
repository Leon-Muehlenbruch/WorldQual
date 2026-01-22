//#include <stdlib.h>
#include <stdio.h>
//#include <string.h>
#include <math.h>
//#include <iostream.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "countryinfo.h"



const short maxcharlength    = 255;   // maximum length of strings

const char directory_file[]           = "DATA.DIR";
const char points_file[]              = "POINTS.DAT";
const char init_file[]                = "INIT.DAT";
const char maxSWI_file[]              = "MaxSWI.DAT";
const char output_file[]              = "COEFF.OUT";
const char IndexAgreenment_file[]     = "IndexOfAgreenment.OUT";

const int n            = 3;    // number of coeffitients in the approximate function  0(1)n-1
const int Nmax         = 199999;  // max number of given points (Xi, Yi) i=0(1)N-1
const int imax         = 4;
const int MaxIteration = 1000;

double N; // number of given points (Xi, Yi) i=0(1)N-1

double g0c0(double c[n], double x[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r += 2*w[i]/z/z;
		//r += 2*w[i]*exp(2*c[1]*x[i]*x[i]); // dom
	}
	return r;
} // g0c0

double g1c0(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r += 2.0*x[i]* w[i] * (z-1.0) * (y[i]-2*c[0]/z)/z/z;
		//r += 2* w[i] * x[i]*x[i]*exp(c[1]*x[i]*x[i])*(y[i]-2*c[0]*exp(c[1]*x[i]*x[i])-c[2]); // dom
	}
	return r;
} // g1c0

double g2c0(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r += 2.0* w[i] * (z-1.0) * (y[i]-2.0*c[0]/z)/z/z;
		//r += 2* w[i] * exp(c[1]*x[i]*x[i]);  // dom
	}
	return r;
} // g2c0

double g0c1(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r += 2.0* w[i] *x[i]* (z-1.0) * (y[i]-2*c[0]/z)/z/z;
		//r += -2* w[i] *x[i]*x[i]*exp(c[1]*x[i]*x[i])*( y[i]-2*c[0]*exp(c[1]*x[i]*x[i])-c[2] ); // dom
	}
	return r;
} // g0c1

double g1c1(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r +=  2.0*x[i]*x[i]*w[i]*c[0]*(z-1.0)*(-z*z*y[i] + 2.0*z*(c[0]+y[i])-3.0*c[0])/pow(z,4.0);
		//r += 2* w[i] *c[0]*pow(x[i],4)*exp(c[1]*x[i]*x[i])*(y[i]-2*c[0]*exp(c[1]*x[0]*x[0])-c[2]); // dom
	}
	return r;
} // g1c1

double g2c1(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r += 2.0*x[i]*w[i]*c[0]*(z-1.0)*(-z*z*y[i] + 2.0*z*(c[0]+y[i])-3.0*c[0])/pow(z,4.0);
		//r += 2* w[i] *c[0]*x[i]*x[i]*exp(c[1]*x[i]*x[i]); // dom
	}
	return r;
} // g2c1

double g0c2(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r += 2* w[i] *(z-1.0)*(y[i]-2.0*c[0]/z)/z/z;
		//r += 2* w[i] *exp(c[1]*x[i]*x[i]); // dom
	}
	return r;
} // g0c2

double g1c2(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r +=  2.0*x[i]*w[i]*c[0]*(z-1.0)*(-z*z*y[i] + 2.0*z*(c[0]+y[i])-3.0*c[0])/pow(z,4.0);
		//r += -2* w[i] *c[0]*x[i]*x[i]*exp(c[1]*x[i]*x[i]);  // dom
	}
	return r;
} // g1c2

double g2c2(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r +=  2.0*w[i]*c[0]*(z-1.0)*(-z*z*y[i] + 2.0*z*(c[0]+y[i])-3.0*c[0])/pow(z,4.0);
		//r +=  2*w[i];  // dom
	}
	
	return r;
} // g2c2

double g0(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r += -2.0* w[i] /z * ( y[i]-c[0]/z ) ; //waterTemp
		//r += -2* w[i] *exp(c[1]*x[i]*x[i]) * ( y[i]-c[0]*exp(c[1]*x[i]*x[i])-c[2] ) ;      // dom
	}
	return r;
} // g0

double g1(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r += 2* x[i] * w[i] *c[0] * (y[i]-c[0]/z) * (z-1.0) / z/z ;
		//r += 2* w[i] *c[0]*x[i]*x[i]*exp(c[1]*x[i]*x[i]) * (y[i]-c[0]*exp(c[1]*x[i]*x[i])-c[2]) ; // dom
	}
	return r;
} // g1

double g2(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber) {
	int i;
	double r=0.0, z;
	for (i=0; i<pointNumber; i++) {
		z=(1.0+exp(c[1]*x[i]+c[2]));
		r += 2.0* w[i] *c[0] * (y[i]-c[0]/z) * (z-1.0) / z/z ;
		//r += -2 * w[i] * (y[i]-c[0]*exp(c[1]*x[i]*x[i])-c[2]);  // dom
	}
	return r;
} // g2

int sign(double a) {
	if (a>0) return 1;
	if (a<0) return -1;
	return 0;
}// sign()

double F(double c0, double c1, double c2, double x) {
	double f;
	//f = c0*exp(c1*x*x)+c2;   // domestic Kurve
	f = c0/(1.0+exp(c1*x+c2)); // wassertemperatur Kurve
	return f;
} // F()

