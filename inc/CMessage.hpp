#ifndef __CMESSAGE_H__
#define __CMESSAGE_H__

#include <osa_thr.h>
#include <osa_msgq.h>
#include <assert.h>

#define SDK_SOK 0
#define SDK_EFAIL -1
#define MSGDRIV_FREE_CMD 0xFFFF
#define SDK_MSGQUE_SEND(to,from,cmd,prm,msgFlags,msg) OSA_msgqSendMsg(to,from,cmd,prm,msgFlags,msg)
#define SDK_ASSERT(f) assert(f)
#define SDK_TIMEOUT_FOREVER OSA_TIMEOUT_FOREVER
typedef OSA_MsgHndl MSG_ID;
typedef void (*MsgApiFun)(void* pParam);

const char MAX_MSGID_NUM = 128;

typedef enum {
	selfTest = 0,
	mainVideo,
	Channel_binding,
	moveAxisX,
	moveAxisY,
	ElectronicZoom,
	saveAxis,
	picp,
	picpChannel,
	chooseVideoChannel,
	frameCtrl,
	frameChannel,
	compression_quality,
	compressionChannel,
	wordColor,
	transparencyM,
	cNum,
	wordType,
	wordSize,
	wordDisEnable,
	XpositionM,
	YpositionM,
	config_Wblock,
	config_Wfield,
	config_Wvalue,
	config_Rblock,
	config_Rfield,
	config_Rvalue,
	config_read,
	zoomSpeed,
	jos_Kboard,
	jos_Kboard2
}Host_CtrlInput;

typedef enum{
	ACK_selfTest_value = 0,  //
	ACK_Channel_bindingStatus_value,  //
	ACK_posMove_value,    //
	ACK_moveAxis_value = 4, //
	ACK_mmtSelect_value = 6,   //
	ACK_ElectronicZoom_value,
	ACK_TrkSearch_value,
	ACK_config_Wblock,
	ACK_config_Wfield,
	ACK_config_Wvalue,
	ACK_config_Rblock = 12,
	ACK_config_Rfield,
	ACK_config_Rvalue,
	ACK_wordColor_value,   //
	ACK_wordType_value,  //
	ACK_wordSize_value,		//
	NAK_wordColor_value,	//
	NAK_wordType_value,	  //
	NAK_wordSize_value,		//
	ACK_jos_Kboard_value,		//
	ACK_param_todef_value,
	ACK_pan_value,
	ACK_tilt_value,
	ACK_zoom_value,
	ACK_value_max,
}ACK_Host_Value;

typedef enum{
	ACK_selfTest = 0,
	ACK_wordColor,
	ACK_wordType,
	ACK_wordSize,
	NAK_wordColor,
	NAK_wordType,
	NAK_wordSize,
	ACK_mainVideoStatus,
	ACK_Channel_bindingStatus,  //
	ACK_avtStatus,
	ACK_avtErrorOutput, //10
	ACK_avtSpeedComp,
	ACK_avtSpeedLv,
	ACK_mmtStatus,
	ACK_mmtSelectStatus,  //
	ACK_EnhStatus,
	ACK_MtdStatus,
	ACK_TrkSearchStatus,
	ACK_posMoveStatus,   //
	ACK_moveAxisStatus,  //
	ACK_ElectronicZoomStatus, //20
	ACK_picpStatus,
	ACK_VideoChannelStatus,
	ACK_frameCtrlStatus,
	ACK_compression_quality,
	ACK_config_Write,
	ACK_config_Read,
	ACK_jos_Kboard,
	ACK_upgradefw,
	ACK_impconfig,
	ACK_pan,
	ACK_Tilt,  //30
	ACK_zoom,
	ACK_param_todef,
}ACK_Host_CtrlInput;

typedef struct _Msg_Tab
{
    int msgId;
    void* refContext;
    MsgApiFun pRtnFun;
}MSGTAB_Class;

typedef struct msgdriv
{
    unsigned short  bIntial;
    OSA_ThrHndl tskHndl;
    unsigned short tskLoop;
    unsigned short istskStopDone;
    OSA_MsgqHndl msgQue;
    MSGTAB_Class msgTab[MAX_MSGID_NUM];
}MSGDRIV_Class, *MSGDRIV_Handle;

class CMessage{
public: 
    MSGDRIV_Handle  MSGDRIV_create();
    void MSGDRIV_destroy(MSGDRIV_Handle handle);
    void MSGDRIV_register(int msgId, MsgApiFun pRtnFun, void* context);
    void MSGDRIV_send(int msgId, void *prm);
    ~CMessage();
    static CMessage* getInstance();

private:
    CMessage();
    CMessage(const CMessage&);
    CMessage& operator=(const CMessage&);
    static CMessage* instance;
    MSGDRIV_Class g_MsgDrvObj;
    void* MSGDRIV_ProcTask();
	
protected:
    static void *pMSGDRIV_ProcTask(void * mContext)
    {
        CMessage* sPthis = (CMessage*)mContext;
        sPthis->MSGDRIV_ProcTask();
        return NULL;
    }
};

#endif
