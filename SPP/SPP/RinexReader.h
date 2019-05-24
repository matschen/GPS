
#ifndef RINEXREADER_H
#define RINEXREADER_H
//#define _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_WARNINGS
//#define _SCL_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstddef>
#include <cstring>
#include <array>
#include <map>
#include <utility>
#include <iomanip>      // setprecision
#include <math.h>   
using namespace std;
#define MAX_STRLEN 100
class RinexReader
{
public:
	RinexReader();
	~RinexReader();
	FILE *ObsFilePointer;
	FILE *NavFilePointer;
	FILE *EnvFilePointer;
	string obsfileName;
	string navfileName;

	struct HEADER           //文件头
	{
		string rinexVersion;
		string markerName; //天线标志的名称
		string markerNumber;//天线标志编号
		int nobsType;   //n Observation types
		vector <string> obsType;
		double interval;
		vector <double> antPos;
	};
	HEADER ObsHeader; 

	struct RECORDS {                      //观测值
		vector<vector<double>> record;
		double time;//
		double YEAR;
		double MONTH;
		double DAY;
		double HOUR;
		double MIN;
		double totalSec;// total time in seconds
		double SEC;
		vector<int>  prn;
		int SvNum;
		int EpochFlag;
	};
	RECORDS ObsRecords;
	vector <RECORDS> AllObs;  //vector to store all obeservation data
	//Navigation data
	struct NAVRECORDS {
		double IODE, Crs, DeltaN, Mo, Cuc, Eccentricity, Cus, Sqrta, Toe, Cic,
			OMEGA, Cis, Io, Crc, omega, OMEGADOT, IDOT, L2CodesChannel, GPSWeek, L2PDataFlag, SVAccuracy, SVHealth, TGD, IODC,
			TransmissionTime, FitInterval, SVClockBias, SVClockDrift, SVClockDriftRate, PRN, SEC, gpstime;
	};
	vector<NAVRECORDS> NavRecords;

	void ReadEnvFile(RinexReader &object);
	void ObsReader(HEADER &head, RECORDS &ObsRecords); 
	void NavReader();
private:

};



#endif