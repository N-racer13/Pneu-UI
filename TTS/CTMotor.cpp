#include "pch.h"
#include "framework.h"
#include "TtsDlg.h"

#include "CTMotor.h"
#include "afxdialogex.h"
#include "sstream"
#include "iomanip"
#include "string"
#include "cstring"
#include "algorithm"
using namespace std;

CTMotor::CTMotor()
{

}
CTMotor::~CTMotor()
{

}

bool CTMotor::Initialize()
{
	hComm = CreateFile(L"COM3",			//friendly name
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
void CTMotor::Close()
{

}

long CTMotor::ReadRpm(void)
{
	char SerialBuffer_test[48];
	sprintf_s(SerialBuffer_test, "can send 8001 1a025050505050505050505050505050\n"); //ask for read
	Status = WriteFile(hComm,// Handle to the Serialport
		SerialBuffer_test,            // Data to be written to the port
		sizeof(SerialBuffer_test),   // No of bytes to write into the port
		&BytesWritten,  // No of bytes written to the port
		NULL);

	CString  test_str = _T("Hello");
	//teensySerial.rdbuf
	//SetDlgItemTextW(IDC_STATIC1, test_str);
	Status = SetCommMask(hComm, EV_RXCHAR);
	NoBytesRead = 1;
	do {
		loop = 0;
		do
		{
			Status = ReadFile(hComm, &ReadData, sizeof(ReadData), &NoBytesRead, NULL);
			if (NoBytesRead > 0) {
				SerialBuffer[loop] = ReadData;
			}
			else {
				break;
			}
			++loop;
		} while (NoBytesRead > 0 && (SerialBuffer[loop - 1] != '\n'));
	} while (NoBytesRead > 0);
	//do
	//{
	//	Status = ReadFile(hComm, &ReadData, sizeof(ReadData), &NoBytesRead, NULL);
	//} while (NoBytesRead > 0 && (SerialBuffer[loop - 1] != '\n'));
	--loop; //Get Actual length of received data

	if (loop != 0) {
		CString test2(SerialBuffer);
		// LOOK FOR 8 BIT HEX WITH RPM
		const char substring[] = "2A02"; // Substring to search for

		const char* found = strstr(SerialBuffer, substring);
		if (found != nullptr) {
			// Check if there are at least 8 characters following the found substring
			if (strlen(found) >= 16) {
				char next8Chars[9]; // Buffer for 8 characters + null terminator
				char sec8Chars[9];
				strncpy_s(next8Chars, found + strlen(substring), 8);
				strncpy_s(sec8Chars, found + strlen(substring) + 8, 8);
				next8Chars[8] = '\0'; // Null terminate the string
				sec8Chars[8] = '\0';
				CString test2(next8Chars);
				CString sectest2(next8Chars);

				// FLIP STRING IN PAIRS
				string reversed_string;
				char buff[48];
				ostringstream ss;
				ss << setfill('0') << setw(8) << hex << next8Chars;
				string result = ss.str();
				for (int i = 0; i < result.length(); i += 2) {
					string pair = result.substr(i, 2);
					reverse(pair.begin(), pair.end());
					reversed_string += pair;
				}
				reverse(reversed_string.begin(), reversed_string.end());
				char buff1[9];
				strcpy_s(buff1, reversed_string.c_str());
				CString test3(buff1);
				//SetDlgItemTextW(IDC_STATIC1, test3);

				// CONVERT HEX STRING TO DECIMAL AND RPM
				unsigned long decimalNumber = stoul(reversed_string, nullptr, 16);
				bool neg = false;
				if (decimalNumber > 2147483647) { decimalNumber = 4294967294 - decimalNumber; neg = true; }
				decimalNumber = decimalNumber * 0.00001 * 60;
				char buff2[21];
				_ultoa_s(decimalNumber, buff2, 10);
				CString test4(buff2);
				//SetDlgItemTextW(IDC_STATIC1, test4);

				// CREATE A BUFFER TO CALCULATE THE AVERAGE
				// NOTE: THIS CAUSES DELAY
				const short max = 10;
				static long buffer[max];
				static short idx = -1;
				static long sum = 0;
				// Circulate the que
				idx < max - 1 ? idx++ : idx = 0;
				// Average
				sum -= buffer[idx];
				// 10000 RPM CUT OFF (FOR STAND STILL)
				if (decimalNumber > 10000) {
					decimalNumber = 0;
				}
				buffer[idx] = decimalNumber;
				sum += decimalNumber;
				int avrg = (double)sum / (double)max;

				if (neg == true) { avrg = -avrg; }
				return (long)avrg;
			}
		}
	}


	
}

void CTMotor::SetCurrent(short current)
{
	//motorSpeedCommand = GetDlgItemInt(IDC_EDIT1);

	string reversed_string;
	char buff[48];
	ostringstream ss;
	if (current >= 0) { ss << setfill('0') << setw(8) << hex << current;}
	else { ss << setfill('f') << setw(8) << hex << current; }
	string result = ss.str();

	for (int i = 0; i < result.length(); i += 2) {
		string pair = result.substr(i, 2);
		reverse(pair.begin(), pair.end());
		reversed_string += pair;
	}

	reverse(reversed_string.begin(), reversed_string.end());

	char SerialBuffer_test[48];
	string test = "can send 8001 010009091c" + reversed_string + "50505050505050\n";
	strcpy_s(buff, test.c_str());
	sprintf_s(SerialBuffer_test, buff); //current command
	Status = WriteFile(hComm,// Handle to the Serialport
		SerialBuffer_test,            // Data to be written to the port
		sizeof(SerialBuffer_test),   // No of bytes to write into the port
		&BytesWritten,  // No of bytes written to the port
		NULL);

	return;
}

void CTMotor::StopCurrent(void) 
{
	char SerialBuffer_test[48];
	sprintf_s(SerialBuffer_test, "can send 8001 01000050505050505050505050505050\n"); //stop
	//sprintf_s(SerialBuffer_test, "can send 8001 010009091c0000000050505050505050\n"); //current command
	Status = WriteFile(hComm,// Handle to the Serialport
		SerialBuffer_test,            // Data to be written to the port
		sizeof(SerialBuffer_test),   // No of bytes to write into the port
		&BytesWritten,  // No of bytes written to the port
		NULL);
}

long float CTMotor::ReadPosition(void)
{
	char SerialBuffer_test[48];
	sprintf_s(SerialBuffer_test, "can send 8001 19015050505050505050505050505050\n"); //ask for read
	Status = WriteFile(hComm,// Handle to the Serialport
		SerialBuffer_test,            // Data to be written to the port
		sizeof(SerialBuffer_test),   // No of bytes to write into the port
		&BytesWritten,  // No of bytes written to the port
		NULL);

	Status = SetCommMask(hComm, EV_RXCHAR);
	NoBytesRead = 1;
	do {
		loop = 0;
		do
		{
			Status = ReadFile(hComm, &ReadData, sizeof(ReadData), &NoBytesRead, NULL);
			if (NoBytesRead > 0) {
				SerialBuffer[loop] = ReadData;
			}
			else {
				break;
			}
			++loop;
		} while (NoBytesRead > 0 && (SerialBuffer[loop - 1] != '\n'));
	} while (NoBytesRead > 0);
	//do
	//{
	//	Status = ReadFile(hComm, &ReadData, sizeof(ReadData), &NoBytesRead, NULL);
	//} while (NoBytesRead > 0 && (SerialBuffer[loop - 1] != '\n'));
	--loop; //Get Actual length of received data

	if (loop != 0) {
		CString test2(SerialBuffer);
		// LOOK FOR 8 BIT HEX WITH RPM
		const char substring[] = "2901"; // Substring to search for

		const char* found = strstr(SerialBuffer, substring);
		if (found != nullptr) {
			// Check if there are at least 8 characters following the found substring
			if (strlen(found) >= 16) {
				char next8Chars[9]; // Buffer for 8 characters + null terminator
				char sec8Chars[9];
				strncpy_s(next8Chars, found + strlen(substring), 8);
				strncpy_s(sec8Chars, found + strlen(substring) + 8, 8);
				next8Chars[8] = '\0'; // Null terminate the string
				sec8Chars[8] = '\0';
				CString test2(next8Chars);
				CString sectest2(next8Chars);

				// FLIP STRING IN PAIRS
				string reversed_string;
				char buff[48];
				ostringstream ss;
				ss << setfill('0') << setw(8) << hex << next8Chars;
				string result = ss.str();
				for (int i = 0; i < result.length(); i += 2) {
					string pair = result.substr(i, 2);
					reverse(pair.begin(), pair.end());
					reversed_string += pair;
				}
				reverse(reversed_string.begin(), reversed_string.end());
				char buff1[9];
				strcpy_s(buff1, reversed_string.c_str());
				CString test3(buff1);
				//SetDlgItemTextW(IDC_STATIC1, test3);

				// CONVERT HEX STRING TO DECIMAL AND RPM
				long float decimalNumber = stoul(reversed_string, nullptr, 16);
				//bool neg = false;
				//if (decimalNumber > 2147483647) { decimalNumber = 4294967294 - decimalNumber; neg = true; }
				decimalNumber = decimalNumber * 0.00001 * 360;
				char buff2[21];
				_ultoa_s(decimalNumber, buff2, 10);
				CString test4(buff2);
				//SetDlgItemTextW(IDC_STATIC1, test4);

				// CREATE A BUFFER TO CALCULATE THE AVERAGE
				// NOTE: THIS CAUSES DELAY
				const short max = 10;
				static long buffer[max];
				static short idx = -1;
				static long sum = 0;
				// Circulate the que
				idx < max - 1 ? idx++ : idx = 0;
				// Average
				sum -= buffer[idx];
				buffer[idx] = decimalNumber;
				sum += decimalNumber;
				int avrg = (double)sum / (double)max;

				//if (neg == true) { avrg = -avrg; }
				return decimalNumber;
				//return (long)avrg;
			}
		}
	}
}