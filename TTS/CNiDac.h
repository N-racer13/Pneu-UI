#pragma once

class CNiDac
{
public:

	CNiDac();
	~CNiDac();


	bool	Initialize();
	void	Close();

	long float	ReadAnalogInput(void);
	//long	ReadRpm(void);
	//short	ReadCurrent(void);
	//void	SetCurrent(short current);
	//short	GetNominalCurrent(void);
	//void	CurrentModeOn(void);


	

private:
	//TaskHandle  taskHandle;
	//DWORD	_ulErrorCode;
	
	//static const WORD	_nNodes = 1;		// Number of Nodes (Axes)
	//static const WORD	_node[_nNodes];		// Number of Nodes (Axes)
	//static const WORD	_aI[_nNodes];		// Digital Inputs
	//static const double	_TR[_nNodes];		// Motor gearbox TR

	
	
	//static const WORD	_Node = 1U;

	short	nomCurr = 582;					// In mA
	bool	currentModeOn = false;
	

	
};

