/*
 * globalDate.h
 *
 *  Created on: 2018年9月26日
 *      Author:yd
 */

#ifndef GLOBALDATE_H_
#define GLOBALDATE_H_

#include <iostream>
#include <vector>
#include <time.h>
#include "configtable.h"

#include "osa_sem.h"
//#include "ipc_custom_head.hpp"


using namespace std;

typedef  struct  {
	u_int8_t   cmdBlock;
	u_int8_t   cmdFiled;
	float    confitData;
}systemSetting;

typedef struct{
     unsigned char SecAcqStat;// eSecTrkMode
     int ImgPixelX; //SecTrk X
     int ImgPixelY; //SecTrk  Y
}selectTrack;

typedef struct{
	volatile unsigned char AvtMoveX;// eTrkRefine (axis or aim) x
	volatile unsigned char AvtMoveY;// eTrkRefine (axis or aim) y
}POSMOVE;

typedef struct{
	volatile unsigned int BoresightPos_x;
	volatile unsigned int BoresightPos_y;
}iBoresightPos;

typedef struct{
	volatile unsigned char AcqStat;
	volatile unsigned int BoresightPos_x;
	volatile unsigned int BoresightPos_y;
}AcqBoxPos_ipc;

typedef struct{
	volatile int AcqBoxW[eSen_Max];
	volatile int AcqBoxH[eSen_Max];
}AcqBoxSize;

typedef struct{
	volatile int dir;
	volatile int alpha;
}osd_triangle;


typedef struct {
		int  m_TrkStat;
		int  m_SenSwStat;
		int m_ZoomLongStat;
		int m_ZoomShortStat;
		int m_zoomSpeed;
		int m_zoomSpeedBak;
		int m_TrkBoxSizeStat;
		int m_SecTrkStat;
		int m_AcqBoxStat;
		int m_AcqStat_ipc;
		int m_IrisUpStat;
		int m_IrisDownStat;
		int m_FoucusFarStat;
		int m_FoucusNearStat;
		int m_PresetStat;
		int m_ImgEnhanceStat;
		int m_MmtStat;
		int m_MmtSelectNum;
		volatile unsigned char m_AimPosXStat;
		volatile unsigned char m_AimPosYStat;
		int m_AxisXStat;
		int m_AxisYStat;
}CurrParaStat,*PCurrParaStat;

typedef struct{
	int block;
	int filed;
}Read_config_buffer;

typedef struct{
	uint channel0:1;
	uint channel1:1;
	uint channel2:1;
	uint channel3:1;
	uint channel4:1;
	uint channel5:1;
	uint reserve5:2;
}channelTable;

typedef enum{
	DefaultOff = 0,
	PosLoop ,
	SpeedLoop
}MtdMode;

typedef enum{
	Default = 0,
	Next,
	Prev,
	Selext
}MtdSelect;

typedef enum{
	Auto = 0,
	Manual
}MtdExitMode;

enum{recv_AutoMtdDate = 0, shield_AutoMtdDate};

typedef struct{
	volatile int mptzx;
	volatile int mptzy;
}MOUSEPTZ;

typedef struct{
	volatile unsigned int preset;
	volatile unsigned int warning;
	volatile unsigned int high_low_level;
	volatile unsigned int trktime;  /*  ms  */
}MtdConfig;

typedef struct{
	unsigned int panPos;
	unsigned int tilPos;
	unsigned int zoom;
}LinkageParam;


typedef struct _uart_params
{
    char   device[64];
    char   name[64];
    Int32  baudrate;
    Int32  databits;
    char   parity[2];
    Int32  stopbits;
    Int32 balladdress;
}G_uart_open_params;

typedef struct camera_Pos
{
	unsigned short setpan;
	unsigned short settilt;
	unsigned short setzoom;
}camera_PosParams;

typedef enum{
	errorValue = 0,
	correctValue
}mtd_filter;

typedef enum{
	pelco_d,
	pelco_p,
	sheenrun,
	Panoramic_mirror
}PTZ_type;

class CGlobalDate
{
public:
	~CGlobalDate(){};

static CGlobalDate* Instance();
static const char chid = 5;
char avtStatus;
int sheenrun[2];
int PanoramicMirror_value[2];
short errorOutPut[2];
short speedComp[2];
signed char speedLv[2];
char  ACK_read_content[128];
vector <float> ACK_read;
vector <int> EXT_Ctrl;
vector <float> Host_Ctrl;
vector<unsigned char>  rcvBufQue;
static vector<Read_config_buffer>  readConfigBuffer;
static vector<int>  defConfigBuffer;
//static selectCh_t selectch;

//static osdbuffer_t recvOsdbuf;
CurrParaStat pParam;
selectTrack m_selectPara;
AcqBoxPos_ipc m_acqBox_Pos;
POSMOVE m_avtMove;
CurrParaStat  m_CurrStat;
systemSetting avtSetting;
osd_triangle m_osd_triangel;
//IMGSTATUS  avt_status;
MtdConfig mtdconfig;
LinkageParam linkagePos;
G_uart_open_params m_uart_params;
camera_PosParams m_camera_pos[5];

OSA_SemHndl m_semHndl_automtd;
OSA_SemHndl m_semHndl_workMode;
OSA_SemHndl m_semHndl_sceneTrk;
int joystick_flag;
int commode;
int feedback;
int choose;
int IrisAndFocus_Ret;
int respupgradefw_stat;
int respupgradefw_perc;
int respimpconfig_stat;
int respimpconfig_perc;
int respexpconfig_stat;
int respexpconfig_len;
unsigned char respexpconfig_buf[1024+256];
struct timeval PID_t;
int time_start;
long int time_stop;
 int frame;
 int target;
 bool resetType;
char ImgMtdStat;
char mtdMode;
char Mtd_ExitMode;
char Mtd_ipc_target; /*receive ipc detect message; 1: find target; 0: no target*/
char Mtd_Limit;  /* Block IPC messages if you enter the trace */
char Mtd_Manual_Limit;  /*检测到目标后才能响应进跟踪指令*/

bool MtdAutoLoop;
int ThreeMode_bak;

int jos_ctrl;
int host_ctrl;
int workMode;
int captureMode;
int captureMode_bak;
int outputMode;
int chid_camera;
int manual_mtd_filter;
int BoresightPos_Xbak[5];
int BoresightPos_Ybak[5];
int stateBak;
int ptzType;
int goToPreset;
int changezoom;

bool host_osdBoxShow[5];
bool host_CROSS_draw_show[5];
unsigned char MtdSetAreaBox;
unsigned char scenetrk;
unsigned char ScenceTrkWait;

unsigned char cameraResolution[chid];
unsigned char AutoBox[chid];
unsigned char osdUserSwitch;
unsigned char fovmode[chid];
unsigned char switchFovLV[chid];
unsigned char continueFovLv[chid];

private:
CGlobalDate();
static CGlobalDate* _Instance;

};
static CGlobalDate* aaa;

#endif /* GLOBALDATE_H_ */
