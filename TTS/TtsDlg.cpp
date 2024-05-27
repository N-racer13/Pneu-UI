
// TTSDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TTS.h"
#include "TtsDlg.h"
#include "CTMotor.h"
#include "CNiDac.h"
#include "CTeensy.h"
#include "afxdialogex.h"
#include <fstream>
#include <queue>
#include <numeric>
#include <Windows.h>
#include <chrono>
#include <thread>
using namespace std;



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// CAboutDlg ****************************************************************************************************************************

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

/// CTTSDlg ****************************************************************************************************************************

CTTSDlg::CTTSDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TTS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hicon1 = AfxGetApp()->LoadIcon(IDI_ICON1);
	hicon2 = AfxGetApp()->LoadIcon(IDI_ICON2);
	hicon3 = AfxGetApp()->LoadIcon(IDI_ICON3);

	_sampleRate = 1;	// Defalut sample rate 
	_maxCurrent = 1500;
	_datapoints = 10;

	_tmotor = NULL;
	_tmotor = new CTMotor();

	_nidaq = NULL;
	_nidaq = new CNiDac();

	_teensy = NULL;
	_teensy = new CTeensy();

	_isOpen = false;
	_isOpen_position = false;
	_isMeasuring = false;
	_isStepper = false;
	_isTesting = false;
}

void CTTSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ANALOG_IN_1, _rEditAnalogIn1);
	DDX_Control(pDX, IDC_SAMPLE_RATE, _rEditSampleRate);
	DDX_Control(pDX, IDC_READ_RPM, _rEditReadRpm);
	DDX_Control(pDX, IDC_SET_CURRENT, _rEditSetCurrent);
	DDX_Control(pDX, IDC_SAVE_DATA, _buttonSaveData);
	DDX_Control(pDX, IDC_RPM_PROGRESS, _rpmProgress);
	DDX_Control(pDX, IDC_FORCE_PROGRESS, _forceProgress);
	DDX_Control(pDX, IDC_CURRENT_PROGRESS, _currentProgress);
	DDX_Control(pDX, IDC_TARGET, _rEditTarget);
	DDX_Control(pDX, IDC_FLOW, _rEditFlow);
	DDX_Control(pDX, IDC_HOSE_LENGTH, _rEditHoseLength);
	DDX_Control(pDX, IDC_COLOR, _rButtonSpin);
	DDX_Control(pDX, IDC_COLOR_SAVE, _rButtonSave);
	DDX_Control(pDX, IDC_COLOR_TORQUEPUSH, _rButtonTorquePush);
	DDX_Control(pDX, IDC_RADIO_MEASUREMENT, _rRadioMeasure);
	DDX_Control(pDX, IDC_RADIO_TEST, _rRadioTest);
	DDX_Control(pDX, IDC_RADIO_STEPPER, _rRadioStepper);
	DDX_Control(pDX, IDC_MAX_CURRENT, _rEditMaxCurrent);
	DDX_Control(pDX, IDC_DATAPOINTS, _rEditDatapoints);
	DDX_Control(pDX, IDC_STATUS, _rEditStatus);
	DDX_Control(pDX, IDC_PRESSURE, _rEditPressure);
	DDX_Control(pDX, IDC_OPEN_FILE, _rButtonOpenFile);
	DDX_Control(pDX, IDC_POSITION_OPEN_FILE, _rButtonPositionOpenFile);
	DDX_Control(pDX, IDC_EDIT1, _rEdit1);
	DDX_Control(pDX, IDC_READ_CURRENT, _rEditReadCurrent);
	DDX_Control(pDX, IDC_STEPPER_OPEN_FILE, _rEditStepperOpenFile);
	DDX_Control(pDX, IDC_STEP_SPEED, _rEditStepSpeed);
	DDX_Control(pDX, IDC_HOSE_LENGTH2, _rEditHoseLength2);
	DDX_Control(pDX, IDC_PRESSURE2, _rEditPressure2);
	DDX_Control(pDX, IDC_FLOW2, _rEditFlow2);
	DDX_Control(pDX, IDC_HOSE_LENGTH3, _rEditHoseLength3);
	DDX_Control(pDX, IDC_PRESSURE3, _rEditPressure3);
	DDX_Control(pDX, IDC_COUNTER_TORQUE, _rEditCounterTorque);
	DDX_Control(pDX, IDC_NUMBER_STEPS, _rEditNumberSteps);
	DDX_Control(pDX, IDC_POSITION_VELOCITY, _rEditPositionVelocity);
	DDX_Control(pDX, IDC_POSITION_ACCELERATION, _rEditPositionAcceleration);
	DDX_Control(pDX, IDC_COLOR_POSITION, _rButtonPosition);
	DDX_Control(pDX, IDC_READ_POS, _rEditPosition);
}

BEGIN_MESSAGE_MAP(CTTSDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()

	ON_BN_CLICKED(IDC_SAVE_DATA, &CTTSDlg::OnClickedSaveData)
	ON_BN_CLICKED(IDC_CURRENT_STOP, &CTTSDlg::OnClickedStop)
	ON_BN_CLICKED(IDC_CALIBRATE, &CTTSDlg::OnClickedCalibrate)
	ON_BN_CLICKED(IDC_SPIN, &CTTSDlg::OnClickedSpin)
	ON_EN_CHANGE(IDC_SAMPLE_RATE, &CTTSDlg::OnChangeSampleRate)
	ON_EN_CHANGE(IDC_SET_CURRENT, &CTTSDlg::OnChangeBoxCurrent)
	ON_EN_CHANGE(IDC_EDIT1, &CTTSDlg::OnChangeSpeed)
	ON_EN_CHANGE(IDC_MAX_CURRENT, &CTTSDlg::OnEnChangeMaxCurrent)
	ON_BN_CLICKED(IDC_RADIO_MEASUREMENT, &CTTSDlg::OnBnClickedRadioMeasurement)
	ON_BN_CLICKED(IDC_RADIO_STEPPER, &CTTSDlg::OnBnClickedRadioStepper)
	ON_BN_CLICKED(IDC_RADIO_TEST, &CTTSDlg::OnBnClickedRadioTest)
	ON_BN_CLICKED(IDC_OPEN_FILE, &CTTSDlg::OnBnClickedOpenFile)
	ON_BN_CLICKED(IDC_POSITION_OPEN_FILE, &CTTSDlg::OnBnClickedPositionOpenFile)
	ON_BN_CLICKED(IDC_ZERO_FORCE, &CTTSDlg::OnBnClickedZeroForce)
	ON_BN_CLICKED(IDC_TORQUE_PUSH, &CTTSDlg::OnBnClickedTorquePush)
	ON_BN_CLICKED(IDC_STEPPER_OPEN_FILE, &CTTSDlg::OnBnClickedStepperOpenFile)
	ON_BN_CLICKED(IDC_STEP_TORQUEPUSH, &CTTSDlg::OnBnClickedStepTorquepush)
	ON_BN_CLICKED(IDC_POSITION_START, &CTTSDlg::OnBnClickedPositionStart)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_RESET, &CTTSDlg::OnBnClickedReset)
END_MESSAGE_MAP()

BOOL CTTSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Initialize EPOS class:
	bool done = _tmotor->Initialize();
	bool done1 = _nidaq->Initialize();
	bool done2 = _teensy->Initialize();

	_isSaving = false; //prevent saving data when OPEN FILE is pressed

	// Initialize edits:
	/*Text Boxes*/
	_rEditAnalogIn1.SetReadOnly(true);
	_rEditReadRpm.SetReadOnly(true);
	_rEditSampleRate.SetReadOnly(false);
	_rEditSetCurrent.SetReadOnly(false);

	CString text;	text.Format(L"%d", _sampleRate);
	_rEditSampleRate.SetWindowTextW((LPCTSTR)text);

	text.Format(L"%d", _maxCurrent);
	_rEditMaxCurrent.SetWindowTextW((LPCTSTR)text);

	text.Format(L"%d", _datapoints);
	_rEditDatapoints.SetWindowTextW((LPCTSTR)text);

	//text.Format(L"%d", (int)_current);
	//_rEditSetCurrent.SetWindowTextW((LPCTSTR)text);

	text = _T("Measurement mode off");
	_rEditStatus.SetWindowTextW((LPCTSTR)text);

	_rpmProgress.SetRange(0, 1000);
	_forceProgress.SetRange(0, 600);
	_currentProgress.SetRange(0, _maxCurrent);

	_rButtonSpin.SetIcon(hicon1);
	_rButtonSave.SetIcon(hicon1);
	_rButtonTorquePush.SetIcon(hicon1);
	_rButtonPosition.SetIcon(hicon1);

	// Set timer for OnTimer:
	if (done)	SetTimer(1, _sampleRate, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTTSDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CTTSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

		//_rButtonStep.SetIcon(hicon3);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTTSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTTSDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	KillTimer(1);

	// Close GL80 and delete object::
	if (_tmotor) {
		//_tmotor->Close();
		_tmotor->StopCurrent();
		_teensy->Stop();
		delete _tmotor;
		_tmotor = NULL;
		delete _nidaq;
		_nidaq = NULL;
		delete _teensy;
		_teensy = NULL;
	}
}

/// CODE ****************************************************************************************************************************

// TIMER:-----------------------------------------------------------------------------------
void CTTSDlg::OnTimer(UINT_PTR nIDEvent)
{
	// Buffer force read:
	ai = 0.0037 * 100 * ((_nidaq->ReadAnalogInput()) - _forceZero); // in Nm
	/*const short max_ai = 10;
	static long float buffer[max_ai];
	static short idx = -1;
	static long float sum = 0;

	// Circulate the que
	idx < max - 1 ? idx++ : idx = 0;

	// Average
	sum -= buffer[idx];
	buffer[idx] = ai;
	sum += ai;

	ai = sum / max;*/


	motorRpm = _tmotor->ReadRpm();
	const short max2 = 2;
	static long float buffer2[max2];
	static short idx2 = -1;
	static long float sum2 = 0;

	// Circulate the que
	idx2 < max2 - 1 ? idx2++ : idx2 = 0;

	// Average
	sum2 -= buffer2[idx2];
	buffer2[idx2] = motorRpm;
	sum2 += motorRpm;

	motorRpm = sum2 / max2;


	_tmotor->SetCurrent(_current);

	CString text;
	_rEditDatapoints.GetWindowText(text);
	_datapoints = _wtoi(text);		// LOOP RUNS TOO SLOW!!

	if (_isMeasuring == true)
	{
		if (_zeroForceCheck == false) { _ForcePID(ai, motorRpm); }
		else if (_targetReached == false) { _RpmPID(_target, motorRpm); }
		if (_isOpen == true && _isSaving == true)
		{
			_saveCounter += 1;
			if (_saveCounter < _datapoints)
			{
				_SaveData(L"Append", ai, motorRpm);
				_saveCounter += 1;
				_rButtonSave.SetIcon(hicon2);
			}
			else
			{
				_saveCounter = _datapoints;
				_rButtonSave.SetIcon(hicon1);
				_isSaving = false;
				text = _T("Data saved");
				_rEditStatus.SetWindowTextW((LPCTSTR)text);
			}
		}
	}
	if (_isTesting == true)
	{
		if (_targetReached == false) { _RpmPID(_target, motorRpm); };
	}

	if (_isStepper == true)
	{
		if (torquePush == true)
		{
			// Check if file is open, load buffer
			if (_isOpen == true)
			{
				if (aiBuffer.size() >= _datapoints)
				{
					aiBuffer.pop();
					motorRpmBuffer.pop();
				}
				aiBuffer.push(ai);
				motorRpmBuffer.push(motorRpm);
			}
			// Metric for max torque
			if (!_teensy->FaultCheck())
			{
				_current -= 5;
			}
			// Max torue reached
			else
			{
				torquePush = false;
				_current = 0;
				_tmotor->SetCurrent(_current);
				_teensy->Stop();
				_rButtonTorquePush.SetIcon(hicon1);
				CString text = _T("Maximum torque reached"); _rEditStatus.SetWindowTextW((LPCTSTR)text);
			}
		}
		// If max torque is reached and if file is open, save data
		else if (_isOpen == true && !aiBuffer.empty())
		{
			_SaveData(L"Append", aiBuffer.front(), motorRpmBuffer.front());
			aiBuffer.pop();
			motorRpmBuffer.pop();
		}

		if (accuracy == true)
		{
			if (trapezoid == false)
			{ 
				/*if (ai * 1000 >= pos_torque)
				{
					
					
					pos_cur = _current;
					trapezoid = true;
					direction = 1;
					_teensy->SetTrapezoid(direction*pos_steps, pos_velocity, pos_acceleration);
					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					position_counter = 1;
				}
				else
				{
					_current -= 1;
				}*/
				pos_cur = -pos_torque*2;
				trapezoid = true;
				direction = 1;
				_teensy->SetTrapezoid(direction * pos_steps, pos_velocity, pos_acceleration);
				stepTarget = pos_steps;
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				position_counter = 1;
				motorRpm = _tmotor->ReadRpm();
			}
			else
			{
				stepsRead = _teensy->GetSteps();
				text.Format(L"%d", stepsRead);
				_rEditPosition.SetWindowTextW((LPCTSTR)text);
				if (abs(motorRpm) == 0 && stepsRead == direction* stepTarget)		//abs(stepsRead) == abs(pos_steps) && 
				{
					if (position_counter % 2 == 0)
					{
						_position = _tmotor->ReadPosition();
						_position = startPosition - _position;
						_SaveData(L"Append", ai, motorRpm);
						direction = -direction;
					}
					if (position_counter >= 40)
					{
						accuracy = false;
						trapezoid = false;
						_teensy->Stop();
						_current = 0;
						_tmotor->SetCurrent(_current);
						_rButtonPosition.SetIcon(hicon1);
					}
					else
					{
						direction = -direction;
						_teensy->SetTrapezoid(direction* pos_steps, pos_velocity, pos_acceleration);
						if (position_counter % 2 == 1)
						{
							/*_tmotor->SetCurrent(_current / 2);
							std::this_thread::sleep_for(std::chrono::milliseconds(50));
							_tmotor->SetCurrent(0);
							std::this_thread::sleep_for(std::chrono::milliseconds(50));
							_current = direction * pos_cur;
							_tmotor->SetCurrent(_current / 2);
							std::this_thread::sleep_for(std::chrono::milliseconds(50));
							_tmotor->SetCurrent(_current);*/
							_current = direction * pos_cur;
							_tmotor->SetCurrent(_current);
							std::this_thread::sleep_for(std::chrono::milliseconds(500));
						}
						//_teensy->SetTrapezoid(direction* pos_steps, pos_velocity, pos_acceleration);
						position_counter += 1;
						stepTarget = pos_steps;
						//stepsRead = _teensy->GetSteps();
						//motorRpm = _tmotor->ReadRpm();
						//std::this_thread::sleep_for(std::chrono::milliseconds(100));
						//Sleep(500);
					}					
				}
				if (abs(motorRpm) == 0 && abs(stepsRead - direction * stepTarget) == 1) // reads 0 when complete i think //maybe leave motorrpm ==0 out for instant transition?
				{
					//stepTarget = abs(direction * pos_steps - stepsRead);
					stepTarget = 1;
					_teensy->SetTrapezoid(direction * stepTarget, pos_velocity, pos_acceleration);
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
				}
				/*if (_teensy->FaultCheck() == true)
				{
					accuracy = false;
					trapezoid = false;
					busy = false;
					_teensy->Stop();
					_current = 0;
					_tmotor->SetCurrent(_current);
					_rButtonPosition.SetIcon(hicon1);
				}*/
			}
		}
	}


	// Force:
	text.Format(L"%f", ai);
	_rEditAnalogIn1.SetWindowTextW((LPCTSTR)text);
	_forceProgressBar(ai);

	// RPM:
	text.Format(L"%d", -motorRpm);
	_rEditReadRpm.SetWindowTextW((LPCTSTR)text);
	_rpmProgressBar(motorRpm);

	// Position
	/*abs_pos = _tmotor->ReadPosition();
	abs_pos = abs_pos - reset_pos;
	text.Format(L"%d", stepsRead);
	_rEditPosition.SetWindowTextW((LPCTSTR)text);*/


	// Current:
	text.Format(L"%d", (int)_current);
	_rEditReadCurrent.SetWindowTextW((LPCTSTR)text);
	_currentProgress.SetRange(0, _maxCurrent);
	_currentProgressBar(_current);

	//text.Format(L"%d", (int)_current);
	//_rEditSetCurrent.SetWindowTextW((LPCTSTR)text);

	if (_current > _maxCurrent) { _current = _maxCurrent; }
	else if (_current < -_maxCurrent) { _current = -_maxCurrent; }

	CDialog::OnTimer(nIDEvent);
}


// SAVE DATA:-------------------------------------------------------------------------------
void CTTSDlg::OnClickedSaveData()
{
	if (_isMeasuring == true) 
	{ 
		if (_isOpen == true)
		{

			_saveCounter = 0;
			_isSaving = true;
			CString text = _T("Saving data...");
			_rEditStatus.SetWindowTextW((LPCTSTR)text);

			_rEditFlow.GetWindowText(text);
			_flow = _wtoi(text);
			_rEditPressure.GetWindowText(text);
			_pressure = _wtoi(text);
			_rEditHoseLength.GetWindowText(text);
			_hoseLength = _wtoi(text);
		}
		else { CString text = _T("No open file, open file first"); _rEditStatus.SetWindowTextW((LPCTSTR)text); }
	}
	else { CString text = _T("Enable Measurement mode first"); _rEditStatus.SetWindowTextW((LPCTSTR)text); }
}
void CTTSDlg::_SaveData(CString filePath, long float ai, UINT motorRpm)
{
	static std::ofstream file;

	if (filePath == L"Close") {
		file.close();
		return;
	}

	CString			cStr;
	std::string		sStr;
	std::string		aiStr;
	std::string		motorRpmStr;
	std::string		pressureStr;
	std::string		flowStr;
	std::string		hoseLengthStr;
	std::string		stepStr;
	std::string		positionStr;


	if (filePath == L"Append") 
	{
		cStr.Format(L"%f", ai);
		aiStr = CT2CA(cStr.operator LPCWSTR());

		cStr.Format(L"%d", motorRpm);
		motorRpmStr = CT2CA(cStr.operator LPCWSTR());

		cStr.Format(L"%d", _pressure);
		pressureStr = CT2CA(cStr.operator LPCWSTR());

		cStr.Format(L"%d", _flow);
		flowStr = CT2CA(cStr.operator LPCWSTR());

		cStr.Format(L"%d", _hoseLength);
		hoseLengthStr = CT2CA(cStr.operator LPCWSTR());
		
		cStr.Format(L"%d", _steps);
		stepStr = CT2CA(cStr.operator LPCWSTR());

		cStr.Format(L"%f", _position);
		positionStr = CT2CA(cStr.operator LPCWSTR());

		file << aiStr << "," << motorRpmStr << "," << pressureStr << "," << flowStr << "," << hoseLengthStr << "," << stepStr << "," << positionStr << std::endl;
		return;
	}

	/// Open file:
	std::string sFilePath = CT2CA(filePath.operator LPCWSTR()); 
	file.open(sFilePath, std::ofstream::out);

	time_t now = time(NULL);
	char strTime[26];
	ctime_s(strTime, sizeof(strTime), &now);
	// Write header:
	file << "Torque Test Results: " << strTime;
	_rEditSampleRate.GetWindowText(cStr);
	sStr = CT2CA(cStr.operator LPCWSTR());
	file << "Sampling Period: " << sStr << " [ms]\n";
	file << std::endl;
	file << "Analog input [mV]" << "," << "Motor Rpm" << "," << "Presure [psi]" << "," << "Flow [sqf/h]" << "," << "Hose length [cm]" << "," << "Steps [steps/s]" << "," << "Position diff [degrees]" << std::endl;
	file << "-----------------------------------------------------------------\n";
}
bool CTTSDlg::_OpenOrSaveOneFileDialogue(bool open, std::string fileExtension, CString* vPath)
{	/// open = true to Open / false to Save (follows CFileDialog fileDlg)
	/// vPath MUST be a vector of [3]
	/// Returns [Folder, FileName (no extension), Folder\FileName.extension]

	// Convert uppercase file extension:
	std::string FE = fileExtension;
	//std::transform(FE.begin(), FE.end(), FE.begin(), ::toupper);

	// Set file filter:
	std::string strF = FE + " File (*." + fileExtension + ")|*." + fileExtension + "|All Files|*.*||";
	std::wstring wstrF = std::wstring(strF.begin(), strF.end());
	const wchar_t* szFilter = wstrF.c_str();

	// Set Default extension:
	std::wstring wstrE = std::wstring(fileExtension.begin(), fileExtension.end());
	const wchar_t* szDefExt = wstrE.c_str();

	// File dialogue:
	CFileDialog fileDlg(open, szDefExt, NULL, OFN_OVERWRITEPROMPT, szFilter);

	// Return filePath:
	if (fileDlg.DoModal() == IDOK) {
		vPath[0] = fileDlg.GetFolderPath() + L"\\";
		vPath[1] = fileDlg.GetFileTitle();
		vPath[2] = fileDlg.GetPathName();
		return true;
	}
	else	return false;
}


// STOP:------------------------------------------------------------------------------------
void CTTSDlg::OnClickedStop()
{
	_current = 0;
	_teensy->Stop();

	_targetReached = true;
	torquePush = false;
	_isSaving = false;
	trapezoid = false;
	busy = false;
	accuracy = false;

	//_isMeasuring = false;
	//_isTesting = false;
	//_isStepper = false;

	CString text = _T("Emergency stop");
	_rEditStatus.SetWindowTextW((LPCTSTR)text);

	//_rRadioTest.SetState(false);
	//_rRadioStepper.SetState(false);
	//_rRadioMeasure.SetState(false);

	_rButtonSpin.SetIcon(hicon1);
	_rButtonSave.SetIcon(hicon1);
	_rButtonTorquePush.SetIcon(hicon1);
	_rButtonPosition.SetIcon(hicon1);

	text.Format(L"%d", (int)_current);
	_rEditSetCurrent.SetWindowTextW((LPCTSTR)text);
	//_tmotor->StopCurrent();
}


// PROGRESS BARS:---------------------------------------------------------------------------
void CTTSDlg::_rpmProgressBar(long motorRpm)
{
	if (motorRpm > 0) { _rpmProgress.SetPos(motorRpm); return; }
	_rpmProgress.SetPos(-motorRpm);
}
void CTTSDlg::_currentProgressBar(float current)
{
	if (current > 0) { _currentProgress.SetPos(current); return; }
	_currentProgress.SetPos(-current);
}
void CTTSDlg::_forceProgressBar(long float ai)
{
	if (ai > 0) {_forceProgress.SetPos(1000 * ai); return;}
	_forceProgress.SetPos(-1000 * ai);
}


// DEFAULT:---------------------------------------------------------------------------------
void CTTSDlg::OnOK()
{
	// Don't close on Enter
//	CDialogEx::OnOK();
}
void CTTSDlg::OnCancel()
{
	// Comes here on Windows close and on ESC
	if (AfxMessageBox(L"Exit Program ?", MB_ICONQUESTION | MB_YESNO) == IDNO)		return;
	CDialogEx::OnCancel();
}


// MODES:-----------------------------------------------------------------------------------
void CTTSDlg::OnBnClickedRadioMeasurement()
{
	CString text = _T("Measurement mode ON");
	_rEditStatus.SetWindowTextW((LPCTSTR)text);
	_isMeasuring = true;
	_isStepper = false;
	_isTesting = false;

	_targetReached = true;

	//_rButtonStep.SetIcon(hicon1);

	_target = 0;
	_current = 0;
}
void CTTSDlg::OnBnClickedRadioTest()
{
	CString text = _T("Test mode ON");
	_rEditStatus.SetWindowTextW((LPCTSTR)text);
	_isMeasuring = false;
	_isStepper = false;
	_isTesting = true;
	
	_targetReached = true;

	_target = 0;
	_current = 0;
}
void CTTSDlg::OnBnClickedRadioStepper()
{
	CString text = _T("Stepper mode ON");
	_rEditStatus.SetWindowTextW((LPCTSTR)text);
	_isMeasuring = false;
	_isStepper = true;
	_isTesting = false;

	torquePush = false;

	_target = 0;
	_current = 0;
}


// PARAMETERS:------------------------------------------------------------------------------
void CTTSDlg::OnEnChangeMaxCurrent()
{
	CString text;
	_rEditMaxCurrent.GetWindowText(text);
	_maxCurrent =  _wtoi(text);
}
void CTTSDlg::OnClickedCalibrate()
{
	_forceZero = _nidaq->ReadAnalogInput();
}
void CTTSDlg::OnChangeSampleRate()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString text;
	_rEditSampleRate.GetWindowText(text);
	short sr = _wtoi(text);

	if (sr < 1)	return;

	_sampleRate = sr;
	SetTimer(1, _sampleRate, NULL);
}


// Free MODE:-------------------------------------------------------------------------------
void CTTSDlg::OnChangeBoxCurrent()
{
	if (_isTesting == true) {
		CString text;
		_rEditSetCurrent.GetWindowText(text);
		_current = _wtoi(text);
		_targetReached = true;
		text = _T("Current changed");
		_rEditStatus.SetWindowTextW((LPCTSTR)text);
	}
}
void CTTSDlg::OnChangeSpeed()
{
	if (_isTesting == true) {
		CString text;
		_rEdit1.GetWindowText(text);
		_target = _wtoi(text);
		_targetReached = false;
	}
}


// SERVO MODE:-------------------------------------------------------------------------------
void CTTSDlg::OnBnClickedOpenFile()
{
	if (_isMeasuring == true)
	{
		CString path[3];
		if (!_isOpen)
			if (!_OpenOrSaveOneFileDialogue(false, "txt", path))
				return;

		// Flip state:
		_isOpen = !_isOpen;
		// Change button state and text:
		if (!_isOpen)
		{
			_rButtonOpenFile.SetWindowTextW(L"OPEN FILE");
			CString text = _T("File closed");
			_rEditStatus.SetWindowTextW((LPCTSTR)text);
		}
		else
		{
			_rButtonOpenFile.SetWindowTextW(L"CLOSE FILE");
			CString text = _T("File opened");
			_rEditStatus.SetWindowTextW((LPCTSTR)text);
		}

		// Opn file for saving
		if (_isOpen)	_SaveData(path[2]);
		// Close file
		else			_SaveData(L"Close");
	}
	else { CString text = _T("Enable Measurement mode first"); _rEditStatus.SetWindowTextW((LPCTSTR)text); }
}
void CTTSDlg::OnBnClickedZeroForce()
{
	if (_isMeasuring == true)
	{
		_zeroForceCheck = false;
		CString text = _T("Moving to zero force point");
		_rEditStatus.SetWindowTextW((LPCTSTR)text);
	}
	else { CString text = _T("Enable Measurement mode first"); _rEditStatus.SetWindowTextW((LPCTSTR)text); }
}
void CTTSDlg::OnClickedSpin()
{
	if (_isMeasuring == true) //&& _zeroForceCheck == true
	{
		_targetReached = false;

		integrator = 0;
		CString text;
		_rEditTarget.GetWindowText(text);
		_target = -_wtoi(text);
	}
	else
	{
		CString text = _T("Enable Measurement mode first");
		_rEditStatus.SetWindowTextW((LPCTSTR)text);
	}


}
void CTTSDlg::_RpmPID(long desRpm, long motorRpm)
{

	long error = desRpm - motorRpm;
	float K = 0.012;	//best 0.03
	float D = 0.10; //best 0.2
	long float I = 0.1; // 0.5

	integrator += (float)error * 0.001;
	if (abs(integrator) > 40 && motorRpm < 200) { integrator = 40; }
	if (motorRpm >= 200 && abs(integrator) > 40 * 200 / motorRpm) { integrator = 40 * 200 / motorRpm; } //50 @ 200rpm

	_current += (K * error + D * (error - preError) / _sampleRate + I * integrator * _sampleRate) * _sampleRate;

	if (_current > _maxCurrent) { _current = _maxCurrent; }
	else if (_current < -_maxCurrent) { _current = -_maxCurrent; }

	preError = error;

	// show stability of the PID
	if (abs(error) < 20) { rpmBuffer += 1 * _sampleRate; }
	else
	{
		rpmBuffer = 0;
		_targetReached = false;
		CString text = _T("Moving to target RPM...");
		_rEditStatus.SetWindowTextW((LPCTSTR)text);

		if (_isMeasuring == true) { _rButtonSpin.SetIcon(hicon2); }
	}
	if (rpmBuffer >= 150)
	{
		_targetReached = true;
		rpmBuffer = 150;
		CString text = _T("Target RPM reached");
		_rEditStatus.SetWindowTextW((LPCTSTR)text);

		if (_isMeasuring == true) { _rButtonSpin.SetIcon(hicon1); }
	}
}
void CTTSDlg::_ForcePID(long float ai, long motorRpm)
{
	if (_isMeasuring == true)
	{
		long float error = ai;
		float K = 10;
		float D = 50;
		float I = 0;
		_current += (K * error + D * (error - preError) / _sampleRate + I * integrator) * _sampleRate;

		if (_current > _maxCurrent) { _current = _maxCurrent; }
		else if (_current < -_maxCurrent) { _current = -_maxCurrent; }

		preError = error;
		//_current = 50;

		CString text1;
		text1.Format(L"%d", (int)_current);
		_rEdit1.SetWindowTextW((LPCTSTR)text1);

		if (abs(ai) < 0.01) { forceBuffer += 1 * _sampleRate; }
		else
		{
			forceBuffer = 0;
			_zeroForceCheck = false;
		}
		if (forceBuffer >= 150)
		{
			_zeroForceCheck = true;
			forceBuffer = 150;
			CString text = _T("Target RPM reached");
			_rEditStatus.SetWindowTextW((LPCTSTR)text);
		}
	}
}


// STEPPER MODE:-----------------------------------------------------------------------------
void CTTSDlg::OnBnClickedTorquePush()
{
	if (_isStepper == true)
	{
		long motorRpm = _tmotor->ReadRpm();
		_torquePush(motorRpm);
		//_current = -300;

		CString text;
		_rEditStepSpeed.GetWindowText(text);
		_steps = _wtoi(text);
		_teensy->SetStep(_steps);

		_rEditHoseLength2.GetWindowText(text);
		_hoseLength = _wtoi(text);

		_rEditPressure2.GetWindowText(text);
		_pressure = _wtoi(text);

		_rEditFlow2.GetWindowText(text);
		_flow = _wtoi(text);
	}
	else { CString text = _T("Enable Stepper mode first"); _rEditStatus.SetWindowTextW((LPCTSTR)text); }
}
void CTTSDlg::_torquePush(long motorRpm)
{
	torquePush = true;
	rpmPush = motorRpm;
	_rButtonTorquePush.SetIcon(hicon2);
	CString text = _T("Increasing current for maximum torque..."); _rEditStatus.SetWindowTextW((LPCTSTR)text);
}
void CTTSDlg::OnBnClickedStepperOpenFile()
{
	if (_isStepper == true)
	{
		CString path[3];
		if (!_isOpen)
			if (!_OpenOrSaveOneFileDialogue(false, "txt", path))
				return;

		// Flip state:
		_isOpen = !_isOpen;
		// Change button state and text:
		if (!_isOpen)
		{
			_rEditStepperOpenFile.SetWindowTextW(L"OPEN FILE");
			CString text = _T("File closed, data saved");
			_rEditStatus.SetWindowTextW((LPCTSTR)text);
		}
		else
		{
			_rEditStepperOpenFile.SetWindowTextW(L"CLOSE FILE");
			CString text = _T("File opened, saving data...");
			_rEditStatus.SetWindowTextW((LPCTSTR)text);
		}

		// Opn file for saving
		if (_isOpen)	_SaveData(path[2]);
		// Close file
		else			_SaveData(L"Close");
	}
	else { CString text = _T("Enable Stepper mode first"); _rEditStatus.SetWindowTextW((LPCTSTR)text); }
}
void CTTSDlg::OnBnClickedStepTorquepush()
{
	if (_isStepper == true)
	{
		CString text;
		_rEditStepSpeed.GetWindowText(text);
		_steps = _wtoi(text);
		_teensy->SetStep(_steps);

	}
	else { CString text = _T("Enable Stepper mode first"); _rEditStatus.SetWindowTextW((LPCTSTR)text); }
}
void CTTSDlg::OnBnClickedPositionOpenFile()
{
	if (_isStepper == true)
	{
		if (_isOpen == false)
		{
			startPosition = _tmotor->ReadPosition();
			CString path[3];
			if (!_isOpen_position)
				if (!_OpenOrSaveOneFileDialogue(false, "txt", path))
					return;

			// Flip state:
			_isOpen_position = !_isOpen_position;
			// Change button state and text:
			if (!_isOpen_position)
			{
				_rButtonPositionOpenFile.SetWindowTextW(L"OPEN FILE");
				CString text = _T("File closed, data saved");
				_rEditStatus.SetWindowTextW((LPCTSTR)text);
			}
			else
			{
				_rButtonPositionOpenFile.SetWindowTextW(L"CLOSE FILE");
				CString text = _T("File opened, saving data...");
				_rEditStatus.SetWindowTextW((LPCTSTR)text);
			}

			// Opn file for saving
			if (_isOpen_position)	_SaveData(path[2]);
			// Close file
			else			_SaveData(L"Close");
		}
		else
		{CString text = _T("Close previous file first."); _rEditStatus.SetWindowTextW((LPCTSTR)text);}
	}
	else { CString text = _T("Enable Stepper mode first"); _rEditStatus.SetWindowTextW((LPCTSTR)text); }
}
void CTTSDlg::OnBnClickedPositionStart()
{
	if (_isStepper == true)
	{
		accuracy = true;
		busy = false;
		trapezoid = false;

		//startPosition = _tmotor->ReadPosition();
		//_position = _tmotor->ReadPosition();

		// TORQUE CONVERSION
		CString text;
		_rEditCounterTorque.GetWindowText(text);
		pos_torque = _wtoi(text);
		// current  = ...

		_rEditHoseLength3.GetWindowText(text);
		_hoseLength = _wtoi(text);

		_rEditPressure3.GetWindowText(text);
		_pressure = _wtoi(text);

		_rEditNumberSteps.GetWindowText(text);
		pos_steps = _wtoi(text);

		_rEditPositionVelocity.GetWindowText(text);
		pos_velocity = _wtoi(text);

		_rEditPositionAcceleration.GetWindowText(text);
		pos_acceleration = _wtoi(text);

		position_counter = 0;

		_teensy->SetHold();
		std::this_thread::sleep_for(std::chrono::milliseconds(500)); // if too low it will ignore next command!!!!!!!!
		startPosition = _tmotor->ReadPosition();

		_rButtonPosition.SetIcon(hicon2);
	}
	else { CString text = _T("Enable Stepper mode first"); _rEditStatus.SetWindowTextW((LPCTSTR)text); }
}

void CTTSDlg::OnBnClickedReset()
{
	reset_pos = _tmotor->ReadPosition();
}
