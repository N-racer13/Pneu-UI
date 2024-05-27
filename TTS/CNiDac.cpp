#include "pch.h"
#include "framework.h"
#include "TtsDlg.h"
#include "Definitions.h"

#include "NIDAQmx.h"
#include <stdio.h>
#include "CNiDac.h"


//const WORD	 CNiDac::_node[_nNodes]		= { 1 };		// Node mapping
//const WORD	 CNiDac::_aI[_nNodes]		= { 1 };		// Analog input mapping
//const double	 CNiDac::_TR[_nNodes]	= { 57./13. };	// Motor gearbox TR


CNiDac::CNiDac()
{
	
}
CNiDac::~CNiDac()
{

}

// CREATE SAMPLING TIME FUNCTION
bool CNiDac::Initialize()
{
	TaskHandle  taskHandle = 0;
	DAQmxCreateTask("", &taskHandle);
	DAQmxCreateAIVoltageChan(taskHandle, "Dev1/ai0", "", DAQmx_Val_Cfg_Default, -10.0, 10.0, DAQmx_Val_Volts, NULL);
	DAQmxCfgSampClkTiming(taskHandle, "", 10000.0, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, 10);


	return true;
}


void CNiDac::Close()
{

}

long float CNiDac::ReadAnalogInput()
{
	//THIS IS FUCKING STUPID WHAT THE HELL NIDAQ

	TaskHandle  taskHandle = 0;

	DAQmxCreateTask("", &taskHandle);
	DAQmxCreateAIVoltageChan(taskHandle, "Dev1/ai0", "", DAQmx_Val_Cfg_Default, -10.0, 10.0, DAQmx_Val_Volts, NULL);
	DAQmxCfgSampClkTiming(taskHandle, "", 10000.0, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, 10);

	short n = 0;
	long ai;
	float64 data[10];
	int32 read;
	
	DAQmxReadAnalogF64(taskHandle, -1, 0.01, DAQmx_Val_GroupByChannel, data, 10, &read, NULL);

	long float sum = 0.0;
	for (int i = 0; i < 10; i++) {
		sum += data[i];
	}
	sum /= 10;
	
	return sum;
}