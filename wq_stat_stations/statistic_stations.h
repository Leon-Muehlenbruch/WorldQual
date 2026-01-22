class statisticStationsClass {
	public:
	int cell;
	int date;
	double con;
	double con2;
	int station_id;
	double measured;
	int flag_uncert;
	double measured_conductivity;
	int flag_uncert_conductivity;
	void setValues() {
		cell = -9999;
		date = -9999;
		con = -9999;
		con2 = -9999;
		station_id = -9999;
		measured = -9999;
		flag_uncert = -9999;
		measured_conductivity = -9999;
		flag_uncert_conductivity = -9999;
	}
	void copyValues(statisticStationsClass line) {
		cell = line.cell;
		date = line.date;
		con = line.con;
		con2 = line.con2;
		station_id = line.station_id;
		measured = line.measured;
		flag_uncert = line.flag_uncert;
		measured_conductivity = line.measured_conductivity;
		flag_uncert_conductivity = line.flag_uncert_conductivity;
	}
};
