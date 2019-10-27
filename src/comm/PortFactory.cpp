#include "PortFactory.hpp"
#include "CUartProc.hpp"
#include "CNetProc.hpp"
#include <iostream>
#include "configtable.h"
#include <stdio.h>
#include <opencv2/core/core.hpp>

using namespace cv;

PortFactory::PortFactory()
{

}
  
PortFactory::~PortFactory()
{
 
}


int  ReadUartParams(uartparams_t &param)
{	
	int value_int;
	float value_float;
	string str;
	string cfgAvtFile;
	 int profileNum = CFGID_BKID_MAX*16;
	int configId_Max = profileNum;
	char  cfg_avt[30] = "cfg_avt_";
	cfgAvtFile = "Profile.yml";
	FILE *fp = fopen(cfgAvtFile.c_str(), "rt");
	if(fp != NULL){
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		if(len < 10)
			return  -1;
		else
		{
			FileStorage fr(cfgAvtFile, FileStorage::READ);
			if(fr.isOpened())
			{
				sprintf(cfg_avt, "cfg_avt_%03d_%02d", CFGID_blkId(CFGID_PTZ_baud), CFGID_feildId(CFGID_PTZ_baud));
				param.baud_rate= (int)fr[cfg_avt];
				
				sprintf(cfg_avt, "cfg_avt_%03d_%02d", CFGID_blkId(CFGID_PTZ_datbit), CFGID_feildId(CFGID_PTZ_datbit));
				param.data_bits= (int)fr[cfg_avt];
				
				sprintf(cfg_avt, "cfg_avt_%03d_%02d", CFGID_blkId(CFGID_PTZ_checkbit), CFGID_feildId(CFGID_PTZ_checkbit));
				value_int = (int)fr[cfg_avt];
				if(0 == value_int)
					param.parity= 'N';
				else if(1 == value_int)
					param.parity= 'O';
				else if(2 == value_int)
					param.parity= 'E';
				else if(4 == value_int)
					param.parity= 'S';
				
				sprintf(cfg_avt, "cfg_avt_%03d_%02d", CFGID_blkId(CFGID_PTZ_stopbit), CFGID_feildId(CFGID_PTZ_stopbit));
				float value = (float)fr[cfg_avt];
				param.stop_bits = value;

				sprintf(cfg_avt, "cfg_avt_%03d_%02d", CFGID_blkId(CFGID_PTZ_flowcon), CFGID_feildId(CFGID_PTZ_flowcon));
				value_int = (int)fr[cfg_avt];
				if(1 == value_int)
					param.flow= 1;
				else if(2 == value_int)
					param.flow= 2;
				else if(3 == value_int)
					param.flow= 0;

				printf("[ReadUartParams] baudrate,flow,data_bits,parity,stopbit(%d,%d,%d,%c,%d)\n", param.baud_rate,param.flow,param.data_bits,param.parity,param.stop_bits);
			}
			else
			{
				printf("[ReadUartParams]open YML failed\n");
				return -1;
			}
		}
	}
	else
	{
		printf("[ReadUartParams] Can not find YML. Please put this file into the folder of execute file\n");
		return -1;
	}

	return 0;
}


CPortInterface* PortFactory::createProduct(int type)
{
	CPortInterface* temp = NULL;
	uartparams_t param = {9600, 0, 8, 'N', 1};
    	switch(type)
    	{
		case 1:
			temp = new CUartProc("/dev/ttyTHS1", 115200, 0, 8, 'N', 1);
	            	break;
	        case 2:
			temp = new CNetProc(10000);
		   	break;
	        case 3:
			//ReadUartParams(param);
			//temp = new CUartProc("/dev/ttyTHS2", param.baud_rate, param.flow, param.data_bits, param.parity, param.stop_bits);
			temp = new CUartProc("/dev/ttyTHS2", 115200, 0, 8, 'N', 1);
			break;
	        default:
	            	break;
	}
    	return temp;
}

