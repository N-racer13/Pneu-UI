#pragma once
#include <iostream>
#include <fstream>

class CTeensy
{
public:

	CTeensy();
	~CTeensy();


	bool	Initialize();
	void	Close();

	void	SetStep(short step);
	void	Stop();
	bool	FaultCheck();
	void	SetTrapezoid(short step, short vel, short acc);
	void	SetHold();
	int		GetSteps();



private:

	static const WORD	_nNodes = 1;		// Number of Nodes (Axes)
	static const WORD	_node[_nNodes];		// Number of Nodes (Axes)
	static const WORD	_aI[_nNodes];		// Digital Inputs
	static const double	_TR[_nNodes];		// Motor gearbox TR

	static const WORD	_Node = 1U;

	HANDLE hComm;  // Handle to the Serial port
	BOOL   Status; // Status
	DCB dcbSerialParams = { 0 };  // Initializing DCB structure
	COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure
	char SerialBuffer[7] = { 0 }; //Buffer to send and receive data
	DWORD BytesWritten = 0;          // No of bytes written to the port
	char  ReadData;        //temperory Character
	DWORD NoBytesRead;     // Bytes read by ReadFile()

	unsigned char loop = 0;
};
