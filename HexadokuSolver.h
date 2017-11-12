#ifndef HexadokuSolver_h__
#define HexadokuSolver_h__

//////////////////////////////////////////////////////////////////////////
/**
  \file     HexadokuSolver.h
  \brief    This file defines the MFC application of the HexadokuSolver.
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

#ifndef __AFXWIN_H__
  #error "'stdafx.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"    // Hauptsymbole

//////////////////////////////////////////////////////////////////////////

class CHexadokuSolverApp : public CWinApp
{
public:
  CHexadokuSolverApp();

public:
  virtual BOOL InitInstance();

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

extern CHexadokuSolverApp theApp;

//////////////////////////////////////////////////////////////////////////

#endif // HexadokuSolver_h__