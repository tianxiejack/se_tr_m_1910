#include "State.h"
#include "StateManger.h"
#include <iostream>

using namespace std;
State* pThis;
State *st1 = NULL, *st2 = NULL, *st3 = NULL, *st4 = NULL, *st5 = NULL, *st6 = NULL, *st7 = NULL, *st8 = NULL;

State::State()
{
	pThis = this;
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

void State::OperationInterface(StateManger* con)
{
	cout<<"State::.."<<endl;
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

void State::OperationChangeState(StateManger*con)
{

}

StateConvention::StateConvention()
{

}

void StateConvention::StateInit()
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

int StateConvention::ChangeState(StateManger* con, char nextState)
{

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

int StateAuto_Mtd::ChangeState(StateManger* con, char nextState)
{

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

int StateSceneSelect::ChangeState(StateManger* con, char nextState)
{

}
//

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

int PlatFormCapture::ChangeState(StateManger* con, char nextState)
{

}
//


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

int BoxCapture::ChangeState(StateManger* con, char nextState)
{

}
//


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

int ManualMtdCapture::ChangeState(StateManger* con, char nextState)
{

}
//

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

int StateTrk::ChangeState(StateManger* con, char nextState)
{

}
//


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

int StateAcq::ChangeState(StateManger* con, char nextState)
{

}


