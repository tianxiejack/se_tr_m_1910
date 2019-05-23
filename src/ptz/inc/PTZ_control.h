#ifndef _PTZ_CONTROL_H
#define _PTZ_CONTROL_H

#include "osa.h"
#include "osa_thr.h"
#include "osa_buf.h"
#include "osa_sem.h"
#include "PTZ_speedTransfer.h"
#include "globalDate.h"
#include "DxTimer.hpp"
#include "ptzInterface.hpp"
#include "pelcoDformat.h"
#include "pelcoPformat.h"
#include "ExtensionFormat.h"
#include "PortFactory.hpp"

#define balladdress 1

typedef enum{
	PELCO_RESPONSE_Null = 0,
	PELCO_RESPONSE_General,
	PELCO_RESPONSE_Query,
	PELCO_RESPONSE_Extended
}PELCO_RESPONSE_t;

class CPTZControl : CptzInterface
{
    OSA_ThrHndl thrHandleDataIn;
    volatile Bool exitDataInThread;
    static Void * port_dataInFxn(Void * prm){
    	CPTZControl *pThis = (CPTZControl*)prm;
    	struct timeval tmp;
    	for(;pThis->exitDataInThread==false;)
    	{
    		pThis->dataInThrd();
    	}
    };

    OSA_ThrHndl thrHandleMove;
    volatile Bool exitThreadMove;
    static Void * MoveThrdFxn(Void * prm){
    	CPTZControl *pThis = (CPTZControl*)prm;
    	struct timeval tmp;
    	for(;pThis->exitThreadMove==false;)
    	{
    		tmp.tv_sec = 0;
    		tmp.tv_usec = 50000;
    		select(0, NULL, NULL, NULL, &tmp);
    		pThis->Move();
        	pThis->MoveSync();
    	}
    	pThis->exitThreadMove = false;
    };
public:
	CPTZControl(AgreeMentBaseFormat* _imp);
	~CPTZControl();

	int Create();
	void Destroy();

	int Move();


	int Preset(int nCtrlType, int iPresetNum);
	int Pattern(int nCtrlType, int iPatternNum);
	int Query(int iQueryType);
	int Dummy();

	void ptzZoomWide();
	void ptzZoomTele();
	void ptzZoomStop();

	int ptzMove(INT32 iDirection, UINT8 bySpeed);
	int ptzStop();
	void ptzSetPos(Uint16 posx, Uint16 posy);
	void setZoomPos(Uint16 value);
	void QueryZoom();
	void QueryPanPos();
	void QueryTiltPos();
	void QueryPos();

	void shRunOutput();
	void PanoramicMirror_Init();
	void PanoramicMirrorOutput();
	void MakeFoucusFar();
	void MakeFocusNear();
	void MakeFocusStop();

	void test_left();
	void test_stop();

public:
	int m_iSetPanSpeed, m_iSetTiltSpeed, m_iSetZoomSpeed,m_iSetIrisSpeed,m_iSetFocusNearSpeed, m_iSetFocusFarSpeed, m_iSetPreset;
	int m_isetZoom;
	int m_iCurTiltSpeed, m_iCurZoomSpeed;
	int	m_iPanPos, m_iTiltPos , m_iZoomPos;
	bool m_bChangeZoomSpeed;
	bool exitQuery_X, exitQuery_Y, exitQueryzoom;
	int fd_ptz;

protected:
	unsigned char m_byAddr;
	unsigned char m_cmd1Code, m_cmd2Code, m_data1Code, m_data2Code;
	int m_iCurPanSpeed,m_iCurIrisSpeed,m_iCurFocusNearSpeed, m_iCurFocusFarSpeed,m_iCurPreset;
	int m_iCurZoom;
	bool m_bChangePanSpeed, m_bChangeTiltSpeed, m_bChangeIrisSpeed,
	m_bChangeFocusNearSpeed, m_bChangeFocusFarSpeed, m_bChangePreset;
	bool m_bStopZoom;
	bool m_changeZoom;
	int m_iIrisPos,m_iMagnification, m_iFocusPos;
	bool m_bQuryZoomPos;
	float m_fZoomLimit;
	int m_circle;

private:
	CPortInterface* pCom;
	static CPTZControl* pThis;
	CGlobalDate* _GlobalDate;
	Uint16 posx_bak, posy_bak, zoom_bak;
	AgreeMentBaseFormat* _agreeMent;
	DxTimer dtimer;
	int breakQuery;
	int breakQuery_X;
	int breakQuery_Y;
	int timeout;
	int outputMode;
	void DXTimerCreat();
	static void Tcallback_QueryX(void* p);
	static void Tcallback_QueryY(void* p);
	int sendCmd(LPPELCO_D_REQPKT pCmd, PELCO_RESPONSE_t tResp = PELCO_RESPONSE_General);

	u_int8_t sendCheck_sum(uint len, u_int8_t *tmpbuf);
	u_int8_t package_frame(uint len, u_int8_t *tmpbuf);
	void trackErrOutput();
	int SendtrackErr();

protected:
	unsigned char recvBuffer[32];
	unsigned int uiCurRecvLen;
    PELCO_RESPONSE_t m_tResponse;
    unsigned int m_nWait;

	unsigned char sendBuffer[64];
	unsigned char sendBuffer1[64];
	LPPELCO_D_REQPKT m_pReq, m_pReqMove;
	LPPELCO_D_RESPPKT m_pResp;

	void dataInThrd();
	void RecvByte(unsigned char byRecv);
	int MoveSync();

	OSA_MutexHndl m_mutex;
	OSA_SemHndl m_sem;


public:
	char m_Mtd_Moitor; /*Recording detection area; 1: record; 0: don't record*/
	unsigned short m_Mtd_Moitor_X,m_Mtd_Moitor_Y,m_Mtd_Moitor_Zoom;
	int m_sync_pos,m_sync_fovComp;
	unsigned short m_rcv_zoomValue;
	bool m_stateChange;

	
};

#endif
