#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "sensorComp.h"
#include "configtable.h"


#define EPSINON   (0.000001f)
#ifndef min
#define min(a, b)	((a)<(b)?(a):(b))
#endif

typedef struct {
	int nCount;
	int nSize;
	SensorComp_BoresightPos *pos;
}SensorComp_BoresightPosTab;

typedef struct {
	SensorComp_CreateParams params;
	int iCurPos;
	float fCurFov;
	int iCurX, iCurY;
}SensorComp_Obj;

View* CSensorComp::backParams()
{
	return &m_viewParam;
}


void CSensorComp::updateFixFovParam( int* data , PlatformCtrl_Obj* pObj )
{
	float value ;
	int base;
	
	for(int i=0 ; i<chid_camera; i++)
	{
		if(chid_camera == 0)
			base = CFGID_INPUT1_BKID ; 
		else
			base = CFGID_INPUT2_BKID + (i-1)*7 ;

		memcpy(&value, data + CFGID_INPUT_FOVX(base , 1), 4);
		m_viewParam.level_Fov_fix[i] = value * (PI / 180) * 1000;
		memcpy(&value, data + CFGID_INPUT_FOVY(base , 1), 4);
		m_viewParam.vertical_Fov_fix[i] = value * (PI / 180) * 1000;

		memcpy(&value, data + CFGID_INPUT_boresightX(base , 1), 4);
		m_viewParam.Boresightpos_fix_x[i] = value;
		memcpy(&value, data + CFGID_INPUT_boresightY(base , 1), 4);
		m_viewParam.Boresightpos_fix_y[i] = value;
	}
	return ;
}



void CSensorComp::updateSwitchFovParam( int* data , PlatformCtrl_Obj* pObj )
{
	float value ;
	int base;
	float *ptr;
	
	for(int j=0 ; j<chid_camera; j++)
	{
		if(j == 0)
			base = CFGID_INPUT1_BKID ; 
		else
			base = CFGID_INPUT2_BKID + (j-1)*7 ;
		
		ptr = &m_viewParam.level_Fov_switch1[j];
		
		for(int i=0 ; i<13 ;i++)
		{
			memcpy(&value, data + CFGID_INPUT_FOVX(base , i+1), 4);
			*(ptr + 0*chid_camera + i*4*chid_camera) = value * (PI / 180) * 1000;
			memcpy(&value, data + CFGID_INPUT_FOVY(base , i+1), 4);
			*(ptr + 1*chid_camera + i*4*chid_camera) = value * (PI / 180) * 1000;
			memcpy(&value, data + CFGID_INPUT_boresightX(base , i+1), 4);
			*(ptr + 2*chid_camera + i*4*chid_camera) = value;
			memcpy(&value, data + CFGID_INPUT_boresightY(base , i+1), 4);
			*(ptr + 3*chid_camera + i*4*chid_camera) = value;
		}
	}	
	return ;
}



void CSensorComp::updateContinueFovParam( int* data , PlatformCtrl_Obj* pObj )
{
	float value ;
	int base;
	float *ptr;
	
	for(int j=0 ; j<chid_camera; j++)
	{
		if(j == 0)
			base = CFGID_INPUT1_BKID ; 
		else
			base = CFGID_INPUT2_BKID + (j-1)*7 ;
		
		ptr = &m_viewParam.level_Fov_continue1[j];
		
		for(int i=0 ; i<13 ;i++)
		{
			memcpy(&value, data + CFGID_INPUT_FOVX(base , i+1), 4);
			*(ptr + 0*chid_camera + i*5*chid_camera) = value * (PI / 180) * 1000;
			memcpy(&value, data + CFGID_INPUT_FOVY(base , i+1), 4);
			*(ptr + 1*chid_camera + i*5*chid_camera) = value * (PI / 180) * 1000;
			memcpy(&value, data + CFGID_INPUT_boresightX(base , i+1), 4);
			*(ptr + 2*chid_camera + i*5*chid_camera) = value;
			memcpy(&value, data + CFGID_INPUT_boresightY(base , i+1), 4);
			*(ptr + 3*chid_camera + i*5*chid_camera) = value;
			memcpy(&value, data + CFGID_INPUT_ZOOMFBS(base , i+1), 4);
			*(ptr + 4*chid_camera + i*5*chid_camera) = value;
		}
	}
	return ;
}



BoresightPos_s CSensorComp::getTheNewestBoresight(int curChid )
{
	if(m_viewParamBK.level_Fov_continue1[curChid] == 0)
	{
		Bpos.x = m_viewParam.Boresightpos_fix_x[curChid];
		Bpos.y = m_viewParam.Boresightpos_fix_y[curChid];		
	}

	float*  ptrx , *ptrxBk, *ptry , *ptryBk;
	ptrx = &m_viewParam.Boresightpos_continue_x1[0];
	ptrxBk = &m_viewParamBK.Boresightpos_continue_x1[0];
	ptry = &m_viewParam.Boresightpos_continue_y1[0];
	ptryBk = &m_viewParamBK.Boresightpos_continue_y1[0];

	for(int i=0 ; i<13;i++)
	{	
		if ( *(ptrx+curChid + i*chid_camera*5) != *(ptrxBk+curChid + i*chid_camera*5) )
			Bpos.x = *(ptrx+curChid + i*chid_camera*5) ; 

		if ( *(ptrx+curChid + i*chid_camera*5) != *(ptrxBk+curChid + i*chid_camera*5) )
			Bpos.y = *(ptry+curChid + i*chid_camera*5) ; 
	}
	
	return Bpos;
}



void CSensorComp::updateParam( int* data , PlatformCtrl_Obj* pObj )
{
	updateFixFovParam( data , pObj );
	updateSwitchFovParam( data , pObj );
	updateContinueFovParam( data , pObj );
	return ;
}


	
BoresightPos_s CSensorComp::updateParam( int* data , PlatformCtrl_Obj* pObj ,int curChid  )
{
	updateParam( data , pObj );
	getTheNewestBoresight(curChid);
	m_viewParamBK = m_viewParam;
	return Bpos;
}



float CSensorComp::ZoomLevelFovCompensation(unsigned short zoom, int chid)
{
	float levelFov;
	float *ptr,*ptr1;

	ptr = &m_viewParam.zoombak1[0];
	ptr1 = ptr + chid_camera*5;

	for(int i=0 ; i< 12 ; i++)
	{
		if( (zoom> *(ptr+i*chid_camera*5))  && (zoom < *(ptr1+i*chid_camera*5)) ) 
			levelFov = linear_interpolation(	*(ptr+i*chid_camera*5) , 
										*(ptr1+i*chid_camera*5) ,
										*(ptr - 4*chid_camera + i*chid_camera*5), 
										*(ptr1 - 4*chid_camera + i*chid_camera*5),
										zoom );

	}
	return levelFov;
}

float CSensorComp::ZoomVerticalFovCompensation(unsigned short zoom, int chid)
{
	float levelFov;
	float *ptr,*ptr1;

	ptr = &m_viewParam.zoombak1[0];
	ptr1 = ptr + chid_camera*5;

	for(int i=0 ; i< 12 ; i++)
	{
		if( (zoom> *(ptr+i*chid_camera*5))  && (zoom < *(ptr1+i*chid_camera*5)) ) 
			levelFov = linear_interpolation(	*(ptr+i*chid_camera*5) , 
										*(ptr1+i*chid_camera*5) ,
										*(ptr - 3*chid_camera + i*chid_camera*5),  
										*(ptr1 - 3*chid_camera + i*chid_camera*5),
										zoom );

	}
	return levelFov;
}



float CSensorComp::linear_interpolation(float x0, float x1, float y0, float y1, float x)
{
	float value, value2;
	value = (y1 - y0) / (x1 - x0);
	value2 = value * (x - x0) + y0;
	return value2;
}



void CSensorComp::SensorComp_CreateParams_Init(SensorComp_CreateParams *pPrm, int i, View* Pserson,int width,int height)
{
	if(pPrm == NULL || Pserson == NULL)
		return;
	memset(pPrm, 0, sizeof(SensorComp_CreateParams));
	
	pPrm->nWidth = width;
	pPrm->nHeight = height;
	pPrm->defaultBoresightPos_x = Pserson->Boresightpos_fix_x[i];
	pPrm->defaultBoresightPos_y = Pserson->Boresightpos_fix_y[i];
	pPrm->fFovMin = 3.0f;
	pPrm->fFovMax = Pserson->level_Fov_fix[i];
	pPrm->fFovY = Pserson->vertical_Fov_fix[i];
	return ;
}



float CSensorComp::dynamicSendBoresightPosX(unsigned short zoom , int chid)
{
	float bPosx;
	
	float* ptr,*ptr1 ;

	ptr = &m_viewParam.zoombak1[0];
	ptr1 = ptr + chid_camera*5;

	for(int i=0 ; i< 12 ; i++)
	{
		if( (zoom> *(ptr+i*chid_camera*5))  && (zoom < *(ptr1+i*chid_camera*5)) ) 
			bPosx = linear_interpolation( *(ptr+i*chid_camera*5) , 
									 *(ptr1+i*chid_camera*5) ,
									 *(ptr - 2*chid_camera + i*chid_camera*5), 
									 *(ptr1 - 2*chid_camera + i*chid_camera*5),
									 zoom );
	}
	return bPosx;
}


float CSensorComp::dynamicSendBoresightPosY(unsigned short zoom , int chid)
{
	float bPosy;
	float* ptr,*ptr1 ;
	ptr = &m_viewParam.zoombak1[0];
	ptr1 = ptr + chid_camera*5;
	float a,b,c,d;
	for(int i=0 ; i< 12 ; i++)
	{
		if( (zoom> *(ptr+i*chid_camera*5))  && (zoom < *(ptr1+i*chid_camera*5)) ) 
			bPosy = linear_interpolation( *(ptr+i*chid_camera*5) , 
									 *(ptr1+i*chid_camera*5) ,
									 *(ptr - 1*chid_camera + i*chid_camera*5),  
									 *(ptr1 - 1*chid_camera + i*chid_camera*5),
									 zoom );
	}
	return bPosy;
}
