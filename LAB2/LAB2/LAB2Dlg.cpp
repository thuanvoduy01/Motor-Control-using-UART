
// LAB2Dlg.cpp : implementation file
//

#include "pch.h"
#include "LAB2.h"
#include "LAB2Dlg.h"
#include "afxdialogex.h"
#include <winnt.h>
#include <vector>
#include <cstddef>
#include <windef.h>
#include <cstringt.h>
#include <afxstr.h>
#include <atlsimpstr.h>
#include <tchar.h>
#include "Motion/MotionGenerator.h"
#include "ultil.h"
#include "ChartCtrl/ChartAxisLabel.h"
#include "ChartCtrl/ChartString.h"
#include "SerialCtrl.h"
#include "ChartCtrl\ChartCtrl.h"
#include "ChartCtrl\ChartLineSerie.h"
#include "ChartCtrl\ChartLabel.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

BYTE bFlag[20] = {};
#define EVENT_SEND_BY_TIMER 2

//STX
BYTE bSTX[] = { 0x02 };
//CMD LAB1
BYTE bMOVL[] = { 0x4D,	0x4F,	0x56,	0x4C };
BYTE bGPOS[] = { 0x47,	0x50,	0x4F,	0x53 };
BYTE bGVEL[] = { 0x47,	0x56,	0x45,	0x4C };
BYTE bSTT[] = { 0x47,	0x53,	0x54,	0x54 };
//CMD LAB2
BYTE bSPID[] = { 0x53,	0x50,	0x49,	0x44 }; //ASCII {SPID}
BYTE bGPID[] = { 0x47,	0x50,	0x49,	0x44 }; //ASCII {GPIO}
BYTE bCTUN[] = { 0x43,	0x54,	0x55,	0x4E }; //ASCII {CTUN}
BYTE bCRUN[] = { 0x43,	0x52,	0x55,	0x4E }; //ASCII {CRUN}
BYTE bCSET[] = { 0x43,	0x53,	0x45,	0x54 }; //ASCII {CSET}
BYTE bGRMS[] = { 0x47,	0x52,	0x4D,	0x53 }; //ASCII {GRMS}
//OPTIO
BYTE bOPT[] = { 0x0,0x0,0x00 };
//DATA
BYTE bDATA[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
//SYNC/ACK
BYTE bSYNC[] = { 0x16 };
BYTE bACK[] = { 0x06 };
//ETX
BYTE bETX[] = { 0x03 };

//RECEIVE
BYTE bProtocolBuffer[18] = { };
BYTE bProtocolData[8] = {};
BYTE bProtocolOption[3] = {};

uint16_t dRMS;
int nLenGraphPID;
double dPIDPlotData[1000] = {};
double dPIDPlotTime[1000] = {};

double fPositionPlotData[1000] = {};
float fVelPlotData[1000] = {};
float fAccPlotData[1000] = {};
int nLenGraphOperation;
int nLengtPositionPlotData = 0;

VOID CLAB2Dlg::DrawPIDGraph()
{
	/*pChartPIDSeries->ClearSerie();
	CChartLineSerie* pCmdStepRef;
	pCmdStepRef = pChartCtrlPID.CreateLineSerie();
	pCmdStepRef->ClearSerie();
	pCmdStepRef->SetWidth(5);
	pCmdStepRef->SetColor(RGB(255, 0, 0));
	for (UINT8 i = 0; i < 200; i++) {
		if (i == 0) {
			dPIDPlotTime[i] = 0.01;
		}
		dPIDPlotTime[i] = dPIDPlotTime[i - 1] + 0.01;
		pChartPIDSeries->AddPoint((double)dPIDPlotTime[i], (double)dPIDPlotData[i]);
		pCmdStepRef->AddPoint((double)dPIDPlotTime[i], (double)65);
	}
	*/
	pChartPIDSeries->ClearSerie();
	for (int i = 0; i < nLenGraphPID; i++) {
		pChartPIDSeries->AddPoint(dPIDPlotTime[i], dPIDPlotData[i]);
	}
}

void CLAB2Dlg::DrawOperationGraph()
{
	pChartPosSeriesRef = pChartCtrlPos.CreateLineSerie();
	pChartPosSeriesRef->SetColor(RGB(255, 0, 0));
	pChartPosSeries->SetWidth(5); //line width
	pChartPosSeriesRef->ClearSerie();

	for (int i = 0; i < nLengtPositionPlotData; i++) {
		pChartPosSeriesRef->AddPoint(dPIDPlotTime[i], fPositionPlotData[i]);
	}
	//pChartVelSeries->ClearSerie();
	//for (int i = 0; i < nLenGraphOperation; i++) {
	//	pChartVelSeries->AddPoint(fPIDPlotTime[i], fVelPlotData[i]);
	//}
	//pChartAccSeries->ClearSerie();
	//for (int i = 0; i < nLenGraphOperation; i++) {
	//	pChartAccSeries->AddPoint(fPIDPlotTime[i], fAccPlotData[i]);
	//}
}

VOID CLAB2Dlg::ProcessData(unsigned char* data, int inLength) {
	//CString Cmd;
	//STX CMD OPTION DATA ACK
	//8 9 10 11 12 13 14 15
	CString str;
	uint16_t nIndex = 0;
	uint16_t nPosition = 0;
	CString csCmd, csOption, csData;
	for (UINT i = 0; i < static_cast<UINT>(inLength); i++) {
		bProtocolBuffer[i] = static_cast<BYTE>(data[i]);
	}
	for (UINT i = 1; i <= 4; i++) {
		csCmd.AppendChar(static_cast<char>(bProtocolBuffer[i]));
	}
	for (UINT i = 5; i <= 7; i++) {
		bProtocolOption[i - 5] = bProtocolBuffer[i];
	}
	for (UINT i = 8; i <= 15; i++) {
		bProtocolData[i - 8] = bProtocolBuffer[i];
	}

	if (!csCmd.Compare(static_cast<CString>("SPID"))) {

	}
	if (!csCmd.Compare(static_cast<CString>("GPID"))) {
		//m_btnREQ
		nLenGraphPID = (bProtocolData[0] << 8) + bProtocolData[1];
		//TODO:
		nIndex = (bProtocolData[2] << 8) + bProtocolData[3];
		nPosition = (bProtocolData[6] << 8) + bProtocolData[7];
		dPIDPlotData[nIndex] = nPosition;
		if (nIndex == (bProtocolData[0] << 8) + bProtocolData[1]) {
			DrawPIDGraph();
		}
	}
	if (!csCmd.Compare(static_cast<CString>("CTUN"))) {
		if (bProtocolBuffer[16] == 0x06) {
			//Receive SPID OK
		}
	}
	if (!csCmd.Compare(static_cast<CString>("CRUN"))) {
		if (bProtocolBuffer[16] == 0x06) {
			//Receive SPID OK
		}
	}

	if (!csCmd.Compare(static_cast<CString>("GRMS"))) {
		if (bProtocolBuffer[16] == 0x06) {
			//Receive SPID OK
		}
	}
	if (!csCmd.Compare(static_cast<CString>("CSET"))) {
		if (bProtocolBuffer[16] == 0x06) {
			//Receive SPID OK
		}
	}
	if (!csCmd.Compare(static_cast<CString>("GRMS"))) {
		//if (bProtocolBuffer[16] == 0x06) {
			//Receive SPID OK
		nLengtPositionPlotData = (bProtocolData[0] << 8) + bProtocolData[1];
		//if ((bProtocolData[2] << 8) + bProtocolData[3] >= nLengtPositionPlotData) return;
		nIndex = (bProtocolData[2] << 8) + bProtocolData[3];
		nPosition = (bProtocolData[6] << 8) + bProtocolData[7];

		fPositionPlotData[nIndex] = nPosition;

		if (nLengtPositionPlotData == nIndex) {
			DrawOperationGraph();
			uint16_t nRms = (bProtocolData[4] << 8) + bProtocolData[5];

			str.Format(static_cast<CString>("%d "), nRms);

			mStRMS.SetWindowText(str);
		}
		//}
	}

}

CLAB2Dlg::CLAB2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LAB2_DIALOG, pParent)
	, m_rad_offline(1)
	, m_rad_online(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLAB2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SERIALNAME, mCbSerialName);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, mCbBaudRate);
	DDX_Control(pDX, IDC_BUTTON_OPEN, mBtnOpen);
	DDX_Control(pDX, IDC_STATIC_INFO, mstaticInfo);
	DDX_Control(pDX, IDC_LIST_RECEIVEDATA, mlsbReadSerial);
	DDX_Control(pDX, IDC_CHARTCTRL_TUNING, pChartCtrlPID);
	DDX_Control(pDX, IDC_CHARTCTRL_POS, pChartCtrlPos);
	DDX_Control(pDX, IDC_CHARTCTRL_VEL, pChartCtrlVel);
	DDX_Control(pDX, IDC_CHARTCTRL_ACC, pChartCtrlAcc);
	DDX_Control(pDX, IDC_BUTTON_DRAW, mBtnProfileDraw);
	DDX_Control(pDX, IDC_BUTTON_SEND, mBtnSendPID);
	DDX_Control(pDX, IDC_BUTTON_TUNING, mBtnTuning);
	DDX_Control(pDX, IDC_BUTTON_REQUEST_PID, mBtnREQ);
	DDX_Control(pDX, IDC_BUTTON_OPRUN, mBtnSendRunPos);
	DDX_Control(pDX, IDC_BUTTON_GET_RMS, mBtnGETRMS);
	DDX_Control(pDX, IDC_BUTTON_SENDPOS, mBtnSendSetting);
	DDX_Control(pDX, IDC_STATIC_INFO2, mStRMS);
}

BEGIN_MESSAGE_MAP(CLAB2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_DROPDOWN(IDC_COMBO_SERIALNAME, &CLAB2Dlg::OnCbnDropdownComboSerialname)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CLAB2Dlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CLAB2Dlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_TUNING, &CLAB2Dlg::OnBnClickedButtonTuning)
	ON_BN_CLICKED(IDC_BUTTON_REQUEST_PID, &CLAB2Dlg::OnBnClickedButtonRequestPid)
	ON_BN_CLICKED(IDC_BUTTON_DRAW, &CLAB2Dlg::OnBnClickedButtonDraw)
	ON_BN_CLICKED(IDC_BUTTON_SENDPOS, &CLAB2Dlg::OnBnClickedButtonSendpos)
	ON_BN_CLICKED(IDC_BUTTON_OPRUN, &CLAB2Dlg::OnBnClickedButtonOprun)
	ON_BN_CLICKED(IDC_BUTTON_GET_RMS, &CLAB2Dlg::OnBnClickedButtonGetRms)
	ON_WM_TIMER()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO1, &CLAB2Dlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CLAB2Dlg::OnBnClickedRadio2)
END_MESSAGE_MAP()

BOOL CLAB2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	mCbBaudRate.InsertString(0, static_cast<CString>("4800"));
	mCbBaudRate.InsertString(1, static_cast<CString>("9600"));
	mCbBaudRate.InsertString(2, static_cast<CString>("19200"));
	mCbBaudRate.InsertString(3, static_cast<CString>("115200"));
	mCbBaudRate.SetCurSel(3);



	// TODO: Add extra initialization here
	//
	//
	//
	// Prepare PID plot time data - Axis x

	for (int i = 0; i < 1000; i++)
	{
		if (i == 0)
		{
			dPIDPlotTime[i] = 0.01;
		}
		else dPIDPlotTime[i] = dPIDPlotTime[i - 1] + 0.01;
	}

	CChartCtrl ref;
	ref.RemoveAllSeries();
	pChartCtrlPID.EnableRefresh(true);
	pBottomAxis = pChartCtrlPID.CreateStandardAxis(CChartCtrl::BottomAxis);
	pLeftAxis = pChartCtrlPID.CreateStandardAxis(CChartCtrl::LeftAxis);
	pBottomAxis->SetMinMax(0, 2);
	pLeftAxis->SetMinMax(0, 100);
	CChartAxisLabel* m_pAxisLabel = pLeftAxis->GetLabel();
	m_pAxisLabel->SetText(L"Position");
	m_pAxisLabel = pBottomAxis->GetLabel();
	m_pAxisLabel->SetText(L"Time");
	pBottomAxis->SetTickIncrement(true, 200);
	pBottomAxis->SetDiscrete(false);
	pBottomAxis->EnableScrollBar(true);

	pChartPIDSeries = pChartCtrlPID.CreateLineSerie();
	pChartPIDSeries->ClearSerie();
	pChartPIDSeries->SetWidth(5); //line width
	pChartPIDSeries->SetColor(RGB(0, 0, 255)); //color of line


	ref.RemoveAllSeries();
	pChartCtrlPos.EnableRefresh(true);
	pBottomAxis = pChartCtrlPos.CreateStandardAxis(CChartCtrl::BottomAxis);
	pLeftAxis = pChartCtrlPos.CreateStandardAxis(CChartCtrl::LeftAxis);
	pBottomAxis->SetMinMax(0, 3);
	pLeftAxis->SetMinMax(0, 1000);
	//pLeftAxis->GetLabel()->SetText("Intensity");
	pBottomAxis->SetTickIncrement(true, 20.0);
	pBottomAxis->SetDiscrete(false);
	pBottomAxis->EnableScrollBar(false);
	pChartPosSeries = pChartCtrlPos.CreateLineSerie();
	pChartPosSeries->ClearSerie();
	pChartPosSeries->SetWidth(5); //line width
	pChartPosSeries->SetColor(RGB(0, 0, 255)); //color of line


	ref.RemoveAllSeries();
	pChartCtrlVel.EnableRefresh(true);
	pBottomAxis = pChartCtrlVel.CreateStandardAxis(CChartCtrl::BottomAxis);
	pLeftAxis = pChartCtrlVel.CreateStandardAxis(CChartCtrl::LeftAxis);
	pBottomAxis->SetMinMax(0, 3);
	pLeftAxis->SetMinMax(-500, 500);
	//pLeftAxis->GetLabel()->SetText("Intensity");
	pBottomAxis->SetTickIncrement(true, 20.0);
	pBottomAxis->SetDiscrete(false);
	pBottomAxis->EnableScrollBar(true);
	pChartVelSeries = pChartCtrlVel.CreateLineSerie();
	pChartVelSeries->ClearSerie();
	pChartVelSeries->SetWidth(5); //line width
	pChartVelSeries->SetColor(RGB(0, 0, 255)); //color of line

	ref.RemoveAllSeries();
	pChartCtrlAcc.EnableRefresh(true);
	pBottomAxis = pChartCtrlAcc.CreateStandardAxis(CChartCtrl::BottomAxis);
	pLeftAxis = pChartCtrlAcc.CreateStandardAxis(CChartCtrl::LeftAxis);
	pBottomAxis->SetMinMax(0, 3);
	pLeftAxis->SetMinMax(-600, 600);
	//pLeftAxis->GetLabel()->SetText("Intensity");
	pBottomAxis->SetTickIncrement(true, 20.0);
	pBottomAxis->SetDiscrete(false);
	pBottomAxis->EnableScrollBar(true);

	pChartAccSeries = pChartCtrlAcc.CreateLineSerie();
	pChartAccSeries->ClearSerie();
	pChartAccSeries->SetWidth(5); //line width
	pChartAccSeries->SetColor(RGB(0, 0, 255)); //color of line

	//m_ChartCtrl./*EnableRefresh*/(true);
	//
	return TRUE; // return TRUE  unless you set the focus to a control
}

void CLAB2Dlg::OnPaint()
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
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CLAB2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLAB2Dlg::OnEventOpen(BOOL bSuccess)
{
	CString str;
	if (bSuccess)
	{
		str = m_csPortName + static_cast<CString>(" open successfully");

		bPortOpened = TRUE;
		mBtnOpen.SetWindowText(static_cast<CString>("Close"));

	}
	else
	{
		str = m_csPortName + static_cast<CString>(" open failed");
	}
	mstaticInfo.SetWindowText(str);
}

void CLAB2Dlg::OnEventClose(BOOL bSuccess)
{
	CString str;
	if (bSuccess)
	{
		str = m_csPortName + static_cast<CString>("close successfully");
		bPortOpened = FALSE;
		mBtnOpen.SetWindowText(static_cast<CString>("Open"));

	}
	else
	{
		str = m_csPortName + static_cast<CString>(" close failed");
	}
	mstaticInfo.SetWindowText(str);
}

void CLAB2Dlg::OnEventRead(unsigned char* inPacket, int inLength)
{
	CString csInPacket;
	csInPacket.Format(static_cast<CString>("REC: "));
	mlsbReadSerial.InsertString(0, csInPacket);
	csInPacket.Empty();

	for (UINT i = 0; i < static_cast<UINT>(inLength); i++) {
		csInPacket.AppendFormat(static_cast<CString>("%02X "), inPacket[i]);
	}
	mlsbReadSerial.InsertString(0, csInPacket);

	ProcessData(inPacket, inLength);

	//m_listboxRead.InsertString(0, csInPacket);
	/*m_listboxRead.AddString(inPacket);*/

	CString str;
	str.Format(static_cast<CString>("%d bytes read"), inLength);

	mstaticInfo.SetWindowText(str);

}
void CLAB2Dlg::OnEventWrite(int nWritten)
{
	if (nWritten > 0)
	{
		CString str;
		str.Format(static_cast<CString>("%d bytes written"), nWritten);
		mstaticInfo.SetWindowText(str);
	}
	else
	{
		mstaticInfo.SetWindowText(static_cast<CString>("Write failed"));
	}

}

void CLAB2Dlg::OnBnClickedButtonOpen()
{
	// TODO: Add your control notification handler code here
	if (bPortOpened == FALSE)
	{
		CString csPortName;
		CString csBaudRate;

		mCbSerialName.GetLBText(mCbSerialName.GetCurSel(), csPortName);
		mCbBaudRate.GetLBText(mCbBaudRate.GetCurSel(), csBaudRate);


		OpenPort(csPortName, csBaudRate);

	}
	else
	{
		ClosePort();
	}
}

void CLAB2Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	BYTE FRAME[50] = {};
	UINT index = 0;
	CString cmd;

	if (!GetPortActivateValue()) return;

	switch (nIDEvent) {
	case EVENT_SEND_BY_TIMER:

		//Write((char*)FRAME, index);
		//cmd.Format((LPCWSTR)"%s", "Timer STT CMD: ");
		//m_listboxRead.InsertString(0, cmd);
		//cmd.Empty();
		//for (UINT i = 0; i < index; i++) {

		//	cmd.AppendFormat((LPCWSTR)"%02X ", FRAME[i]);

		//}
		//m_listboxRead.InsertString(0, cmd);
		break;
	default:
		break;
	}

	__super::OnTimer(nIDEvent);
}

void CLAB2Dlg::OnCbnDropdownComboSerialname()
{
	// TODO: Add your control notification handler code here
	mCbSerialName.ResetContent();

	TCHAR lpTargetPath[5000]; // buffer to store the path of the COMPORTS
	bool gotPort = false; // in case the port is not found

	for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
	{
		CString str;
		str.Format(_T("%d"), i);
		CString ComName = CString("COM") + str; // converting to COM0, COM1, COM2


		DWORD test = QueryDosDevice(ComName, static_cast<LPWSTR>(lpTargetPath), 2000);

		// Test the return value and error if any
		if (test != 0) //QueryDosDevice returns zero if it didn't find an object
		{
			mCbSerialName.AddString(static_cast<CString>(ComName)); // add to the ComboBox
			gotPort = true; // found port
		}

		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			//lpTargetPath[2000]; // in case the buffer got filled, increase size of the buffer.
			continue;
		}

	}

	if (!gotPort) // if not port
		mCbSerialName.AddString(static_cast<CString>("No Active Ports Found")); // to display error message incase no ports found
}

double CLAB2Dlg::GetDlgItemDouble(int nID)
{
	CString sTemp;
	GetDlgItem(nID)->GetWindowText(sTemp);
	double dRet = _tstof(static_cast<LPCTSTR>(sTemp));
	return dRet;
}

void CLAB2Dlg::OnBnClickedButtonSend()
{
	// TODO: Add your control notification handler code here
	CString strData;
	//DWORD dwRetCode;
	memset(bDATA, '\0', 8);

	double dKp = GetDlgItemDouble(IDC_EDIT_KP);
	double dKi = GetDlgItemDouble(IDC_EDIT_KI);
	double dKd = GetDlgItemDouble(IDC_EDIT_KD);
	// TODO: Add your control notification handler code here
	BYTE bProtocol[50] = {};
	UINT index = 0;

	uint8_t nLengTithes;
	BYTE bKp[2], bKi[2], bKd[3];

	FloatToByteArray(dKp, bKp, &nLengTithes);
	bDATA[0] = bKp[0];
	bDATA[1] = bKp[1];

	FloatToByteArray(dKi, bKi, &nLengTithes);
	bDATA[2] = bKi[0];
	bDATA[3] = bKi[1];

	FloatToByteArrayWithNipes(dKd, bKd, &nLengTithes);
	bKd[2] = nLengTithes;
	bDATA[4] = bKd[0];
	bDATA[5] = bKd[1];
	bDATA[6] = bKd[2];

	if (!GetPortActivateValue()) return;

	memcpy(bProtocol + index, bSTX, sizeof(bSTX));
	index += sizeof(bSTX);
	memcpy(bProtocol + index, bSPID, sizeof(bSPID));
	index += sizeof(bSPID);
	memcpy(bProtocol + index, bOPT, sizeof(bOPT));
	index += sizeof(bOPT);
	memcpy(bProtocol + index, bDATA, sizeof(bDATA));
	index += sizeof(bDATA);
	memcpy(bProtocol + index, bSYNC, sizeof(bSYNC));
	index += sizeof(bSYNC);
	memcpy(bProtocol + index, bETX, sizeof(bETX));
	index += sizeof(bETX);
	Write((char*)bProtocol, index);

	//Show on text box
	CString cmd;
	cmd.Format(static_cast<CString>("CMD: "));
	mlsbReadSerial.InsertString(0, cmd);
	cmd.Empty();
	for (UINT i = 0; i < index; i++) {

		cmd.AppendFormat(L"%02X ", bProtocol[i]);

	}
	mlsbReadSerial.InsertString(0, cmd);
}

void CLAB2Dlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here

		//1 second interval timer
	SetTimer(EVENT_SEND_BY_TIMER, 1000, nullptr);         // handle to main window 

}


void CLAB2Dlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	KillTimer(EVENT_SEND_BY_TIMER);

	mlsbReadSerial.InsertString(0, _T("Stop timer"));
}

void CLAB2Dlg::OnBnClickedButtonDraw()
{
	double dPosRef = GetDlgItemDouble(IDC_EDIT_POS_MAX);
	double dVelMax = GetDlgItemDouble(IDC_EDIT_VELMAX);
	double dAccMax = GetDlgItemDouble(IDC_EDIT_ACC_MAX);

	MotionGenerator* trapezoidalProfile = new MotionGenerator(static_cast<float>(dVelMax), static_cast<float>(dAccMax),
		0);

	float fCalPosition[200], fCalVel[200], fCalAcc[200];
	// Retrieve calculated position
	double positionRef = dPosRef;
	float positionRefC[200];
	//float position = trapezoidalProfile->update(positionRef);
	//float velocity, acceleration;

	float fTime[200];
	for (int i = 0; i < 200; i++) {
		positionRefC[i] = dPosRef;
		if (i == 0) {
			fTime[i] = 0.025;
		}
		else fTime[i] = fTime[i - 1] + 0.025;
	}
	for (int i = 0; i < 200; i++) {

		fCalPosition[i] = trapezoidalProfile->update(positionRefC[i], fTime[i]);  // NOLINT(bug prone-narrowing-conversions)
		// Check if profile is finished
		fCalVel[i] = trapezoidalProfile->getVelocity();

		//// Retrieve current acceleration
		fCalAcc[i] = trapezoidalProfile->getAcceleration();


		//// Retrieve current velocity

	}
	if (trapezoidalProfile->getFinished()) {};

	// Reset internal state
	trapezoidalProfile->reset();

	pChartPosSeries->ClearSerie();
	for (int i = 0; i < 200; i++) {
		//pChartPosSeries->AddPoint(fTime[i], fCalPosition[i]);

		pChartPosSeries->AddPoint(fTime[i], dPosRef);
	}

	pChartVelSeries->ClearSerie();
	for (int i = 0; i < 200; i++) {
		pChartVelSeries->AddPoint(fTime[i], fCalVel[i]);
	}
	pChartAccSeries->ClearSerie();
	for (int i = 0; i < 200; i++) {
		pChartAccSeries->AddPoint(fTime[i], fCalAcc[i]);
	}

}

void CLAB2Dlg::OnBnClickedButtonTuning()
{
	BYTE bProtocol[50] = {};
	UINT index = 0;

	// TODO: Add your control notification handler code here
	if (!GetPortActivateValue()) return;
	memset(bDATA, '\0', 8);

	memcpy(bProtocol + index, bSTX, sizeof(bSTX));
	index += sizeof(bSTX);
	memcpy(bProtocol + index, bCTUN, sizeof(bCTUN));
	index += sizeof(bCTUN);
	memcpy(bProtocol + index, bOPT, sizeof(bOPT));
	index += sizeof(bOPT);
	memcpy(bProtocol + index, bDATA, sizeof(bDATA));
	index += sizeof(bDATA);
	memcpy(bProtocol + index, bSYNC, sizeof(bSYNC));
	index += sizeof(bSYNC);
	memcpy(bProtocol + index, bETX, sizeof(bETX));
	index += sizeof(bETX);
	Write((char*)bProtocol, index);

	//Show on text box
	CString cmd;
	cmd.Format(static_cast<CString>("CMD: "));
	mlsbReadSerial.InsertString(0, cmd);
	cmd.Empty();
	for (UINT i = 0; i < index; i++) {

		cmd.AppendFormat(static_cast<CString>("%02X "), bProtocol[i]);

	}
	mlsbReadSerial.InsertString(0, cmd);
}


void CLAB2Dlg::OnBnClickedButtonRequestPid()
{
	BYTE bProtocol[50] = {};
	UINT index = 0;
	// TODO: Add your control notification handler code here
	memset(bDATA, '\0', 8);
	if (!GetPortActivateValue()) return;


	memcpy(bProtocol + index, bSTX, sizeof(bSTX));
	index += sizeof(bSTX);
	memcpy(bProtocol + index, bGPID, sizeof(bGPID));
	index += sizeof(bGPID);
	memcpy(bProtocol + index, bOPT, sizeof(bOPT));
	index += sizeof(bOPT);
	memcpy(bProtocol + index, bDATA, sizeof(bDATA));
	index += sizeof(bDATA);
	memcpy(bProtocol + index, bSYNC, sizeof(bSYNC));
	index += sizeof(bSYNC);
	memcpy(bProtocol + index, bETX, sizeof(bETX));
	index += sizeof(bETX);
	Write((char*)bProtocol, index);

	//Show on text box
	CString cmd;
	//cmd.Format((CString)"%s", "CMD: ");
	//m_listboxRead.InsertString(0, cmd);
	//cmd.Empty();
	for (UINT i = 0; i < index; i++) {

		cmd.AppendFormat(static_cast<CString>("%02X "), bProtocol[i]);

	}
	mlsbReadSerial.InsertString(0, cmd);
}

void CLAB2Dlg::OnBnClickedButtonOprun()
{
	BYTE bProtocol[50] = {};
	UINT index = 0;

	memset(bDATA, '\0', 8);
	// TODO: Add your control notification handler code here
	if (!GetPortActivateValue()) return;


	memcpy(bProtocol + index, bSTX, sizeof(bSTX));
	index += sizeof(bSTX);
	memcpy(bProtocol + index, bCRUN, sizeof(bCRUN));
	index += sizeof(bCRUN);
	memcpy(bProtocol + index, bOPT, sizeof(bOPT));
	index += sizeof(bOPT);
	memcpy(bProtocol + index, bDATA, sizeof(bDATA));
	index += sizeof(bDATA);
	memcpy(bProtocol + index, bSYNC, sizeof(bSYNC));
	index += sizeof(bSYNC);
	memcpy(bProtocol + index, bETX, sizeof(bETX));
	index += sizeof(bETX);
	Write((char*)bProtocol, index);

	//Show on text box
	CString cmd;
	cmd.Format(static_cast<CString>("CMD: "));
	mlsbReadSerial.InsertString(0, cmd);
	cmd.Empty();
	for (UINT i = 0; i < index; i++) {

		cmd.AppendFormat(static_cast<CString>("%02X "), bProtocol[i]);

	}
	mlsbReadSerial.InsertString(0, cmd);
}
void CLAB2Dlg::OnBnClickedButtonGetRms()
{
	BYTE bProtocol[50] = {};
	UINT index = 0;
	memset(bDATA, '\0', 8);
	// TODO: Add your control notification handler code here
	if (!GetPortActivateValue()) return;


	memcpy(bProtocol + index, bSTX, sizeof(bSTX));
	index += sizeof(bSTX);
	memcpy(bProtocol + index, bGRMS, sizeof(bGRMS));
	index += sizeof(bGRMS);
	memcpy(bProtocol + index, bOPT, sizeof(bOPT));
	index += sizeof(bOPT);
	memcpy(bProtocol + index, bDATA, sizeof(bDATA));
	index += sizeof(bDATA);
	memcpy(bProtocol + index, bSYNC, sizeof(bSYNC));
	index += sizeof(bSYNC);
	memcpy(bProtocol + index, bETX, sizeof(bETX));
	index += sizeof(bETX);
	Write((char*)bProtocol, index);

	CString cmd;
	cmd.Format(static_cast<CString>("CMD: "));
	mlsbReadSerial.InsertString(0, cmd);
	cmd.Empty();
	for (UINT i = 0; i < index; i++) {

		cmd.AppendFormat(static_cast<CString>("%02X "), bProtocol[i]);

	}
	mlsbReadSerial.InsertString(0, cmd);
}

void CLAB2Dlg::OnBnClickedButtonSendpos()
{
	BYTE bProtocol[50] = {};
	UINT index = 0;
	memset(bDATA, '\0', 8);

	double dPosRef = GetDlgItemDouble(IDC_EDIT_POS_MAX);
	double dVelMax = GetDlgItemDouble(IDC_EDIT_VELMAX);
	double dAccMax = GetDlgItemDouble(IDC_EDIT_ACC_MAX);

	//dPosRef = 
	bDATA[2] = (static_cast<UINT16>(dAccMax) & 0xFF00) >> 8;
	bDATA[3] = static_cast<UINT16>(dAccMax) & 0xFF;
	bDATA[4] = (static_cast<UINT16>(dVelMax) & 0xFF00) >> 8;
	bDATA[5] = static_cast<UINT16>(dVelMax) & 0xFF;
	bDATA[6] = (static_cast<UINT16>(dPosRef) & 0xFF00) >> 8;
	bDATA[7] = static_cast<UINT16>(dPosRef) & 0xFF;
	// TODO: Add your control notification handler code here
	if (!GetPortActivateValue()) return;


	memcpy(bProtocol + index, bSTX, sizeof(bSTX));
	index += sizeof(bSTX);
	memcpy(bProtocol + index, bCSET, sizeof(bCSET));
	index += sizeof(bCSET);
	memcpy(bProtocol + index, bOPT, sizeof(bOPT));
	index += sizeof(bOPT);
	memcpy(bProtocol + index, bDATA, sizeof(bDATA));
	index += sizeof(bDATA);
	memcpy(bProtocol + index, bSYNC, sizeof(bSYNC));
	index += sizeof(bSYNC);
	memcpy(bProtocol + index, bETX, sizeof(bETX));
	index += sizeof(bETX);
	Write((char*)bProtocol, index);

	//Show on text box
	CString cmd;
	cmd.Format(static_cast<CString>("CMD: "));
	mlsbReadSerial.InsertString(0, cmd);
	cmd.Empty();
	for (UINT i = 0; i < index; i++) {

		cmd.AppendFormat(static_cast<CString>("%02X "), bProtocol[i]);

	}
	mlsbReadSerial.InsertString(0, cmd);
}

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

void CLAB2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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