#ifndef _PTZ_SPEED_TRANSFER_H
#define _PTZ_SPEED_TRANSFER_H

#include <iostream>
#include "globalDate.h"

using namespace std;


class CPTZSpeedTransfer 
{
public:
	CPTZSpeedTransfer();
	~CPTZSpeedTransfer();
	void create();
	static CGlobalDate* _GlobalDate;
	int m_PanSpeedTab[64];
	int m_TilSpeedTab[64];
	bool m_bTabInit;

	int GetPanSpeed(float fIn);
	int GetTiltSpeed(float fIn);
	int SpeedTabInit();
	int PTZSpeed_creat();
	void UpdateParams();

private:
	int SpeedLevelPan[10];
	int SpeedLeveTilt[10];

};
#endif
