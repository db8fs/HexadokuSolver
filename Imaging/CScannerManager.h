#ifndef CScannerManager_h__
#define CScannerManager_h__

//////////////////////////////////////////////////////////////////////////
/**
  \file     CScannerManager.h
  \brief    WIA1 implementation of a scanner manager.
  \author   Falk Schilling <falk.schilling.de (at) ieee.org >
  \license  GPLv3
  \remarks  For your own modifications: Sample code for WIA can be 
            obtained through MSDN or by Windows SDK/DDK Samples.

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

#include "CScanner.h"
#include <Wia.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////

namespace WIA2 {

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/**
  \interface  CScannerManager
  \detail    This is the implementation of the WIA device manager.
*/
//////////////////////////////////////////////////////////////////////////

class CScannerManager :  public IScannerManager
{
  CComPtr<IWiaDevMgr2>    m_DeviceManager;
  CComPtr<IEnumWIA_DEV_INFO>  m_WIADeviceInfo;

  CContainerScanner      m_Devices; 

  explicit CScannerManager();
  explicit CScannerManager( const CScannerManager& rhs) throw(...);
  ~CScannerManager();

public:

  //! gets the singleton object
  static CScannerManager & GetInstance();

  //! initializes the scanner manager
  virtual bool Init();

  //! cleans up allocated resources
  virtual bool Cleanup();

  //! the devices that have been enumerated 
  virtual IContainerScanner & Scanners() ;

  //! the native device manager object 
  void* Object();

};


//////////////////////////////////////////////////////////////////////////

} // namespace WIA

//////////////////////////////////////////////////////////////////////////

#endif // CScannerManager_h__