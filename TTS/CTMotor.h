#pragma once
#include <iostream>
#include <fstream>

class CTMotor
{
public:

	CTMotor();
	~CTMotor();


	bool	Initialize();
	void	Close();

	long	ReadRpm(void);
	void	SetCurrent(short current);
	void	StopCurrent(void);
	long float	ReadPosition();

	

private:
	
	static const WORD	_nNodes = 1;		// Number of Nodes (Axes)
	static const WORD	_node[_nNodes];		// Number of Nodes (Axes)
	static const WORD	_aI[_nNodes];		// Digital Inputs
	static const double	_TR[_nNodes];		// Motor gearbox TR
	
	static const WORD	_Node = 1U;

	short	nomCurr = 582;					// In mA
	bool	currentModeOn = false;
	
	HANDLE hComm;  // Handle to the Serial port
	BOOL   Status; // Status
	DCB dcbSerialParams = { 0 };  // Initializing DCB structure
	COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure
	char SerialBuffer[48] = { 0 }; //Buffer to send and receive data
	DWORD BytesWritten = 0;          // No of bytes written to the port
	char  ReadData;        //temperory Character
	DWORD NoBytesRead;     // Bytes read by ReadFile()
	unsigned char loop = 0;

};