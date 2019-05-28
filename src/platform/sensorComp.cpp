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


void CSensorComp::updateFixFovParam( int* data )
{
	float value ;
	int base;
	
	for(int i=0 ; i<chid_camera; i++)
	{
		if(i == 0)
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



void CSensorComp::updateSwitchFovParam( int* data  )
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



void CSensorComp::updateContinueFovParam( int* data )
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
			memcpy(&value, data + CFGID_INPUT_ZOOMFBS(base , i), 4);
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
	ptrx = &m_viewParam.Boresightpos_continue_x1[curChid];
	ptrxBk = &m_viewParamBK.Boresightpos_continue_x1[curChid];
	ptry = &m_viewParam.Boresightpos_continue_y1[curChid];
	ptryBk = &m_viewParamBK.Boresightpos_continue_y1[curChid];

	for(int i=0 ; i<13;i++)
	{	
		if ( *(ptrx+curChid + i*chid_camera*5) != *(ptrxBk+curChid + i*chid_camera*5) )
			Bpos.x = *(ptrx+curChid + i*chid_camera*5) ; 

		if ( *(ptrx+curChid + i*chid_camera*5) != *(ptrxBk+curChid + i*chid_camera*5) )
			Bpos.y = *(ptry+curChid + i*chid_camera*5) ; 
	}
	
	return Bpos;
}



void CSensorComp::updateFov( int* data ,PlatformCtrl_Obj* pObj ,int zoom , int inchId)
{
	int level ;
	float *pfovx, *pfovy;
	int base , chid;

	if(-1 == inchId){
		 base = getBaseAddress(data);
		 chid =  data[CFGID_RTS_mainch];
	}else{
		if(0 == inchId)
			base = CFGID_INPUT1_BKID;
		else
			base = CFGID_INPUT2_BKID + (inchId-1)*7;
		 chid = inchId;
	}

 	switch(data[CFGID_INPUT_FOVTYPE(base)])
 	{
		case 0:
			updateFixFovParam( data );
			break;
		case 1:
			updateSwitchFovParam( data );
			break;
		case 2:
			updateContinueFovParam( data );
			break;
		default:
			break;
 	}

	if(pObj != NULL)
	{
 		switch(data[CFGID_INPUT_FOVTYPE(base)])
		{
			case 0:
				pObj->privates.fovX = m_viewParam.level_Fov_fix[chid];
				pObj->privates.fovY = m_viewParam.vertical_Fov_fix[chid];
				break;
			case 1:
				level = data[CFGID_INPUT_FOVCLASS(base)];
				if(level < 0)
					level =0;
				else if(level >4)
					level = 4;
				pfovx = &m_viewParam.level_Fov_switch1[chid];
				pfovy = &m_viewParam.vertical_Fov_switch1[chid];
				pfovx += level*chid_camera*4;
				pfovy += level*chid_camera*4;	
				pObj->privates.fovX =  *pfovx;
				pObj->privates.fovY =  *pfovy;
				break;
			case 2:
				pObj->privates.fovX = ZoomLevelFovCompensation(zoom, chid);
				pObj->privates.fovY = ZoomVerticalFovCompensation(zoom, chid);
				break;
			default:
				break;
		}

		printf(" cur id   :%d    , fov x,y = (%f , %f )  \n" , chid , pObj->privates.fovX  ,pObj->privates.fovY);

	}

	return ;
}

	
BoresightPos_s CSensorComp::updateParam( int* data , PlatformCtrl_Obj* pObj ,int curChid  )
{
	updateFov( data );
	getTheNewestBoresight(curChid);
	m_viewParamBK = m_viewParam;
	return Bpos;
}


float CSensorComp::ZoomLevelFovCompensation(unsigned short zoom, int chid)
{
	float levelFov;
	float *ptr,*ptr1;

	ptr = &m_viewParam.zoombak1[chid];
	ptr1 = ptr + chid_camera*5;

	if(zoom !=0 )
		for(int i=0 ; i< 12 ; i++)
		{
			if( (zoom >= *(ptr+i*chid_camera*5))  && (zoom <= *(ptr1+i*chid_camera*5)) ) 
			{
				levelFov = linear_interpolation(	*(ptr+i*chid_camera*5) , 
											*(ptr1+i*chid_camera*5) ,
											*(ptr - 4*chid_camera + i*chid_camera*5), 
											*(ptr1 - 4*chid_camera + i*chid_camera*5),
											zoom );
				break;
			}
		}
	else
		levelFov = m_viewParam.level_Fov_continue1[chid];
	return levelFov;
}


float CSensorComp::ZoomVerticalFovCompensation(unsigned short zoom, int chid)
{
	float verticalFov;
	float *ptr,*ptr1;

	ptr = &m_viewParam.zoombak1[chid];
	ptr1 = ptr + chid_camera*5;

	if(zoom != 0)
		for(int i=0 ; i< 12 ; i++)
		{
			if( (zoom >= *(ptr+i*chid_camera*5))  && (zoom <= *(ptr1+i*chid_camera*5)) )
			{
				verticalFov = linear_interpolation(	*(ptr+i*chid_camera*5) , 
											*(ptr1+i*chid_camera*5) ,
											*(ptr - 3*chid_camera + i*chid_camera*5),  
											*(ptr1 - 3*chid_camera + i*chid_camera*5),
											zoom );
				break;
			}
		}	
	else
		verticalFov = m_viewParam.vertical_Fov_continue1[chid];

	return verticalFov;
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
	return ;
}



float CSensorComp::dynamicSendBoresightPosX(unsigned short zoom , int chid)
{
	float bPosx;
	
	float* ptr,*ptr1 ;
	ptr = &m_viewParam.zoombak1[chid];
	ptr1 = &m_viewParam.zoombak2[chid];
	if(zoom != 0)
		for(int i=0 ; i< 12 ; i++)
		{
			if( (zoom>= *(ptr+i*chid_camera*5))  && (zoom <= *(ptr1+i*chid_camera*5)) ) 
				bPosx = linear_interpolation( *(ptr+i*chid_camera*5) , 
										 *(ptr1+i*chid_camera*5) ,
										 *(ptr - 2*chid_camera + i*chid_camera*5), 
										 *(ptr1 - 2*chid_camera + i*chid_camera*5),
										 zoom );
		}
	else
		bPosx = (float)m_viewParam.Boresightpos_continue_x1[chid];
	return bPosx;
}


float CSensorComp::dynamicSendBoresightPosY(unsigned short zoom , int chid)
{
	float bPosy;
	float* ptr,*ptr1 ;
	ptr = &m_viewParam.zoombak1[chid];
	ptr1 = ptr + chid_camera*5;
	float a,b,c,d;

	if(zoom != 0)
		for(int i=0 ; i< 12 ; i++)
		{
			if( (zoom >= *(ptr+i*chid_camera*5))  && (zoom <= *(ptr1+i*chid_camera*5)) ) 
				bPosy = linear_interpolation( *(ptr+i*chid_camera*5) , 
										 *(ptr1+i*chid_camera*5) ,
										 *(ptr - 1*chid_camera + i*chid_camera*5),  
										 *(ptr1 - 1*chid_camera + i*chid_camera*5),
										 zoom );
		}
	else
		bPosy = (float)m_viewParam.Boresightpos_continue_y1[chid];
	return bPosy;
}


int CSensorComp::getBaseAddress(int* data , int* getId)
{
	int base;
	switch(data[CFGID_RTS_mainch])
	{
		case 0:
			base = CFGID_INPUT1_BKID;
			break;
		case 1:
			base = CFGID_INPUT2_BKID;
			break;
		case 2:
			base = CFGID_INPUT3_BKID;
			break;
		case 3:
			base = CFGID_INPUT4_BKID;
			break;
		case 4:
			base = CFGID_INPUT5_BKID;
			break;
		default:
			base = CFGID_INPUT2_BKID;
			break;
	}

	if(getId != NULL)
		*getId = data[CFGID_RTS_mainch];
	
	return base;
}


void CSensorComp::getCameraResolution(int* data)
{
	float tmpx,tmpy;
	int base;
	for(int i=0; i<chid_camera ; i++)
	{
		if(0 == i)
			base = CFGID_INPUT1_BKID;
		else
		 	base = CFGID_INPUT2_BKID + (i-1)*7;

		switch(data[CFGID_INPUT_CHRES(base)])
		{
			case 0:
			case 1:
			case 5:
				data[CFGID_INPUT_RESX(base)] = 1920;
				data[CFGID_INPUT_RESY(base)] = 1080;
				m_viewParam.width[i]  = 1920;
				m_viewParam.height[i] = 1080;
				break;
			case 2:
			case 3:
				data[CFGID_INPUT_RESX(base)] = 1280;
				data[CFGID_INPUT_RESY(base)] = 720;
				m_viewParam.width[i]  = 1280;
				m_viewParam.height[i] = 720;
				break;
			case 6:
				data[CFGID_INPUT_RESX(base)] = 720;
				data[CFGID_INPUT_RESY(base)] = 576;
				m_viewParam.width[i]  = 720;
				m_viewParam.height[i] = 576;
				break;
			default:
				data[CFGID_INPUT_RESX(base)] = 1920;
				data[CFGID_INPUT_RESY(base)] = 1080;
				m_viewParam.width[i]  = 1920;
				m_viewParam.height[i] = 1080;
				break;
		}	
	}
	return ;
}


BoresightPos_s CSensorComp::calcBoresightContinue(int zoom , int chid)
{
	BoresightPos_s ret;	
	ret.x = dynamicSendBoresightPosX(zoom , chid);
	ret.y = dynamicSendBoresightPosY(zoom , chid);
	return ret;
}

	
BoresightPos_s CSensorComp::getBoresight(int* data , int zoom,int chid)
{	
	BoresightPos_s tmp;
	int tmpdata,base,id;
	int level;
	if(-1 == chid)
		 base = getBaseAddress(data , &id);
	else{
		if(0 == chid)
			base = CFGID_INPUT1_BKID;
		else
			base = CFGID_INPUT2_BKID + (chid-1)*7;
		id = chid;
	}

	switch(data[CFGID_INPUT_FOVTYPE(base)])
	{
		case 0:
			memcpy(&tmp.x, &data[CFGID_INPUT_boresightX(base,1)] , 4);
			memcpy(&tmp.y, &data[CFGID_INPUT_boresightY(base,1)] , 4);
			break;
		case 1:
			level = data[CFGID_INPUT_FOVCLASS(base)];
			if(level < 0)
				level =0;
			else if(level >4)
				level = 4;	
			memcpy(&tmp.x, &data[CFGID_INPUT_boresightX(base, level+1)] , 4);
			memcpy(&tmp.y, &data[CFGID_INPUT_boresightY(base, level+1 )] , 4);	
			break;
		case 2:
			tmp = calcBoresightContinue(zoom , id);
			break;
		default:
			break;
	}	
	tmpdata = (int)tmp.x;	
	memcpy(&data[CFGID_INPUT_CURBOREX(base)] ,&tmpdata,  4);
	tmpdata = (int)tmp.y;	
	memcpy(&data[CFGID_INPUT_CURBOREY(base)] ,&tmpdata,  4);
	return tmp;
}


void CSensorComp::updateWH(int* data ,PlatformCtrl_Obj* pObj , int chid)
{
	int id;
	if( -1 == chid)
		id = data[CFGID_RTS_mainch];
	else
		id = chid;
	
	pObj->privates.width = m_viewParam.width[id];
	pObj->privates.height = m_viewParam.height[id];
	return ;
}

