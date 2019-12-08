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
