/*
 * eventParsing.hpp
 *
 *  Created on: 2019年5月7日
 *      Author: d
 */

#ifndef EVENTPARSING_HPP_
#define EVENTPARSING_HPP_
#include <iostream>
#include "js_usb.hpp"
#include "PortFactory.hpp"
#include "CMessage.hpp"
#include <vector>
#include  "osa_thr.h"
#include "osa_mutex.h"

using namespace std;

#define AXIS_X 0
#define AXIS_Y 1
#define POV_BUTTON 5
#define BUTTON 6

#define    RECV_BUF_SIZE   1024
#define  TIME_FOR_THREAD_END 3

typedef enum
{
	/*button1234 ==>data[5]
	 * other ===> data[6]
	 * */
	js_1 = 0x1f,
	js_2 = 0x2f,
	js_3 = 0x4f,
	js_4 = 0x8f,
	js_5 = 0x01,
	js_6 = 0x02,
	js_7 = 0x04,
	js_8 = 0x08,
	js_9 = 0x10,
	js_10 = 0x20,
	js_11 = 0x40,
	js_12 = 0x80,
}js_button;

typedef enum
{
	/*pov ==> data[5]*/
	js_pov_reset = 0x0f,
	js_pov_up = 0x00,
	js_pov_rightUp = 0x01,
	js_pov_right = 0x02,
	js_pov_rightDown = 0x03,
	js_pov_down = 0x04,
	js_pov_leftDown = 0x05,
	js_pov_left = 0x06,
	js_pov_leftUp = 0x07,
}js_pov;

typedef enum _sys_msg_id_ {
    	MSGID_SYS_INIT  = 0,
   	MSGID_EXT_INPUT_TRACKCTRL,
	MSGID_EXT_INPUT_SwitchSensor,
	MSGID_EXT_INPUT_ZOOMLONGCTRL,
    	MSGID_EXT_INPUT_ZOOMSHORTCTRL,
    	MSGID_EXT_INPUT_TRACKSEARCHCTRL,
    	MSGID_EXT_INPUT_captureMode,
    	MSGID_EXT_INPUT_IrisAndFocusAndExit,
    	MSGID_EXT_INPUT_workMode,
    	MSGID_EXT_INPUT_JOSPOS,
    	MSGID_EXT_INPUT_POVPOSX,
    	MSGID_EXT_INPUT_POVPOSY,

	MSGID_COM_INPUT_SELFCHECK,
	MSGID_COM_INPUT_SWITCHSENSOR,
	MSGID_COM_INPUT_WORKMODE,
	MSGID_COM_INPUT_CAPTUREMODE,
	MSGID_COM_INPUT_TRACKCTRL,
	MSGID_COM_INPUT_TRKMOVE,
	MSGID_COM_INPUT_SECTRKCTRL,
	MSGID_COM_INPUT_SECTRKPOS,
	MSGID_COM_INPUT_MTDSELECT,
	MSGID_COM_INPUT_GATEMOVE,
	MSGID_COM_INPUT_ZOOMCTRL,
	MSGID_COM_INPUT_APERTURECTRL,
	MSGID_COM_INPUT_FOCUSCTRL,
	MSGID_COM_INPUT_SETPLATSPEED,
	MSGID_COM_INPUT_SETPLATANGLE,
	MSGID_COM_INPUT_PRESETPOS,
	MSGID_COM_INPUT_SETZOOM,
	MSGID_COM_INPUT_GETPLATANGLE,
	MSGID_COM_INPUT_GETZOOM,
	MSGID_COM_INPUT_TRKOUTPUT,
	MSGID_COM_INPUT_SETCFG,
	MSGID_COM_INPUT_GETCFG,
	MSGID_COM_INPUT_SETOSD,
	MSGID_COM_INPUT_DEFAULTCFG,
	MSGID_COM_INPUT_SAVECFG,

    	MAX_MSG_NUM
}MSG_PROC_ID;

typedef struct{
	int connfd;
	bool bConnecting;
    	OSA_ThrHndl getDataThrID;
    	OSA_ThrHndl sendDataThrID;
}CConnectVECTOR;

typedef struct{
	int block;
	int field;
	float value;
}Set_config_t;

typedef struct{
	int block;
	int field;
}Get_config_t;

typedef struct
{
	volatile unsigned char osdID;
	volatile unsigned char type;
	volatile unsigned char buf[128];
}osdbuffer_t;

typedef struct{
	int fd;
	int displaychid;
	int capturechid;
	int workmode;
	int capturemode;
	int trkctrl;
	int trkmove;
	int sectrkctrl;
	short sectrkx;
	short sectrky;
	int mtdselect;
	int gatemove;
	unsigned short gatestepx;
	unsigned short gatestepy;
	int zoomctrl;
	int aperturectrl;
	int focusctrl;
	short platspeedx;
	short platspeedy;
	unsigned short platanglex;
	unsigned short platangley;
	int presetpos;
	unsigned short setzoom;
	int trkoutput;
	vector<Set_config_t>  setConfigQueue;
	vector<Get_config_t>  getConfigQueue;
	vector<int>  defConfigQueue;
	osdbuffer_t setosd;
}ComParams_t;

class CEventParsing
{
public:
	CEventParsing();
	~CEventParsing();
	CjoyStick* _Js;
	CMessage* _Msg;
	static void thread_jsEvent();
private:
	static CEventParsing* pThis;
	bool exit_jsParsing,exit_comParsing,exit_netParsing;
	unsigned char* js_data;
	void parsingJostickEvent(unsigned char* jos_data);




public:
	static void thread_comrecvEvent();
	static void thread_comsendEvent();
	static void thread_Getaccept();
	static int thread_ReclaimConnect();
private:
	void parsingframe(unsigned char *tmpRcvBuff, int sizeRcv, int fd);
	int parsingComEvent(int fd);
	unsigned char check_sum(int len_t);
	static void *thread_netrecvEvent(void *p);
public:
	ComParams_t ComParams;
private:
	int comfd;
	CPortInterface *pCom1, *pCom2;
	vector<unsigned char>  rcvBufQue;
	vector<CConnectVECTOR *>  connetVector;
	OSA_MutexHndl mutexConn;

};




#endif /* EVENTPARSING_HPP_ */
