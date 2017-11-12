//////////////////////////////////////////////////////////////////////////
/**
  \file     HexadokuSolverDlg.cpp
  \brief    This file implements the main MFC dialog for the HexadokuSolver.
  \author   Falk Schilling <falk.schilling.de (at) ieee.org >

  This file is part of HexasudokuSolver.

  HexasudokuSolver is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  HexasudokuSolver is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with HexasudokuSolver.  If not, see <http://www.gnu.org/licenses/>.
*/
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HexadokuSolver.h"
#include "HexadokuSolverDlg.h"
#include "CScannerManager.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////

#include <iostream>

////////////////////////////////////////////////////////////////////////////

class CAboutDlg : public CDialogEx
{
public:
  CAboutDlg();

// Dialogfelddaten
  enum { IDD = IDD_ABOUTBOX };

  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
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

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

CHexadokuSolverDlg::CHexadokuSolverDlg(CWnd* pParent /*=NULL*/)
  : CDialogEx(CHexadokuSolverDlg::IDD, pParent),
    m_ScannerManager( WIA2::CScannerManager::GetInstance() ),
    m_ContainerScanner( m_ScannerManager.Scanners() ),
    m_Scanner(NULL),
    m_Combobox(NULL)    
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CHexadokuSolverDlg::~CHexadokuSolverDlg()
{
  try
  {
    Cleanup();
  }
  catch (...)
  {
  }
}


void CHexadokuSolverDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CHexadokuSolverDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_CLOSE()
  ON_WM_DESTROY()
  ON_BN_CLICKED(IDC_BUTTON_START, &CHexadokuSolverDlg::OnBnClickedButtonStart)
  ON_CBN_SELCHANGE(IDC_COMBO_DEVICE, &CHexadokuSolverDlg::OnCbnSelchangeDevice)
  ON_MESSAGE( WM_APP, &CHexadokuSolverDlg::OnUserMessage)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

BOOL CHexadokuSolverDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  m_Combobox = static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_DEVICE) );

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

  SetIcon(m_hIcon, TRUE);      // Großes Symbol verwenden
  SetIcon(m_hIcon, FALSE);    // Kleines Symbol verwenden

  // initializes and enumerates all available scanners -> should be done again on each connect/disconnect event 
  // -> build some application middleware to decouple from MFC stuff  

  // initialize scanner manager
  if(m_ScannerManager.Init())
  {

    // fill combobox with devices and select default device
    if(NULL != m_Combobox)
    {
      m_Combobox->ResetContent();
        
      for(m_ContainerScanner.First(); !m_ContainerScanner.isDone(); m_ContainerScanner.Next())
      {
        IScanner* const scanner = m_ContainerScanner.CurrentElement();

        if(NULL != scanner)
        {
          m_Combobox->AddString( scanner->GetDeviceName() );
        }
      }

      int NumScanners = m_ContainerScanner.Count();

      if(NumScanners > 0)
      {
        m_Combobox->EnableWindow( NumScanners > 1 );
        SelectDevice(0);
      }
      else
      {
        m_Combobox->AddString(L"No Scanners found!");
        m_Combobox->SetCurSel(0);
        m_Combobox->EnableWindow( FALSE );
      }
    }
  }
  
  return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
}

LPARAM CHexadokuSolverDlg::OnUserMessage(WPARAM wParam, LPARAM lParam)
{
  OnBnClickedButtonStart();
  return 0;
}

void CHexadokuSolverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHexadokuSolverDlg::OnPaint()
{
  if (IsIconic())
  {
    CPaintDC dc(this); // Gerätekontext zum Zeichnen

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Symbol in Clientrechteck zentrieren
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Symbol zeichnen
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialogEx::OnPaint();
  }
}

HCURSOR CHexadokuSolverDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}



void CHexadokuSolverDlg::OnBnClickedButtonStart()
{
  if(NULL != m_Scanner)
  {
    IContainerDocument & documents = m_Scanner->Documents();

    for( documents.First(); !documents.isDone(); documents.Next() )
    {
      ::IDocument* const doc = documents.CurrentElement();

      if(NULL != doc)
      {
        doc->Image().Subscribe(*this);

        doc->Download();
        doc->Image().Save( L"c:\\temp\\blub.bmp" );
      }
    }

    //m_Scanner->Scan();
  }
}

void CHexadokuSolverDlg::OnImageEvent( const IImage & img ) const
{
  // TODO: use message queue

  CDialog* const surface = (CDialog* const)(GetDlgItem(IDC_STATIC_PREVIEW));

  if(NULL != surface)
  {
    static int blub = 0;

    unsigned char* buf = 0;
    unsigned long size = 0;
    img.GetRawData(&buf, &size);

    BITMAPFILEHEADER bfh = {0};
    bfh.bfOffBits = 54;
    bfh.bfType = MAKEWORD('B','M');
    bfh.bfSize = size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    bfh = bfh;

#if 1
    RECT rect = {0};
    CDC* cdc = surface->GetDC();

    if(NULL != cdc)
    {
      surface->GetWindowRect(&rect);
      surface->ScreenToClient(&rect);
      
//      img.Lock();
      img.Draw(  cdc->GetSafeHdc(), 
            rect.left, rect.top,  
            rect.right - rect.left, rect.bottom - rect.top );

//      img.Unlock();


      surface->ReleaseDC( cdc );

      surface->RedrawWindow(0, 0, RDW_INVALIDATE);
      surface->UpdateWindow();


    }
#endif

  }
  
}


void CHexadokuSolverDlg::OnScannerEvent( const IScannerEvent & evt ) const
{
  GUID guid;
  CString str;
  str.Format(L"ScannerEvent occured: %ls\n", evt.GetName() );
  AfxMessageBox(str, MB_OK);
  //OutputDebugString(str);
    
  CLSIDFromString(evt.GetEventID(), &guid);
    
  if(guid == WIA_EVENT_SCAN_OCR_IMAGE || 
    guid == WIA_EVENT_SCAN_IMAGE )
  {
    //this->OnBnClickedButtonStart();

    SendMessage( WM_APP, 0, 0);
  }

    SendMessage( WM_APP, 0, 0);

  // HACK: decoupling from WIA thread context necessary
}


void CHexadokuSolverDlg::SelectDevice(int id)
{

  if(NULL != m_Combobox)
  {
    m_Combobox->SetCurSel( id );
  }


  m_Scanner = NULL;

  IContainerScanner & devices = m_ScannerManager.Scanners();

  int i=0;
  for( devices.First(); !devices.isDone(); devices.Next(), ++i )
  {
    IScanner* const scanner = devices.CurrentElement();

    if(NULL != scanner && id == i)
    {
      m_Scanner = scanner;
      break;
    }
  }

  if(m_Scanner != NULL)
  {
    IContainerEvent & events = m_Scanner->Events();

    for(events.First(); !events.isDone(); events.Next())
    {
      IScannerEvent* const evt = events.CurrentElement();

      if( NULL != evt)
      {
        evt->Subscribe(*this);

        // TODO: let the user optionally configure the scanner events to listen for

        OutputDebugString( m_Scanner->GetDeviceName() );
        OutputDebugString( L": " );
        OutputDebugString( evt->GetName() );
        OutputDebugString( L"\n" );

      }
    }
  }
}

void CHexadokuSolverDlg::OnCbnSelchangeDevice()
{
  if(NULL != m_Combobox)
  {
    SelectDevice( m_Combobox->GetCurSel() );
  }
}

void CHexadokuSolverDlg::Cleanup()
{
  m_ScannerManager.Cleanup();

  m_Scanner = NULL;
  m_Combobox = NULL;
}

void CHexadokuSolverDlg::OnClose()
{
  CDialogEx::OnClose();
  Cleanup();
}

void CHexadokuSolverDlg::OnDestroy()
{
  CDialogEx::OnDestroy();
  Cleanup();
}

void CHexadokuSolverDlg::OnCancel()
{
  CDialogEx::OnCancel();
  Cleanup();
}

void CHexadokuSolverDlg::OnOK()
{
  CDialogEx::OnOK();
  Cleanup();
}
