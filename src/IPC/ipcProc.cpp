#include <sys/time.h>
#include <assert.h>
#include <stdint.h>
#include "ipcProc.h"
#include "configtable.h"

CIPCProc* CIPCProc::pThis = 0;
//static float *sysConfig = NULL;
//static unsigned char *userConfig = NULL;

CIPCProc::CIPCProc()
	:m_thrRcvRun(0)
{
	pthread_mutex_init(&mutex,NULL);
	exitThreadIPCRcv = false;
}
CIPCProc::~CIPCProc()
{
	Destroy();

}

int CIPCProc::Create()
{
	pThis = this;
	if(m_thrRcvRun)
	{
		OSA_thrCreate(&thrHandlPCRcv,
				IPC_childdataRcvn,
		                  0,
		                  0,
		                  this);
	}
	 return 0;
}

int CIPCProc::Destroy()
{
	if(m_thrRcvRun)
	{
		exitThreadIPCRcv=true;
		OSA_thrJoin(&thrHandlPCRcv);
	}
	return 0;
}

int CIPCProc::IPCSendMsg(CMD_ID cmd, void* prm, int len)
{
	if(len >= PARAMLEN || (prm == NULL && len > 0))
		return -1;

	SENDST sendData;
	memset(sendData.param, 0, PARAMLEN);
	sendData.cmd_ID = cmd;
	if(len > 0)
		memcpy(sendData.param, prm, len);

	pthread_mutex_lock(&mutex);
	ipc_sendmsg(&sendData, IPC_TOIMG_MSG);
	pthread_mutex_unlock(&mutex);

	return 0;
}

int CIPCProc::IPCRecvMsg(void* prm)
{
	SENDST recvData;
	IPC_PRM_INT *pIn = (IPC_PRM_INT *)&recvData.param;
	ipc_recvmsg(&recvData,IPC_FRIMG_MSG);
	switch(recvData.cmd_ID)
	{
	case read_shm_block:
		{
			int configId = pIn->intPrm[0];
			int blkid = CFGID_blkId(configId);
			int feildId = CFGID_feildId(configId);
			/*if(blkid == CFGID_RTS_BKID)
			{
				if(prm != NULL)
					memcpy(prm, , );
			}*/
		}
		break;
	case read_shm_single:
		{
			int trkstat = pIn->intPrm[6];

			printf("trkstat = %d \n",trkstat);
		
			printf("err x,y :(%f , %f) \n", pIn->intPrm[7] , pIn->intPrm[8]);

		}
		

		
		//if(pIn->intPrm[0] == CFGID_RTS_trkstat)
		//	return (CFGID_RTS_trkstat);
		//else if(pIn->intPrm[0] == CFGID_RTS_mtddet)
		//	return (CFGID_RTS_mtddet);

		
		break;

	default:
		break;
	}
	return 0;
}

void CIPCProc::getIPCMsgProc()
{
	SENDST recvData;
	IPC_PRM_INT *pIn = (IPC_PRM_INT *)&recvData.param;
	while(!exitThreadIPCRcv)
	{
		//ipc_recvmsg(&recvData,IPC_FRIMG_MSG);
		//pthread_mutex_lock(&mutex);

		switch(recvData.cmd_ID)
		{
		default:
			break;
		}
		//pthread_mutex_unlock(&mutex);
	}
}
