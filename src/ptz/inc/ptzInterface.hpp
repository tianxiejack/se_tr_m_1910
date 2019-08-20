/*
 * ptzInterface.hpp
 *
 *  Created on: 2019年5月14日
 *      Author: d
 */

#ifndef PTZINTERFACE_HPP_
#define PTZINTERFACE_HPP_

class CptzInterface
{
public:
		virtual void queryPos()=0;
		virtual void queryZoom()=0;
		virtual void getpos(int& pan, int& til)=0;
		virtual void getzoom(int& zoom)=0;

		virtual void ptzStop()=0;
		virtual void ptzSetPos(unsigned int posx, unsigned int posy)=0;
		virtual void setZoomPos(unsigned int value)=0;
		virtual void setZoomSpeed(int speed)=0;
		virtual void setPanSpeed(int speed)=0;
		virtual void setTiltSpeed(int speed)=0;
		virtual void setIrisSpeed(int speed)=0;
		virtual void setFocusFarSpeed(int speed)=0;
		virtual void setFocusNearSpeed(int speed)=0;

		virtual int runToPrepos(int arg = 0)=0;
		virtual void setPrepos(int& preposx,int& preposy)=0;
		virtual void simpleQueryZoom()=0;

		virtual void setMtdMonitorP(int value)=0;
		virtual void setMtdMonitorT(int value)=0;
		virtual void setMtdMonitorZ(int value)=0;
};

#endif /* PTZINTERFACE_HPP_ */
