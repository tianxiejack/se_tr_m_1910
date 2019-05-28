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
	m_viewParam = _Sensor->backParams();
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

void CplatFormControl::PlatformCtrl_sensor_Init(int* data)
{
	_Sensor->getCameraResolution(data);
	_Sensor->updateFov(data);
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
	pObj->privates.fovX = m_viewParam->level_Fov_fix[pObj->inter.output.iSensor];
	pObj->privates.fovY = m_viewParam->vertical_Fov_fix[pObj->inter.output.iSensor];
	pObj->privates.width = m_viewParam->width[pObj->inter.output.iSensor];
	pObj->privates.height =  m_viewParam->height[pObj->inter.output.iSensor];

	for(i = 0; i < DevUsr_MAX; i++)
	    pObj->privates.hDevUsers[i] = _DeviceUser->DeviceUser_Create(&pObj->params.deviceUsrParam[i], pObj);

	pObj->privates.filter[0] = _Fiter->PlatformFilter_Create(&pObj->params.platformFilterParam[0]);
	pObj->privates.filter[1] = _Fiter->PlatformFilter_Create(&pObj->params.platformFilterParam[1]);

	pObj->privates.hWinFilter = _Kalman->KalmanOpen(6, 3, 0);
	_Kalman->KalmanInitParam(pObj->privates.hWinFilter, 0.0, 0.0, 0, 0.0);
	
	return &pObj->inter;
}



void CplatFormControl::PlatformCtrl_CreateParams_Init(PlatformCtrl_CreateParams *pPrm, int* m_Prm, View* m_Sensor)
{
	int i , base;
	if(pPrm == NULL || m_Sensor == NULL)
	return;
	memset(pPrm, 0, sizeof(PlatformCtrl_CreateParams));

	for(int i=0;i<5;i++)
	{
		if(i == 0)
			base = CFGID_INPUT1_BKID ; 
		else
			base = CFGID_INPUT2_BKID + (i-1)*7;
		
		_Sensor->SensorComp_CreateParams_Init(&pPrm->sensorParams[i], i, m_Sensor,m_Prm[CFGID_INPUT_RESX(base)],m_Prm[CFGID_INPUT_RESY(base)]);
		pPrm->sensorParams[i].iIndex = i;
		pPrm->fFovx[i] = pPrm->sensorParams[i].fFovMax;
		pPrm->fFovy[i] = pPrm->sensorParams[i].fFovY;
	}
	
	if( m_Prm[CFGID_RTS_mainch] >=0 &&  m_Prm[CFGID_RTS_mainch] <= 5)
		pPrm->iSensorInit = m_Prm[CFGID_OUTPUT_DEFCH];
	else
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

	pPrm->deadbandX = 30;
	pPrm->deadbandY = 30;

	pPrm->acqOutputType = 2;


	memcpy(&pPrm->joystickRateDemandParam.fDeadbandx, m_Prm + CFGID_JOS_deadx, 4);// 0.1f;
	memcpy(&pPrm->joystickRateDemandParam.fDeadbandy, m_Prm + CFGID_JOS_deady, 4);
	memcpy(&pPrm->joystickRateDemandParam.fInputGain_X1, m_Prm + CFGID_JOS_inputgainx1, 4);// 0.2f;
	memcpy(&pPrm->joystickRateDemandParam.fInputGain_Y1, m_Prm + CFGID_JOS_inputgainy1, 4);// 0.3f;
	memcpy(&pPrm->joystickRateDemandParam.fInputGain_X2, m_Prm + CFGID_JOS_inputgainx2, 4);// 0.2f;
	memcpy(&pPrm->joystickRateDemandParam.fInputGain_Y2, m_Prm + CFGID_JOS_inputgainy2, 4);// 0.3f;
	memcpy(&pPrm->joystickRateDemandParam.fCutpoint1x, m_Prm + CFGID_JOS_cutpointx1, 4);//0.7f;
	memcpy(&pPrm->joystickRateDemandParam.fCutpoint1y, m_Prm + CFGID_JOS_cutpointy1, 4);
	memcpy(&pPrm->joystickRateDemandParam.fCutpoint2x, m_Prm + CFGID_JOS_cutpointx2, 4);//0.7f;
	memcpy(&pPrm->joystickRateDemandParam.fCutpoint2y, m_Prm + CFGID_JOS_cutpointy2, 4);
	
	pPrm->joystickRateDemandParam.fPlatformAcquisitionModeGain_X = 5000.0f;
	pPrm->joystickRateDemandParam.fPlatformAcquisitionModeGain_Y = 5000.0f;

	pPrm->bTrkWinFilter = 1;
	return ;
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
	//x
	if(pParam->fDeadbandx > pParam->fCutpoint1x)
		pParam->fDeadbandx = pParam->fCutpoint1x;
	if(pParam->fCutpoint1x > pParam->fCutpoint2x)
		pParam->fCutpoint1x = pParam->fCutpoint2x;

	if(pParam->fCutpoint2x > 1)
		pParam->fCutpoint2x = 1;

	if(pParam->fCutpoint2x < 0)
		pParam->fCutpoint2x = 0;
	if(pParam->fCutpoint1x < 0)
		pParam->fCutpoint1x = 0;
	if(pParam->fDeadbandx < 0)
		pParam->fDeadbandx = 0;

	//y
	if(pParam->fDeadbandy > pParam->fCutpoint1y)
		pParam->fDeadbandy = pParam->fCutpoint1y;
	if(pParam->fCutpoint1y > pParam->fCutpoint2y)
		pParam->fCutpoint1y = pParam->fCutpoint2y;

	if(pParam->fCutpoint2y > 1)
		pParam->fCutpoint2y = 1;

	if(pParam->fCutpoint2y < 0)
		pParam->fCutpoint2y = 0;
	if(pParam->fCutpoint1y < 0)
		pParam->fCutpoint1y = 0;
	if(pParam->fDeadbandy < 0)
		pParam->fDeadbandy = 0;

	return ;
}



void CplatFormControl::joyXHandle(PlatformCtrl_Obj *pObj)
{
	float fTmp;
	float fTmp2, K, K2;

	JoystickRateDemandParam *pParam = &pObj->params.joystickRateDemandParam;

	//X Axis
	fTmp = pObj->inter.devUsrInput[DevUsr_AcqJoystickXInput];
	
	if(fabsf(fTmp) < pParam->fDeadbandx)
		fTmp = 0.0;

	if(fabsf(fTmp) < pParam->fCutpoint1x)
	{
		if(fTmp > 0){
			fTmp -= pParam->fDeadbandx;
			fTmp *= pParam->fInputGain_X1;
		}
		else if (fTmp < 0){
			fTmp = fabsf(fTmp);
			fTmp -= pParam->fDeadbandx;
			fTmp *= pParam->fInputGain_X1;	
			fTmp *= -1;
		}
	}
	else if(fabsf(fTmp) >= pParam->fCutpoint1x && fabsf(fTmp) < pParam->fCutpoint2x)
	{
		if(fTmp >= 0){
			fTmp = pParam->fInputGain_X1* (pParam->fCutpoint1x - pParam->fDeadbandx)
				+ pParam->fInputGain_X2*( fTmp - pParam->fCutpoint1x);
		}else{
			fTmp = fabsf(fTmp);
			fTmp = pParam->fInputGain_X1* (pParam->fCutpoint1x - pParam->fDeadbandx)
				+ pParam->fInputGain_X2*( fTmp - pParam->fCutpoint1x);
			fTmp *= -1;
		}
	}
	else
	{
		if(fTmp > 0){
			fTmp2 = pParam->fInputGain_X1* (pParam->fCutpoint1x- pParam->fDeadbandx)
				+ pParam->fInputGain_X2*( fTmp - pParam->fCutpoint1x);
			K = 1 - fTmp2;
			K /= (1 - pParam->fCutpoint2x);
			fTmp -= pParam->fCutpoint2x;
			fTmp *= K ;
			fTmp += fTmp2;
		}
		else if (fTmp < 0){
			fTmp = fabsf(fTmp);
			fTmp2 = pParam->fInputGain_X1* (pParam->fCutpoint1x - pParam->fDeadbandx)
				+ pParam->fInputGain_X2*( fTmp - pParam->fCutpoint1x);
			K = 1 - fTmp2;
			K /= (1 - pParam->fCutpoint2x);
			fTmp -= pParam->fCutpoint2x;
			fTmp *= K ;
			fTmp += fTmp2;
			fTmp *= -1;
		}
	}

	fTmp *= pParam->fPlatformAcquisitionModeGain_X;

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

	if(fabsf(fTmp) < pParam->fDeadbandy)
		fTmp = 0.0;

	if(fabsf(fTmp) < pParam->fCutpoint1y)
	{
		if(fTmp > 0){
		fTmp -= pParam->fDeadbandy;
		fTmp *= pParam->fInputGain_Y1;
		}
		else if (fTmp < 0){
			fTmp = fabsf(fTmp);
			fTmp -= pParam->fDeadbandy;
			fTmp *= pParam->fInputGain_Y1;	
			fTmp *= -1;
		}
	}
	else if(fabsf(fTmp) >= pParam->fCutpoint1y && fabsf(fTmp) < pParam->fCutpoint2y)
	{
		if(fTmp >= 0){
			fTmp = pParam->fInputGain_Y1* (pParam->fCutpoint1y- pParam->fDeadbandy)
				+ pParam->fInputGain_Y2*( fTmp - pParam->fCutpoint1y);
		}else{
			fTmp = fabsf(fTmp);
			fTmp = pParam->fInputGain_Y1* (pParam->fCutpoint1y- pParam->fDeadbandy)
				+ pParam->fInputGain_Y2*( fTmp - pParam->fCutpoint1y);
			fTmp *= -1;
		}
	}
	else
	{
		if(fTmp > 0){
			fTmp2 = pParam->fInputGain_Y1* (pParam->fCutpoint1y- pParam->fDeadbandy)
				+ pParam->fInputGain_Y2*( fTmp - pParam->fCutpoint1y);
			K = 1 - fTmp2;
			K /= (1 - pParam->fCutpoint2y);
			fTmp -= pParam->fCutpoint2y;
			fTmp *= K ;
			fTmp += fTmp2;
		}
		else if (fTmp < 0){
			fTmp = fabsf(fTmp);
			fTmp2 = pParam->fInputGain_Y1* (pParam->fCutpoint1y- pParam->fDeadbandy)
				+ pParam->fInputGain_Y2*( fTmp - pParam->fCutpoint1y);
			K = 1 - fTmp2;
			K /= (1 - pParam->fCutpoint2y);
			fTmp -= pParam->fCutpoint2y;
			fTmp *= K ;
			fTmp += fTmp2;
			fTmp *= -1;
		}
	}

	fTmp *= pParam->fPlatformAcquisitionModeGain_Y;

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

printf("fTmp = %f  , width = %f \n" , fTmp , pObj->privates.width);
printf(" fov X : %f   , demand X  = %f  \n", pObj->privates.fovX , pObj->privates.curRateDemandX  );

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
	PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

	if(pObj == NULL)
	{
		printf("%s :  pObj = %p \n", __func__, pObj);
		return -1;
	}
	fTmp = fValue;
	pObj->inter.virtalInput[iIndex] = fTmp;

	return 0;
}


void CplatFormControl::PlatformCtrl_reset4trk(HPLTCTRL handle)
{
	PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;
	_Fiter->PlatformFilter_reset4trk(pObj->privates.filter[0]);
	_Fiter->PlatformFilter_reset4trk(pObj->privates.filter[1]);
	return ;
}


BoresightPos_s CplatFormControl::getBoresight(int* data , int zoom)
{
	return _Sensor->getBoresight(data, zoom);
}


void CplatFormControl::updateFov(int* data , HPLTCTRL handle ,int zoom)
{
	PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;
	_Sensor->updateFov(data ,pObj, zoom);
	return;
}


void CplatFormControl::switchSensor(int* data, HPLTCTRL handle ,int chid,int zoom)
{
	PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;
	pObj->inter.output.iSensor = chid;
	_Sensor->updateWH(data  ,pObj , chid);
	_Sensor->getBoresight(data , zoom , chid );
	_Sensor->updateFov(data, pObj, zoom , chid);
	return ;
}

