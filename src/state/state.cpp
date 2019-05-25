#include "State.h"
#include <iostream>

using namespace std;
State* pThis = NULL;
State* State::m_st1 = NULL;
State* State::m_st2 = NULL;
State* State::m_st3 = NULL;
State* State::m_st4 = NULL;
State* State::m_st5 = NULL;

CPTZControl* State::_ptz = NULL;
AgreeMentBaseFormat* State::_agreement = NULL;
CPlatformInterface* State::m_Platform = NULL;
CPTZSpeedTransfer*  State::m_ptzSpeed = NULL;
CIPCProc* State::m_ipc = NULL;
int* State::cfg_value = NULL;
View* State::viewParam = NULL;
HPLTCTRL  State::m_plt = NULL;
int State::curState = STATE_PTZ;
PlatformCtrl_CreateParams State::m_cfgPlatParam = {0};
PLATFORMCTRL_TrackerInput State::m_pltInput = {0};
PLATFORMCTRL_Output State::m_pltOutput; 
DxTimer* State::m_timer = NULL;

State::State()
{
	if(pThis == NULL)
		pThis = this;
	if(_agreement == NULL)
		_agreement = new IPelcoDFormat();
	if(_ptz == NULL)
		_ptz = new CPTZControl(_agreement,notifyZoom);
	if(m_Platform == NULL)
		m_Platform = new CplatFormControl();
	if(m_ptzSpeed == NULL)
		m_ptzSpeed = new CPTZSpeedTransfer();
	if(m_ipc == NULL)
		m_ipc = new CIPCProc();
	if(m_timer == NULL)
		m_timer = new DxTimer();
	
	selectch = {1, 1, 1, 1, 1, 0};
	curValidChid = selectch.idx;
}

State::~State()
{
}

void State::OperationInterface(StateManger* con)
{
	if(cfg_value[CFGID_RTS_mtden])
	{
		ipcParam.intPrm[0] = 0;
		m_ipc->IPCSendMsg(mtd, ipcParam.intPrm, 4);
	}

	if(cfg_value[CFGID_RTS_trken])
	{
		ipcParam.intPrm[0] = 0;
		if(cfg_value[CFGID_RTS_trkmode])
			m_ipc->IPCSendMsg(sceneTrk, ipcParam.intPrm, 4);
		else
			m_ipc->IPCSendMsg(trk, ipcParam.intPrm, 4);
	}	
	return ;
}

void State::platformCreate()
{
	viewParam = m_Platform->sensorParams();
	m_Platform->PlatformCtrl_sensor_Init(cfg_value);
	
	
	m_Platform->PlatformCtrl_CreateParams_Init(&m_cfgPlatParam, cfg_value, viewParam);
	OSA_assert(m_plt == NULL);
	m_plt = m_Platform->PlatformCtrl_Create(&m_cfgPlatParam);
}

void State::StateInit()
{
	if(m_st1 == NULL)
		m_st1 = new StateAuto_Mtd();
	if(m_st2 == NULL)
		m_st2 = new StateSceneSelect();
	if(m_st3 == NULL)
		m_st3 = new PlatFormCapture();
	if(m_st4 == NULL)
		m_st4 = new BoxCapture();
	if(m_st5 == NULL)
		m_st5 = new ManualMtdCapture();
	platformCreate();

	_ptz->m_Mtd_Moitor_X = cfg_value[CFGID_PREPOS_preposx];	
	_ptz->m_Mtd_Moitor_Y = cfg_value[CFGID_PREPOS_preposy];
	_ptz->m_Mtd_Moitor_Zoom = cfg_value[CFGID_PREPOS_prezoom];
	return ;
}

int State::ChangeState(StateManger* con, char nextState)
{
	curState = nextState;
	switch(nextState)
	{
	case STATE_AUTOMTD:
		con->ChangeState(m_st1);
		break;

	case STATE_SCENETRK:
		con->ChangeState(m_st2);
		break;

	case STATE_PTZ:
		con->ChangeState(m_st3);
		break;

	case STATE_BOX:
		con->ChangeState(m_st4);
		break;

	case STATE_MANUALMTD:
		con->ChangeState(m_st5);
		break;
	}
	printf("change the state   = %d \n" , curState );
	return curState;
}

void State::TrkCtrl(char Enable)
{
	struct timeval tmp;
	ipcParam.intPrm[0] = Enable;
	m_ipc->IPCSendMsg(trk, ipcParam.intPrm, 4);
	if(Enable && m_plt != NULL)
	{	
		m_Platform->PlatformCtrl_reset4trk(m_plt);
	}
	if(!Enable)
	{
		_ptz->ptzStop();
		tmp.tv_sec = 0;
		tmp.tv_usec = 30*1000;
		select(0, NULL, NULL, NULL, &tmp);
		_ptz->ptzStop();
	}
	return ;
}

void State::switchSensor(char chid)
{
	switchSensor_interface(chid);
}

void State::ZoomCtrl(char type)
{
	if(type == 0x01)
		_ptz->m_iSetZoomSpeed = -1;
	else if(type == 0x02)
		_ptz->m_iSetZoomSpeed = 1;
	else
		_ptz->m_iSetZoomSpeed = 0;
	return ;
}


void State::virtualAxisMove(int x, int y)
{
	printf("x , y  = (%d  , %d) \n" , x, y);
	float fx,fy;
	fx = (float)x/32767;
	fy = (float)y/32767;
	axisMove_interface(fx , fy);
	return ;
}


void State::axisMove(int x, int y)
{
	float fx,fy;
	x -= (JOS_VALUE_MAX>>1);
	y -= (JOS_VALUE_MAX>>1);
	fx = (float)x/(JOS_VALUE_MAX>>1);
	fy = (float)y/(JOS_VALUE_MAX>>1);
	axisMove_interface(fx , fy);
	return ;
}

void State::trkSearch(int type, int x, int y)
{
	if(!cfg_value[CFGID_RTS_trken])
		return;
	
	if( x<0)
		x =0;
	else  if(x > 1920)
		x =1920;
	if( y<0)
		y =0;
	else  if(y > 1080)
		y =1080;

	if(cfg_value[CFGID_RTS_mainch] == video_pal)
	{
		x = (int)((float)x*720.0/1920.0);
		y = (int)((float)y*576.0/1080.0);
	}
	ipcParam.intPrm[0] = type;
	ipcParam.intPrm[1] = x;
	ipcParam.intPrm[2] = y;
	m_ipc->IPCSendMsg(sectrk, ipcParam.intPrm, 3*4);
	return ;
}


void State::Iris_FocusCtrl(int type, int dir)
{
	int alpha = 255;

	if(dir == -1)
	{
		uninit_time ();
	}
	else if(dir == 1)
	{
		uninit_time ();
	}
	else
	{
		alpha = 0;
		init_sigaction();
		init_time();
	}

	if(type == Exit)
	{
		alpha = 0;
		dir = 0;
		Ctrl_Iris(dir);
		Ctrl_Focus(dir);
	}
	ipcParam.intPrm[0] = dir;
	ipcParam.intPrm[1] = alpha;
	switch(type)
	{
		case iris:
			m_ipc->IPCSendMsg(Iris, ipcParam.intPrm, 2*4);
			Ctrl_Iris(dir);
			break;
		case Focus:
			m_ipc->IPCSendMsg(focus, ipcParam.intPrm, 2*4);
			Ctrl_Focus(dir);
			break;
		case Exit:
			m_ipc->IPCSendMsg(exit_IrisAndFocus, ipcParam.intPrm, 2*4);
			break;
		default:
			break;
	}
	return ;
}


void State::switchSensor_interface(int chid)
{
	int SensorStat = cfg_value[CFGID_RTS_mainch];
	if(chid == 0xff)
	{
		if(SensorStat != selectch.idx)
			selectch.idx = SensorStat;
		do{
				selectch.idx++;
				selectch.idx = selectch.idx%5;
		}while(!selectch.validCh[selectch.idx]);
		ipcParam.intPrm[0] = selectch.idx;
		m_ipc->IPCSendMsg(sensor, ipcParam.intPrm, 4);
	}
	else
	{
		if(selectch.validCh[chid] == 1)
		{
			ipcParam.intPrm[0] = selectch.idx = chid;
			m_ipc->IPCSendMsg(sensor, ipcParam.intPrm, 4);
		}
	}

	curValidChid = selectch.idx;
	return ;
}


void State::axisMove_interface(float x, float y)
{	
	m_Platform->PlatformCtrl_VirtualInput(m_plt, DevUsr_AcqJoystickXInput, x);
	m_Platform->PlatformCtrl_VirtualInput(m_plt, DevUsr_AcqJoystickYInput, y);

	m_pltInput.iTrkAlgState= 0;
	m_Platform->PlatformCtrl_TrackerInput(m_plt, &m_pltInput);
	m_Platform->PlatformCtrl_TrackerOutput(m_plt, &m_pltOutput);
//	printf("%s : LINE : %d   , x = %f , y = %f \n",__func__, __LINE__,  m_pltOutput.fPlatformDemandX, m_pltOutput.fPlatformDemandY);
	_ptz->m_iSetPanSpeed = m_ptzSpeed->GetPanSpeed(m_pltOutput.fPlatformDemandX);
	_ptz->m_iSetTiltSpeed = m_ptzSpeed->GetTiltSpeed(m_pltOutput.fPlatformDemandY);
	return ;
}


void State::Ctrl_Iris(int dir)
{
	if(dir < 0)
		_ptz->m_iSetIrisSpeed = -1;
	else if(dir  > 0)
		_ptz->m_iSetIrisSpeed = 1;
	else
		_ptz->m_iSetIrisSpeed = 0;
}

void State::Ctrl_Focus(int dir)
{
	if(dir < 0)
		_ptz->m_iSetFocusFarSpeed = -1;
	else if(dir > 0)
		_ptz->m_iSetFocusNearSpeed = 1;
	else{
		_ptz->m_iSetFocusFarSpeed = 0;
		_ptz->m_iSetFocusNearSpeed = 0;
	}
}

void State::init_sigaction()
{
	struct sigaction tact;

	tact.sa_handler = detectionFunc;
	tact.sa_flags = 0;
	sigemptyset (&tact.sa_mask);
	sigaction (SIGALRM, &tact, NULL);
}

void State::init_time()
{
	struct itimerval value;

	value.it_value.tv_sec = 6;
	value.it_value.tv_usec = 0;
	value.it_interval = value.it_value;
	/* set ITIMER_REAL */
	setitimer (ITIMER_REAL, &value, NULL);
}

void State::uninit_time()
{
	struct itimerval value;

	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 0;
	value.it_interval = value.it_value;
	/* set ITIMER_REAL */
	setitimer (ITIMER_REAL, &value, NULL);
}

void State::detectionFunc (int sign)
{
	pThis->uninit_time();
	pThis->ipcParam.intPrm[0] = 0;
	pThis->ipcParam.intPrm[1] = 0;
	m_ipc->IPCSendMsg(exit_IrisAndFocus, pThis->ipcParam.intPrm, 2*4);
}


void State::trkMovControl(int status,int errx,int erry)
{
	if(m_plt == NULL)
		return ;

	m_pltInput.iTrkAlgState = status;
	m_pltInput.fTargetBoresightErrorX = errx;
	m_pltInput.fTargetBoresightErrorY = erry;
	
	m_Platform->PlatformCtrl_TrackerInput(m_plt, &m_pltInput);
	m_Platform->PlatformCtrl_TrackerOutput(m_plt, &m_pltOutput);
	
	if(1)//(cfg_value[CFGID_PTZ_protype] == 1)
	{
		_ptz->m_iSetPanSpeed = m_ptzSpeed->GetPanSpeed((int)m_pltOutput.fPlatformDemandX);
		_ptz->m_iSetTiltSpeed = m_ptzSpeed->GetTiltSpeed((int)m_pltOutput.fPlatformDemandY);

		//printf("%s :    panSpeed :%d  , tilSpeed : %d  \n",__func__,_ptz->m_iSetPanSpeed,_ptz->m_iSetTiltSpeed);
	}
	return ;
}


void State::pov_move(int x,int y)
{
	ipcParam.intPrm[0] = x;
	ipcParam.intPrm[1] = y;	
	m_ipc->IPCSendMsg(posmove,ipcParam.intPrm,4*2);
	return ;
}

void State::PreposHandle(int arg)
{
	if(1 == arg )
		_ptz->runToPrepos();
	else if (2  == arg)
		_ptz->setPrepos(cfg_value[CFGID_PREPOS_preposx] , cfg_value[CFGID_PREPOS_preposy]);
	return ;
}


void State::getcurRate(float& curx , float& cury)
{
	curx = m_pltOutput.fPlatformDemandX;
	cury = m_pltOutput.fPlatformDemandY;
	return ;
}


void State::notifyZoom(int zoom)
{	
	BoresightPos_s pos = pThis->m_Platform->getBoresight(cfg_value , zoom);
	//send ipc	
	return;
}
