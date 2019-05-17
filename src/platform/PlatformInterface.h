
#ifndef PLATFORMINTERFACE_H_
#define PLATFORMINTERFACE_H_

#include <iostream>
#include "platformparam.h"
#include "sensorComp.h"


class CPlatformInterface{

public:
	virtual ~CPlatformInterface(){};

	virtual View* sensorParams() = 0;
	virtual void PlatformCtrl_sensor_Init(float* data) = 0;
	virtual void PlatformCtrl_CreateParams_Init(PlatformCtrl_CreateParams *pPrm, float* m_Prm, View* m_Sensor) = 0;
	virtual HPLTCTRL PlatformCtrl_Create(PlatformCtrl_CreateParams *pPrm) = 0;

	virtual int PlatformCtrl_VirtualInput(HPLTCTRL handle, int iIndex, float fValue) = 0;
	virtual int PlatformCtrl_sensorCompensation(HPLTCTRL handle, int chid,unsigned int zoom) = 0;
	
	virtual void PlatformCtrl_UpdateParams(HPLTCTRL handle, float* data, int block) = 0;

	virtual int PlatformCtrl_TrackerInput(HPLTCTRL handle, PLATFORMCTRL_TrackerInput *pInput) = 0;
	virtual int PlatformCtrl_TrackerOutput(HPLTCTRL handle, PLATFORMCTRL_Output *pOutput) = 0;

	virtual void PlatformCtrl_reset4trk(HPLTCTRL handle)=0;

};



#endif /* PLATFORMINTERFACE_H_ */
