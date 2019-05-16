#ifndef _SENSORCOMPENSATE_H
#define _SENSORCOMPENSATE_H

#include <string.h>
#include <vector>
#include "platformparam.h"

using namespace std;

const double PI = 3.14159269798;

const char chid_camera = 5;

typedef struct{
    /*固定视场*/
	float level_Fov_fix[chid_camera];
	float vertical_Fov_fix[chid_camera];
	float Boresightpos_fix_x[chid_camera];
	float Boresightpos_fix_y[chid_camera];
/*可切换视场*/
	float level_Fov_switch1[chid_camera];
	float vertical_Fov_switch1[chid_camera];
	float Boresightpos_switch_x1[chid_camera];
	float Boresightpos_switch_y1[chid_camera];

	float level_Fov_switch2[chid_camera];
	float vertical_Fov_switch2[chid_camera];
	float Boresightpos_switch_x2[chid_camera];
	float Boresightpos_switch_y2[chid_camera];

	float level_Fov_switch3[chid_camera];
	float vertical_Fov_switch3[chid_camera];
	float Boresightpos_switch_x3[chid_camera];
	float Boresightpos_switch_y3[chid_camera];

	float level_Fov_switch4[chid_camera];
	float vertical_Fov_switch4[chid_camera];
	float Boresightpos_switch_x4[chid_camera];
	float Boresightpos_switch_y4[chid_camera];

	float level_Fov_switch5[chid_camera];
	float vertical_Fov_switch5[chid_camera];
	float Boresightpos_switch_x5[chid_camera];
	float Boresightpos_switch_y5[chid_camera];
/*连续视场*/
	float level_Fov_continue1[chid_camera];
	float vertical_Fov_continue1[chid_camera];
	float Boresightpos_continue_x1[chid_camera];
	float Boresightpos_continue_y1[chid_camera];
	float zoombak1[chid_camera];

	float level_Fov_continue2[chid_camera];
	float vertical_Fov_continue2[chid_camera];
	float Boresightpos_continue_x2[chid_camera];
	float Boresightpos_continue_y2[chid_camera];
	float zoombak2[chid_camera];

	float level_Fov_continue3[chid_camera];
	float vertical_Fov_continue3[chid_camera];
	float Boresightpos_continue_x3[chid_camera];
	float Boresightpos_continue_y3[chid_camera];
	float zoombak3[chid_camera];

	float level_Fov_continue4[chid_camera];
	float vertical_Fov_continue4[chid_camera];
	float Boresightpos_continue_x4[chid_camera];
	float Boresightpos_continue_y4[chid_camera];
	float zoombak4[chid_camera];

	float level_Fov_continue5[chid_camera];
	float vertical_Fov_continue5[chid_camera];
	float Boresightpos_continue_x5[chid_camera];
	float Boresightpos_continue_y5[chid_camera];
	float zoombak5[chid_camera];

	float level_Fov_continue6[chid_camera];
	float vertical_Fov_continue6[chid_camera];
	float Boresightpos_continue_x6[chid_camera];
	float Boresightpos_continue_y6[chid_camera];
	float zoombak6[chid_camera];

	float level_Fov_continue7[chid_camera];
	float vertical_Fov_continue7[chid_camera];
	float Boresightpos_continue_x7[chid_camera];
	float Boresightpos_continue_y7[chid_camera];
	float zoombak7[chid_camera];

	float level_Fov_continue8[chid_camera];
	float vertical_Fov_continue8[chid_camera];
	float Boresightpos_continue_x8[chid_camera];
	float Boresightpos_continue_y8[chid_camera];
	float zoombak8[chid_camera];

	float level_Fov_continue9[chid_camera];
	float vertical_Fov_continue9[chid_camera];
	float Boresightpos_continue_x9[chid_camera];
	float Boresightpos_continue_y9[chid_camera];
	float zoombak9[chid_camera];

	float level_Fov_continue10[chid_camera];
	float vertical_Fov_continue10[chid_camera];
	float Boresightpos_continue_x10[chid_camera];
	float Boresightpos_continue_y10[chid_camera];
	float zoombak10[chid_camera];

	float level_Fov_continue11[chid_camera];
	float vertical_Fov_continue11[chid_camera];
	float Boresightpos_continue_x11[chid_camera];
	float Boresightpos_continue_y11[chid_camera];
	float zoombak11[chid_camera];

	float level_Fov_continue12[chid_camera];
	float vertical_Fov_continue12[chid_camera];
	float Boresightpos_continue_x12[chid_camera];
	float Boresightpos_continue_y12[chid_camera];
	float zoombak12[chid_camera];

	float level_Fov_continue13[chid_camera];
	float vertical_Fov_continue13[chid_camera];
	float Boresightpos_continue_x13[chid_camera];
	float Boresightpos_continue_y13[chid_camera];
	float zoombak13[chid_camera];
}View;



class CSensorComp{
public:
	CSensorComp(){};
	~CSensorComp(){};


	View* backParams();
	BoresightPos_s UpdateParams(float* data, int block, int field, int curChid, PlatformCtrl_Obj* pObj);

	void SensorComp_CreateParams_Init(SensorComp_CreateParams *pPrm, int i, View* Pserson,int width ,int height);
	
	float ZoomLevelFovCompensation(unsigned short zoom, int chid);
	float ZoomVerticalFovCompensation(unsigned short zoom, int chid);

private:
	void UpdateSwitchFovParams_one(int chid, int field, int curChid, PlatformCtrl_Obj* pObj);
	void UpdateSwitchFovParams_two(int chid, int field, int curChid, PlatformCtrl_Obj* pObj);
	void UpdateContinueFovParams_one(int chid, int field, int curChid, PlatformCtrl_Obj* pObj);
	void UpdateContinueFovParams_two(int chid, int field, int curChid, PlatformCtrl_Obj* pObj);
	void UpdateContinueFovParams_three(int chid, int field, int curChid, PlatformCtrl_Obj* pObj);
	void UpdateContinueFovParams_four(int chid, int field, int curChid, PlatformCtrl_Obj* pObj);
	void UpdateContinueFovParams_five(int chid, int field, int curChid, PlatformCtrl_Obj* pObj);

	float linear_interpolation_Fov(float x0, float x1, float y0, float y1, float x);


private:
	View m_viewParam;
	BoresightPos_s Bpos;
	
};


#endif
