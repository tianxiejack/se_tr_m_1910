#ifndef __CUARTPROC_H__
#define __CUARTPROC_H__

#include "CPortInterface.hpp"
#include <iostream>

using namespace std;

class CUartProc : public CPortInterface
{
public:
	CUartProc(const string dev_name,const int baud_rate, const int data_bits, const char parity, const int stop_bits);
	~CUartProc();
	int copen();
	int cclose();
	int crecv(int fd, void *buf,int len);
	int csend(int fd, void *buf,int len);
private:
	int setPort(int fd, const int baud_rate, const int data_bits, char parity, const int stop_bits);
	
private:
	string devname;
	int baudrate;
	int databits;
	char paritybits;
	int stopbits;

};

#endif
