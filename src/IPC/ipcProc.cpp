#include <sys/time.h>
#include <assert.h>
#include <stdint.h>
#include "ipcProc.h"
#include "configtable.h"

CIPCProc* CIPCProc::pThis = 0;
//static float *sysConfig = NULL;
//static unsigned char *userConfig = NULL;

CIPCProc::CIPCProc()
{
	pthread_mutex_init(&mutex,NULL);
}
CIPCProc::~CIPCProc()
{
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
	ipc_sendmsg(IPC_TOIMG_MSG ,&sendData);
	ipc_sendmsg(IPC_TOIMG_MSG ,&sendData);
	pthread_mutex_unlock(&mutex);

	return 0;
}


int CIPCProc::IPCRecvGstMsg(void* prm)
{
	SENDST recvData;
	IPC_PRM_INT *pIn = (IPC_PRM_INT *)&recvData.param;
	ipc_recvmsg( IPC_GSTREAM_PTZ , &recvData );
	int flag = -1;
	switch(recvData.cmd_ID)
	{
		case 0:
			m_gstRectParm.status = pIn->intPrm[0];	
			flag = 0;
			break;
		case 1:
			m_gstRectParm.status = pIn->intPrm[0];
			memcpy((void*)&m_gstRectParm.errx ,(void*)&pIn->intPrm[1],sizeof(float));
			memcpy((void*)&m_gstRectParm.erry ,(void*)&pIn->intPrm[2],sizeof(float));
			flag = 1;
			break;
		default:
			break;
	}
	return flag;
}


int CIPCProc::IPCRecvMsg(void* prm)
{
	SENDST recvData;
	IPC_PRM_INT *pIn = (IPC_PRM_INT *)&recvData.param;
	ipc_recvmsg(IPC_FRIMG_MSG,&recvData);
	switch(recvData.cmd_ID)
	{
		case read_shm_block:
			{
				int configId = pIn->intPrm[0];
				int blkid = CFGID_blkId(configId);
				int feildId = CFGID_feildId(configId);
			}
			break;
		case read_shm_single:
			{	
				if(pIn->intPrm[0] == CFGID_RTS_mainch)
					return (CFGID_RTS_mainch);
				else if(pIn->intPrm[0] == CFGID_RTS_mainch2)
					return (CFGID_RTS_mainch2);
				else if(pIn->intPrm[0] == CFGID_RTS_trken)
					return (CFGID_RTS_trken);
				else if(pIn->intPrm[0] == CFGID_RTS_sectrkstat)
					return (CFGID_RTS_sectrkstat);
				else if(pIn->intPrm[0] == CFGID_RTS_trkstat)
					return (CFGID_RTS_trkstat);
				else if(pIn->intPrm[0] == CFGID_RTS_mtddet)
					return (CFGID_RTS_mtddet);
			}		
			break;
		

		default:
			break;
	}
	return 0;
}

