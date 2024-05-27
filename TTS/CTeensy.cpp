#include "pch.h"
#include "framework.h"
#include "TtsDlg.h"

#include "CTeensy.h"
#include "afxdialogex.h"
#include "sstream"
#include "iomanip"
#include "string"
#include "cstring"
#include "algorithm"
#include "iostream"
#include "cstdlib"
using namespace std;

CTeensy::CTeensy()
{

}
CTeensy::~CTeensy()
{

}

bool CTeensy::Initialize()
{
	hComm = CreateFile(L"COM4",			//friendly name
		GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
		0,                                 // No Sharing, ports cant be shared
		NULL,                              // No Security
		OPEN_EXISTING,                     // Open existing port only
		0,                                 // Non Overlapped I/O
		NULL);
	if (hComm == INVALID_HANDLE_VALUE) {
		int error = GetLastError();
		return FALSE;
	}
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	dcbSerialParams.BaudRate = CBR_9600;      //BaudRate = 9600
	dcbSerialParams.ByteSize = 8;             //ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;    //StopBits = 1
	dcbSerialParams.Parity = NOPARITY;      //Parity = None
	Status = SetCommState(hComm, &dcbSerialParams);
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutConstant = 5;
	timeouts.ReadTotalTimeoutMultiplier = 5;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	SetCommTimeouts(hComm, &timeouts);


	return true;
}

void CTeensy::Close()
{

}

void CTeensy::SetStep(short step)
{
	char buff[7];
	ostringstream ss;
	ss << step;
	string result = ss.str();
	string test = "v:" + result + "\n"; // "\n" !!!!!!!!!!!
	strcpy_s(buff, test.c_str());
	char SerialBuffer_test[7];
	sprintf_s(SerialBuffer_test, buff); //current command
	Status = WriteFile(hComm,// Handle to the Serialport
		SerialBuffer_test,            // Data to be written to the port
		sizeof(SerialBuffer_test),   // No of bytes to write into the port
		&BytesWritten,  // No of bytes written to the port
		NULL);

	return;
}

void CTeensy::Stop()
{
	char buff[4];
	string test = "i:\n";
	strcpy_s(buff, test.c_str());
	char SerialBuffer_test[4];
	sprintf_s(SerialBuffer_test, buff); //current command
	Status = WriteFile(hComm,// Handle to the Serialport
		SerialBuffer_test,            // Data to be written to the port
		sizeof(SerialBuffer_test),   // No of bytes to write into the port
		&BytesWritten,  // No of bytes written to the port
		NULL);

	return;
}

bool CTeensy::FaultCheck()
{
	// Send Read Request
	char buff[7];
	string test = "stat:\n";
	strcpy_s(buff, test.c_str());
	char SerialBuffer_test[7];
	sprintf_s(SerialBuffer_test, buff); //current command
	Status = WriteFile(hComm,// Handle to the Serialport
		SerialBuffer_test,            // Data to be written to the port
		sizeof(SerialBuffer_test),   // No of bytes to write into the port
		&BytesWritten,  // No of bytes written to the port
		NULL);

	// Read Read
	char BufferRead[30];
	Status = SetCommMask(hComm, EV_RXCHAR);
	NoBytesRead = 1;
	do {
		loop = 0;
		do
		{
			Status = ReadFile(hComm, &ReadData, sizeof(ReadData), &NoBytesRead, NULL);
			if (NoBytesRead > 0) {
				BufferRead[loop] = ReadData;
			}
			else {
				break;
			}
			++loop;
		} while (NoBytesRead > 0 && (BufferRead[loop - 1] != '\n'));
	} while (NoBytesRead > 0);
	--loop; //Get Actual length of received data

	
	if (loop != 0) {
		BufferRead[28] = '\0';
		int num1, num2, num3, num4;
		if (sscanf_s(BufferRead, "%d, %d, %d, %d", &num1, &num2, &num3, &num4) == 4) {
			// Checking if the difference between the second and third numbers is greater than 1
			return abs(num2 - num3) > 3;
		}
		else {
			// Invalid input format
			std::cerr << "Invalid input format.\n";
			return false;
		}
	}
	return false;
}

void CTeensy::SetTrapezoid(short step, short vel, short acc)
{
	char buff[19];
	ostringstream ss1;
	ss1 << step;
	string result1 = ss1.str();
	ostringstream ss2;
	ss2 << vel;
	string result2 = ss2.str();
	ostringstream ss3;
	ss3 << acc;
	string result3 = ss3.str();
	string test = "s:" + result1 + ":v:" + result2 + ":a:" + result3 + "\n";
	strcpy_s(buff, test.c_str());
	char SerialBuffer_test[19];
	sprintf_s(SerialBuffer_test, buff); //current command
	Status = WriteFile(hComm,// Handle to the Serialport
		SerialBuffer_test,            // Data to be written to the port
		sizeof(SerialBuffer_test),   // No of bytes to write into the port
		&BytesWritten,  // No of bytes written to the port
		NULL);

	return;
}

void CTeensy::SetHold()
{
	char buff[4];
	string test = "h:\n";
	strcpy_s(buff, test.c_str());
	char SerialBuffer_test[4];
	sprintf_s(SerialBuffer_test, buff); //current command
	Status = WriteFile(hComm,// Handle to the Serialport
		SerialBuffer_test,            // Data to be written to the port
		sizeof(SerialBuffer_test),   // No of bytes to write into the port
		&BytesWritten,  // No of bytes written to the port
		NULL);

	return;
}

int CTeensy::GetSteps(void)
{
	// Send Read Request
	char buff[7];
	string test = "stat:\n";
	strcpy_s(buff, test.c_str());
	char SerialBuffer_test[7];
	sprintf_s(SerialBuffer_test, buff); //current command
	Status = WriteFile(hComm,// Handle to the Serialport
		SerialBuffer_test,            // Data to be written to the port
		sizeof(SerialBuffer_test),   // No of bytes to write into the port
		&BytesWritten,  // No of bytes written to the port
		NULL);

	// Read Read
	char BufferRead[30];
	Status = SetCommMask(hComm, EV_RXCHAR);
	NoBytesRead = 1;
	do {
		loop = 0;
		do
		{
			Status = ReadFile(hComm, &ReadData, sizeof(ReadData), &NoBytesRead, NULL);
			if (NoBytesRead > 0) {
				BufferRead[loop] = ReadData;
			}
			else {
				break;
			}
			++loop;
		} while (NoBytesRead > 0 && (BufferRead[loop - 1] != '\n'));
	} while (NoBytesRead > 0);
	--loop; //Get Actual length of received data


	if (loop != 0) {
		BufferRead[28] = '\0';
		int num1, num2, num3, num4;
		if (sscanf_s(BufferRead, "%d, %d, %d, %d", &num1, &num2, &num3, &num4) == 4) {
			// Checking if the difference between the second and third numbers is greater than 1
			return num2;
		}
		else {
			// Invalid input format
			std::cerr << "Invalid input format.\n";
			return false;
		}
	}
	return false;
}