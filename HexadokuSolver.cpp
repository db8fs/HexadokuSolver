//////////////////////////////////////////////////////////////////////////
/**
  \file     HexadokuSolver.cpp
  \brief    This file holds the MFC application of the HexadokuSolver.
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CHexadokuSolverApp, CWinApp)
  ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

CHexadokuSolverApp::CHexadokuSolverApp()
{
  m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}


//////////////////////////////////////////////////////////////////////////

//! Singleton application object
CHexadokuSolverApp theApp;

//////////////////////////////////////////////////////////////////////////

BOOL CHexadokuSolverApp::InitInstance()
{
  INITCOMMONCONTROLSEX InitCtrls;
  InitCtrls.dwSize = sizeof(InitCtrls);
  InitCtrls.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&InitCtrls);

  CWinApp::InitInstance();
  AfxEnableControlContainer();

  CShellManager *pShellManager = new CShellManager;

  SetRegistryKey(_T("Vom lokalen Anwendungs-Assistenten generierte Anwendungen"));

  CoInitialize(0);

  CHexadokuSolverDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  
  if (nResponse == IDOK)
  {
    ;
  }
  else if (nResponse == IDCANCEL)
  {
    ;
  }

  dlg.Cleanup();

  CoUninitialize();

  if (pShellManager != NULL)
  {
    delete pShellManager;
  }

  return FALSE;
}

//////////////////////////////////////////////////////////////////////////
