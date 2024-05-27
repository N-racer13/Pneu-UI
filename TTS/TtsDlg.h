
// TtsDlg.h : header file
//

#pragma once
#include "CTMotor.h"
#include "CNiDac.h"
#include "CTeensy.h"
#include "string"
#include <queue>
using namespace std;

// CTTSDlg dialog
class CTTSDlg : public CDialogEx
{
// Private variables
private:
	CTMotor*	_tmotor;
	CNiDac*		_nidaq;
	CTeensy*	_teensy;
	UINT		_sampleRate;
	UINT		_datapoints;
	float		_current = 0;
	int			_maxCurrent;
	int			_saveCounter = 0;
	long float 	_forceZero = 0.0;
	bool		_isOpen;
	bool		_isOpen_position;
	bool		_isSaving;
	bool		_isMeasuring;
	bool		_isTesting;
	bool		_isStepper;
	bool		_targetReached = true;
	bool		torquePush = false;
	long		rpmPush = 0;
	int			_flow;
	int			_hoseLength;
	int			_pressure;
	long		_target = 0;
	int			forceBuffer;
	int			rpmBuffer;
	float		preError = 0.0;
	float		integrator = 0.0;
	bool		_zeroForceCheck = true;
	long		noLoadRpm = 0;
	queue<long float>	aiBuffer;
	queue<int>	motorRpmBuffer;
	int			_bufferCounter = 0;
	short		_steps;
	long float		startPosition = 0;
	long float		_position = 0;
	int			counterTorsue = 0;
	bool		accuracy;
	int			pos_torque = 0;
	int			pos_steps = 0;
	int			pos_velocity = 0;
	int			pos_acceleration = 0;
	bool		trapezoid;
	int			trapCount = 0;
	int			pos_cur = 0;
	bool		busy = false;
	int			position_counter = 0;
	int			direction = 1;
	long float		abs_pos;
	long float		reset_pos;
	int			stepsRead;
	int			stepTarget;

	long float	ai;

	long		motorRpm;

// Private functions:
	bool _OpenOrSaveOneFileDialogue(bool open, std::string fileExtension, CString* vPath);
	void _SaveData(CString filePath, long float ai = 0, UINT motorRpm = 0);
	void _RpmPID(long desRpm, long motorRpm);

// Construction
public:
	CTTSDlg(CWnd* pParent = nullptr);	// standard constructor
	void _ForcePID(long float ai, long motorRpm);


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TTS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:
	CEdit _rEditAnalogIn1;
	CEdit _rEditSampleRate;
	CEdit _rEditReadRpm;
	CEdit _rEditReadCurrent;
	CEdit _rEditSetCurrent;
	CEdit _rEditTarget;
	CEdit _rEditFlow;
	CEdit _rEditFlow2;
	CEdit _rEditMaxCurrent;
	CEdit _rEditDatapoints;
	CEdit _rEditStatus;
	CEdit _rEditPressure;
	CEdit _rEditPressure2;
	CEdit _rEditPressure3;
	CEdit _rEditHoseLength;
	CEdit _rEditHoseLength2;
	CEdit _rEditHoseLength3;
	CEdit _rEdit1;
	CEdit _rEditStepSpeed;
	CEdit _rEditCounterTorque;
	CEdit _rEditNumberSteps;
	CEdit _rEditPositionVelocity;
	CEdit _rEditPositionAcceleration;
	CEdit _rEditPosition;
	CButton _buttonSaveData;
	CButton _buttonMeasuring;
	CButton _rButtonSpin;
	CButton _rButtonSave;
	CButton _rButtonTorquePush;
	CButton _rRadioMeasure;
	CButton _rRadioTest;
	CButton _rRadioStepper;
	CButton _rButtonOpenFile;
	CButton _rEditStepperOpenFile;
	CButton _rButtonPosition;
	CButton _rButtonPositionOpenFile;
	CProgressCtrl _rpmProgress;
	CProgressCtrl _forceProgress;
	CProgressCtrl _currentProgress;

	HICON hicon1;
	HICON hicon2;
	HICON hicon3;

public:
	afx_msg void OnClickedSaveData();
	afx_msg void OnClickedStop();
	afx_msg void OnClickedCalibrate();
	afx_msg void OnClickedSpin();
	afx_msg void OnChangeSampleRate();
	afx_msg void OnChangeBoxCurrent();
	afx_msg void OnChangeSpeed();
	
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void _rpmProgressBar(long motorRpm);
	afx_msg void _forceProgressBar(long float ai);
	afx_msg void _currentProgressBar(float current);
	afx_msg void _torquePush(long motorRpm);
	afx_msg void OnBnClickedRadioMeasurement();
	afx_msg void OnBnClickedRadioTest();
	afx_msg void OnBnClickedOpenFile();
	afx_msg void OnBnClickedPositionOpenFile();
	afx_msg void OnBnClickedZeroForce();
	afx_msg void OnBnClickedTorquePush();
	afx_msg void OnBnClickedRadioStepper();
	afx_msg void OnEnChangeMaxCurrent();
	afx_msg void OnBnClickedStepperOpenFile();
	afx_msg void OnBnClickedStepTorquepush();
	afx_msg void OnBnClickedPositionStart();
	afx_msg void OnEnChangeCounterTorque();
	afx_msg void OnBnClickedReset();
};
