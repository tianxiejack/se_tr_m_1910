#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <osa.h>
#include "platformControl.h"
#include "configtable.h"


const float EPSINON = 0.000001f;


CplatFormControl::CplatFormControl()
{
	PlatformCtrl_ClassInit();
}



CplatFormControl::~CplatFormControl()
{
	delete _Fiter;
	delete _Kalman;
	delete _DeviceUser;
	delete _Sensor;
}



void CplatFormControl::PlatformCtrl_ClassInit()
{
	_Sensor = new CSensorComp();
	_Fiter = new CPlatformFilter( 50 , 5 , 1 ,1200);
	_Kalman = new CKalman_PTZ();
	_DeviceUser = new CDeviceUser();
}

View* CplatFormControl::sensorParams()
{
	return _Sensor->backParams();
}

void CplatFormControl::PlatformCtrl_sensor_Init(float* data)
{
	_Sensor->updateParam(data);
	return ;
}



HPLTCTRL CplatFormControl::PlatformCtrl_Create(PlatformCtrl_CreateParams *pPrm)
{
	PlatformCtrl_Obj *pObj;
	int i;
	if(pPrm == NULL)
	    return NULL;
	pObj = (PlatformCtrl_Obj *)malloc(sizeof(PlatformCtrl_Obj));
	if(pObj == NULL)
	    return NULL;

	memset(pObj, 0, sizeof(PlatformCtrl_Obj));
	memcpy(&pObj->params, pPrm, sizeof(PlatformCtrl_CreateParams));
	pObj->inter.object = pObj;
	pObj->privates.iTrkAlgStateBak = 0;

	pObj->inter.output.iSensor = pObj->params.iSensorInit;
	pObj->privates.fovX = pObj->params.fFovx[pObj->inter.output.iSensor];
	pObj->privates.fovY = pObj->params.fFovy[pObj->inter.output.iSensor];
	pObj->privates.width = (float)pObj->params.sensorParams[pObj->inter.output.iSensor].nWidth;
	pObj->privates.height = (float)pObj->params.sensorParams[pObj->inter.output.iSensor].nHeight;

	for(i = 0; i < DevUsr_MAX; i++)
	    pObj->privates.hDevUsers[i] = _DeviceUser->DeviceUser_Create(&pObj->params.deviceUsrParam[i], pObj);

	pObj->privates.filter[0] = _Fiter->PlatformFilter_Create(&pObj->params.platformFilterParam[0]);
	pObj->privates.filter[1] = _Fiter->PlatformFilter_Create(&pObj->params.platformFilterParam[1]);

	pObj->privates.hWinFilter = _Kalman->KalmanOpen(6, 3, 0);
	_Kalman->KalmanInitParam(pObj->privates.hWinFilter, 0.0, 0.0, 0, 0.0);

	return &pObj->inter;
}



void CplatFormControl::PlatformCtrl_CreateParams_Init(PlatformCtrl_CreateParams *pPrm, float* m_Prm, View* m_Sensor)
{
	int i;
	if(pPrm == NULL || m_Sensor == NULL)
	return;
	memset(pPrm, 0, sizeof(PlatformCtrl_CreateParams));

	i = 1;
	_Sensor->SensorComp_CreateParams_Init(&pPrm->sensorParams[i], i, m_Sensor,1920,1080);
	pPrm->sensorParams[i].iIndex = i;
	pPrm->fFovx[i] = pPrm->sensorParams[i].fFovMax;
	pPrm->fFovy[i] = pPrm->sensorParams[i].fFovY;

	
	pPrm->iSensorInit = 1;
	for(i=0; i<DevUsr_MAX; i++){
		_DeviceUser->DeviceUser_CreateParams_Init(&pPrm->deviceUsrParam[i]);
		pPrm->deviceUsrParam[i].iIndex = i;
		pPrm->deviceUsrParam[i].iInputSrcNum = i;
		pPrm->deviceUsrParam[i].inputSrcType = DEVUSER_InputSrcType_Virtual;
	}
	pPrm->deviceUsrParam[DevUsr_AcqWinSizeXInput].gainType = DEVUSER_GainType_Universal;
	pPrm->deviceUsrParam[DevUsr_AcqWinSizeYInput].gainType = DEVUSER_GainType_Universal;
	pPrm->deviceUsrParam[DevUsr_TrkWinSizeXInput].gainType = DEVUSER_GainType_Universal;
	pPrm->deviceUsrParam[DevUsr_TrkWinSizeYInput].gainType = DEVUSER_GainType_Universal;

	_Fiter->PlatformFilter_updatePidParams(&pPrm->platformFilterParam[0],&pPrm->platformFilterParam[1],m_Prm);

	pPrm->demandMaxX = 8000;
	pPrm->demandMaxY = 8000;

	pPrm->bleedUsed = Bleed_BrosightError;
	pPrm->deadbandX = 30;
	pPrm->deadbandY = 30;

	pPrm->acqOutputType = 2;


	pPrm->Kx = m_Prm[CFGID_PID_RATIOX];
	pPrm->Ky = m_Prm[CFGID_PID_RATIOY];
	pPrm->Error_X = m_Prm[CFGID_PID_ERRORX];
	pPrm->Error_Y = m_Prm[CFGID_PID_ERRORY];
	pPrm->Time = m_Prm[CFGID_PID_TIME];
	pPrm->bleedX =m_Prm[CFGID_PID_LIMITX];
	pPrm->bleedY =m_Prm[CFGID_PID_LIMITY];

	pPrm->joystickRateDemandParam.fDeadband =m_Prm[CFGID_JOS_deadx];// 0.1f;
	pPrm->joystickRateDemandParam.fDeadbandy =m_Prm[CFGID_JOS_deady];
	
	pPrm->joystickRateDemandParam.fInputGain_X1 =m_Prm[CFGID_JOS_inputgainx1];// 0.2f;
	pPrm->joystickRateDemandParam.fInputGain_Y1 =m_Prm[CFGID_JOS_inputgainy1];// 0.3f;
	pPrm->joystickRateDemandParam.fInputGain_X2 =m_Prm[CFGID_JOS_inputgainx2];// 0.2f;
	pPrm->joystickRateDemandParam.fInputGain_Y2 =m_Prm[CFGID_JOS_inputgainy2];// 0.3f;

	pPrm->joystickRateDemandParam.fCutpoint1 = m_Prm[CFGID_JOS_cutpointx1];//0.7f;
	pPrm->joystickRateDemandParam.fCutpoint1y = m_Prm[CFGID_JOS_cutpointy1];
	pPrm->joystickRateDemandParam.fCutpoint2 = m_Prm[CFGID_JOS_cutpointx2];//0.7f;
	pPrm->joystickRateDemandParam.fCutpoint2y = m_Prm[CFGID_JOS_cutpointy2];
	
	pPrm->joystickRateDemandParam.fPlatformAcquisitionModeGain_X = 5000.0f;
	pPrm->joystickRateDemandParam.fPlatformAcquisitionModeGain_Y = 5000.0f;

	pPrm->bTrkWinFilter = 1;
	return ;
}



int CplatFormControl::PlatformCtrl_sensorCompensation(HPLTCTRL handle, int chid,unsigned int zoom)
{
	PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;
	pObj->privates.fovX = _Sensor->ZoomLevelFovCompensation(zoom, chid);
	pObj->privates.fovY = _Sensor->ZoomVerticalFovCompensation(zoom, chid);
	return 0;
}



void CplatFormControl::PlatformCtrl_Delete(HPLTCTRL handle)
{
	int i;
	PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;
	if(pObj == NULL)
		return ;

	_Fiter->PlatformFilter_Delete(pObj->privates.filter[0]);
	_Fiter->PlatformFilter_Delete(pObj->privates.filter[1]);

	_Kalman->KalmanClose(pObj->privates.hWinFilter);

	for(i = 0; i < DevUsr_MAX; i++)
		_DeviceUser->DeviceUser_Delete(pObj->privates.hDevUsers[i]);

	free(pObj);
	return;
}

static int PlatformCtrl_PlatformCompensation(PlatformCtrl_Obj *pObj)
{
	static float fTmp;

	fTmp = pObj->privates.curRateDemandX;
	
	if(fabs(fTmp) > pObj->params.demandMaxX){
		if(fTmp > 0)
			fTmp = pObj->params.demandMaxX;
		else
			fTmp = -pObj->params.demandMaxX;
	}
	
	if(fabsf(fTmp) < pObj->params.deadbandX)
		fTmp = 0;
	
	pObj->inter.output.fPlatformDemandX = fTmp;


	fTmp = pObj->privates.curRateDemandY;
	
	if(fabs(fTmp) > pObj->params.demandMaxY){
		if(fTmp > 0)
			fTmp = pObj->params.demandMaxY;
		else
			fTmp = -pObj->params.demandMaxY;
	}
	
	if(fabsf(fTmp) < pObj->params.deadbandY)
		fTmp = 0;

	pObj->inter.output.fPlatformDemandY = fTmp;
	
	return 0;
}



void CplatFormControl::joyParamProcHandle(PlatformCtrl_Obj *pObj)
{
	JoystickRateDemandParam *pParam = &pObj->params.joystickRateDemandParam;

	/*param limit*/
	if(pParam->fDeadband > pParam->fCutpoint1)
		pParam->fDeadband = pParam->fCutpoint1;
	if(pParam->fCutpoint1 > pParam->fCutpoint2)
		pParam->fCutpoint1 = pParam->fCutpoint2;

	if(pParam->fCutpoint2 > 1)
		pParam->fCutpoint2 = 1;

	if(pParam->fCutpoint2 < 0)
		pParam->fCutpoint2 = 0;
	if(pParam->fCutpoint1 < 0)
		pParam->fCutpoint1 = 0;
	if(pParam->fDeadband < 0)
		pParam->fDeadband = 0;
	
	return ;
}



void CplatFormControl::joyXHandle(PlatformCtrl_Obj *pObj)
{
	float fTmp;
	float fTmp2, K, K2;

	JoystickRateDemandParam *pParam = &pObj->params.joystickRateDemandParam;

	//X Axis
	fTmp = pObj->inter.devUsrInput[DevUsr_AcqJoystickXInput];

	if(fabsf(fTmp) < pParam->fDeadband)
		fTmp = 0.0;

	if(fabsf(fTmp) < pParam->fCutpoint1)
	{
		if(fTmp > 0){
			fTmp -= pParam->fDeadband;
			fTmp *= pParam->fInputGain_X1;
		}
		else if (fTmp < 0){
			fTmp = fabsf(fTmp);
			fTmp -= pParam->fDeadband;
			fTmp *= pParam->fInputGain_X1;	
			fTmp *= -1;
		}
	}
	else if(fabsf(fTmp) >= pParam->fCutpoint1 && fabsf(fTmp) < pParam->fCutpoint2)
	{
		if(fTmp >= 0){
			fTmp = pParam->fInputGain_X1* (pParam->fCutpoint1- pParam->fDeadband)
				+ pParam->fInputGain_X2*( fTmp - pParam->fCutpoint1);
		}else{
			fTmp = fabsf(fTmp);
			fTmp = pParam->fInputGain_X1* (pParam->fCutpoint1- pParam->fDeadband)
				+ pParam->fInputGain_X2*( fTmp - pParam->fCutpoint1);
			fTmp *= -1;
		}
	}
	else
	{
		if(fTmp > 0){
			fTmp2 = pParam->fInputGain_X1* (pParam->fCutpoint1- pParam->fDeadband)
				+ pParam->fInputGain_X2*( fTmp - pParam->fCutpoint1);
			K = 1 - fTmp2;
			K /= (1 - pParam->fCutpoint2);
			fTmp -= pParam->fCutpoint2;
			fTmp *= K ;
			fTmp += fTmp2;
		}
		else if (fTmp < 0){
			fTmp = fabsf(fTmp);
			fTmp2 = pParam->fInputGain_X1* (pParam->fCutpoint1- pParam->fDeadband)
				+ pParam->fInputGain_X2*( fTmp - pParam->fCutpoint1);
			K = 1 - fTmp2;
			K /= (1 - pParam->fCutpoint2);
			fTmp -= pParam->fCutpoint2;
			fTmp *= K ;
			fTmp += fTmp2;
			fTmp *= -1;
		}
	}

	fTmp *= pParam->fPlatformAcquisitionModeGain_X;

	if(pObj->params.acqOutputType == AcqOutputType_ShapedAndGainedAndIntegrated){
	}
	else
		_Fiter->calcCurPlatAvrRate(pObj->privates.filter[0], fTmp);

	pObj->privates.curRateDemandX = fTmp;
	
	return;
}


void CplatFormControl::joyYHandle(PlatformCtrl_Obj *pObj)
{
	float fTmp;
	float fTmp2, K, K2;

	JoystickRateDemandParam *pParam = &pObj->params.joystickRateDemandParam;

	// Y Axis
	fTmp = pObj->inter.devUsrInput[DevUsr_AcqJoystickYInput];

	if(fabsf(fTmp) < pParam->fDeadband)
		fTmp = 0.0;

	if(fabsf(fTmp) < pParam->fCutpoint1)
	{
		if(fTmp > 0){
		fTmp -= pParam->fDeadband;
		fTmp *= pParam->fInputGain_Y1;
		}
		else if (fTmp < 0){
			fTmp = fabsf(fTmp);
			fTmp -= pParam->fDeadband;
			fTmp *= pParam->fInputGain_Y1;	
			fTmp *= -1;
		}
	}
	else if(fabsf(fTmp) >= pParam->fCutpoint1 && fabsf(fTmp) < pParam->fCutpoint2)
	{
		if(fTmp >= 0){
			fTmp = pParam->fInputGain_Y1* (pParam->fCutpoint1- pParam->fDeadband)
				+ pParam->fInputGain_Y2*( fTmp - pParam->fCutpoint1);
		}else{
			fTmp = fabsf(fTmp);
			fTmp = pParam->fInputGain_Y1* (pParam->fCutpoint1- pParam->fDeadband)
				+ pParam->fInputGain_Y2*( fTmp - pParam->fCutpoint1);
			fTmp *= -1;
		}
	}
	else
	{
		if(fTmp > 0){
			fTmp2 = pParam->fInputGain_Y1* (pParam->fCutpoint1- pParam->fDeadband)
				+ pParam->fInputGain_Y2*( fTmp - pParam->fCutpoint1);
			K = 1 - fTmp2;
			K /= (1 - pParam->fCutpoint2);
			fTmp -= pParam->fCutpoint2;
			fTmp *= K ;
			fTmp += fTmp2;
		}
		else if (fTmp < 0){
			fTmp = fabsf(fTmp);
			fTmp2 = pParam->fInputGain_Y1* (pParam->fCutpoint1- pParam->fDeadband)
				+ pParam->fInputGain_Y2*( fTmp - pParam->fCutpoint1);
			K = 1 - fTmp2;
			K /= (1 - pParam->fCutpoint2);
			fTmp -= pParam->fCutpoint2;
			fTmp *= K ;
			fTmp += fTmp2;
			fTmp *= -1;
		}
	}

	fTmp *= pParam->fPlatformAcquisitionModeGain_Y;

	if(pObj->params.acqOutputType == AcqOutputType_ShapedAndGainedAndIntegrated){
	}
	else
		_Fiter->calcCurPlatAvrRate(pObj->privates.filter[1], fTmp);

	pObj->privates.curRateDemandY = fTmp;
	
	return;
}



void CplatFormControl::acqRateJosHandle(PlatformCtrl_Obj *pObj)
{
	float fTmp;
	float fTmp2, K, K2;

	JoystickRateDemandParam *pParam = &pObj->params.joystickRateDemandParam;

	joyParamProcHandle(pObj);
	joyXHandle(pObj);
	joyYHandle(pObj);

	return ;   
}



int CplatFormControl::PlatformCtrl_AcqRateDemand(PlatformCtrl_Obj *pObj)
{
	pObj->privates.curRateDemandX = 0.0f;
	pObj->privates.curRateDemandY = 0.0f;

	switch(pObj->params.acqOutputType)
	{
		case AcqOutputType_Zero:
			break;

		case AcqOutputType_JoystickInput:
			pObj->privates.curRateDemandX = pObj->inter.devUsrInput[DevUsr_AcqJoystickXInput];
			pObj->privates.curRateDemandY = pObj->inter.devUsrInput[DevUsr_AcqJoystickYInput];
			break;

		case AcqOutputType_ShapedAndGained:
		case AcqOutputType_ShapedAndGainedAndIntegrated:
			acqRateJosHandle(pObj);
			break;

		default:
			break;
	}
	return 0;
}



int CplatFormControl::PlatformCtrl_OutPlatformDemand(PlatformCtrl_Obj *pObj)
{
	PLATFORMCTRL_TrackerInput *pInput = &pObj->inter.trackerInput;
	static float fTmp, fTmpX,  fTmpY;

	if(pInput->iTrkAlgState == PlatStateType_Acquire)
	{
		PlatformCtrl_AcqRateDemand(pObj);

		if(pObj->privates.iTrkAlgStateBak != pInput->iTrkAlgState)
		{
			//printf("=====================PlatForm Reset====================\n");
			_Fiter->PlatformFilter_Reset( pObj->privates.filter[0] );
			_Fiter->PlatformFilter_Reset( pObj->privates.filter[1] );
			pObj->privates.curRateDemandX = 0;
			pObj->privates.curRateDemandY = 0;
			fTmp = fTmpX = fTmpY = pInput->fTargetBoresightErrorX = pInput->fTargetBoresightErrorY = 0;
		}
	}

	if(pInput->iTrkAlgState == PlatStateType_Trk_Lock)
	{
		fTmpX = pInput->fTargetBoresightErrorX;
		fTmpY = pInput->fTargetBoresightErrorY;

		if(fabs(fTmpX) < 1.01)
		fTmpX = 0;
		if(fabs(fTmpY) < 1.01)
		fTmpY = 0;

		fTmp = fTmpX * pObj->privates.fovX / pObj->privates.width;	
		pObj->privates.curRateDemandX = _Fiter->pidAlg(pObj->privates.filter[0], fTmp);

		fTmp = fTmpY * pObj->privates.fovY / pObj->privates.height;
		pObj->privates.curRateDemandY = _Fiter->pidAlg(pObj->privates.filter[1], fTmp);
	}


	if(pInput->iTrkAlgState == PlatStateType_Trk_Break_Lock_2)
	{
		pObj->privates.curRateDemandX = 0;
		pObj->privates.curRateDemandY = 0;
		_Fiter->PlatformFilter_Reset( pObj->privates.filter[0] );
		_Fiter->PlatformFilter_Reset( pObj->privates.filter[1] );
	}

	PlatformCtrl_PlatformCompensation(pObj); 

	pObj->privates.lastPlatStat = pInput->iTrkAlgState;

	return 0;
}



int CplatFormControl::PlatformCtrl_BuildDevUsrInput(PlatformCtrl_Obj *pObj)
{
	int i;
	for(i = 0; i < DevUsr_MAX; i++)
		pObj->inter.devUsrInput[i] = _DeviceUser->DeviceUser_Get(pObj->privates.hDevUsers[i]);

	return 0;
}



static int PlatformCtrl_ProccesDevUsrInput(PlatformCtrl_Obj *pObj)
{
	memcpy(pObj->privates.usrInputBak, pObj->inter.devUsrInput, sizeof(pObj->privates.usrInputBak));
	return 0;
}



int CplatFormControl::PlatformCtrl_TrackerInput(HPLTCTRL handle, PLATFORMCTRL_TrackerInput *pInput)
{
	int iRet = 0;

	PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

	if(pObj == NULL || pInput == NULL)
	    return -1;

	memcpy(&pObj->inter.trackerInput, pInput, sizeof(PLATFORMCTRL_TrackerInput));

	PlatformCtrl_BuildDevUsrInput(pObj);

	PlatformCtrl_OutPlatformDemand(pObj);

	pObj->inter.output.iTrkAlgState = pInput->iTrkAlgState;


	#if 0
		pObj->inter.output.fTargetBoresightErrorX = pInput->fTargetBoresightErrorX;
		pObj->inter.output.fTargetBoresightErrorY = pInput->fTargetBoresightErrorY;

		if(pInput->iTrkAlgState == 0 || pInput->iTrkAlgState == 1)
		{
			if(pObj->privates.iTrkAlgStateBak > 1)
			{
			_Kalman->KalmanInitParam(pObj->privates.hWinFilter, 0.0, 0.0, 0, 0.0);
			}
		}
		else
		{
			pObj->inter.output.fWindowPositionX = pInput->fTargetBoresightErrorX;
			pObj->inter.output.fWindowPositionY = pInput->fTargetBoresightErrorY;

			if(pObj->params.bTrkWinFilter)
			{
				double dbM[3];
				dbM[0] = 0.0f;
				dbM[1] = pInput->fTargetBoresightErrorX;
				dbM[2] = pInput->fTargetBoresightErrorY;
				_Kalman->Kalman(pObj->privates.hWinFilter, dbM, NULL);

				pObj->inter.output.fWindowPositionX = (float)pObj->privates.hWinFilter->state_post[2];
				pObj->inter.output.fWindowPositionY = (float)pObj->privates.hWinFilter->state_post[4];
			}
		}
	#endif

	PlatformCtrl_ProccesDevUsrInput(pObj);
	pObj->privates.iTrkAlgStateBak = pInput->iTrkAlgState;
	pObj->privates.acqOutputTypeBak = pObj->params.acqOutputType;
	return iRet;
}



int CplatFormControl::PlatformCtrl_TrackerOutput(HPLTCTRL handle, PLATFORMCTRL_Output *pOutput)
{
	int iRet = 0;
	PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

	if(pObj == NULL || pOutput == NULL)
		return -1;

	memcpy(pOutput, &pObj->inter.output, sizeof(PLATFORMCTRL_Output));
	return iRet;
}



int CplatFormControl::PlatformCtrl_VirtualInput(HPLTCTRL handle, int iIndex, float fValue)
{
	float fTmp;
	printf("handle = %p \n", handle);
	PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

	if(pObj == NULL)
	{
		printf("pObj = %p \n", pObj);
		return -1;
	}
	fTmp = fValue;
	pObj->inter.virtalInput[iIndex] = fTmp;

	return 0;
}



void CplatFormControl::PlatformCtrl_UpdateParams(HPLTCTRL handle, float* m_Prm, int block)
{
	 PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;
	 JoystickRateDemandParam *pPrm = &pObj->params.joystickRateDemandParam;
	 int num;
	 if(block == 1)
	 {
		num = (block-1)*16;
		pPrm->fDeadband =m_Prm[++num];
		pPrm->fCutpoint1 = m_Prm[++num];
		pPrm->fInputGain_X1 =m_Prm[++num];
		pPrm->fInputGain_Y1 =m_Prm[++num];
		pPrm->fCutpoint2 = m_Prm[++num];
		pPrm->fInputGain_X2 =m_Prm[++num];
		pPrm->fInputGain_Y2 =m_Prm[++num];
	 }

	 if(block == 91)
	 {
		PlatformFilter_Obj *Pidx = (PlatformFilter_Obj*)pObj->privates.filter[0];
		PlatformFilter_Obj *Pidy = (PlatformFilter_Obj*)pObj->privates.filter[1];
		num = (block-1)*16;
		Pidx->params.Kp  = m_Prm[++num];
		Pidx->params.Ki = m_Prm[++num];
		Pidx->params.Kd = m_Prm[++num];
		Pidx->params.K = m_Prm[++num];

		Pidy->params.Kp  = m_Prm[++num];
		Pidy->params.Ki = m_Prm[++num];
		Pidy->params.Kd = m_Prm[++num];
		Pidy->params.K = m_Prm[++num];
	 }

	 if(block == 92)
	 {
		num = (block-1)*16;
		pObj->params.Kx = m_Prm[num];
		pObj->params.Ky = m_Prm[++num];
		pObj->params.Error_X = m_Prm[++num];
		pObj->params.Error_Y = m_Prm[++num];
		pObj->params.Time = m_Prm[++num];
		pObj->params.bleedX =m_Prm[++num];
		pObj->params.bleedY =m_Prm[++num];
	 }

	 if(block == 100)
	 {
		num = (block-1)*16 + 4;
		pObj->params.demandMaxX = m_Prm[num];
		pObj->params.demandMaxY = m_Prm[++num];
		pObj->params.deadbandX = m_Prm[++num];
		pObj->params.deadbandY = m_Prm[++num];
	 }
}



void CplatFormControl::PlatformCtrl_reset4trk(HPLTCTRL handle)
{
	PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;
	_Fiter->PlatformFilter_reset4trk(pObj->privates.filter[0]);
	_Fiter->PlatformFilter_reset4trk(pObj->privates.filter[1]);

	return ;
}
