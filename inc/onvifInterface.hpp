/*
 * onvifInterface.hpp
 *
 *  Created on: 2019年8月20日
 *      Author: alex
 */

#ifndef ONVIFINTERFACE_HPP_
#define ONVIFINTERFACE_HPP_

class IOnvif
{
public:
	static IOnvif *getinstance();

	virtual void stop()=0;



};

#endif /* ONVIFINTERFACE_HPP_ */
