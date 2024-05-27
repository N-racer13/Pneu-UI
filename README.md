# Pneu-UI
Contains code for Pneumatic motor control and UI. All code written in C++ for Microsoft Visual C desktop app.
  - CNiDac: responsible for NI force senor integration, uses NI custom libraries
  - CTmotor: controls brushless DC motor over CAN and contains anti-windup PID algorithms for speed and position control
  - CTeensy: interfaces with Teensy board for pneumatic valve control
  - TTSDlg: Main script that interfaces makes calls to all scripts
  - Remaining scripts necessary for correct functioning in Microsoft
