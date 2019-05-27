
#ifndef PLATFORMINTERFACE_H_
#define PLATFORMINTERFACE_H_

#include <iostream>
#include "platformparam.h"
#include "sensorComp.h"


class CPlatformInterface{

public:
	virtual ~CPlatformInterface(){};

	virtual View* sensorParams() = 0;
	virtual void PlatformCtrl_sensor_Init(int* data) = 0;
	virtual void PlatformCtrl_CreateParams_Init(PlatformCtrl_CreateParams *pPrm, int* m_Prm, View* m_Sensor) = 0;
	virtual HPLTCTRL PlatformCtrl_Create(PlatformCtrl_CreateParams *pPrm) = 0;

	virtual int PlatformCtrl_VirtualInput(HPLTCTRL handle, int iIndex, float fValue) = 0;
	
	virtual void PlatformCtrl_UpdateParams(HPLTCTRL handle, float* data, int block) = 0;

	virtual int PlatformCtrl_TrackerInput(HPLTCTRL handle, PLATFORMCTRL_TrackerInput *pInput) = 0;
	virtual int PlatformCtrl_TrackerOutput(HPLTCTRL handle, PLATFORMCTRL_Output *pOutput) = 0;

	virtual void PlatformCtrl_reset4trk(HPLTCTRL handle)=0;
	virtual BoresightPos_s getBoresight(int* data , int zoom)=0;
	virtual void updateFov(int* data , HPLTCTRL handle = NULL ,int zoom = 0)=0;
	virtual void switchSensor(int* data, HPLTCTRL handle ,int chid , int zoom)=0;
};



#endif /* PLATFORMINTERFACE_H_ */
