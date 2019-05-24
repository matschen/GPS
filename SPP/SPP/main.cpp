#include "stdafx.h"
#include "RinexReader.h"
#include "SvPosition.h"
#include "SPP.h"


int main(int argc, const char * argv[])
{
	int start_s = clock();

	RinexReader rinex;
	rinex.ReadEnvFile(rinex);
	rinex.ObsReader(rinex.ObsHeader, rinex.ObsRecords);

	
	
	int stop_s = clock();
	cout << "time: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) << endl;
	cout << "Complete!!!" << endl;
	return 0;
}
