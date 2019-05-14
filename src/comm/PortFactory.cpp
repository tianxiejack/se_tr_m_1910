#include "PortFactory.hpp"
#include "CUartProc.hpp"
#include "CNetProc.hpp"
#include <iostream>

PortFactory::PortFactory()
{

}
  
PortFactory::~PortFactory()
{
 
}

CPortInterface* PortFactory::createProduct(int type)
{
	CPortInterface* temp = NULL;
    	switch(type)
    	{
		case 1:
			temp = new CUartProc("/dev/ttyTHS1", 115200, 8, 'N', 1);
	            	break;
	        case 2:
			temp = new CNetProc(10000);
		   	break;
	        case 3:
	            	temp = new CUartProc("/dev/ttyTHS2", 9600, 8, 'N', 1);
	            	break;
	        default:
	            	break;
	}
    	return temp;
}
