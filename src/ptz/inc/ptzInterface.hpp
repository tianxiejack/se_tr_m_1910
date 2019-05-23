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
};



#endif /* PTZINTERFACE_HPP_ */
