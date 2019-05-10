/*
 * main.cpp
 *
 *  Created on: 2019年5月8日
 *      Author: d
 */
#include "EventManager.hpp"

int main()
{
	CEventParsing parsing;
	CjoyStick* js = new CjoyStick();
	CEventManager* eventManager = new CEventManager();

	std::thread T1(CjoyStick::readJoystickEvent);
	std::thread T2(CEventParsing::thread_jsEvent);

	struct timeval tmp;
	while(1)
	{
		tmp.tv_sec = 0;
		tmp.tv_usec = 10000;
		select(0, NULL, NULL, NULL, &tmp);
	}
	T1.join();
	T2.join();
	return 0;
}

