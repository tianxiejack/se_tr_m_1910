#ifndef _PLATFORM_CONTROL_H
#define _PLATFORM_CONTROL_H

#include "PlatformInterface.h"


class CplatFormControl : public CPlatformInterface{

public:
	CplatFormControl();
	~CplatFormControl();

	View* sensorParams();
	void PlatformCtrl_sensor_Init(float* data);
	void PlatformCtrl_CreateParams_Init(PlatformCtrl_CreateParams *pPrm, float* m_Prm, View* m_Sensor);
	HPLTCTRL PlatformCtrl_Create(PlatformCtrl_CreateParams *pPrm);

	void PlatformCtrl_Delete(HPLTCTRL handle);
	void PlatformCtrl_reset4trk(HPLTCTRL handle);

	int PlatformCtrl_TrackerInput(HPLTCTRL handle, PLATFORMCTRL_TrackerInput *pInput);
	int PlatformCtrl_TrackerOutput(HPLTCTRL handle, PLATFORMCTRL_Output *pOutput);
	int PlatformCtrl_VirtualInput(HPLTCTRL handle, int iIndex, float fValue);
	int PlatformCtrl_sensorCompensation(HPLTCTRL handle, int chid,unsigned int zoom);
	void PlatformCtrl_UpdateParams(HPLTCTRL handle, float* data, int block);
	BoresightPos_s PlatformCtrl_SensorUpdateParams(float* data, int block, int field, int curChid, PlatformCtrl_Obj *pObj);

private:
	void PlatformCtrl_ClassInit();
	int PlatformCtrl_AcqRateDemand(PlatformCtrl_Obj *pObj);
	int PlatformCtrl_OutPlatformDemand(PlatformCtrl_Obj *pObj);
	int PlatformCtrl_BuildDevUsrInput(PlatformCtrl_Obj *pObj);
	void acqRateJosHandle(PlatformCtrl_Obj *pObj);
	void joyYHandle(PlatformCtrl_Obj *pObj);
	void joyXHandle(PlatformCtrl_Obj *pObj);
	void joyParamProcHandle(PlatformCtrl_Obj *pObj);
	
private:
	CPlatformFilter* _Fiter;
	CKalman_PTZ* _Kalman;
	CDeviceUser* _DeviceUser;
	CSensorComp* _Sensor;
	
};

#endif
