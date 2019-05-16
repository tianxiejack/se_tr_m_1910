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
	int iCurPos;
	float fCurFov;
	int iCurX, iCurY;
}SensorComp_Obj;

View* CSensorComp::backParams()
{
	return &m_viewParam;
}

BoresightPos_s CSensorComp::UpdateParams(float* data, int block, int field, int curChid, PlatformCtrl_Obj* pObj)
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

