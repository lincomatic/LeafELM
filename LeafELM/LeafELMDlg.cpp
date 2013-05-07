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

// LeafELMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LeafELM.h"
#include "LeafELMDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLeafELMDlg dialog



CLeafELMDlg::CLeafELMDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLeafELMDlg::IDD, pParent)
{
  ofp = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	memset(candata,0,sizeof(CAN_DATA)*30);

	m_Soc = 0;
	m_PackCap = 0;
	m_PackHealth = 0;

	m_Vmax = 0;
	m_Vmin = 0;
	m_Vavg = 0;
	for (int i=0;i < BATT_CNT;i++) {
	  m_Volts[i] = 0;
	  m_Shunts[i] = 0;
	}

	m_BattTemp[0] = 0;
	m_BattTemp[1] = 0;
	m_BattTemp[2] = 0;
	m_BattTemp[3] = 0;
}

CLeafELMDlg::~CLeafELMDlg()
{
  serial.Close();
  if (ofp) {
    fflush(ofp);
    fclose(ofp);
  }
}

void CLeafELMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONNECT, m_btnConnect);
	DDX_Control(pDX, IDC_CMB_COMMPORT, m_cmbCommPort);
}

BEGIN_MESSAGE_MAP(CLeafELMDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT, &CLeafELMDlg::OnBnClickedConnect)
END_MESSAGE_MAP()


// CLeafELMDlg message handlers

BOOL CLeafELMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	FindSerialPorts();

	ShowWindow(SW_SHOWNORMAL);

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLeafELMDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLeafELMDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (IsIconic())
	{

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
	  dc.SetBkMode(OPAQUE);
	  CRect cr;
	  GetClientRect(&cr);
	  dc.FillSolidRect(0,0,cr.Width(),cr.Height(),RGB(255,255,255));
	  CSize ts = dc.GetTextExtent("0000",4);
	  CSize ts2 = dc.GetTextExtent("00-00",5);
	  char s[80];
	  int i=0;
	  int xx,yy,x,y;
	  for (y=0;y < 12;y++) {
	    yy = 5+y*(ts.cy+5);
	    
	    sprintf(s,"%02d-%02d",i,i+7);
	    dc.SetTextColor(RGB(255,255,255));
	    dc.SetBkColor(RGB(0,0,0));
	    dc.TextOut(5,yy,s);
	    dc.SetBkColor(RGB(255,255,255));
	    for (x=0;x < 8;x++) {
	      xx = 10+ts2.cx+x*(ts.cx+5);
	      
	      COLORREF txtcolor;
	      if (m_Volts[i] == m_Vmax) {
		txtcolor = RGB(0,192,0);
	      }
	      else if (m_Volts[i] == m_Vmin) {
		txtcolor = RGB(255,0,0);
	      }
	      else {
		txtcolor = RGB(0,0,0);
	      }
	      DrawVoltage(&dc,xx,yy,m_Volts[i],txtcolor);
	      if (m_Shunts[i]) DrawBox(&dc,xx,yy,ts.cx,ts.cy,RGB(0,0,255));
	      i++;
	    }
	    
	    dc.SetTextColor(RGB(0,0,0));
	    xx = 25+ts2.cx+x*(ts.cx+5);
	    if (y == 0) {
	      sprintf(s,"Vmin: %d",m_Vmin);
	      dc.TextOutA(xx,yy,s);
	    }
	    else if (y == 1) {
	      sprintf(s,"Vmax: %d",m_Vmax);
	      dc.TextOutA(xx,yy,s);
	    }
	    else if (y == 2) {
	      sprintf(s,"Vavg: %d",m_Vavg);
	      dc.TextOutA(xx,yy,s);
	    }
	    else if (y == 3) {
	      sprintf(s,"Vdif: %d",m_Vmax-m_Vmin);
	      dc.TextOutA(xx,yy,s);
	    }
	  }

	  y += 1;
	  yy = 5+y*(ts.cy+5);
	  xx = 5;
	  sprintf(s,"SOC: %2d.%04d%%\n",m_Soc/10000,m_Soc % 10000);
	  dc.TextOutA(xx,yy,s);
	  
	  sprintf(s,"Batt Capacity (?): %2d.%04d Ah\n",m_PackCap/10000,m_PackCap % 10000);
	  yy += ts.cy+5;
	  dc.TextOutA(xx,yy,s);
	  
	  
	  sprintf(s,"Batt Health (?): %2d.%02d%%\n",m_PackHealth/100,m_PackHealth % 100);
	  yy += ts.cy+5;
	  dc.TextOutA(xx,yy,s);
	  
	  sprintf(s,"Batt Temps: %02dC %02dC %02dC %02dC",(int)m_BattTemp[0],(int)m_BattTemp[1],(int)m_BattTemp[2],(int)m_BattTemp[3]);
	  yy += ts.cy+5;
	  dc.TextOutA(xx,yy,s);

	  if (serial.IsOpen()) {
	    if (!QueryELM())
	      Invalidate();
	  }

	  
	  //		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLeafELMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLeafELMDlg::DrawVoltage(CDC *dc,int x,int y,int volts,COLORREF color)
{
  char s[16];
  sprintf(s,"%04d",volts);
  dc->SetTextColor(color);
  dc->SetBkMode(OPAQUE);
  dc->TextOutA(x,y,s);
}

void CLeafELMDlg::DrawBox(CDC *dc,int x,int y,int w,int h,COLORREF color)
{
  CPen p(PS_SOLID,1,color);
  CPen *op = dc->SelectObject(&p);
  dc->MoveTo(x,y);
  dc->LineTo(x+w,y);
  dc->LineTo(x+w,y+h);
  dc->LineTo(x,y+h);
  dc->LineTo(x,y);
  dc->SelectObject(op);
}

int CLeafELMDlg::ReadLine(char *buf,int maxchar)
{
  
  *buf = 0;
  int totbytesread = 0;
  char c;
  do {
    c = 0;
    DWORD bytesread = 0;
    serial.Read(&c,1,&bytesread);
    if (bytesread && (c != CR)) {
      buf[totbytesread++] = c;
    }
  } while(((c != CR) && (c != PROMPT) && (totbytesread < maxchar-1)));
  buf[totbytesread] = 0;
  return totbytesread;
}

// read until '>' read - return 0 if OK found
int CLeafELMDlg::WaitAck(char *buf,int maxchar)
{
  int rc = 1;
  do {
    ReadLine(buf,maxchar);
    if (!strcmp(buf,"OK")) {
      rc = 0;
    }
  } while(!strchr(buf,'>'));
  return rc;
}


// group 1
void CLeafELMDlg::Group1()
{
  m_Soc = candata[4][5];
  m_Soc <<= 8;
  m_Soc |= candata[4][6];
  m_Soc <<= 8;
  m_Soc |= candata[4][7];

  m_PackCap = candata[5][2];
  m_PackCap <<= 8;
  m_PackCap |= candata[5][3];
  m_PackCap <<= 8;
  m_PackCap |= candata[5][4];

  m_PackHealth = candata[4][2];
  m_PackHealth <<= 8;
  m_PackHealth |= candata[4][3];
}


// group 2
void CLeafELMDlg::CellVoltages()
{
  m_Vmin = 9999;
  m_Vmax = 0;
  m_Vavg = 0;
  int v;
  int i=0;
  int j=4;

  for (v=0;v < BATT_CNT;v++) {
    m_Volts[v] = (candata[i][j] << 8);
    if (++j == 8) {
      j = 1;
      i++;
    }
    m_Volts[v] |= candata[i][j];
    if (++j == 8) {
      j = 1;
      i++;
    }
    m_Vavg += m_Volts[v];
    if (m_Volts[v] > m_Vmax) {
      m_Vmax = m_Volts[v];
    }
    if (m_Volts[v] < m_Vmin) {
      m_Vmin = m_Volts[v];
    }
  }
  m_Vavg /= BATT_CNT;
}

// group 4
void CLeafELMDlg::PackTemps()
{
  //  uint16_t adc[4];
  //  adc[0] = (candata[0][4] << 8)|candata[0][5];
  m_BattTemp[0] = candata[0][6];
  //  adc[1] = (candata[0][7] << 8)|candata[1][1];
  m_BattTemp[1] = candata[1][2];
  //  adc[2] = (candata[1][3] << 8)|candata[1][4];
  m_BattTemp[2] = candata[1][5];
  //  adc[3] = (candata[1][6] << 8)|candata[1][7];
  m_BattTemp[3] = candata[2][1];
}

// group 6
void CLeafELMDlg::Shunts()
{
  int x = 4;
  int y = 0;
  int b=3;
  for (int i=0;i < 96;i++) {
    if (candata[y][x] & (1 << b)) {
      m_Shunts[i] = 1;
    }
    else {
      m_Shunts[i] = 0;
    }
    if (b--==0) {
      if (++x == 8) {
	y++;
	x = 1;
      }
      b = 3;
    }
  }
}


int CLeafELMDlg::ReadCanData(int msgcnt)
{
  unsigned udata[10];
  memset(udata,0,sizeof(unsigned)*10);

  int i,j;
  for (i=0;i < msgcnt;i++) {
    ReadLine(buf,sizeof(buf));
    sscanf(buf,"%x %x %x %x %x %x %x %x %x %x",&udata[0],&udata[1],&udata[2],&udata[3],&udata[4],&udata[5],&udata[6],&udata[7],&udata[8],&udata[9]);
    if ((udata[0]==0x7bb) && (udata[1] == 8)) {
      if (ofp) fprintf(ofp,"%02x %02x %02x %02x %02x %02x %02x %02x\n",
	     udata[2],udata[3],udata[4],udata[5],udata[6],udata[7],udata[8],udata[9]);

      for (j=0;j < 8;j++) {
	candata[i][j] = udata[j+2];
      }
    }
    else {
      return 1;
    }
  }
  return 0;
}


int CLeafELMDlg::OpenSerial(char *portname)
{
  char devname[25];
  sprintf(devname,"\\\\.\\%s",portname);
  char s[80];
  

  LONG    lLastError = ERROR_SUCCESS;

  // Attempt to open the serial port
  lLastError = serial.Open(devname,0,0,false);
  if (lLastError != ERROR_SUCCESS) {
    sprintf(s,"Error %ld: Can't open %s",lLastError,portname);
	MessageBox(s);
    return 1;
  }

  lLastError = serial.Setup(CSerial::EBaud38400,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
  if (lLastError != ERROR_SUCCESS) {
	MessageBox("Error setting comm parameters");
    return 2;
  }

  serial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
  return 0;
}

int CLeafELMDlg::SetupELM()
{
  LONG    lLastError = ERROR_SUCCESS;

  // The serial port is now ready and we can send/receive data. If
  // the following call blocks, then the other side doesn't support
  // hardware handshaking.
  lLastError = serial.Write("ATZ\r"); // reset
  do {
    ReadLine(buf,sizeof(buf));
    printf("%s\n",buf);
  } while(!strchr(buf,PROMPT));
  lLastError = serial.Write("ATE0\r"); // turn off echo
  WaitAck(buf,sizeof(buf));
  lLastError = serial.Write("ATL0\r"); // disable CRLF
  WaitAck(buf,sizeof(buf));
  //    lLastError = serial.Write("ATS0\r"); // suppress spaces
  //`    WaitAck(uf,sizeof(buf));;
  lLastError = serial.Write("ATSP6\r"); // can 11 bits 500kbps
  WaitAck(buf,sizeof(buf));

  lLastError = serial.Write("ATH1\r"); // show headers
  WaitAck(buf,sizeof(buf));
  lLastError = serial.Write("ATD1\r"); // show data length
  WaitAck(buf,sizeof(buf));

  return 0;
}

int CLeafELMDlg::QueryELM()
{
  LONG lLastError = serial.Write("ATSH79B\r"); 
  WaitAck(buf,sizeof(buf));
  lLastError = serial.Write("ATFCSH79B\r");
  WaitAck(buf,sizeof(buf));
  lLastError = serial.Write("ATFCSD300020\r");
  WaitAck(buf,sizeof(buf));
  lLastError = serial.Write("ATFCSM1\r");
  WaitAck(buf,sizeof(buf));

  //
  // 
  //

  serial.Write("21 01\r");
  if (ReadCanData(6)) return 1;
  WaitAck(buf,sizeof(buf));
  Group1();

  serial.Write("21 02\r");
  if (ReadCanData(29)) return 2;
  WaitAck(buf,sizeof(buf));
  CellVoltages();

  serial.Write("21 04\r");
  if (ReadCanData(3)) return 4;
  WaitAck(buf,sizeof(buf));
  PackTemps();

  serial.Write("21 06\r");
  if (ReadCanData(4)) return 6;
  WaitAck(buf,sizeof(buf));
  Shunts();

  return 0;
}


void CLeafELMDlg::OnBnClickedConnect()
{
  int idx = m_cmbCommPort.GetCurSel();
  if (idx != CB_ERR) {
    char sport[25],fn[25];
    m_cmbCommPort.GetLBText(idx,sport);
    sprintf(fn,"\\\\.\\%s",sport);
    OpenSerial(fn);
    if (serial.IsOpen()) {
      SetupELM();
    }
  }
}

void CLeafELMDlg::FindSerialPorts()
{
  for (int i=0;i < 256;i++) {
    char sport[25];
    sprintf(sport,"\\\\.\\COM%d",i);
	HANDLE h=CreateFile(sport, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (h != INVALID_HANDLE_VALUE) {
		sprintf(sport,"COM%d",i);
		m_cmbCommPort.AddString(sport);
		CloseHandle(h);
    }
  }
  m_cmbCommPort.SetCurSel(0);
}
