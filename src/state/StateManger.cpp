#include "StateManger.h"

StateManger::StateManger()
{

}

StateManger::StateManger(State* state)
{
	this->_state = state;
}

StateManger::~StateManger()
{
	delete _state;
}

void StateManger::OperationInterface()
{
	_state->OperationInterface(this);
}

bool StateManger::ChangeState(State* state)
{
	this->_state = state;
	return true;
}

void StateManger::OperationChangState()
{
	_state->OperationChangeState(this);
}

int StateManger::CurStateInterface()
{
	printf("_state->curState  = %d  \n", curState);
	return curState;
}

void StateManger::ToStateConvention()
{
	curState = _state->State::ChangeState(this, STATE_CONVENTION);
	_state->OperationChangeState(this);
}

void StateManger::ToStateAuto_Mtd()
{
	curState = _state->State::ChangeState(this, STATE_AUTOMTD);
	_state->OperationChangeState(this);
}

void StateManger::ToStateSceneSelect()
{
	curState = _state->State::ChangeState(this, STATE_SCENETRK);
	_state->OperationChangeState(this);
}

void StateManger::ToPlatFormCapture()
{
	curState = _state->State::ChangeState(this, STATE_PTZ);
	_state->OperationChangeState(this);
}

void StateManger::ToBoxCapture()
{
	curState = _state->State::ChangeState(this, STATE_BOX);
	_state->OperationChangeState(this);
}

void StateManger::ToManualMtdCapture()
{
	curState = _state->State::ChangeState(this, STATE_MANUALMTD);
	_state->OperationChangeState(this);
}

void StateManger::ToStateTrk()
{
	curState = _state->State::ChangeState(this, STATE_TRK);
	_state->OperationChangeState(this);
}

void StateManger::ToStateAcq()
{
	curState = _state->State::ChangeState(this, STATE_ACQ);
	_state->OperationChangeState(this);
}

void StateManger::inter_SwitchSensor(char chid)
{
	_state->switchSensor(chid);
}

void StateManger::inter_AxisMove(int iDirection, int speed)
{
	_state->axisMove(iDirection, speed);
}
