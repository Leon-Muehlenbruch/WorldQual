#include "Newton.h"

// classes
countryInfoClass countryInfo;         // provides number of countries, ISO etc.

using namespace std;
int main(int argc, char* argv[]) {

	char filename[maxcharlength];    
	char headerline[maxcharlength];       // variable for line input of header
	//char line[maxcharlength];           // variable for line inputs
	char input_dir[maxcharlength];        // input directory for data
	char output_dir[maxcharlength];       // output directory for data


	short int nc;                         // number of Image 2.2 countries
	short int iso_num;                    // ISO number of country
	short int array_pos, array_pos_comp;  // array position of country from countryInfoClass

	double cCountry[n], c1Country[n];      // coeffitients in the approximate function
	double deltac[n];   // iteration step
	double norm1, norm2, const0, const1, const2;
	double stepmin=10;

	double xCountry[Nmax], yCountry[Nmax], wCountry[Nmax];  // given points (Xi, Yi) and point's weight
	double a[n][n+1];                                       // Jacobi function
	int point, country, i, k, j, iteration;

	// parameter for Householder-Transformation
	double r, alfa, ak, f;
	double d[n];

	N=Nmax;
	//N=4;
	

	// ----------------------------------
	// directory file
	// ----------------------------------
	sprintf(filename,"%s",directory_file); 
	{
//		ifstream inputfile(filename, ios::in | ios::nocreate);
		ifstream inputfile(filename, ios::in);
		if (!inputfile) {
			sprintf(input_dir,".");
			sprintf(output_dir,".");
			cerr << "\nMissing file " << filename << endl;
			cout << "current directory set for INPUT and OUTPUT\n" << endl;
		}
		
		else {
			// read commentlines indicated by # 
			while (inputfile && inputfile.peek()=='#') {
				inputfile.getline(headerline,maxcharlength);
			}
			
			// read input directory
			inputfile >> input_dir;
			inputfile.ignore(250, '\n');
			cout << "Setting input dir to " <<  input_dir << "." << endl;
			
			//read output directory
			inputfile >> output_dir;
			cout << "Setting output dir to " <<  output_dir << "." <<  endl;
		}
		inputfile.close();
	}	// close block
	
	// initialization country info and setting number of countries
	countryInfo.init(input_dir, 40);
	cout << "countryInfo.init done\n";
	nc = countryInfo.getNumberOfCountries();
	cout << "nc=" << nc<<endl;
	//====================================

	int* pointNumber = new int [nc];              // number of points for country
	double* IndexOfAgreement = new double [nc];   // index of agreenment

	double **c;        // coeffitients in the approximate function
	c  = new double*[n];
	for (i=0; i<n; i++) {
		c[i] = new double[nc];
		if (!c[i])  cerr << "ERROR: not enough memory\n" << endl;
	}

	double **c1;        // coeffitients in the approximate function
	c1 = new double*[n];
	for (i=0; i<n; i++) {
		c1[i] = new double[nc];
		if (!c1[i]) cerr << "ERROR: not enough memory\n" << endl;
	}

	double **x;	double **y;	double **w;  // given points (Xi, Yi) and point's weight
	x = new double* [Nmax];
	y = new double* [Nmax];
	w = new double* [Nmax];
	for (point=0; point<Nmax; point++) {
		x[point] = new double[nc];
		if (!x[point])  cerr << "ERROR: not enough memory\n" << endl;

		y[point] = new double[nc];
		if (!y[point])  cerr << "ERROR: not enough memory\n" << endl;

		w[point] = new double[nc];
		if (!w[point])  cerr << "ERROR: not enough memory\n" << endl;

	}

	//#############################################################
	//                INITIALIZATION OF ARRAYS                    #
	//#############################################################
	for (country=0; country<nc; country++) {
		pointNumber[country]      = 0;
		IndexOfAgreement[country] = 0;
	}

	for (i=0; i<n; i++) {
		for (country=0; country<nc; country++) {
			c[i][country]  = 0;
			c1[i][country] = 0;
		}
	}

		
	for (point=0; point<Nmax; point++) {
		for (country=0; country<nc; country++) {
			x[point][country] = 0;
			y[point][country] = 0;
			w[point][country] = 0;
		}
	}
	for (point=0; point<pointNumber[country]; point++) {
		xCountry[point] = 0;
		yCountry[point] = 0;
		wCountry[point] = 0;
	}
	
	//#######################################################
	//                         INPUT 
	//#######################################################
	
	//=======================================================
	//              gegebene Wertepaare                     =
	//=======================================================
	array_pos_comp=-1;
	sprintf(filename,"%s/%s", input_dir, points_file); 
	{
		cout <<"lesen gegebene Wertepaare: "<< filename<<endl;
//		ifstream inputfile(filename,ios::in | ios::nocreate);
		ifstream inputfile(filename,ios::in);
		if (!inputfile) {
			cerr << "\nMissing file " << filename << endl;
			return 1;
		}
		else {
			// read commentlines indicated by # 
			while (inputfile && inputfile.peek()=='#') {
				inputfile.getline(headerline,maxcharlength);
			}
			
			// read points
			point=0;
			inputfile >> iso_num;
			do {				
				
				array_pos = countryInfo.getArrayPosition(iso_num);
				if (array_pos!=array_pos_comp) {
					if (array_pos_comp>=0) pointNumber[array_pos_comp]=point;
					array_pos_comp=array_pos;

					point=0;
				}

				inputfile.ignore(250, '\t'); // read tab stop after iso num
				inputfile.ignore(250, '\t'); // do not read country names
				inputfile >> x[point][array_pos]
					      >> y[point][array_pos]
						  >> w[point][array_pos];
				inputfile.ignore(250, '\n');
				point++;
				inputfile >> iso_num; 
			}  while (inputfile);	  
		} // else (!inputfile)
		
		pointNumber[array_pos]=point-1;
		inputfile.close();
	} // close block

	//=======================================================
	//                 initial value                        =
	//=======================================================

	sprintf(filename, "%s/%s", input_dir, init_file);
	{
		cout <<"read initial value : "<< filename<<endl;
//		ifstream inputfile(filename, ios::in | ios::nocreate);
		ifstream inputfile(filename, ios::in);
		if (!inputfile) {
			cerr << "\nMissing file " << filename << endl;
			return 1;
		}
		else {
			// read commentlines indicated by #
			while (inputfile && inputfile.peek()=='#') {
				inputfile.getline(headerline, maxcharlength);
			}

			// read initial values
			i=0;
			inputfile >> iso_num;
			do {				
				array_pos = countryInfo.getArrayPosition(iso_num);
				inputfile.ignore(250, '\t'); // read tab stop after iso:num
				inputfile.ignore(250, '\t'); // do not read country names
				inputfile >> c[0][array_pos] >> c[1][array_pos] >> c[2][array_pos];
				c1[0][array_pos] = c[0][array_pos];  // c0 wird nicht geändert
				inputfile.ignore(250, '\n');
				i++;
				inputfile >> iso_num;
			} while (inputfile);
		} // else

		inputfile.close();
	} // close block
	

	//=======================================================
	//                        maxSWI                        =
	//=======================================================
	sprintf(filename, "%s/%s", input_dir, maxSWI_file);
	{
		cout <<"read maxSWI : "<< filename<<endl;
//		ifstream inputfile(filename, ios::in | ios::nocreate);
		ifstream inputfile(filename, ios::in);
		if (!inputfile) {
			cerr << "\nMissing file " << filename << endl;
			return 1;
		}
		else {
			// read commentlines indicated by #
			while (inputfile && inputfile.peek()=='#') {
				inputfile.getline(headerline, maxcharlength);
			}

			// read maxSWI
			inputfile >> iso_num;
			do {
				
				array_pos = countryInfo.getArrayPosition(iso_num);
				inputfile.ignore(250, '\t'); // read tab stop after iso_num
				inputfile.ignore(250, '\t'); // do not read country names

				point = pointNumber[array_pos];
				inputfile >> x[point][array_pos] >> y[point][array_pos];   // read last point with max swi
				
				pointNumber[array_pos]++;
				w[point][array_pos]=1.0;                                   // weight last point
				inputfile.ignore(250, '\n');
				inputfile >> iso_num;
			} while (inputfile);
		} // else

		inputfile.close();
	} // close block

	
	//####################################################################
	//                    BEGIN OF CALCULATIONS                          #
	//####################################################################
	for (country=0; country<nc; country++) {

		if( pointNumber[country] < n ) { // not enough points
			//cout << country << " - " << countryInfo.getIsoNumber(country) 
			//	 << " " << pointNumber[country] << " not enough points" << endl;
			continue; 
		}
		//cout << country << " " << countryInfo.getIsoNumber(country) << " " << pointNumber[country] << endl;


/*	// Anfangswert

	//USA, Canada
	c[0] = -510; c[1]=-0.00000001; c[2]=1;
	c[0] = -100; c[1]=-1e-8;     c[2]=1;
	const0=c[0]; const1= c[1]; const2=c[2];

	//c[0] = -250; c[1]=-0.00000001; c[2]=1;
*/	
	const0=c[0][country]; const1= c[1][country]; const2=c[2][country];
	
	iteration=0;   // counter of iterations



iter:
	
	for (i=0; i<n; i++) {
		cCountry[i]=c[i][country];
		cout << "c["<<i<<"]= "<<cCountry[i]<<endl;
	}
//int xxx;
//cin >> xxx;
	for (point=0; point<pointNumber[country]; point++) {
		xCountry[point] = x[point][country];
		yCountry[point] = y[point][country];
		wCountry[point] = w[point][country];
	}

	// Jacobi Matrix
	
	a[0][0] = g1c1(cCountry, xCountry, yCountry, wCountry, pointNumber[country]);           
	a[0][1] = g1c2(cCountry, xCountry, yCountry, wCountry, pointNumber[country]); 
	a[0][2] =  -g1(cCountry, xCountry, yCountry, wCountry, pointNumber[country]);

	a[1][0] = g2c0(cCountry, xCountry, yCountry, wCountry, pointNumber[country]); 
	a[1][1] = g2c1(cCountry, xCountry, yCountry, wCountry, pointNumber[country]); 
	a[1][2] =  -g2(cCountry, xCountry, yCountry, wCountry, pointNumber[country]);


    int nn=n-1; // Anzahl Konstanten zu ändern diesmal nicht 3, sondern 2 (c0 bleibt wie es war - Höhe dr Kurve)
	// Lösung des linearen Gleichnugssystems mit der Jacobi Matrix zur Berechnung von deltac
	for (i=0; i<nn; i++) {
		// 1
		r = 0.0;

		for (k=i; k<nn; k++)  r+=a[k][i]*a[k][i];
		
		alfa    = sqrt(r)*sign(a[i][i]);
		ak=1/(r + alfa * a[i][i]);
		a[i][i] += alfa;
		
		// 2
		d[i]=-alfa;

		for (k=i+1; k<nn+1; k++) {
			f=0.0;
			for (j=i; j<nn; j++)  f+=a[j][k]*a[j][i];
			f*=ak;
			
			for (j=i; j<nn; j++)  a[j][k]-=f*a[j][i];
		} // k

	} // i
	
	for (i=nn-1; i>=0; i--) {
		deltac[i]=a[i][nn];
		for (k=i+1; k<nn; k++)  deltac[i]-=a[i][k]*deltac[k];
		
		deltac[i]=deltac[i]/d[i];
	} // i

	// lineare Gleichnugssystem gelöst

	// Dämpfung Newton-Verfahren
	j=0;
	i=0;
	do {
		for (j=1; j<n; j++) {
			c1[j][country] = c[j][country]+deltac[j-1]/pow(2, i);
			c1Country[j]   = c1[j][country];
		}

		norm1 = pow( pow(g1(c1Country, xCountry, yCountry, wCountry, pointNumber[country]), 2) 
			       + pow(g2(c1Country, xCountry, yCountry, wCountry, pointNumber[country]), 2), 0.5);
		
		norm2 = pow( pow(g1(cCountry,  xCountry, yCountry, wCountry, pointNumber[country]), 2) 
			       + pow(g2(cCountry,  xCountry, yCountry, wCountry, pointNumber[country]), 2), 0.5);

		if ( norm1 < norm2 ) break;
		i++;

	} while (i<=imax && norm1>=norm2);

	if ( norm1 >= norm2 ) {
		for (j=1; j<n; j++) {
			c1[j][country] = c[j][country]+deltac[j-1];
			c1Country[j]   = c1[j][country];
		}
	}

	norm1=norm2=0.0;
	for (i=1; i<n; i++) {
		norm1+=pow(c[i][country]-c1[i][country], 2);
		norm2+=pow(c1[i][country], 2);
	} // i
	norm1=pow(norm1, 0.5);
	norm2=pow(norm2, 0.5);
	if ((norm1)<=0.001 ) {
		//cout << "\niter = " << iteration <<"  === "<<  norm1;// cin >> i;
	} else 	if (iteration<MaxIteration) {
		/*
		cout << "\niteration: " << iteration 
			 << "  c[0]= " << c[0][country] 
			 << "  c[1]= " << c[1][country] 
			 << "  c[2]= " << c[2][country] ;
		*/
		//if (c[0]>-50) cin >> i;
		for (i=1; i<n; i++)	c[i][country]=c1[i][country];
		iteration++;
		/*
		if (c[0]<-450) { 
			c[0]=const0+50; c[1]=const1; c[2]=const2;
			const0+=50;
			iteration=0; 
			cout << "\n-------------\n" 
			 << "  c[0]= " << c[0] 
			 << "  c[1]= " << c[1] 
			 << "  c[2]= " << c[2] ;
			cin >> i; 
		}
		if ( (c[0]>-50) && (stepmin>1)){
			c[0]=const0-stepmin; c[1]=const1; c[2]=const2;
			const0-=stepmin;
			stepmin-=1;
			iteration=0; 
			cout << "\n========\n" 
			 << "  c[0]= " << c[0] 
			 << "  c[1]= " << c[1] 
			 << "  c[2]= " << c[2] ;
			cin >> i; 
		}
		*/
		goto iter;
	}

	/*
	cout << "\n\nresult:\n\n";
	for (i=0; i<n; i++)
		cout << " c " << i << " = " << c1[i][country] << "    ";
	cout << endl;
	*/
	cout << "iteration: " << iteration<<" norm1: " <<norm1 << " norm2: "<<norm1<<endl;


	//index of agreement
	double averageY, PE, RMSE;

	averageY=0.0;
	for (i=0; i<pointNumber[country]-1; i++) averageY+=y[i][country];
	averageY/=(pointNumber[country]-1);

	PE=0.0; RMSE=0.0;
	for (i=0; i<pointNumber[country]-1; i++) {
		//double ddd=fabs(y[i][country]-averageY);
		//double sss=fabs(F(c[0][country], c[1][country], c[2][country], x[i][country])-averageY);
		//double aaa=fabs(y[i][country]-averageY)+fabs(F(c[0][country], c[1][country], c[2][country], x[i][country])-averageY);
		//double xxx=pow( fabs(y[i][country]-averageY)+fabs(F(c[0][country], c[1][country], c[2][country], x[i][country])-averageY), 2 );
		PE   += pow( fabs(y[i][country]-averageY)+fabs(F(c[0][country], c[1][country], c[2][country], x[i][country])-averageY), 2 );
		RMSE += pow( y[i][country]-F(c[0][country], c[1][country], c[2][country], x[i][country]), 2);
	} // i
	
	IndexOfAgreement[country]=1-RMSE/PE;

/*
	averageY=0.0;
	for (i=1; i<pointNumber[country]-1; i++) averageY+=y[i][country];
	averageY/=(pointNumber[country]-2);

	PE=0.0; RMSE=0.0;
	for (i=1; i<pointNumber[country]-1; i++) {
		double ddd=fabs(y[i][country]-averageY);
		double sss=fabs(F(c[0][country], c[1][country], c[2][country], x[i][country])-averageY);
		double aaa=fabs(y[i][country]-averageY)+fabs(F(c[0][country], c[1][country], c[2][country], x[i][country])-averageY);
		double xxx=pow( fabs(y[i][country]-averageY)+fabs(F(c[0][country], c[1][country], c[2][country], x[i][country])-averageY), 2 );
		PE   += pow( fabs(y[i][country]-averageY)+fabs(F(c[0][country], c[1][country], c[2][country], x[i][country])-averageY), 2 );
		RMSE += pow( y[i][country]-F(c[0][country], c[1][country], c[2][country], x[i][country]), 2);
	} // i
	
	IndexOfAgreement[country]=1-RMSE/PE;
*/

//	cout << endl << "Index of agreement = " << IndexOfAgreement[country] << endl; 
//	cout << endl << "RMSE " << RMSE << endl;
//	cout << endl << "PE   " << PE << endl;
//	cout << endl << "norm1 " << norm1 << endl;

	} // for(coutry)

	// ###################################################################
	//                      BEGIN OF DATA OUTPUT                         #
	// ###################################################################

	sprintf(filename,"%s/%s", output_dir, output_file); 
	{
		ofstream outputfile (filename);
		if (!outputfile) {
			cerr << "\nCould not open " << filename << " for output" << endl;
			return (1);
		}
		else {
			// write file description
			outputfile << "iso_num\tCountry\tc0\tc1\tc2\tIndex of agreement";
			outputfile << endl;
			
			for (country=0; country<nc; country++) {
				if(pointNumber[country] < n) continue;

				outputfile << countryInfo.getIsoNumber(country) << "\t" 
					       << countryInfo.getCountryName(country) << "\t";
				for (i=0; i<n; i++)
					outputfile << c1[i][country] << "\t";
				outputfile << IndexOfAgreement[country] << endl;
			}
			cout << filename << " successfully written" << endl;
			
		} // else outputfile
		
		outputfile.close();
	} // close block


	delete[] pointNumber;
	delete[] IndexOfAgreement;
	delete[] c; delete[] c1; delete[] x; delete[] y; delete[] w;

	return 0;
} // main


