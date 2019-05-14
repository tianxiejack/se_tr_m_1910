#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "platformFilter.h"
#include "platformparam.h"

#define EPSINON   (0.000001f)


int CPlatformFilter::PlatformFilter_GetIndex(HPLTFILTER hFilter)
{
    PlatformFilter_Obj *pObj = (PlatformFilter_Obj*)hFilter;

    if(pObj == NULL)
        return -1;

    return pObj->params.iIndex;
}

HPLTFILTER CPlatformFilter::PlatformFilter_Create(PlatformFilter_CreateParams *pPrm)
{
    PlatformFilter_Obj *pObj;

    if(pPrm == NULL)
        return NULL;

    pObj = (PlatformFilter_Obj *)malloc(sizeof(PlatformFilter_Obj));

    if(pObj == NULL)
        return NULL;

    memset(pObj, 0, sizeof(PlatformFilter_Obj));

    memcpy(&pObj->params, pPrm, sizeof(PlatformFilter_CreateParams));

    return pObj;
}

void CPlatformFilter::PlatformFilter_Delete(HPLTFILTER hFilter)
{
    PlatformFilter_Obj *pObj = (PlatformFilter_Obj*)hFilter;

    if(pObj == NULL)
        return ;

    free(pObj);
}

void CPlatformFilter::PlatformFilter_Reset(HPLTFILTER hFilter)
{
	PlatformFilter_Obj *pObj = (PlatformFilter_Obj*)hFilter;
	if(pObj == NULL)
		return ;

	memset(&pObj->privates, 0, sizeof(pObj->privates));
	return ;
}



void CPlatformFilter::PlatformFilter_updatePidParams(PlatformFilter_CreateParams *pPrm,PlatformFilter_CreateParams *pPrm2,vector<float> m_Prm)
{
	if(pPrm == NULL || pPrm2 == NULL)
		return;
	int i = 1441;
	if(pPrm->Kp  != m_Prm[i])
		pPrm->Kp  = m_Prm[i];
	if(pPrm->Ki != m_Prm[++i])
		pPrm->Ki = m_Prm[i];
	if(pPrm->Kd != m_Prm[++i])
		pPrm->Kd = m_Prm[i];
	if(pPrm->K != m_Prm[++i])
		pPrm->K = m_Prm[i];

	if(pPrm2->Kp  != m_Prm[++i])
		pPrm2->Kp  = m_Prm[i];
	if(pPrm2->Ki != m_Prm[++i])
		pPrm2->Ki = m_Prm[i];
	if(pPrm2->Kd != m_Prm[++i])
		pPrm2->Kd = m_Prm[i];
	if(pPrm2->K != m_Prm[++i])
		pPrm2->K = m_Prm[i];
}


int CPlatformFilter::PlatformFilter_reset4trk(HPLTFILTER hFilter)
{
	int ret = -1;
	PlatformFilter_Obj *pObj = (PlatformFilter_Obj*)hFilter;

	if(pObj == NULL)
		return ret;

	pObj->privates.Xc[2] = pObj->privates.Xc[1] = pObj->privates.Xc[0] = 0;
	pObj->privates.Yc[0] = pObj->privates.Yc[1] = pObj->privates.Yc_out;
	pObj->privates.Uc[1] = 0; 

	pObj->params.stable = 0;
	pObj->params.count= 0;
	
	return 0;	
}



void CPlatformFilter::calcCurPlatAvrRate(HPLTFILTER hFilter , float curRate)
{
	PlatformFilter_Obj *pObj = (PlatformFilter_Obj*)hFilter;

	pObj->privates.Ycbuf[7] = pObj->privates.Ycbuf[6];
	pObj->privates.Ycbuf[6] = pObj->privates.Ycbuf[5];
	pObj->privates.Ycbuf[5] = pObj->privates.Ycbuf[4];
	pObj->privates.Ycbuf[4] = pObj->privates.Ycbuf[3];
	pObj->privates.Ycbuf[3] = pObj->privates.Ycbuf[2];
	pObj->privates.Ycbuf[2] = pObj->privates.Ycbuf[1];
	pObj->privates.Ycbuf[1] = pObj->privates.Ycbuf[0];
	pObj->privates.Ycbuf[0] = curRate;

	pObj->privates.Yc_out = (pObj->privates.Ycbuf[0] + pObj->privates.Ycbuf[1] + pObj->privates.Ycbuf[2] + pObj->privates.Ycbuf[3] + \
		pObj->privates.Ycbuf[4] + pObj->privates.Ycbuf[5] + pObj->privates.Ycbuf[6] + pObj->privates.Ycbuf[7]) / 8;

	return ;
}



float CPlatformFilter::pidAlg(HPLTFILTER hFilter, float curXc)
{

	float ret = 0.0;
	float Kp = 0.0;
	float Ki = 0.0;
	float Kd = 0.0;
	float K = 0.0;
	int indexKi =1;
	int indexKd = 1;
 	PlatformFilter_Obj *pObj = (PlatformFilter_Obj*)hFilter;

	if(pObj == NULL)
		return ret;

	Kp = pObj->params.Kp;
	Ki = pObj->params.Ki;
	Kd = pObj->params.Kd;
	K = pObj->params.K;

	if((pObj->privates.Xc[2] ==0) && (pObj->privates.Xc[1]==0))
	{
		if(fabs(pObj->privates.Xc[0] - curXc) > 1)
		{
			pObj->privates.Xc[2] = pObj->privates.Xc[1];
			pObj->privates.Xc[1] = pObj->privates.Xc[0];
		}
		pObj->privates.Xc[0] = curXc;
	}else{
		pObj->privates.Xc[2] = pObj->privates.Xc[1];
		pObj->privates.Xc[1] = pObj->privates.Xc[0];
		pObj->privates.Xc[0] = curXc;
	}

 	if(!pObj->params.stable)
	{
		if((pObj->privates.Xc[2] ==0) && (pObj->privates.Xc[1]==0)){
			indexKi = 0;
			indexKd = 0;
		} 
		else {
			if((fabs(pObj->privates.Xc[0] - pObj->privates.Xc[1]) < 1)) {
				pObj->privates.Uc[1] += pObj->privates.Xc[0];
			}

			if(fabs(pObj->privates.Xc[0]) <= 5 ) {
				pObj->params.stable = true;
			}

			if(fabs(pObj->privates.Xc[0]) > 5 )
				pObj->params.count++;
			
			if(pObj->params.count > 50)
				pObj->params.stable = true;
		}
	}
	else
		pObj->privates.Uc[1] += pObj->privates.Xc[0];

	
	pObj->privates.Yc[1] = pObj->privates.Yc[0];

	pObj->privates.Uc[0] =  Kp * pObj->privates.Xc[0]
	                                      + indexKi*Ki *  pObj->privates.Uc[1]
	                                      + indexKd*Kd * (pObj->privates.Xc[0] - pObj->privates.Xc[1]);

	ret = K * pObj->privates.Yc[1] + (1-K) * pObj->privates.Uc[0];

	pObj->privates.Yc[0] = ret;

	if(fabs(ret) > 1200){
		if(ret > 0)
			ret = 1200 ;
		else
			ret = -1200;
		pObj->privates.Yc[0] = ret ;	
	}
 
	return ret;
}
