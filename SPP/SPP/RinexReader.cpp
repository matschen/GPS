#include "stdafx.h"
#include "RinexReader.h"

RinexReader::RinexReader()
{
}

RinexReader::~RinexReader()
{
}
void RinexReader::ReadEnvFile(RinexReader& object) {    //引入导航TXT文件，读取要输入的观测文件和导航文件
	                                 //
	 fopen_s(&EnvFilePointer,"EnvFile.txt", "rt");
	 if (EnvFilePointer==NULL) perror("Error opening file");
	 char line[MAX_STRLEN];

	 fgets(line,MAX_STRLEN,EnvFilePointer);
	 char *p;  //使用strtok分割字符串不安全，引入**p保存分割后剩下的字符串
	 obsfileName = strtok_s(line, " ",&p);  //字符串分割，得到O文件名字
	 navfileName = strtok_s(NULL, " ",&p);  //得到N文件名字
	 fclose(EnvFilePointer);
	 fopen_s(&ObsFilePointer, obsfileName.c_str(), "rt"); //open observation file
	 if (ObsFilePointer == NULL) perror("Error opening file");
	 fopen_s(&NavFilePointer, navfileName.c_str(), "rt");
	 if (NavFilePointer == NULL) perror("Error opening file");
}
void RinexReader::ObsReader(HEADER &head, RECORDS &ObsRecords)
{
	
	
	char line[MAX_STRLEN];
	fgets(line, MAX_STRLEN, ObsFilePointer);
	//read header
	while (strstr(line,"END OF HEADER") == NULL)
	{
		if (strstr(line,"RINEX VERSION"))
		{
			string sline(line);  // convert the line (char) to string class type sLine
			string token = sline.substr(5, 4);   //select version
			ObsHeader.rinexVersion = token;
		}
		
		if (strstr(line,"MARKER NAME")) 
		{
			string sline(line);  // convert the line (char) to string class type sLine
			string token = sline.substr(0, 60);// select marker name
			ObsHeader.markerName = token;
		}

		if (strstr(line, "MARKER NUMBER"))
		{
			string sline(line);  // convert the line (char) to string class type sLine
			string token = sline.substr(0, 20);
			ObsHeader.markerNumber = token;
		}

		if (strstr(line, "# / TYPES OF OBSERV"))
		{
			string sline(line);  // convert the line (char) to string class type sLine
			string token = sline.substr(0, 6);
			ObsHeader.nobsType = stoi(token);

			//如果观测值小于等于9
			if (ObsHeader.nobsType <= 9)  
			{
				string sline(line);  // convert the line (char) to string class type sLine
				string typetoken = sline.substr(10, 50);
				char cToken[MAX_STRLEN];
				strcpy_s(cToken, typetoken.c_str());
				char *leftStr;
				char *cuttedStr = strtok_s(cToken, " ", &leftStr);
				while (cuttedStr)
				{
					ObsHeader.obsType.push_back(cuttedStr);
					cuttedStr = strtok_s(NULL, " ", &leftStr);
				}

			}
			else
			{
				//如果观测值超过9种
			}
		}
		
		if (strstr(line, "INTERVAL"))
		{
			string sline(line);  // convert the line (char) to string class type sLine
			string token = sline.substr(0, 10);
			ObsHeader.interval = stoi(token);
		}

		if (strstr(line, "APPROX POSITION XYZ")) //WGS-84
		{
			string sline(line);
			string tokenX = sline.substr(0, 14);
			string tokenY = sline.substr(14, 14);
			string tokenZ = sline.substr(28, 14);

			ObsHeader.antPos.push_back(stof(tokenX));
			ObsHeader.antPos.push_back(stof(tokenY));
			ObsHeader.antPos.push_back(stof(tokenZ));
		}
		fgets(line, MAX_STRLEN, ObsFilePointer);
	}
	
	fgets(line, MAX_STRLEN, ObsFilePointer);
	//read data epoch by epoch
	//epoch,satellite
	int epoch = 0;
	while (!feof(ObsFilePointer))
	{
		if (strstr(line, "G"))   //read date satellite numbers
		{
			epoch++;
			vector <int> temp_prn; //the same reason as temp_record
			string sline(line);
			string year = sline.substr(1, 2);
			string month = sline.substr(4, 2);
			string day = sline.substr(7, 2);
			string hour = sline.substr(10, 2);
			string minute = sline.substr(13, 2);
			string second = sline.substr(16, 11);
			string epochFlag = sline.substr(28, 1);
			string svNum = sline.substr(29, 3);

			ObsRecords.YEAR = stod(year);
			ObsRecords.MONTH = stod(month);
			ObsRecords.DAY = stod(day);
			ObsRecords.HOUR = stod(hour);
			ObsRecords.MIN = stod(minute);
			ObsRecords.SEC = stod(second);
			ObsRecords.EpochFlag = stoi(epochFlag);
			ObsRecords.SvNum = stoi(svNum);
			ObsRecords.EpochFlag = epoch;

			string prnList = sline.substr(32, 36);
			if (ObsRecords.SvNum > 12)
			{
				fgets(line, MAX_STRLEN, ObsFilePointer);
				string sline(line);
				prnList += sline.substr(32, 3);
		    }
			char cPrnList[MAX_STRLEN];   //read SvNum,use strtok_s to cut the string
			strcpy_s(cPrnList, prnList.c_str());
			char *p;
			char *cuttedStr = strtok_s(cPrnList, "G", &p);
			while (cuttedStr)
			{
				temp_prn.push_back(atoi(cuttedStr));
				cuttedStr = strtok_s(NULL, "G", &p);
			}
			ObsRecords.prn = temp_prn;
		}
		vector <vector<double>> temp_record; //this temp aims to record, then copy to record in structure, afterthat be emptied.	
										  //cannot directly use pushback to give data to variant in class, otherwise the data will added up epoch by epoch
		//observation data 
		fgets(line, MAX_STRLEN, ObsFilePointer);
		

		
		for (int i = 0; i < ObsRecords.SvNum; i++)
		{
			if (head.nobsType <= 5) //if observation types less than 6
			{
				vector <double> oneObs; //observation data from one satellite, including different kinds and frequency(C1,L1,L2)
										//char cOneObs[MAX_STRLEN];

				string sline(line);
				for (int j = 0; j < head.nobsType; j++)
				{
					string obs = sline.substr(16 * j, 14);
					string blank = "              ";
					if (!(obs == blank))
					{
						oneObs.push_back(stod(obs));
					}
					else
					{
						oneObs.push_back(0);
					}
				}
				temp_record.push_back(oneObs);
				fgets(line, MAX_STRLEN, ObsFilePointer);

			}
			if (head.nobsType > 5)
			{
				vector <double> oneObs; //observation data from one satellite, including different kinds and frequency(C1,L1,L2)
										//char cOneObs[MAX_STRLEN];

				string sline(line);
				string blank = "              ";
				string obs;
				for (int j = 0; j < 5; j++)
				{
					sline = sline + blank + blank; //有时某个观测值可能为空，观测数大于5并且在第二行的末尾一个观测值为空时
					obs = sline.substr(16 * j, 14);     //,line转为string类的sline时，由于\n的存在，sline就会少一个观测值的位数。
					if (!(obs == blank))                //加上一个位数的空格，可以保证用stod函数时obs不为空。
					{
						oneObs.push_back(stod(obs));
					}
					else
					{
						oneObs.push_back(0);
					}
				}
				fgets(line, MAX_STRLEN, ObsFilePointer);
				
				for (int j = 0; j < head.nobsType-5; j++)
				{
					string sline(line);	
					int tPos = sline.find('\n');
					sline.erase(sline.begin()+tPos);
					sline = sline + blank + blank + blank; 
					obs = sline.substr(16 * j, 14);
					if (!(obs == blank))
					{
						oneObs.push_back(stod(obs));
					}
					else
					{
						oneObs.push_back(0);
					}
				}

				temp_record.push_back(oneObs);
				fgets(line, MAX_STRLEN, ObsFilePointer);
			}
		}
		
		ObsRecords.record = temp_record;
		AllObs.push_back(ObsRecords);
		
	}	
}
void RinexReader::NavReader()
{
}
;