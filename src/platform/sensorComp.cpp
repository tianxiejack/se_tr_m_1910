#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "sensorComp.h"

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

	SensorComp_BoresightPosTab *pBoresightPosRunTab;

	SensorComp_BoresightPosTab *pBoresightPosCorrTab;

	SENSORCOMP_MODE curMode;

	SensorComp_BoresightPosTab *pCurTab;

	int iCurPos;

	float fCurFov;

	int iCurX, iCurY;

}SensorComp_Obj;



BoresightPos_s CSensorComp::UpdateParams(vector<float> data, int block, int field, int curChid, PlatformCtrl_Obj* pObj)
{
	int check, chid;
	float value;
	switch(block)
	{
		case 24:case 25:case 26:case 27:case 56:case 57:case 111:case 112: chid = 0; break;
		case 59:case 60:case 61:case 62:case 63:case 64:case 113:case 114: chid = 1; break;
		case 66:case 67:case 68:case 69:case 70:case 71:case 115:case 116: chid = 2; break;
		case 73:case 74:case 75:case 76:case 77:case 78:case 117:case 118: chid = 3; break;
		case 80:case 81:case 82:case 83:case 84:case 85:case 119:case 120: chid = 4; break;
	}
	/* 固定视场*/
	if(block == 24 || block == 59 || block == 66 || block == 73 || block == 80)
	{
		check = (block-1)*16;
		value =data[check];
		value = value * (PI / 180) * 1000;
		m_viewParam.level_Fov_fix[chid]= value;
		value =data[++check];
		value = value * (PI / 180) * 1000;
		m_viewParam.vertical_Fov_fix[chid]=value;
		m_viewParam.Boresightpos_fix_x[chid]=(int)data[++check];
		m_viewParam.Boresightpos_fix_y[chid]=(int)data[++check];

		if(pObj != NULL && curChid == chid)
		{
			pObj->privates.fovX = m_viewParam.level_Fov_fix[chid];
			pObj->privates.fovY = m_viewParam.vertical_Fov_fix[chid];
			pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_fix_x[chid];
			pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_fix_y[chid];
		}

	}
	/*可切换视场*/
	if(block == 25 || block == 60 || block == 67 || block == 74 || block == 81)
	{
		check = (block-1)*16;
		value=data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_switch1[chid]= value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_switch1[chid]=value;
		m_viewParam.Boresightpos_switch_x1[chid]=(int)data[++check];
		m_viewParam.Boresightpos_switch_y1[chid]=(int)data[++check];

		value=data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_switch2[chid]=value;
		value=data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_switch2[chid]=value;
		m_viewParam.Boresightpos_switch_x2[chid]=(int)data[++check];
		m_viewParam.Boresightpos_switch_y2[chid]=(int)data[++check];

		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_switch3[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_switch3[chid]=value;
		m_viewParam.Boresightpos_switch_x3[chid]=(int)data[++check];
		m_viewParam.Boresightpos_switch_y3[chid]=(int)data[++check];

		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_switch4[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_switch4[chid]=value;
		m_viewParam.Boresightpos_switch_x4[chid]=(int)data[++check];
		UpdateSwitchFovParams_one(chid, field, curChid, pObj);
	}

	if(block == 26 || block == 61 || block == 68 || block == 75 || block == 82)
	{
		check = (block-1)*16;
		m_viewParam.Boresightpos_switch_y4[chid]=(int)data[check];
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_switch5[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_switch5[chid]=value;
		m_viewParam.Boresightpos_switch_x5[chid]=(int)data[++check];
		m_viewParam.Boresightpos_switch_y5[chid]=(int)data[++check];
		UpdateSwitchFovParams_two(chid, field, curChid, pObj);
	}

	/*连续视场*/
	if(block == 27 || block == 62 || block == 69 || block == 76 || block == 83)
	{
		check = (block-1)*16;
		m_viewParam.zoombak1[chid]=data[check];
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue1[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue1[chid]=value;
		m_viewParam.Boresightpos_continue_x1[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y1[chid]=(int)data[++check];

		m_viewParam.zoombak2[chid]=data[++check];
		value=data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue2[chid]=value;
		value=data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue2[chid]=value;
		m_viewParam.Boresightpos_continue_x2[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y2[chid]=(int)data[++check];

		m_viewParam.zoombak3[chid]=data[++check];
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue3[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue3[chid]=value;
		m_viewParam.Boresightpos_continue_x3[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y3[chid]=(int)data[++check];

		m_viewParam.zoombak4[chid]=data[++check];
		UpdateContinueFovParams_one(chid, field, curChid, pObj);
	}
	if(block == 56 || block == 63 || block == 70 || block == 77 || block == 84)
	{
		check = (block-1)*16;
		value =data[check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue4[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue4[chid]=value;
		m_viewParam.Boresightpos_continue_x4[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y4[chid]=(int)data[++check];

		m_viewParam.zoombak5[chid]=data[++check];
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue5[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue5[chid]=value;
		m_viewParam.Boresightpos_continue_x5[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y5[chid]=(int)data[++check];

		m_viewParam.zoombak6[chid]=data[++check];
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue6[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue6[chid]=value;
		m_viewParam.Boresightpos_continue_x6[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y6[chid]=(int)data[++check];

		m_viewParam.zoombak7[chid]=data[++check];
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue7[chid]=value;
		UpdateContinueFovParams_two(chid, field, curChid, pObj);
	}
	if(block == 57 || block == 64 || block == 71 || block == 78 || block == 85)
	{
		check = (block-1)*16;
		value =data[check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue7[chid]=value;
		m_viewParam.Boresightpos_continue_x7[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y7[chid]=(int)data[++check];
		UpdateContinueFovParams_three(chid, field, curChid, pObj);
	}

	if(block == 111 || block == 113 || block == 115 || block == 117 || block == 119)
	{
		check = (block-1)*16;
		m_viewParam.zoombak8[chid]=data[check];
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue8[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue8[chid]=value;
		m_viewParam.Boresightpos_continue_x8[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y8[chid]=(int)data[++check];

		m_viewParam.zoombak9[chid]=data[++check];
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue9[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue9[chid]=value;
		m_viewParam.Boresightpos_continue_x9[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y9[chid]=(int)data[++check];

		m_viewParam.zoombak10[chid]=data[++check];
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue10[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue10[chid]=value;
		m_viewParam.Boresightpos_continue_x10[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y10[chid]=(int)data[++check];

		m_viewParam.zoombak11[chid]=data[++check];
		UpdateContinueFovParams_four(chid, field, curChid, pObj);
	}
	if(block == 112 || block == 114 || block == 116 || block == 118 || block == 120)
	{
		check = (block-1)*16;
		value =data[check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue11[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue11[chid]=value;
		m_viewParam.Boresightpos_continue_x11[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y11[chid]=(int)data[++check];

		m_viewParam.zoombak12[chid]=data[++check];
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue12[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue12[chid]=value;
		m_viewParam. Boresightpos_continue_x12[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y12[chid]=(int)data[++check];

		m_viewParam.zoombak13[chid]=data[++check];
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.level_Fov_continue13[chid]=value;
		value =data[++check];
		value=value*(PI / 180) * 1000;
		m_viewParam.vertical_Fov_continue13[chid]=value;
		m_viewParam.Boresightpos_continue_x13[chid]=(int)data[++check];
		m_viewParam.Boresightpos_continue_y13[chid]=(int)data[++check];
		UpdateContinueFovParams_five(chid, field, curChid, pObj);
	}
	if(pObj != NULL)
	{
		Bpos.x = pObj->params.sensorParams[1].defaultBoresightPos_x;
		Bpos.y = pObj->params.sensorParams[1].defaultBoresightPos_y;
	}
	return Bpos;
}

void CSensorComp::UpdateSwitchFovParams_one(int chid, int field, int curChid, PlatformCtrl_Obj* pObj)
{
	if(pObj != NULL)
	{
		if(curChid != chid)
			return ;
		switch(field)
		{
			case 1:pObj->privates.fovX = m_viewParam.level_Fov_switch1[chid];break;
			case 2:pObj->privates.fovY = m_viewParam.vertical_Fov_switch1[chid];break;
			case 3:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_switch_x1[chid];break;
			case 4:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_switch_y1[chid];break;
			case 5:pObj->privates.fovX = m_viewParam.level_Fov_switch2[chid];break;
			case 6:pObj->privates.fovY = m_viewParam.vertical_Fov_switch2[chid];break;
			case 7:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_switch_x2[chid];break;
			case 8:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_switch_y2[chid];break;
			case 9:pObj->privates.fovX = m_viewParam.level_Fov_switch3[chid];break;
			case 10:pObj->privates.fovY = m_viewParam.vertical_Fov_switch3[chid];break;
			case 11:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_switch_x3[chid];break;
			case 12:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_switch_y3[chid];break;
			case 13:pObj->privates.fovX = m_viewParam.level_Fov_switch4[chid];break;
			case 14:pObj->privates.fovY = m_viewParam.vertical_Fov_switch4[chid];break;
			case 15:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_switch_x4[chid];break;
		}
	}
	
	return ;
}

void CSensorComp::UpdateSwitchFovParams_two(int chid, int field, int curChid, PlatformCtrl_Obj* pObj)
{
	if(pObj != NULL)
	{
		if(curChid != chid)
			return ;
		switch(field)
		{
			case 0:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_switch_y4[chid];break;
			case 1:pObj->privates.fovX = m_viewParam.level_Fov_switch5[chid];break;
			case 2:pObj->privates.fovY = m_viewParam.vertical_Fov_switch5[chid];break;
			case 3:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_switch_x5[chid];break;
			case 4:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_switch_y5[chid];break;
		}
	}
}

void CSensorComp::UpdateContinueFovParams_one(int chid, int field, int curChid, PlatformCtrl_Obj* pObj)
{
	if(pObj != NULL)
	{
		if(curChid != chid)
			return;
		switch(field)
		{
		case 1:pObj->privates.fovX = m_viewParam.level_Fov_continue1[chid];break;
		case 2:pObj->privates.fovY = m_viewParam.vertical_Fov_continue1[chid];break;
		case 3:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x1[chid];break;
		case 4:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y1[chid];break;

		case 6:pObj->privates.fovX = m_viewParam.level_Fov_continue2[chid];break;
		case 7:pObj->privates.fovY = m_viewParam.vertical_Fov_continue2[chid];break;
		case 8:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x2[chid];break;
		case 9:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y2[chid];break;

		case 11:pObj->privates.fovX = m_viewParam.level_Fov_continue3[chid];break;
		case 12:pObj->privates.fovY = m_viewParam.vertical_Fov_continue3[chid];break;
		case 13:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x3[chid];break;
		case 14:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y3[chid];break;
		}
	}
}

void CSensorComp::UpdateContinueFovParams_two(int chid, int field, int curChid, PlatformCtrl_Obj* pObj)
{
	if(pObj != NULL)
	{
		if(curChid != chid)
			return;
		switch(field)
		{
		case 0:pObj->privates.fovX = m_viewParam.level_Fov_continue4[chid];break;
		case 1:pObj->privates.fovY = m_viewParam.vertical_Fov_continue4[chid];break;
		case 2:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x4[chid];break;
		case 3:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y4[chid];break;

		case 5:pObj->privates.fovX = m_viewParam.level_Fov_continue5[chid];break;
		case 6:pObj->privates.fovY = m_viewParam.vertical_Fov_continue5[chid];break;
		case 7:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x5[chid];break;
		case 8:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y5[chid];break;

		case 10:pObj->privates.fovX = m_viewParam.level_Fov_continue6[chid];break;
		case 11:pObj->privates.fovY = m_viewParam.vertical_Fov_continue6[chid];break;
		case 12:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x6[chid];break;
		case 13:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y6[chid];break;
		case 15:pObj->privates.fovX = m_viewParam.level_Fov_continue7[chid];break;
		}
	}
}
void CSensorComp::UpdateContinueFovParams_three(int chid, int field, int curChid, PlatformCtrl_Obj* pObj)
{
	if(pObj != NULL)
	{
		if(curChid != chid)
			return;
		switch(field)
		{
		case 0:pObj->privates.fovY = m_viewParam.vertical_Fov_continue7[chid];break;
		case 1:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x7[chid];break;
		case 2:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y7[chid];break;
		}
	}
}
void CSensorComp::UpdateContinueFovParams_four(int chid, int field, int curChid, PlatformCtrl_Obj* pObj)
{
	if(pObj != NULL)
	{
		if(curChid != chid)
			return;
		switch(field)
		{
		case 1:pObj->privates.fovX = m_viewParam.level_Fov_continue8[chid];break;
		case 2:pObj->privates.fovY = m_viewParam.vertical_Fov_continue8[chid];break;
		case 3:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x8[chid];break;
		case 4:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y8[chid];break;

		case 6:pObj->privates.fovX = m_viewParam.level_Fov_continue9[chid];break;
		case 7:pObj->privates.fovY = m_viewParam.vertical_Fov_continue9[chid];break;
		case 8:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x9[chid];break;
		case 9:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y9[chid];break;

		case 11:pObj->privates.fovX = m_viewParam.level_Fov_continue10[chid];break;
		case 12:pObj->privates.fovY = m_viewParam.vertical_Fov_continue10[chid];break;
		case 13:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x10[chid];break;
		case 14:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y10[chid];break;
		}
	}
}
void CSensorComp::UpdateContinueFovParams_five(int chid, int field, int curChid, PlatformCtrl_Obj* pObj)
{
	if(pObj != NULL)
	{
		if(curChid != chid)
			return;
		switch(field)
		{
		case 0:pObj->privates.fovX = m_viewParam.level_Fov_continue11[chid];break;
		case 1:pObj->privates.fovY = m_viewParam.vertical_Fov_continue11[chid];break;
		case 2:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x11[chid];break;
		case 3:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y11[chid];break;

		case 5:pObj->privates.fovX = m_viewParam.level_Fov_continue12[chid];break;
		case 6:pObj->privates.fovY = m_viewParam.vertical_Fov_continue12[chid];break;
		case 7:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x12[chid];break;
		case 8:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y12[chid];break;

		case 10:pObj->privates.fovX = m_viewParam.level_Fov_continue13[chid];break;
		case 11:pObj->privates.fovY = m_viewParam.vertical_Fov_continue13[chid];break;
		case 12:pObj->params.sensorParams[1].defaultBoresightPos_x = m_viewParam.Boresightpos_continue_x13[chid];break;
		case 13:pObj->params.sensorParams[1].defaultBoresightPos_y = m_viewParam.Boresightpos_continue_y13[chid];break;
		}
	}
}

float CSensorComp::ZoomLevelFovCompensation(unsigned short zoom, int chid)
{
	float levelFov, value, value2;
	View* ps;
	ps = &m_viewParam;
	if(zoom >= ps->zoombak1[chid] && zoom <= ps->zoombak2[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak1[chid], ps->zoombak2[chid], ps->level_Fov_continue1[chid],\
				ps->level_Fov_continue2[chid], zoom);
	}
	else if(zoom > ps->zoombak2[chid] && zoom <= ps->zoombak3[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak2[chid], ps->zoombak3[chid], ps->level_Fov_continue2[chid],\
				ps->level_Fov_continue3[chid], zoom);
	}
	else if(zoom > ps->zoombak3[chid] && zoom <= ps->zoombak4[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak3[chid], ps->zoombak4[chid], ps->level_Fov_continue3[chid],\
				ps->level_Fov_continue4[chid], zoom);
	}
	else if(zoom > ps->zoombak4[chid] && zoom <= ps->zoombak5[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak4[chid], ps->zoombak5[chid], ps->level_Fov_continue4[chid],\
				ps->level_Fov_continue5[chid], zoom);
	}
	else if(zoom > ps->zoombak5[chid] && zoom <= ps->zoombak6[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak5[chid], ps->zoombak6[chid], ps->level_Fov_continue5[chid],\
				ps->level_Fov_continue6[chid], zoom);
	}
	else if(zoom > ps->zoombak6[chid] && zoom <= ps->zoombak7[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak6[chid], ps->zoombak7[chid], ps->level_Fov_continue6[chid],\
				ps->level_Fov_continue7[chid], zoom);
	}
	//
	else if(zoom > ps->zoombak7[chid] && zoom <= ps->zoombak8[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak7[chid], ps->zoombak8[chid], ps->level_Fov_continue7[chid],\
				ps->level_Fov_continue8[chid], zoom);
	}
	else if(zoom > ps->zoombak8[chid] && zoom <= ps->zoombak9[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak8[chid], ps->zoombak9[chid], ps->level_Fov_continue8[chid],\
				ps->level_Fov_continue9[chid], zoom);
	}
	else if(zoom > ps->zoombak9[chid] && zoom <= ps->zoombak10[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak9[chid], ps->zoombak10[chid], ps->level_Fov_continue9[chid],\
				ps->level_Fov_continue10[chid], zoom);
	}
	else if(zoom > ps->zoombak10[chid] && zoom <= ps->zoombak11[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak10[chid], ps->zoombak11[chid], ps->level_Fov_continue10[chid],\
				ps->level_Fov_continue11[chid], zoom);
	}
	else if(zoom > ps->zoombak11[chid] && zoom <= ps->zoombak12[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak11[chid], ps->zoombak12[chid], ps->level_Fov_continue11[chid],\
				ps->level_Fov_continue12[chid], zoom);
	}
	else if(zoom > ps->zoombak12[chid] && zoom <= ps->zoombak13[chid])
	{
		levelFov = linear_interpolation_Fov(ps->zoombak12[chid], ps->zoombak13[chid], ps->level_Fov_continue12[chid],\
				ps->level_Fov_continue13[chid], zoom);
	}

	return levelFov;
}

float CSensorComp::ZoomVerticalFovCompensation(unsigned short zoom, int chid)
{
	float verticalFov, value, value2;
	View* ps;
	ps = &m_viewParam;
	if(zoom >= ps->zoombak1[chid] && zoom <= ps->zoombak2[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak1[chid], ps->zoombak2[chid], ps->vertical_Fov_continue1[chid],\
				ps->vertical_Fov_continue2[chid], zoom);
	}
	else if(zoom > ps->zoombak2[chid] && zoom <= ps->zoombak3[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak2[chid], ps->zoombak3[chid], ps->vertical_Fov_continue2[chid],\
				ps->vertical_Fov_continue3[chid], zoom);
	}
	else if(zoom > ps->zoombak3[chid] && zoom <= ps->zoombak4[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak3[chid], ps->zoombak4[chid], ps->vertical_Fov_continue3[chid],\
				ps->vertical_Fov_continue4[chid], zoom);
	}
	else if(zoom > ps->zoombak4[chid] && zoom <= ps->zoombak5[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak4[chid], ps->zoombak5[chid], ps->vertical_Fov_continue4[chid],\
				ps->vertical_Fov_continue5[chid], zoom);
	}
	else if(zoom > ps->zoombak5[chid] && zoom <= ps->zoombak6[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak5[chid], ps->zoombak6[chid], ps->vertical_Fov_continue5[chid],\
				ps->vertical_Fov_continue6[chid], zoom);
	}
	else if(zoom > ps->zoombak6[chid] && zoom <= ps->zoombak7[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak6[chid], ps->zoombak7[chid], ps->vertical_Fov_continue6[chid],\
				ps->vertical_Fov_continue7[chid], zoom);
	}
	//
	else if(zoom > ps->zoombak7[chid] && zoom <= ps->zoombak8[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak7[chid], ps->zoombak8[chid], ps->vertical_Fov_continue7[chid],\
				ps->vertical_Fov_continue8[chid], zoom);
	}
	else if(zoom > ps->zoombak8[chid] && zoom <= ps->zoombak9[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak8[chid], ps->zoombak9[chid], ps->vertical_Fov_continue8[chid],\
				ps->vertical_Fov_continue9[chid], zoom);
	}
	else if(zoom > ps->zoombak9[chid] && zoom <= ps->zoombak10[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak9[chid], ps->zoombak10[chid], ps->vertical_Fov_continue9[chid],\
				ps->vertical_Fov_continue10[chid], zoom);
	}
	else if(zoom > ps->zoombak10[chid] && zoom <= ps->zoombak11[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak10[chid], ps->zoombak11[chid], ps->vertical_Fov_continue10[chid],\
				ps->vertical_Fov_continue11[chid], zoom);
	}
	else if(zoom > ps->zoombak11[chid] && zoom <= ps->zoombak12[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak11[chid], ps->zoombak12[chid], ps->vertical_Fov_continue11[chid],\
				ps->vertical_Fov_continue12[chid], zoom);
	}
	else if(zoom > ps->zoombak12[chid] && zoom <= ps->zoombak13[chid])
	{
		verticalFov = linear_interpolation_Fov(ps->zoombak12[chid], ps->zoombak13[chid], ps->vertical_Fov_continue12[chid],\
				ps->vertical_Fov_continue13[chid], zoom);
	}

	return verticalFov;
}

float CSensorComp::linear_interpolation_Fov(float x0, float x1, float y0, float y1, float x)
{
	float value, value2;
	value = (y1 - y0) / (x1 - x0);
	value2 = value * (x - x0) + y0;
	return value2;
}

/************************************************************************/
/*    boresightPosTab                                                   */
/************************************************************************/
static inline SensorComp_BoresightPosTab* boresightPosTab_alloc(int size)
{
	SensorComp_BoresightPosTab *tab = NULL;

	assert(size > 0);

	tab = (SensorComp_BoresightPosTab*)malloc(sizeof(SensorComp_BoresightPosTab));

	assert(tab != NULL);

	memset(tab, 0, sizeof(SensorComp_BoresightPosTab));

	tab->pos = (SensorComp_BoresightPos*)malloc(sizeof(SensorComp_BoresightPos)*size);

	assert(tab->pos != NULL);

	memset(tab->pos, 0, sizeof(SensorComp_BoresightPos)*size);

	tab->nSize = size;

	return tab;
}

static inline void boresightPosTab_free(SensorComp_BoresightPosTab* tab)
{
	assert(tab != NULL);

	if(tab->pos != NULL)
		free(tab->pos);

	free(tab);
}

static inline void boresightPosTab_BuildDef(SensorComp_BoresightPosTab* tab, int x, int y,
									   float fFovMin, float fFovMax)
{
	assert(tab != NULL);
	assert(tab->nSize >= 2);

	tab->nCount = 2;
	tab->pos[0].fFov = fFovMin;
	tab->pos[0].x = x;
	tab->pos[0].y = y;
	tab->pos[1].fFov = fFovMax;
	tab->pos[1].x = x;
	tab->pos[1].y = y;
}

inline void boresightPosTab_Build(SensorComp_BoresightPosTab* tab,
									SensorComp_BoresightPos* posArray,
									int nArraySize)
{
	assert(tab != NULL);
	assert(posArray != NULL);
	assert(tab->nSize >= nArraySize);

	memcpy(tab->pos, posArray, nArraySize*sizeof(SensorComp_BoresightPos));

	tab->nCount = nArraySize;
}

static inline SensorComp_BoresightPosTab* boresightPosTab_copy(SensorComp_BoresightPosTab* dest,
														  SensorComp_BoresightPosTab* src)
{
	assert(dest != NULL);
	assert(src != NULL);

	if(dest->nSize < src->nCount)
	{
		free(dest->pos);		
		dest->pos = (SensorComp_BoresightPos*)malloc(sizeof(SensorComp_BoresightPos)*src->nCount);
		dest->nSize = src->nCount;
		assert(dest->pos != NULL);
	}

	boresightPosTab_Build(dest, src->pos, src->nCount);

	return dest;
}

static inline int boresightPosTab_find(SensorComp_BoresightPosTab* tab,
									float fov)
{
	int i = 0;

	assert(tab != NULL);
	
	for(i=tab->nCount-1; i>=0; i--)
	{
		if(fov >= tab->pos[i].fFov - EPSINON)
			break;
	}

	return i;
}

static inline int boresightPosTab_insert(SensorComp_BoresightPosTab* tab,
									SensorComp_BoresightPos* pBP)
{
	int i = 0;
	int iPos;
	
	assert(tab != NULL);
	
	for(i=tab->nCount-1; i>=0; i--)
	{
		if(pBP->fFov >= tab->pos[i].fFov - EPSINON)
			break;
	}

	if(pBP->fFov <= tab->pos[i].fFov + EPSINON || i == 0)
	{
		tab->pos[i].x = pBP->x;
		tab->pos[i].y = pBP->y;
		return i;
	}

	if(tab->nCount+1 > tab->nSize && tab->nSize < SENSORCOMP_BORESIGHTPOS_TAB_MAX_SIZE)
	{
		SensorComp_BoresightPos* posArray = NULL;
		SensorComp_BoresightPos* posArrayBak;
		int nSize = tab->nSize + 8;
		nSize = min(nSize, SENSORCOMP_BORESIGHTPOS_TAB_MAX_SIZE);

		posArray = (SensorComp_BoresightPos*)malloc(sizeof(SensorComp_BoresightPos)*nSize);
		assert(posArray != NULL);
		memcpy(posArray, tab->pos, tab->nCount*sizeof(SensorComp_BoresightPos));
		posArrayBak = tab->pos;
		tab->pos = posArray;
		free(posArrayBak);
		tab->nSize = nSize;
	}

	iPos = i+1;

	if(iPos < tab->nCount)
		memmove(&tab->pos[iPos+1], &tab->pos[iPos], sizeof(SensorComp_BoresightPos)*(tab->nCount-iPos));

	tab->pos[iPos].fFov = pBP->fFov;
	tab->pos[iPos].x = pBP->x;
	tab->pos[iPos].y = pBP->y;

	tab->nCount ++;

	return iPos;
}

/************************************************************************/
/*    SensorComp                                                        */
/************************************************************************/

static inline void SensorComp_GetBoresightPos_(SensorComp_Obj *pObj, float fFov, int *pX, int *pY)
{
	int x = 0, y = 0;

	float dfov = fFov - pObj->pCurTab->pos[pObj->iCurPos].fFov;

	if((dfov < EPSINON && dfov > -EPSINON) 
		|| pObj->iCurPos == pObj->pCurTab->nCount-1)
	{
		x = pObj->pCurTab->pos[pObj->iCurPos].x;
		y = pObj->pCurTab->pos[pObj->iCurPos].y;
	}
	else
	{
		float dx, dy, unitFov;
		dx = (float)(pObj->pCurTab->pos[pObj->iCurPos+1].x - pObj->pCurTab->pos[pObj->iCurPos].x);
		dy = (float)(pObj->pCurTab->pos[pObj->iCurPos+1].y - pObj->pCurTab->pos[pObj->iCurPos].y);
		unitFov = pObj->pCurTab->pos[pObj->iCurPos+1].fFov - pObj->pCurTab->pos[pObj->iCurPos].fFov;
		assert(dfov > EPSINON);
		assert(unitFov > EPSINON);
		x = pObj->pCurTab->pos[pObj->iCurPos].x;
		y = pObj->pCurTab->pos[pObj->iCurPos].y;
		x += (int)((dx * dfov)/unitFov);
		y += (int)((dy * dfov)/unitFov);
	}
	*pX = x;
	*pY = y;
}

int CSensorComp::SensorComp_GetIndex(HSENSORCOMP hComp)
{
	SensorComp_Obj *pObj = (SensorComp_Obj*)hComp;
	if(pObj == NULL)
		return -1;

	return pObj->params.iIndex;
}

HSENSORCOMP CSensorComp::SensorComp_Create(SensorComp_CreateParams *pPrm)
{
	SensorComp_Obj *pObj;

	int tabSize = SENSORCOMP_BORESIGHTPOS_TAB_DEF_SIZE;

	if(pPrm == NULL)
		return NULL;

	pObj = (SensorComp_Obj *)malloc(sizeof(SensorComp_Obj));

	if(pObj == NULL)
		return NULL;

	memset(pObj, 0, sizeof(SensorComp_Obj));

	memcpy(&pObj->params, pPrm, sizeof(SensorComp_CreateParams));

	if(pPrm->initTab != NULL)
		tabSize = pPrm->nInitTabSize;

	assert(tabSize > 0);

	pObj->pBoresightPosRunTab = boresightPosTab_alloc(tabSize);
	if(pObj->pBoresightPosRunTab == NULL)
	{
		SensorComp_Delete(pObj);
		return NULL;
	}

	pObj->pBoresightPosCorrTab = boresightPosTab_alloc(tabSize*2);
	if(pObj->pBoresightPosCorrTab == NULL)
	{
		SensorComp_Delete(pObj);
		return NULL;
	}

	if(pPrm->initTab != NULL)
		boresightPosTab_Build(pObj->pBoresightPosRunTab, pPrm->initTab, tabSize);
	else
		boresightPosTab_BuildDef(pObj->pBoresightPosRunTab, 
			pPrm->defaultBoresightPos_x, 
			pPrm->defaultBoresightPos_y,
			pPrm->fFovMin,
			pPrm->fFovMax);

	pObj->curMode = SENSORCOMP_MODE_RUN;
	pObj->pCurTab = pObj->pBoresightPosRunTab;
	pObj->fCurFov = pObj->pBoresightPosRunTab->pos[0].fFov;
	pObj->iCurPos = 0;
	pObj->iCurX = pObj->pBoresightPosRunTab->pos[0].x;
	pObj->iCurY = pObj->pBoresightPosRunTab->pos[0].y;

	SensorComp_SwitchMode(pObj, pObj->params.initMode, 0);

	return pObj;
}

void CSensorComp::SensorComp_Delete(HSENSORCOMP hComp)
{
	SensorComp_Obj *pObj = (SensorComp_Obj*)hComp;
	if(pObj == NULL)
		return ;

	if(pObj->pBoresightPosRunTab != NULL)
		boresightPosTab_free(pObj->pBoresightPosRunTab);
	pObj->pBoresightPosRunTab = NULL;

	if(pObj->pBoresightPosCorrTab != NULL)
		boresightPosTab_free(pObj->pBoresightPosCorrTab);
	pObj->pBoresightPosCorrTab = NULL;

	free(pObj);
}

SENSORCOMP_MODE CSensorComp::SensorComp_GetMode(HSENSORCOMP hComp)
{
	SensorComp_Obj *pObj = (SensorComp_Obj*)hComp;
	if(pObj == NULL)
		return error;
	
	return pObj->curMode;
}

int CSensorComp::SensorComp_SwitchMode(HSENSORCOMP hComp, SENSORCOMP_MODE mode, int flag)
{
	SensorComp_Obj *pObj = (SensorComp_Obj*)hComp;
	if(pObj == NULL)
		return -1;

	if(pObj->curMode == mode)
		return mode;

	if(mode == SENSORCOMP_MODE_RUN)
	{
		if(flag == 1)
		{
			pObj->pBoresightPosRunTab = 
				boresightPosTab_copy(pObj->pBoresightPosRunTab, pObj->pBoresightPosCorrTab);
		}
		pObj->pCurTab = pObj->pBoresightPosRunTab;
	}

	if(mode == SENSORCOMP_MODE_CORR_CLEAN)
	{
		boresightPosTab_BuildDef(pObj->pBoresightPosCorrTab, 
			pObj->params.defaultBoresightPos_x, 
			pObj->params.defaultBoresightPos_y,
			pObj->params.fFovMin,
			pObj->params.fFovMax);
		pObj->pCurTab = pObj->pBoresightPosCorrTab;
	}

	if(mode == SENSORCOMP_MODE_CORR_NORMAL)
	{
		pObj->pBoresightPosCorrTab = 
			boresightPosTab_copy(pObj->pBoresightPosCorrTab, pObj->pBoresightPosRunTab);
		pObj->pCurTab = pObj->pBoresightPosCorrTab;
	}

	pObj->iCurPos = boresightPosTab_find(pObj->pCurTab, pObj->fCurFov);

	SensorComp_GetBoresightPos_(pObj, pObj->fCurFov, &pObj->iCurX, &pObj->iCurY);

	pObj->curMode = mode;
	
	return pObj->curMode;	
}

int CSensorComp::SensorComp_GetBoresightPos(HSENSORCOMP hComp, float fFov, int *pX, int *pY)
{
	SensorComp_Obj *pObj = (SensorComp_Obj*)hComp;
	float dfov;

	if(pObj == NULL || pX == NULL || pY == NULL)
		return -1;
	
	dfov = fFov - pObj->fCurFov;

	if(dfov < EPSINON && dfov > -EPSINON) 
	{
		*pX = pObj->iCurX;
		*pY = pObj->iCurY;

		return 0;
	}

	assert((fFov>=pObj->params.fFovMin));

	pObj->fCurFov = fFov;

	pObj->iCurPos = boresightPosTab_find(pObj->pCurTab, pObj->fCurFov);
	
	SensorComp_GetBoresightPos_(pObj, pObj->fCurFov, &pObj->iCurX, &pObj->iCurY);

	*pX = pObj->iCurX;
	*pY = pObj->iCurY;

	return 0;
}

int CSensorComp::SensorComp_SetBoresightPos(HSENSORCOMP hComp, float fFov, int x, int y)
{
	SensorComp_Obj *pObj = (SensorComp_Obj*)hComp;
	SensorComp_BoresightPos bp;
	
	if(pObj == NULL || fFov < pObj->params.fFovMin)
		return -1;

	if(x<0 || x>pObj->params.nWidth)
		return -1;

	if(y<0 || y>pObj->params.nHeight)
		return -1;

	if(pObj->curMode == SENSORCOMP_MODE_RUN)
		return -1;

	bp.fFov = fFov;
	bp.x = x;
	bp.y = y;
	
	boresightPosTab_insert(pObj->pCurTab, &bp);

	return 0;
}

int CSensorComp::SensorComp_GetTab(HSENSORCOMP hComp, SensorComp_BoresightPos **ppTab, int *pSize)
{
	SensorComp_Obj *pObj = (SensorComp_Obj*)hComp;
	if(pObj == NULL || ppTab == NULL || pSize == NULL)
		return -1;

	*ppTab = pObj->pCurTab->pos;
	*pSize = pObj->pCurTab->nCount;

	return 0;
}

int CSensorComp::SensorComp_CleanTab(HSENSORCOMP hComp)
{
	SensorComp_Obj *pObj = (SensorComp_Obj*)hComp;
	SensorComp_CreateParams *pPrm;
	if(pObj == NULL)
		return -1;

	pPrm = &pObj->params;

	boresightPosTab_BuildDef(pObj->pCurTab, 
		pPrm->defaultBoresightPos_x, 
		pPrm->defaultBoresightPos_y,
		pPrm->fFovMin,
		pPrm->fFovMax);

	pObj->iCurPos = boresightPosTab_find(pObj->pCurTab, pObj->fCurFov);
	
	SensorComp_GetBoresightPos_(pObj, pObj->fCurFov, &pObj->iCurX, &pObj->iCurY);

	return 0;
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
	pPrm->initMode = SENSORCOMP_MODE_RUN;
	return ;
}

