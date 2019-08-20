/*
 * ptz_onvif_control.hpp
 *
 *  Created on: 2019年8月20日
 *      Author: alex
 */

#ifndef PTZ_ONVIF_CONTROL_HPP_
#define PTZ_ONVIF_CONTROL_HPP_

#include "ptzInterface.hpp"
#include "onvifInterface.hpp"

class CPTZ_ONVIF : public CptzInterface
{
	public:
		CPTZ_ONVIF();
		virtual ~CPTZ_ONVIF();
	
	public:  //interface
		void queryPos();
		void queryZoom();
		void getpos(int& pan, int& til);
		void getzoom(int& zoom);

		void ptzStop();
		void ptzSetPos(unsigned int posx, unsigned int posy);
		void setZoomPos(unsigned int value);

		void setZoomSpeed(int speed);
		void setPanSpeed(int speed);
		void setTiltSpeed(int speed);

		void setIrisSpeed(int speed);
		void setFocusFarSpeed(int speed);
		void setFocusNearSpeed(int speed);

		int runToPrepos(int arg = 0);
		void setPrepos(int& preposx,int& preposy);

		void simpleQueryZoom();
		void setMtdMonitorP(int value);
		void setMtdMonitorT(int value);
		void setMtdMonitorZ(int value);

	private:
		IOnvif* m_onvifHanle;
	
};



#endif /* PTZ_ONVIF_CONTROL_HPP_ */
