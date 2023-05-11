
// LAB2Dlg.h : header file
//

#ifndef LAB2_H_
#define LAB2_H_
#pragma once

#include "SerialCtrl.h"
#include "afxwin.h"

#include "ChartCtrl/ChartCtrl.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartLabel.h"

// CLAB2Dlg dialog
class CLAB2Dlg : public CDialogEx, public CSerialIO
{
// Construction
public:
	CLAB2Dlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAB2_DIALOG };
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
	DECLARE_MESSAGE_MAP()
	int m_rad_offline;
	int m_rad_online;
public:
	virtual void OnEventOpen(BOOL bSuccess);
	virtual void OnEventClose(BOOL bSuccess);
	virtual void OnEventRead(unsigned char* inPacket, int inLength);
	virtual void OnEventWrite(int nWritten);
	CListBox mlsbReadSerial;
	afx_msg void OnCbnDropdownComboSerialname();
	CComboBox mCbSerialName;
	CComboBox mCbBaudRate;
	BOOL bPortOpened;
	CButton mBtnOpen;
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonTuning();
	afx_msg void OnBnClickedButtonRequestPid();
	VOID DrawPIDGraph();
	VOID DrawOperationGraph();
	VOID ProcessData(unsigned char* data, int inLength);
	CChartCtrl pChartCtrlPID;
	CChartStandardAxis* pBottomAxis;
	CChartStandardAxis* pLeftAxis;
	CChartLineSerie* pChartPIDSeries;
	CChartLineSerie* pChartPosSeries;
	CChartLineSerie* pChartVelSeries;
	CChartLineSerie* pChartAccSeries;
	CChartLineSerie* pChartPosSeriesRef;
	afx_msg void OnBnClickedButtonDraw();
	afx_msg void OnBnClickedButtonSendpos();
	double dAccMax;
	afx_msg void OnBnClickedButtonOprun();
	afx_msg void OnBnClickedButtonGetRms();
	CStatic mstaticInfo;
	CChartCtrl pChartCtrlVel;
	CChartCtrl pChartCtrlPos;
	CChartCtrl pChartCtrlAcc;
	CButton mBtnProfileDraw;
	CButton mBtnSendPID;
	CButton mBtnTuning;
	CButton mBtnREQ;
	CButton mBtnSendRunPos;
	CButton mBtnGETRMS;
	CButton mBtnSendSetting;
	CStatic mStRMS;
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	double GetDlgItemDouble(int nID);
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
#endif //LAB2_H_
