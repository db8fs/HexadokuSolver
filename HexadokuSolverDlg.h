#ifndef HexadokuSolverDlg_h__
#define HexadokuSolverDlg_h__

//////////////////////////////////////////////////////////////////////////
/**
  \file     HexadokuSolverDlg.h
  \brief    This file holds the main MFC dialog for the HexadokuSolver.
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

#include "ScannerAPI.h"

//////////////////////////////////////////////////////////////////////////
/**
  \class  CHexadokuSolverDlg
  \brief  The main dialog for capturing data from scanners.
*/
//////////////////////////////////////////////////////////////////////////

class CHexadokuSolverDlg :  public CDialogEx,
              public IScannerObserver,
              public IImageObserver
{
public:
  CHexadokuSolverDlg(CWnd* pParent = NULL);
  CHexadokuSolverDlg(const CHexadokuSolverDlg & ); // not impl
  ~CHexadokuSolverDlg();

  enum { IDD = IDD_HEXADOKUSOLVER_DIALOG };

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);


protected:
  HICON m_hIcon;

  IScannerManager &  m_ScannerManager;
  IContainerScanner & m_ContainerScanner;
  IScanner*      m_Scanner;

  CComboBox*      m_Combobox;
  

  // Generierte Funktionen für die Meldungstabellen
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();

  afx_msg void OnClose();
  afx_msg void OnDestroy();
  afx_msg void OnOK();
  afx_msg void OnCancel();
  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnBnClickedButtonStart();
  afx_msg void OnCbnSelchangeDevice();

  //////////////////////////////////////////////////////////////////////////

  virtual void OnScannerEvent(const IScannerEvent & obj) const;

  virtual void OnImageEvent(const IImage & img) const;

  void SelectDevice(int id);

  void Cleanup();
  
};

//////////////////////////////////////////////////////////////////////////

#endif // HexadokuSolverDlg_h__
