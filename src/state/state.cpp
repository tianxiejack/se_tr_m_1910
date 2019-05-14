#include "State.h"
#include <iostream>

using namespace std;
State* pThis;
State *st1 = NULL, *st2 = NULL, *st3 = NULL, *st4 = NULL, *st5 = NULL, *st6 = NULL, *st7 = NULL, *st8 = NULL;

State::State()
{
	pThis = this;
	selectch = {1, 1, 1, 1, 1, 0};
	curValidChid = selectch.idx;
	_agreement = new IPelcoDFormat();
	_ptz = new CPTZControl(_agreement);
	_ptz->Create();
}

State::~State()
{

}

void State::StateInit()
{
	if(st1 == NULL)
		st1 = new StateConvention();
	if(st2 == NULL)
		st2 = new StateAuto_Mtd();
	if(st3 == NULL)
		st3 = new StateSceneSelect();
	if(st4 == NULL)
		st4 = new PlatFormCapture();
	if(st5 == NULL)
		st5 = new BoxCapture();
	if(st6 == NULL)
		st6 = new ManualMtdCapture();
	if(st7 == NULL)
		st7 = new StateTrk();
	if(st8 == NULL)
		st8 = new StateAcq();
}

int State::ChangeState(StateManger* con, char nextState)
{
	curState = nextState;
	switch(nextState)
	{
	case STATE_CONVENTION:
		con->ChangeState(st1);
		break;

	case STATE_AUTOMTD:
		con->ChangeState(st2);
		break;

	case STATE_SCENETRK:
		con->ChangeState(st3);
		break;

	case STATE_PTZ:
		con->ChangeState(st4);
		break;

	case STATE_BOX:
		con->ChangeState(st5);
		break;

	case STATE_MANUALMTD:
		con->ChangeState(st6);
		break;

	case STATE_TRK:
		con->ChangeState(st7);
		break;

	case STATE_ACQ:
		con->ChangeState(st8);
		break;
	}
	return curState;
}

void State::switchSensor(char chid)
{
	static bool closeMtd = false;
	struct timeval tmp;
	int SensorStat; //= GetIpcAddress(Sensor);
	if(1)//SensorStat != selectch.idx)
		selectch.idx = SensorStat;
	do{
			selectch.idx++;
			selectch.idx = selectch.idx%5;
	}while(!selectch.validCh[selectch.idx]);
	if(curValidChid != selectch.idx)
		if(curState == STATE_MANUALMTD){
			//m_GlobalDate->ImgMtdStat = m_GlobalDate->mtdMode = 0;
			//usd_API_MTDMode();
			closeMtd = true;
			tmp.tv_sec = 0;
			tmp.tv_usec = 200000;
			select(0, NULL, NULL, NULL, &tmp);
		}
	curValidChid = selectch.idx;
	//m_GlobalDate->chid_camera = selectch.idx;
	//m_ipc->IpcSensorSwitch(selectch.idx);

	if(closeMtd){
		tmp.tv_sec = 1;
		tmp.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tmp);
		//ImgMtdStat = 1;
		//mtdMode = 0;
		//usd_API_MTDMode();
		closeMtd = false;
	}
}

void State::axisMove(int iDirection, int speed)
{
	_ptz->ptzMove(iDirection, speed);
}

/*=====================================*/
StateConvention::StateConvention()
{

}

StateConvention::~StateConvention()
{

}

void StateConvention::OperationInterface(StateManger*con)
{
	cout<<"0   StateConvention"<<endl;
}

void StateConvention::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

StateAuto_Mtd::StateAuto_Mtd()
{

}

StateAuto_Mtd::~StateAuto_Mtd()
{

}

void StateAuto_Mtd::OperationInterface(StateManger*con)
{
	cout<<"1  StateAuto_Mtd......"<<endl;
}

void StateAuto_Mtd::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

StateSceneSelect::StateSceneSelect()
{

}

StateSceneSelect::~StateSceneSelect()
{

}

void StateSceneSelect::OperationInterface(StateManger* con)
{
	cout<<"2  StateSceneSelect"<<endl;
}

void StateSceneSelect::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

PlatFormCapture::PlatFormCapture()
{

}

PlatFormCapture::~PlatFormCapture()
{

}

void PlatFormCapture::OperationInterface(StateManger* con)
{
	cout<<"3  PlatFormCapture"<<endl;
}

void PlatFormCapture::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

BoxCapture::BoxCapture()
{

}

BoxCapture::~BoxCapture()
{

}

void BoxCapture::OperationInterface(StateManger* con)
{
	cout<<"4  BoxCapture"<<endl;
}

void BoxCapture::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

ManualMtdCapture::ManualMtdCapture()
{

}

ManualMtdCapture::~ManualMtdCapture()
{

}

void ManualMtdCapture::OperationInterface(StateManger* con)
{
	cout<<"5  ManualMtdCapture"<<endl;
}

void ManualMtdCapture::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

StateTrk::StateTrk()
{

}

StateTrk::~StateTrk()
{

}

void StateTrk::OperationInterface(StateManger* con)
{
	cout<<"6  StateTrk"<<endl;
}

void StateTrk::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

StateAcq::StateAcq()
{

}

StateAcq::~StateAcq()
{

}

void StateAcq::OperationInterface(StateManger* con)
{
	cout<<"7  StateAcq"<<endl;
}

void StateAcq::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

