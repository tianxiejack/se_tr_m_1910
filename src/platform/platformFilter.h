#ifndef _PLATFORM_FILTER_H
#define _PLATFORM_FILTER_H
#include <iostream>
#include <vector>
using namespace std;

typedef void * HPLTFILTER;

typedef struct
{
	bool stable;
	int count;
	int iIndex;
	float Kp;
	float Ki;
	float Kd;
	float K;
}PlatformFilter_CreateParams;

typedef struct
{
    float Yc[2];
    float Xc[3];
    float Uc[2];
    float Ycbuf[8];
    float Yc_out;
} PlatformFilter_pri;

typedef struct
{
	PlatformFilter_CreateParams params;
	PlatformFilter_pri privates;
} PlatformFilter_Obj;



class CPlatformFilter{

public:
	CPlatformFilter(int frameNum = 50,float speedDiff2integral = 1,float speedDiff2count = 5 ,float speedMax = 1200);
	~CPlatformFilter(){};


	HPLTFILTER PlatformFilter_Create(PlatformFilter_CreateParams *pPrm);
	int PlatformFilter_GetIndex(HPLTFILTER hFilter);
	void PlatformFilter_Delete(HPLTFILTER hFilter);
	void PlatformFilter_Reset(HPLTFILTER hFilter);
	int PlatformFilter_reset4trk(HPLTFILTER hFilter);
	void calcCurPlatAvrRate(HPLTFILTER hFilter , float curRate);
	void PlatformFilter_updatePidParams(PlatformFilter_CreateParams *pPrm,PlatformFilter_CreateParams *pPrm2, int* m_Prm);
	float pidAlg(HPLTFILTER hFilter, float curXc);

private:
	int m_frameNum;
	float m_speedDiff2integral , m_speedDiff2Count , m_speedMax;

	
};

#endif
