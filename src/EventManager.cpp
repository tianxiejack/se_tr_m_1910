/*
 * EventManager.cpp
 *
 *  Created on: 2019年5月8日
 *      Author: d
 */
#include "EventManager.hpp"
#include <opencv2/core/core.hpp>
#include <cstdarg>

CEventManager* CEventManager::pThis = NULL;
extern ACK_ComParams_t ACK_ComParams;
extern OSA_SemHndl  m_semHndl;
extern OSA_SemHndl m_semHndl_s;
const int profileNum = CFGID_BKID_MAX*16;

using namespace cv;

CEventManager::CEventManager()
{
	pThis = this;
	exit_ipcthread = false;
	memset(winPos, 0, sizeof(winPos));
	_Msg = CMessage::getInstance();
	_StateManager = new StateManger(new PlatFormCapture());
	_Handle = _Msg->MSGDRIV_create();
	MSG_register();
	m_ipc = (CIPCProc*)_StateManager->returnIpc();
	IPC_Creat();
	ReadConfigFile();
	_StateManager->GetParams(cfg_value);
	outtype = cfg_value[CFGID_PTZ_feedbacktype];
	_StateManager->_state->StateInit();

	SELF_semCreate(&m_semSendpos);
	SELF_semCreate(&m_semSendZoom);

}

CEventManager::~CEventManager()
{
	SELF_semDelete(&m_semSendpos);
	SELF_semDelete(&m_semSendZoom);

	exit_ipcthread = true;
	delete pThis;
	delete _StateManager;
	delete m_ipc;
	_Msg->MSGDRIV_destroy(_Handle);
	delete _Msg;
}

void CEventManager::IPC_Creat()
{
    Ipc_init();
   	int ret = Ipc_create();
	if(ret == -1)
	{
		printf("[%s] %d ipc create error \n", __func__, __LINE__);
		return;
	}
	cfg_value = (int *)ipc_getSharedMem(IPC_IMG_SHA);
	usr_value = (char *)ipc_getSharedMem(IPC_USER_SHA);
}

void CEventManager::thread_ipcEvent()
{
	float value;
	float errorx, errory;
	int cfgid = -1;
	while(!pThis->exit_ipcthread)
	{
		cfgid = pThis->m_ipc->IPCRecvMsg(NULL);
		switch(cfgid)
		{
			case CFGID_RTS_mainch:
				pThis->signalFeedBack(4, pThis->outcomtype, ACK_Sensor, 1, (int)pThis->cfg_value[CFGID_RTS_mainch]);
				break;
			case CFGID_RTS_mainch2:
				pThis->signalFeedBack(4, pThis->outcomtype, ACK_Sensor, 2, (int)pThis->cfg_value[CFGID_RTS_mainch]);
				break;
			case CFGID_RTS_trken:
				pThis->signalFeedBack(3, pThis->outcomtype, ACK_TrkStat, (int)pThis->cfg_value[CFGID_RTS_trken]);
				break;
			case CFGID_RTS_sectrkstat:
			{
				int sectrkstat = pThis->cfg_value[CFGID_RTS_sectrkstat];
				if((0 == sectrkstat) || (3 == sectrkstat))
					pThis->signalFeedBack(3, pThis->outcomtype, ACK_SectrkStat, 3);
				else if(1 == sectrkstat)
					pThis->signalFeedBack(3, pThis->outcomtype, ACK_SectrkStat, 1);
				else if(2 == sectrkstat)
					pThis->signalFeedBack(3, pThis->outcomtype, ACK_SectrkStat, 2);
			}
				break;	
			case CFGID_RTS_trkstat:

				pThis->m_pixelErr.status = pThis->cfg_value[CFGID_RTS_trkstat];
				memcpy(&value, pThis->cfg_value + CFGID_RTS_trkerrx, 4);
				pThis->m_pixelErr.errx = value;
				memcpy(&value, pThis->cfg_value + CFGID_RTS_trkerry, 4);
				pThis->m_pixelErr.erry = value;
				
				pThis->_Msg->MSGDRIV_send(MSGID_COM_INPUT_TRKCONTROL, 0);
				pThis->_StateManager->_state->recvTrkmsg();
					
				if(1 == pThis->outtype)
				{
					memcpy(&errorx, pThis->cfg_value + CFGID_RTS_trkerrx, 4);
					memcpy(&errory, pThis->cfg_value + CFGID_RTS_trkerry, 4);
					pThis->signalFeedBack_output(pThis->outcomtype, (int)pThis->cfg_value[CFGID_RTS_trkstat], pThis->outtype, errorx, errory);
				}else if(2 == pThis->outtype){
					float x,y;
					pThis->_StateManager->_state->getcurRate(x,y);
					pThis->signalFeedBack_output(pThis->outcomtype, (int)pThis->cfg_value[CFGID_RTS_trkstat], pThis->outtype, x, y);
				}
				break;

			case CFGID_RTS_mtddet:	
					pThis->_StateManager->_state->mtdhandle(pThis->cfg_value[CFGID_RTS_mtddet]);
				break;

			default:
				break;
		}
	}
}

void CEventManager::MSG_register()
{
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_TRACKCTRL, MSG_Trk, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_SwitchSensor, MSG_SwitchSensor, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_TRACKSEARCHCTRL, MSG_TrkSearch, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_captureMode, MSG_CaptureMode, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_IrisAndFocusAndExit, MSG_IrisAndFocus, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_workMode, MSG_WorkMode, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_JOSPOS, MSG_JosPos, NULL);

	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SELFCHECK, MSG_Com_SelfCheck, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_TRKCONTROL, MSG_Com_TrkMovControl, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_TRKMOVE, MSG_Com_TrkMove, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SECTRKPOS, MSG_Com_SecTrkPos, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_MTDSELECT, MSG_Com_MtdSelect, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_GATEMOVE, MSG_Com_Gatemove, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_FOCUSCTRL, MSG_Com_FocusCtrl, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_APERTURECTRL, MSG_Com_IrisCtrl, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_ZOOMCTRL, MSG_Com_ZoomCtrl, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETPLATSPEED, MSG_Com_SetPlatSpeed, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETPLATANGLE, MSG_Com_SetPlatAngle, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_PREPOS, MSG_Com_PreposHandle, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETZOOM, MSG_Com_SetZoom, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_QUERYPTZPOS, MSG_Com_QueryPtzPos, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_GETZOOM, MSG_Com_GetZoom, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_TRKOUTPUT, MSG_Com_TrkOutput, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETCFG, MSG_Com_SetCfg, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_GETCFG, MSG_Com_GetCfg, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETOSD, MSG_Com_SetOsd, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_DEFAULTCFG, MSG_Com_DefaultCfg, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SAVECFG, MSG_Com_SaveCfg, NULL);

	return ;
}

void CEventManager::MSG_Trk(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	if(tmp->trkctrl == 0)
		tmp->trkctrl = !pThis->cfg_value[CFGID_RTS_trkstat];
	if(tmp->trkctrl == 2)
		tmp->trkctrl = 0;
	pThis->_StateManager->inter_TrkCtrl(tmp->trkctrl);
	pThis->outcomtype = tmp->comtype;
}


void CEventManager::MSG_SwitchSensor(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int displaychid = tmp->displaychid;
	int capturechid = tmp->capturechid;
	pThis->_StateManager->inter_SwitchSensor(capturechid);
	pThis->outcomtype = tmp->comtype;
}



void CEventManager::MSG_TrkSearch(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	if(tmp->sectrkctrl == 2)
		pThis->_StateManager->inter_TrkSearch(tmp->sectrkctrl, pThis->winPos[0], pThis->winPos[1]);
	if(!pThis->cfg_value[CFGID_RTS_trken])
		tmp->sectrkctrl = 0;
	pThis->outcomtype = tmp->comtype;

	int sectrkstat = pThis->cfg_value[CFGID_RTS_sectrkstat];
	if(1 == sectrkstat)
		pThis->_StateManager->_state->m_Platform->PlatformCtrl_reset4trk(pThis->_StateManager->_state->m_plt);
	return ;
}



void CEventManager::MSG_CaptureMode(void* p)
{
	static int captureModeBack;
	ComParams_t *tmp = (ComParams_t *)p;
	if(tmp->workmode == ROUTINE_STATE)
	{
		if(tmp->capturemode == 0)
			pThis->_StateManager->ToPlatFormCapture();
		else if(tmp->capturemode == 1)
			pThis->_StateManager->ToBoxCapture();
		else if(tmp->capturemode == 2)
			pThis->_StateManager->ToManualMtdCapture();
		captureModeBack = tmp->capturemode;
	}
	else
		tmp->capturemode = captureModeBack;

	int capturemode = pThis->_StateManager->CurStateInterface();
	if(STATE_PTZ == capturemode)
		pThis->signalFeedBack(3, tmp->comtype, ACK_CaptureMode, 1);
	else if(STATE_BOX == capturemode)
		pThis->signalFeedBack(3, tmp->comtype, ACK_CaptureMode, 2);
	else if(STATE_MANUALMTD == capturemode)
		pThis->signalFeedBack(3, tmp->comtype, ACK_CaptureMode, 3);
	return ;
}


void CEventManager::MSG_IrisAndFocus(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	if(tmp->irisctrl == 0 && tmp->focusctrl == 0)
		pThis->_StateManager->inter_Iris_FocusCtrl(Exit, 0);
	printf("tmp->irisctrl = %d , tmp->focusctrl = %d \n", tmp->irisctrl, tmp->focusctrl);
	return ;
}


void CEventManager::MSG_WorkMode(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	if(tmp->workmode == ROUTINE_STATE)
	{
		if(tmp->capturemode == 0)
			pThis->_StateManager->ToPlatFormCapture();
		else if(tmp->capturemode == 1)
			pThis->_StateManager->ToBoxCapture();
		else if(tmp->capturemode == 2)
			pThis->_StateManager->ToManualMtdCapture();
	}
	else if(tmp->workmode == STATE_AUTOMTD)
		pThis->_StateManager->ToStateAuto_Mtd();
	else if(tmp->workmode == STATE_SCENETRK)
		pThis->_StateManager->ToStateSceneSelect();

	int workmode = pThis->_StateManager->CurStateInterface();
	if((ROUTINE_STATE == workmode) || (STATE_PTZ == workmode) ||(STATE_BOX == workmode) || (STATE_MANUALMTD == workmode))
		pThis->signalFeedBack(3, tmp->comtype, ACK_Workmode, 1);
	else if(STATE_AUTOMTD == workmode)
		pThis->signalFeedBack(3, tmp->comtype, ACK_Workmode, 2);
	else if(STATE_SCENETRK == workmode)
		pThis->signalFeedBack(3, tmp->comtype, ACK_Workmode, 3);
	return ;
}



void CEventManager::MSG_JosPos(void* p)
{
	int dir;
	ComParams_t *tmp = (ComParams_t *)p;
	if(tmp->sectrkctrl == 0x01)
	{
		if(!pThis->cfg_value[CFGID_RTS_trken])
			tmp->sectrkctrl = 0;
		pThis->winPos[0] = 1920 * ((float)tmp->platspeedx / JOS_VALUE_MAX);
		pThis->winPos[1] = 1080 * ((float)tmp->platspeedy / JOS_VALUE_MAX);
		pThis->_StateManager->inter_TrkSearch(tmp->sectrkctrl, (int)pThis->winPos[0], (int)pThis->winPos[1]);
	}
	else if(tmp->irisctrl == 0x03)
	{
		if(tmp->platspeedy > 0xb0)
			dir = -1;
		else if(tmp->platspeedy < 0x40)
			dir = 1;
		else
			dir = 0;
		pThis->_StateManager->inter_Iris_FocusCtrl(iris, dir);
	}
	else if(tmp->focusctrl == 0x03)
	{
		if(tmp->platspeedy > 0xb0)
			dir = -1;
		else if(tmp->platspeedy < 0x40)
			dir = 1;
		else
			dir = 0;
		pThis->_StateManager->inter_Iris_FocusCtrl(Focus, dir);
	}
	else if(pThis->cfg_value[CFGID_RTS_trkstat])
	{
		//do nothing
	}
	else
	{
		pThis->_StateManager->inter_AxisMove(tmp->platspeedx, tmp->platspeedy);
	}
	return ;
}

void CEventManager::MSG_Com_SelfCheck(void* p)
{
	printf("MSG_Com_SelfCheck start\n");
	return ;
}


void CEventManager::MSG_Com_TrkMovControl(void* p)
{	
	pThis->_StateManager->_state->trkMovControl(pThis->m_pixelErr.status , pThis->m_pixelErr.errx , pThis->m_pixelErr.erry );
	return ;
}


void CEventManager::MSG_Com_TrkMove(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int trkmove = tmp->trkmove;
	int x = 0, y =0;
	if(trkmove&(0x1<<0))
		x = 0x1;
	else if(trkmove & (0x1<<1))
		x = 0x2;

	if(trkmove&(0x1<<2))
		y = 0x1;
	else if(trkmove & (0x1<<3))
		y = 0x2;

	pThis->_StateManager->_state->pov_move( x, y);
	return ;
}


void CEventManager::MSG_Com_SecTrkPos(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	pThis->_StateManager->inter_TrkSearch(tmp->sectrkctrl, tmp->sectrkx, tmp->sectrky);
	return ;
}


void CEventManager::MSG_Com_MtdSelect(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int mtdselect = tmp->mtdselect;
	pThis->_StateManager->_state->switchTarget(tmp->mtdselect);
	return ;
}


void CEventManager::MSG_Com_Gatemove(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	pThis->_StateManager->_state->moveAcqrect(tmp->gatemove,tmp->gatestepx,tmp->gatestepy);
	return ;
}


void CEventManager::MSG_Com_ZoomCtrl(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	pThis->_StateManager->inter_ZoomCtrl(tmp->zoomctrl);
	return ;
}


void CEventManager::MSG_Com_IrisCtrl(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	pThis->_StateManager->inter_Iris_FocusCtrl(iris, tmp->irisctrl);
	return ;
}


void CEventManager::MSG_Com_FocusCtrl(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	pThis->_StateManager->inter_Iris_FocusCtrl(Focus, tmp->focusctrl);
	return;
}


void CEventManager::MSG_Com_SetPlatSpeed(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	pThis->_StateManager->_state->virtualAxisMove(tmp->platspeedx,tmp->platspeedy);
	return ;
}


void CEventManager::MSG_Com_SetPlatAngle(void* p)
{
	int pan, til;
	ComParams_t *tmp = (ComParams_t *)p;
	unsigned short platPan = tmp->platPan;
	unsigned short platTilt = tmp->platTilt;
	
	pThis->_StateManager->_state->_ptz->ptzSetPos(platPan, platTilt);
	
	pThis->_StateManager->_state->_ptz->queryPos();
	pThis->_StateManager->_state->_ptz->getpos(pan, til);
	pThis->signalFeedBack(4, tmp->comtype, ACK_CtrlPos, pan, til);
	return ;
}


void CEventManager::MSG_Com_PreposHandle(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;	

	pThis->_StateManager->_state->PreposHandle(tmp->prepos);
	if(2 == tmp->prepos)
		pThis->SaveConfig(tmp->comtype);

	return ;
}


void CEventManager::MSG_Com_SetZoom(void* p)
{
	int zoom;
	ComParams_t *tmp = (ComParams_t *)p;
	unsigned short setzoom = tmp->setzoom;
	pThis->_StateManager->_state->_ptz->setZoomPos(setzoom);

	pThis->_StateManager->_state->_ptz->queryZoom();
	pThis->_StateManager->_state->_ptz->getzoom(zoom);
	pThis->signalFeedBack(3, tmp->comtype, ACK_SetZoom, zoom);
	return ;
}


void CEventManager::MSG_Com_QueryPtzPos(void* p)
{
	int pan, til;
	ComParams_t *tmp = (ComParams_t *)p;


	//pThis->_StateManager->_state->_ptz->queryPos();
	//pThis->_StateManager->_state->_ptz->getpos(pan, til);
	pThis->signalFeedBack(4, tmp->comtype, ACK_QueryPos, pan, til);
	return ;
}


void CEventManager::MSG_Com_GetZoom(void* p)
{
	int zoom;
	ComParams_t *tmp = (ComParams_t *)p;
	
	pThis->_StateManager->_state->_ptz->queryZoom();
	pThis->_StateManager->_state->_ptz->getzoom(zoom);
	
	pThis->signalFeedBack(3, tmp->comtype, ACK_QueryZoom, zoom);

	return ;
}


void CEventManager::MSG_Com_TrkOutput(void* p)
{
	Set_config_t settmp;
	ComParams_t *tmp = (ComParams_t *)p;
	unsigned short trkoutput = tmp->trkoutput;
	pThis->outtype = tmp->trkoutput;
	
	settmp.block= CFGID_PTZ_BKID;
	settmp.field= 15;
	memcpy(&settmp.value, &tmp->trkoutput, 4);
	tmp->setConfigQueue.push_back(settmp);
	
	pThis->_Msg->MSGDRIV_send(MSGID_COM_INPUT_SETCFG, p);
	return ;
}


void CEventManager::MSG_Com_SetCfg(void* p)
{
	Set_config_t tmpcfg;
	ComParams_t *tmp = (ComParams_t *)p;
	while(tmp->setConfigQueue.size()){
		tmpcfg = tmp->setConfigQueue[0];
		printf("setcfg block,field,value(%d,%d,%f)\n", tmpcfg.block,tmpcfg.field,tmpcfg.value);
		OSA_semWait(&m_semHndl_s, OSA_TIMEOUT_FOREVER);
		pThis->SetConfig(tmp->comtype, tmpcfg.block, tmpcfg.field, tmpcfg.value, NULL);
		tmp->setConfigQueue.erase(tmp->setConfigQueue.begin());
	}
}
void CEventManager::MSG_Com_GetCfg(void* p)
{
	Get_config_t tmpcfg;
	ComParams_t *tmp = (ComParams_t *)p;
	while(tmp->getConfigQueue.size()){
		tmpcfg = tmp->getConfigQueue[0];
		printf("getcfg block,field(%d,%d)\n", tmpcfg.block,tmpcfg.field);
		OSA_semWait(&m_semHndl_s, OSA_TIMEOUT_FOREVER);
		pThis->GetConfig(tmp->comtype, tmpcfg.block, tmpcfg.field);
		tmp->getConfigQueue.erase(tmp->getConfigQueue.begin());
	}
	return ;
}


void CEventManager::MSG_Com_SetOsd(void* p)
{
	osdbuffer_t tmpcfg;
	ComParams_t *tmp = (ComParams_t *)p;
	tmpcfg = tmp->setosd;

	int index = tmpcfg.osdID - 1;
	int length = strlen((char *)tmpcfg.buf);
	memset(pThis->usr_value + index * USEROSD_LENGTH, 0, USEROSD_LENGTH );
	memcpy(pThis->usr_value + index * USEROSD_LENGTH, tmpcfg.buf, length<USEROSD_LENGTH?length:USEROSD_LENGTH);
	printf("osdid:%d, type:%d, buf=%s\n",tmpcfg.osdID, tmpcfg.type, tmpcfg.buf);
	pThis->m_ipc->IPCSendMsg(read_shm_usrosd, &index, 4);
	return ;
}


void CEventManager::MSG_Com_DefaultCfg(void* p)
{
	int status , block;
	ComParams_t *tmp = (ComParams_t *)p;
	while(tmp->defConfigQueue.size()){
		block = tmp->defConfigQueue.at(0);
		printf("default block=%d\n", block);
		OSA_semWait(&m_semHndl_s, OSA_TIMEOUT_FOREVER);
		printf("after default block=%d\n", block);
		pThis->DefaultConfig(tmp->comtype, block);
		tmp->defConfigQueue.erase(tmp->defConfigQueue.begin());
	}
	return ;
}


void CEventManager::MSG_Com_SaveCfg(void* p)
{
	printf("MSG_Com_SaveCfg start\n");
	ComParams_t *tmp = (ComParams_t *)p;
	pThis->SaveConfig(tmp->comtype);
	return ;
}

int  CEventManager::ReadConfigFile()
{	
	string str;
	string cfgAvtFile;
	int configId_Max = profileNum;
	char  cfg_avt[30] = "cfg_avt_";
	cfgAvtFile = "Profile.yml";
	FILE *fp = fopen(cfgAvtFile.c_str(), "rt");
	if(fp != NULL){
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		if(len < 10)
			return  -1;
		else
		{
			FileStorage fr(cfgAvtFile, FileStorage::READ);
			if(fr.isOpened())
			{
				for(int i=0; i<configId_Max; i++)
				{
					int blkId = (i/CFGID_FEILD_MAX);
					int feildId = (i%CFGID_FEILD_MAX);
					int usrosdId = -1;
					sprintf(cfg_avt, "cfg_avt_%03d_%02d", blkId, feildId);
					if((blkId >= CFGID_OSD_BKID) && (blkId <= CFGID_OSD_BKID + 15))
						usrosdId = blkId - CFGID_OSD_BKID;
					else if((blkId >= CFGID_OSD2_BKID) && (blkId <= CFGID_OSD2_BKID + 15))
						usrosdId = blkId - CFGID_OSD2_BKID + 16;
					if((usrosdId >= 0) && (i == CFGID_OSD_CONTENT(usrosdId) || i == CFGID_OSD2_CONTENT(usrosdId)))
					{
						str = (string)fr[cfg_avt];
						memset(usr_value + usrosdId * USEROSD_LENGTH, 0, USEROSD_LENGTH);
						str.copy(usr_value+usrosdId*USEROSD_LENGTH, str.length()<USEROSD_LENGTH?str.length():USEROSD_LENGTH, 0);
					}
					else if(i == CFGID_PTZ_netip)
					{
						str = (string)fr[cfg_avt];
						unsigned int intip = stringip2int(str);
						memcpy(cfg_value + i, &intip, 4);
					}
					else
					{
						if(is_float(i))
						{
							float value = (float)(fr[cfg_avt]);
							memcpy(cfg_value + i, &value, 4);
						}
						else
						{
							int value = (int)fr[cfg_avt];
							memcpy(cfg_value + i, &value, 4);
						}
					}
				}
			}
			else
			{
				printf("[get params]open YML failed\n");
				exit(-1);
			}
		}
	}
	else
	{
		printf("[get params] Can not find YML. Please put this file into the folder of execute file\n");
		exit (-1);
	}
	m_ipc->IPCSendMsg(read_shm_config, NULL, 0);
	return 0;
}

int CEventManager::SetConfig(comtype_t comtype, int block, int field, int value,char *inBuf)
{
	block -= 1;
	int i = CFGID_BUILD(block, field);
	int usrosdId = -1;
	
	if((block >= CFGID_OSD_BKID) && (block <= CFGID_OSD_BKID + 15))
		usrosdId = block - CFGID_OSD_BKID;
	else if((block >= CFGID_OSD2_BKID) && (block <= CFGID_OSD2_BKID + 15))
		usrosdId = block - CFGID_OSD2_BKID + 16;
	if((usrosdId >= 0) && (i == CFGID_OSD_CONTENT(usrosdId) || i == CFGID_OSD2_CONTENT(usrosdId)))
	{
		return 0;
	}
	
	memcpy(cfg_value + i, &value, 4);
	m_ipc->IPCSendMsg(read_shm_single, &i, 4);

	if(((block >= CFGID_INPUT1_BKID) && (block <= CFGID_INPUT1_BKID + 6)) || ((block >= CFGID_INPUT2_BKID) && (block <= CFGID_INPUT5_BKID + 6)) )
		_StateManager->_state->m_Platform->PlatformCtrl_sensor_Init(cfg_value);

	int value2;
	memcpy(&value2, cfg_value + i, 4);
	Set_config_t tmp = {block+1, field, value2};
	ACK_ComParams.comtype = comtype;
	ACK_ComParams.cmdid  = ACK_SetConfig;
	ACK_ComParams.setConfigQueue.push_back(tmp);
	OSA_semSignal(&m_semHndl);

	return 0;
}
int CEventManager::GetConfig(comtype_t comtype, int block, int field)
{
	int app_block = block - 1;
	int i = CFGID_BUILD(app_block, field);
	int usrosdId = -1;
	
	if((app_block >= CFGID_OSD_BKID) && (app_block <= CFGID_OSD_BKID + 15))
		usrosdId = app_block - CFGID_OSD_BKID;
	else if((app_block >= CFGID_OSD2_BKID) && (app_block <= CFGID_OSD2_BKID + 15))
		usrosdId = app_block - CFGID_OSD2_BKID + 16;
	if((usrosdId >= 0) && (i == CFGID_OSD_CONTENT(usrosdId) || i == CFGID_OSD2_CONTENT(usrosdId)))
	{
		Get_osd_t tmp = {block, field, {0}};
		int length = strlen(usr_value + usrosdId * USEROSD_LENGTH)<128?strlen(usr_value + usrosdId * USEROSD_LENGTH):128;
		memcpy(tmp.buf, usr_value + usrosdId * USEROSD_LENGTH, length);
		ACK_ComParams.comtype = comtype;
		ACK_ComParams.cmdid  = ACK_GetOsd;
		ACK_ComParams.getOsdQueue.push_back(tmp);
	}
	else
	{
		//float value = cfg_value[i];
		int value;
		memcpy(&value, cfg_value + i, 4);
		Set_config_t tmp = {block, field, value};
		ACK_ComParams.comtype = comtype;
		ACK_ComParams.cmdid  = ACK_GetConfig;
		ACK_ComParams.getConfigQueue.push_back(tmp);
	}
	
	OSA_semSignal(&m_semHndl);

	return 0;
}
int CEventManager::DefaultConfig(comtype_t comtype, int blockId)
{
	printf("DefaultConfig blockId=%d\n", blockId);
	int sendid;
	blockId -= 1;
	string str;
	string cfgAvtFile;
	int configId_Max =profileNum;
	char  cfg_avt[20] = "cfg_avt_";
	cfgAvtFile = "Profile_default.yml";
	FILE *fp = fopen(cfgAvtFile.c_str(), "rt");

	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		if(len < 10)
			return  -1;
		else
		{
			FileStorage fr(cfgAvtFile, FileStorage::READ);
			if(fr.isOpened())
			{
				for(int i=0; i<configId_Max; i++)
				{
					int blkId = (i/CFGID_FEILD_MAX);
					int feildId = (i%CFGID_FEILD_MAX);
					int usrosdId = -1;
					
					sprintf(cfg_avt, "cfg_avt_%03d_%02d", blkId, feildId);

					if(blkId != CFGID_RTS_BKID)
					{
						if((-1 == blockId) || (blkId == blockId))
						{
							if((blkId >= CFGID_OSD_BKID) && (blkId <= CFGID_OSD_BKID + 15))
								usrosdId = blkId - CFGID_OSD_BKID;
							else if((blkId >= CFGID_OSD2_BKID) && (blkId <= CFGID_OSD2_BKID + 15))
								usrosdId = blkId - CFGID_OSD2_BKID + 16;
							if((usrosdId >= 0) && (i == CFGID_OSD_CONTENT(usrosdId) || i == CFGID_OSD2_CONTENT(usrosdId)))
							{
								str = (string)fr[cfg_avt];
								memset(usr_value + usrosdId * USEROSD_LENGTH, 0, USEROSD_LENGTH);
								str.copy(usr_value+usrosdId*USEROSD_LENGTH, str.length()<USEROSD_LENGTH?str.length():USEROSD_LENGTH, 0);

								if(blockId != -1)
								{
									m_ipc->IPCSendMsg(read_shm_usrosd, &usrosdId, 4);
								}
							}
							else if(i == CFGID_PTZ_netip)
							{
								str = (string)fr[cfg_avt];
								unsigned int intip = stringip2int(str);
								memcpy(cfg_value + i, &intip, 4);
							}
							else
							{
								if(is_float(i))
								{
									float value = (float)(fr[cfg_avt]);
									memcpy(cfg_value + i, &value, 4);
								}
								else
								{
									int value = (int)fr[cfg_avt];
									memcpy(cfg_value + i, &value, 4);
								}
							}

							if(((blkId >= CFGID_INPUT1_BKID) && (blkId <= CFGID_INPUT1_BKID + 6)) || ((blkId >= CFGID_INPUT2_BKID) && (blkId <= CFGID_INPUT5_BKID + 6)) )
								_StateManager->_state->m_Platform->PlatformCtrl_sensor_Init(cfg_value);
						}
					}
				}

				if(-1 == blockId)
				{
					m_ipc->IPCSendMsg(read_shm_config, NULL, 0);
				}
				else
				{
					m_ipc->IPCSendMsg(read_shm_block, &blockId, 0);
				}

				signalFeedBack(3, comtype, ACK_DefaultConfig, blockId+1);

			}
			else
			{
				printf("[back to default] YML open failed\n");
			}
		}
	}
	else
	{
		printf("[back to default] Can not find YML. Please put this file into the folder of execute file\n");
	}
	return 0;
}
int CEventManager::SaveConfig(comtype_t comtype)
{
	printf("-----------save config------\n");
	string cfgAvtFile;
	int configId_Max = profileNum;
	char  cfg_avt[30] = "cfg_avt_";
	cfgAvtFile = "Profile.yml";
	int status = -1;

	FILE *fp = fopen(cfgAvtFile.c_str(), "rt+");

	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		
		if(len < 10)
			status  = 0;
		else
		{
			FileStorage fr(cfgAvtFile, FileStorage::WRITE);
			if(fr.isOpened())
			{
				for(int i=0; i<configId_Max; i++)
				{
					int blkId = (i/CFGID_FEILD_MAX);
					int feildId = (i%CFGID_FEILD_MAX);
					int usrosdId = -1;
					sprintf(cfg_avt, "cfg_avt_%03d_%02d", blkId, feildId);
					if((blkId >= CFGID_OSD_BKID) && (blkId <= CFGID_OSD_BKID + 15))
						usrosdId = blkId - CFGID_OSD_BKID;
					else if((blkId >= CFGID_OSD2_BKID) && (blkId <= CFGID_OSD2_BKID + 15))
						usrosdId = blkId - CFGID_OSD2_BKID + 16;
					if((usrosdId >= 0) && (i == CFGID_OSD_CONTENT(usrosdId) || i == CFGID_OSD2_CONTENT(usrosdId)))
					{
						int length = strlen(usr_value + usrosdId * USEROSD_LENGTH)<USEROSD_LENGTH?strlen(usr_value+ usrosdId * USEROSD_LENGTH):USEROSD_LENGTH;
						string str;
						for(int j = 0; j < length; j++)
							str += usr_value[usrosdId * USEROSD_LENGTH + j];
						
						fr<< cfg_avt << str;
					}
					else if(i == CFGID_PTZ_netip)
					{
						unsigned int intip;
						memcpy(&intip, cfg_value + i, 4);
						string str = intip2string(intip);
						fr<< cfg_avt << str;
					}
					else
					{
						if(is_float(i))
						{
							float value;
							memcpy(&value, cfg_value + i, 4);
							fr<< cfg_avt << value;
						}
						else
						{
							int value;
							memcpy(&value, cfg_value + i, 4);
							fr<< cfg_avt << value;
						}
					}
				}
				status  = 1;
				
			}
			else
			{
				status  = 0;
			}
		}
	}
	else
	{
		fclose(fp);
		status  = 0;
	}

	signalFeedBack(3, comtype, ACK_saveconfig, status);
		
}

void CEventManager::signalFeedBack(int argnum ...)
{
	int flag = 0;

	va_list ap;
	va_start(ap, argnum);
	ACK_ComParams.comtype = va_arg(ap, comtype_t);
	ACK_ComParams.cmdid  = va_arg(ap, int);
	printf("[%s]fd=%d,type=%d, cmdid=%d\n", __FUNCTION__, ACK_ComParams.comtype.fd, ACK_ComParams.comtype.type, ACK_ComParams.cmdid);
	
	switch(ACK_ComParams.cmdid)
	{
		case ACK_Sensor:
			ACK_ComParams.displaychid = va_arg(ap, int);
			ACK_ComParams.capturechid = va_arg(ap, int);
			printf("[%s]displaychid=%d,capturechid=%d\n", __FUNCTION__, ACK_ComParams.displaychid, ACK_ComParams.capturechid);
			break;
		case ACK_Workmode:
			ACK_ComParams.workmode = va_arg(ap, int);
			printf("[%s]workmode=%d\n", __FUNCTION__, ACK_ComParams.workmode);
			break;
		case ACK_CaptureMode:
			ACK_ComParams.capturemode = va_arg(ap, int);
			printf("[%s]capturemode=%d\n", __FUNCTION__, ACK_ComParams.capturemode);
			break;
		case ACK_TrkStat:
			ACK_ComParams.trkctrl = va_arg(ap, int);
			printf("[%s]rkctrl=%d\n", __FUNCTION__,ACK_ComParams.trkctrl);
			break;
		case ACK_SectrkStat:
			ACK_ComParams.sectrkctrl = va_arg(ap, int);
			printf("[%s]sectrkctrl=%d\n", __FUNCTION__, ACK_ComParams.sectrkctrl);
			break;
		case ACK_CtrlPos:
			ACK_ComParams.ctrlpan = va_arg(ap, int);
			ACK_ComParams.ctrltil = va_arg(ap, int);
			break;
		case ACK_SetZoom:
			ACK_ComParams.setzoom = va_arg(ap, int);
			break;
		case ACK_QueryPos:
			ACK_ComParams.querypan = va_arg(ap, int);
			ACK_ComParams.querytil = va_arg(ap, int);
			break;
		case ACK_QueryZoom:
			ACK_ComParams.queryzoom = va_arg(ap, int);
			break;
		case ACK_DefaultConfig:
			ACK_ComParams.defConfigQueue.push_back(va_arg(ap, int));
			printf("[%s], default block=%d\n", __FUNCTION__, ACK_ComParams.defConfigQueue[0]);
			break;
		case ACK_saveconfig:
			ACK_ComParams.saveconfig = va_arg(ap, int);
			break;
		default:
			flag = 1;
			break;
	}
	va_end(ap);
	if(!flag)
	{
		OSA_semSignal(&m_semHndl);
	}
}
void CEventManager::signalFeedBack_output(comtype_t comtype, int trkstat, int outtype, float x, float y)
{
	ACK_ComParams.comtype = comtype;
	ACK_ComParams.cmdid  = ACK_output;
	ACK_ComParams.trkstat = trkstat;
	ACK_ComParams.outtype = outtype;
	ACK_ComParams.trkerrx = x;
	ACK_ComParams.trkerry = y;
	printf("[%s]trkstat=%d,outtype=%d,x,y=(%f,%f)\n", __FUNCTION__, ACK_ComParams.trkstat, ACK_ComParams.outtype, ACK_ComParams.trkerrx, ACK_ComParams.trkerry);
	OSA_semSignal(&m_semHndl);
}
int CEventManager::is_float(int cfgid)
{
	int float_cfgid[profileNum] = {
		CFGID_JOS_deadx,
		CFGID_JOS_deady,
		CFGID_JOS_inputgainx1,
		CFGID_JOS_inputgainy1,
		CFGID_JOS_inputgainx2,
		CFGID_JOS_inputgainy2,
		CFGID_JOS_cutpointx1,
		CFGID_JOS_cutpointy1,
		CFGID_JOS_cutpointx2,
		CFGID_JOS_cutpointy2,
		CFGID_PTZ_stopbit,

		CFGID_TRK_BASE,
		CFGID_TRK_BASE+1,
		CFGID_TRK_BASE+2,
		CFGID_TRK_BASE+3,
		CFGID_TRK_BASE+4,
		CFGID_TRK_BASE+5,
		CFGID_TRK_BASE+6,
		CFGID_TRK_BASE+7,
		CFGID_TRK_BASE+8,
		CFGID_TRK_BASE+9,
		CFGID_TRK_BASE+10,
		CFGID_TRK_BASE+11,
		CFGID_TRK_BASE+12,
		CFGID_TRK_BASE+13,
		CFGID_TRK_BASE+14,
		CFGID_TRK_BASE+15,

		CFGID_TRK_BASE2,
		CFGID_TRK_BASE2+1,
		CFGID_TRK_BASE2+2,
		CFGID_TRK_BASE2+3,
		CFGID_TRK_BASE2+4,
		CFGID_TRK_BASE2+5,
		CFGID_TRK_BASE2+6,
		CFGID_TRK_BASE2+7,
		CFGID_TRK_BASE2+8,
		CFGID_TRK_BASE2+9,
		CFGID_TRK_BASE2+10,
		CFGID_TRK_BASE2+11,
		CFGID_TRK_BASE2+12,
		CFGID_TRK_BASE2+13,
		CFGID_TRK_BASE2+14,
		CFGID_TRK_BASE2+15,
		
		CFGID_TRK_BASE3,
		CFGID_TRK_BASE3+1,
		CFGID_TRK_BASE3+2,
		CFGID_TRK_BASE3+3,
		CFGID_TRK_BASE3+4,
		CFGID_TRK_BASE3+5,
		CFGID_TRK_BASE3+6,
		CFGID_TRK_BASE3+7,
		CFGID_TRK_BASE3+8,
		CFGID_TRK_BASE3+9,
		CFGID_TRK_BASE3+10,
		CFGID_TRK_BASE3+11,
		CFGID_TRK_BASE3+12,
		CFGID_TRK_BASE3+13,
		CFGID_TRK_BASE3+14,
		CFGID_TRK_BASE3+15,
		
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 1),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 1),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 2),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 2),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 3),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 3),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 4),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 4),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 5),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 5),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 6),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 6),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 7),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 7),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 8),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 8),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 9),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 9),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 10),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 10),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 11),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 11),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 12),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 12),
		CFGID_INPUT_FOVX(CFGID_INPUT1_BKID , 13),
		CFGID_INPUT_FOVY(CFGID_INPUT1_BKID , 13),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 1),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 2),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 3),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 4),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 5),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 6),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 7),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 8),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 9),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 10),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 11),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 12),
		CFGID_INPUT_Feedback(CFGID_INPUT1_BKID, 13),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 1),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 1),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 2),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 2),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 3),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 3),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 4),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 4),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 5),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 5),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 6),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 6),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 7),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 7),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 8),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 8),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 9),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 9),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 10),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 10),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 11),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 11),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 12),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 12),
		CFGID_INPUT_boresightX(CFGID_INPUT1_BKID, 13),
		CFGID_INPUT_boresightY(CFGID_INPUT1_BKID, 13),

		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 1),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 1),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 2),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 2),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 3),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 3),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 4),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 4),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 5),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 5),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 6),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 6),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 7),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 7),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 8),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 8),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 9),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 9),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 10),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 10),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 11),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 11),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 12),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 12),
		CFGID_INPUT_FOVX(CFGID_INPUT2_BKID , 13),
		CFGID_INPUT_FOVY(CFGID_INPUT2_BKID , 13),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 1),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 2),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 3),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 4),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 5),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 6),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 7),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 8),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 9),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 10),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 11),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 12),
		CFGID_INPUT_Feedback(CFGID_INPUT2_BKID, 13),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 1),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 1),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 2),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 2),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 3),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 3),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 4),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 4),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 5),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 5),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 6),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 6),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 7),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 7),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 8),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 8),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 9),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 9),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 10),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 10),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 11),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 11),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 12),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 12),
		CFGID_INPUT_boresightX(CFGID_INPUT2_BKID, 13),
		CFGID_INPUT_boresightY(CFGID_INPUT2_BKID, 13),

		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 1),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 1),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 2),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 2),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 3),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 3),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 4),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 4),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 5),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 5),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 6),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 6),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 7),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 7),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 8),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 8),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 9),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 9),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 10),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 10),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 11),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 11),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 12),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 12),
		CFGID_INPUT_FOVX(CFGID_INPUT3_BKID , 13),
		CFGID_INPUT_FOVY(CFGID_INPUT3_BKID , 13),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 1),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 2),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 3),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 4),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 5),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 6),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 7),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 8),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 9),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 10),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 11),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 12),
		CFGID_INPUT_Feedback(CFGID_INPUT3_BKID, 13),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 1),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 1),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 2),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 2),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 3),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 3),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 4),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 4),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 5),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 5),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 6),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 6),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 7),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 7),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 8),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 8),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 9),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 9),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 10),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 10),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 11),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 11),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 12),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 12),
		CFGID_INPUT_boresightX(CFGID_INPUT3_BKID, 13),
		CFGID_INPUT_boresightY(CFGID_INPUT3_BKID, 13),

		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 1),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 1),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 2),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 2),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 3),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 3),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 4),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 4),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 5),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 5),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 6),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 6),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 7),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 7),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 8),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 8),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 9),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 9),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 10),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 10),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 11),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 11),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 12),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 12),
		CFGID_INPUT_FOVX(CFGID_INPUT4_BKID , 13),
		CFGID_INPUT_FOVY(CFGID_INPUT4_BKID , 13),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 1),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 2),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 3),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 4),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 5),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 6),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 7),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 8),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 9),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 10),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 11),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 12),
		CFGID_INPUT_Feedback(CFGID_INPUT4_BKID, 13),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 1),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 1),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 2),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 2),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 3),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 3),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 4),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 4),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 5),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 5),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 6),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 6),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 7),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 7),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 8),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 8),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 9),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 9),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 10),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 10),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 11),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 11),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 12),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 12),
		CFGID_INPUT_boresightX(CFGID_INPUT4_BKID, 13),
		CFGID_INPUT_boresightY(CFGID_INPUT4_BKID, 13),

		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 1),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 1),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 2),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 2),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 3),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 3),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 4),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 4),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 5),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 5),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 6),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 6),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 7),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 7),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 8),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 8),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 9),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 9),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 10),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 10),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 11),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 11),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 12),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 12),
		CFGID_INPUT_FOVX(CFGID_INPUT5_BKID , 13),
		CFGID_INPUT_FOVY(CFGID_INPUT5_BKID , 13),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 1),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 2),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 3),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 4),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 5),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 6),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 7),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 8),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 9),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 10),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 11),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 12),
		CFGID_INPUT_Feedback(CFGID_INPUT5_BKID, 13),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 1),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 1),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 2),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 2),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 3),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 3),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 4),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 4),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 5),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 5),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 6),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 6),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 7),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 7),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 8),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 8),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 9),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 9),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 10),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 10),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 11),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 11),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 12),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 12),
		CFGID_INPUT_boresightX(CFGID_INPUT5_BKID, 13),
		CFGID_INPUT_boresightY(CFGID_INPUT5_BKID, 13),

		CFGID_PID_KPX,
		CFGID_PID_KIX,
		CFGID_PID_KDX,
		CFGID_PID_KX,
		CFGID_PID_KPY,
		CFGID_PID_KIY,
		CFGID_PID_KDY,
		CFGID_PID_KY,

		-1
	};

	for(int i = 0; i < profileNum; i++)
	{
		if(-1 == float_cfgid[i])
			return 0;
		
		if(cfgid == float_cfgid[i])
			return 1;
	}

	return 0;
}

string CEventManager::intip2string(unsigned int intip)
{
	int value0, value1, value2, value3;
	value0 = ((intip >> 24) & 0xff);
	value1 = ((intip >> 16) & 0xff);
	value2 = ((intip >> 8) & 0xff);
	value3 = intip & 0xff;

	string str = to_string(value0) + "." + to_string(value1) + "." + to_string(value2) + "." + to_string(value3);
	return str;
}
unsigned int CEventManager::stringip2int(string str)
{
	unsigned int intip = 0;
	int value;
	vector<string> AllStr = csplit(str, ".");
	for(int i = 0; i < AllStr.size(); i++)
	{
		if(i > 3)
			break;
		
		value = atoi(AllStr[i].c_str());
		int offset = (3 - i) * 8;
		intip |= ((value & 0xff) << offset);
	}
	return intip;
}

vector<string> CEventManager::csplit(const string& str, const string& delim) 
{  
	vector<string> res;  
	if("" == str) return res;  

	char strs[str.length() + 1] ;
	strcpy(strs, str.c_str());   
 
	char d[delim.length() + 1];  
	strcpy(d, delim.c_str());  
 
	char *p = strtok(strs, d);  
	while(p) {  
		string s = p;
		res.push_back(s);
		p = strtok(NULL, d);  
	}  
 
	return res;  
} 
