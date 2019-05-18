#include "State.h"
#include <iostream>

using namespace std;
State* pThis = NULL;
State *st1 = NULL, *st2 = NULL, *st3 = NULL, *st4 = NULL, *st5 = NULL;

CPTZControl* State::_ptz = NULL;
AgreeMentBaseFormat* State::_agreement = NULL;
CPlatformInterface* State::m_Platform = NULL;
CPTZSpeedTransfer*  State::m_ptzSpeed = NULL;
CIPCProc* State::m_ipc = NULL;
float* State::cfg_value = NULL;
View* State::viewParam = NULL;
HPLTCTRL  State::m_plt = NULL;

State::State()
{
	if(pThis == NULL)
		pThis = this;
	if(_agreement == NULL)
		_agreement = new IPelcoDFormat();
	if(_ptz == NULL)
		_ptz = new CPTZControl(_agreement);
	if(m_Platform == NULL)
		m_Platform = new CplatFormControl();
	if(m_ptzSpeed == NULL)
		m_ptzSpeed = new CPTZSpeedTransfer();
	if(m_ipc == NULL)
		m_ipc = new CIPCProc();

	curState = STATE_PTZ;
	selectch = {1, 1, 1, 1, 1, 0};
	curValidChid = selectch.idx;
	OSA_semCreate(&m_sem, 1, 0);
}

State::~State()
{
	OSA_semSignal(&m_sem);
	OSA_semDelete(&m_sem);
}

void State::OperationInterface(StateManger* con)
{
	if(cfg_value[CFGID_RTS_mtden])
	{
		ipcParam.intPrm[0] = 0;
		m_ipc->IPCSendMsg(mtd, ipcParam.intPrm, 4);
	}
	if(cfg_value[CFGID_RTS_trkalg])
	{
		ipcParam.intPrm[0] = 0;
		m_ipc->IPCSendMsg(sceneTrk, ipcParam.intPrm, 4);
	}
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
	if(st1 == NULL)
		st1 = new StateAuto_Mtd();
	if(st2 == NULL)
		st2 = new StateSceneSelect();
	if(st3 == NULL)
		st3 = new PlatFormCapture();
	if(st4 == NULL)
		st4 = new BoxCapture();
	if(st5 == NULL)
		st5 = new ManualMtdCapture();
	platformCreate();
}

int State::ChangeState(StateManger* con, char nextState)
{
	curState = nextState;
	switch(nextState)
	{
	case STATE_AUTOMTD:
		con->ChangeState(st1);
		break;

	case STATE_SCENETRK:
		con->ChangeState(st2);
		break;

	case STATE_PTZ:
		con->ChangeState(st3);
		break;

	case STATE_BOX:
		con->ChangeState(st4);
		break;

	case STATE_MANUALMTD:
		con->ChangeState(st5);
		break;
	}
	return curState;
}

void State::TrkCtrl(char Enable)
{
	ipcParam.intPrm[0] = Enable;
	m_ipc->IPCSendMsg(trk, ipcParam.intPrm, 4);
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
}

void State::axisMove(int x, int y)
{
	axisMove_interface(x, y);
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
}

void State::axisMove_interface(int x, int y)
{
	m_Platform->PlatformCtrl_VirtualInput(m_plt, DevUsr_AcqJoystickXInput, (float)x/JOS_VALUE_MAX);
	m_Platform->PlatformCtrl_VirtualInput(m_plt, DevUsr_AcqJoystickYInput, (float)y/JOS_VALUE_MAX);

	m_pltInput.iTrkAlgState= 0;
	m_Platform->PlatformCtrl_TrackerInput(m_plt, &m_pltInput);
	m_Platform->PlatformCtrl_TrackerOutput(m_plt, &m_pltOutput);
	printf("x = %f , y = %f \n", m_pltOutput.fPlatformDemandX, m_pltOutput.fPlatformDemandY);
	_ptz->m_iSetPanSpeed = m_ptzSpeed->GetPanSpeed(m_pltOutput.fPlatformDemandX);
	_ptz->m_iSetTiltSpeed = m_ptzSpeed->GetTiltSpeed(m_pltOutput.fPlatformDemandY);
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







