/*
 * platformparam.h
 *
 *  Created on: 2018年9月18日
 *      Author: jet
 */

#ifndef PLATFORMPARAM_H_
#define PLATFORMPARAM_H_

#include "deviceUser.h"
#include "platformFilter.h"
#include "Kalman.h"

const int  VIRTUAL_DEVICE_COUNT	= 24;
const int  ANALOG_DEVICE_COUNT	= 1;
const int  SENSOR_COUNT	= 5;


#ifndef ONLYREAD
#	define ONLYREAD
#endif
typedef void * HPLTFILTER;
typedef void * HSENSORCOMP;

typedef enum{
	DevUsr_AcqWinSizeXInput = 0,
	DevUsr_AcqWinSizeYInput,
	DevUsr_TrkWinSizeXInput,
	DevUsr_TrkWinSizeYInput,
	DevUsr_AcqJoystickXInput,
	DevUsr_AcqJoystickYInput,
	DevUsr_AimpointRefineXInput,
	DevUsr_AimpointRefineYInput,
	DevUsr_MAX
}eDevUsr;

typedef enum{
	AcqOutputType_Zero = 0,
	AcqOutputType_JoystickInput,
	AcqOutputType_ShapedAndGained,
	AcqOutputType_ShapedAndGainedAndIntegrated,
	AcqOutputType_Max
}eAcqOutputType;

typedef enum
{
	PlatStateType_Acquire = 0 ,
	PlatStateType_Trk_Lock = 1,
	PlatStateType_Trk_Break_Lock_1 = 2,
	PlatStateType_Trk_Break_Lock_2 = 3,
}eTrkStateType,ePlatStateType;

typedef enum{
	Bleed_Disable        = 0,
	Bleed_BrosightError  = 1,
	Bleed_PlatformDemand = 2
}ePlatfromCtrlBleed;


#pragma pack (4)

typedef struct{
	int x;
	int y;
}BoresightPos_s;

typedef struct
{
	float fFov;
	int x;
	int y;
}SensorComp_BoresightPos;

typedef struct
{
	int iIndex;
	int nWidth;
	int nHeight;
	int defaultBoresightPos_x;
	int defaultBoresightPos_y;
	float fFovMin;
	float fFovMax;
	float fFovY;
	int nInitTabSize;
}SensorComp_CreateParams;


typedef struct{
	int iSensor;
	ONLYREAD int iTrkAlgState;
	ONLYREAD float fPlatformDemandX;
	ONLYREAD float fPlatformDemandY;
}PLATFORMCTRL_Output;

typedef struct{
	ONLYREAD int iTrkAlgState;
	ONLYREAD float fTargetBoresightErrorX;
	ONLYREAD float fTargetBoresightErrorY;
}PLATFORMCTRL_TrackerInput;

typedef  struct {
	void *object;
	ONLYREAD float devUsrInput[DevUsr_MAX];
	ONLYREAD float localInput[DevUsr_MAX];
	ONLYREAD float virtalInput[VIRTUAL_DEVICE_COUNT];
	ONLYREAD float analogInput[ANALOG_DEVICE_COUNT];
	ONLYREAD PLATFORMCTRL_TrackerInput trackerInput;
	ONLYREAD PLATFORMCTRL_Output output;
}PLATFORMCTRL_Interface;
#pragma pack ()

typedef PLATFORMCTRL_Interface * HPLTCTRL;
typedef SensorComp_CreateParams SensorParams;
typedef DeviceUser_CreateParams DeviceUserParam;
typedef PlatformFilter_CreateParams PlatformFilterParam;

#pragma pack (4)
typedef struct {
	float fOffset_X;
	float fOffset_Y;
	float fDeadband;
	float fCutpoint1;
	float fInputGain_X1;
	float fInputGain_Y1;
	float fCutpoint2;
	float fInputGain_X2;
	float fInputGain_Y2;
	float fPlatformAcquisitionModeGain_X;
	float fPlatformAcquisitionModeGain_Y;

	int   iShapingMode;
	float fPlatformIntegratingGain_X;
	float fPlatformIntegratingGain_Y;
	float fPlatformIntegratingDecayGain;
}JoystickRateDemandParam;

typedef struct
{
	SensorParams sensorParams[SENSOR_COUNT];
	DeviceUserParam deviceUsrParam[DevUsr_MAX];
	PlatformFilterParam platformFilterParam[2];
	JoystickRateDemandParam joystickRateDemandParam;
	float demandMaxX;
	float demandMinX;
	float demandMaxY;
	float demandMinY;
	float deadbandX;
	float deadbandY;
	int bleedUsed;
	float bleedX;
	float bleedY;
	int iSensorInit;
	float fFovx[SENSOR_COUNT];
	float fFovy[SENSOR_COUNT];
	int acqOutputType;
	int bTrkWinFilter;
	int Kx;
	int Ky;
	float Error_X;
	float Error_Y;
	int Time;
}PlatformCtrlParam;
#pragma pack ()

typedef PlatformCtrlParam PlatformCtrl_CreateParams;

#pragma pack (4)
typedef struct
{
	float fovX;//mRad
	float fovY;//mRad
	float width;//pix
	float height;//pix
	float curRateDemandX;
	float curRateDemandY;
	int lastPlatStat;
	HDEVUSR hDevUsers[DevUsr_MAX];
	float usrInputBak[DevUsr_MAX];

	HPLTFILTER filter[2];
	int iFilter;
	HKalman hWinFilter;
	int iTrkAlgStateBak;
	int acqOutputTypeBak;
	
} PLATFORMCTRL_pri;

typedef struct
{
    PLATFORMCTRL_Interface inter;
    PlatformCtrlParam params;
    PLATFORMCTRL_pri privates;
}PlatformCtrl_Obj;
#pragma pack ()

#endif /* PLATFORMPARAM_H_ */
