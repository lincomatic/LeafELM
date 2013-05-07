/*
Copyright (c) 2013 Sam C. Lin

The files in this project free software; you can redistribute
and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

These files are distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

// LeafELMDlg.h : header file
//

#pragma once

#include "Serial.h"
#include "afxwin.h"


#define CR 13 // \r - carriage return
#define PROMPT '>'

typedef long int32_t;
typedef unsigned long uint32_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef char int8_t;
typedef unsigned char uint8_t;


typedef unsigned char CAN_DATA[8];



// CLeafELMDlg dialog
class CLeafELMDlg : public CDialogEx
{
// Construction
public:
	CLeafELMDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CLeafELMDlg();

// Dialog Data
	enum { IDD = IDD_LEAFELM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	FILE *ofp;
	char buf[256];
	CSerial serial;

	CAN_DATA candata[30];
	int m_Volts[BATT_CNT];
	int m_Vmax,m_Vmin,m_Vavg;
	char m_Shunts[BATT_CNT];
	int32_t m_Soc,m_PackCap,m_PackHealth;
	uint8_t m_BattTemp[4];


	int SetupELM();

	int ReadLine(char *buf,int maxchar);
	int WaitAck(char *buf,int maxchar);
	void Group1();
	void CellVoltages();
	void PackTemps();	
	void Shunts();
	int ReadCanData(int msgcnt);

	void DrawVoltage(CDC *dc,int x,int y,int volts,COLORREF color);
	void DrawBox(CDC *dc,int x,int y,int w,int h,COLORREF color);
	int OpenSerial(char *portname);
	int QueryELM();

	void FindSerialPorts();

	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedConnect();
	CButton m_btnConnect;
	CComboBox m_cmbCommPort;
};
