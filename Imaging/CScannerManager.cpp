#include "stdafx.h"
#include "CScanner.h"
#include "CScannerManager.h"

//////////////////////////////////////////////////////////////////////////
/**
  \file     CScannerManager.cpp
  \brief    WIA2 implementation of a scanner manager.
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

WIA2::CScannerManager::CScannerManager()
  :  m_DeviceManager(NULL),
  m_WIADeviceInfo(NULL),
  m_Devices()
{
}

WIA2::CScannerManager::CScannerManager( const CScannerManager& rhs ) throw(...)
{
  throw std::runtime_error("Singleton object");
}

WIA2::CScannerManager::~CScannerManager()
{
  try
  {
    Cleanup();
  }
  catch (...)
  {
  }
}


bool WIA2::CScannerManager::Init()
{
  bool bRet = false;
  HRESULT hRes = S_OK;
  
  do 
  {
    hRes = CoCreateInstance( CLSID_WiaDevMgr2, NULL, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr2, (void**) &m_DeviceManager);
    
    if( !SUCCEEDED(hRes) )
    {
      AfxMessageBox( _com_error(hRes).ErrorMessage(), MB_OK );
      break;
    }

    if(!m_DeviceManager)
    {
      break;
    }

    // enumerate devices
    hRes = m_DeviceManager->EnumDeviceInfo( WIA_DEVINFO_ENUM_ALL, &m_WIADeviceInfo );

    if( !SUCCEEDED(hRes) )
    {
      AfxMessageBox( _com_error(hRes).ErrorMessage(), MB_OK);
      break;
    }

    HRESULT hResEnum = S_OK;
    
    while ( hResEnum == S_OK )
    {
      CComPtr<IWiaPropertyStorage> pWiaPropertyStorage;
      hResEnum = m_WIADeviceInfo->Next(1, &pWiaPropertyStorage, NULL);
      
      if(hResEnum == S_OK)
      {
        PROPSPEC PropSpec; 
        PROPVARIANT PropVar;
        
        PropSpec.ulKind = PRSPEC_PROPID;
        PropSpec.propid = WIA_DIP_DEV_ID;

        hRes = pWiaPropertyStorage->ReadMultiple( 1, &PropSpec, &PropVar );

        if(SUCCEEDED(hRes))
        {
          CString DeviceID;

          if( VT_BSTR == PropVar.vt)
          {
            DeviceID = PropVar.bstrVal;
          }

          // we found something, create device object
          CComPtr<IWiaItem2> pWiaItem2Root;

          hRes = m_DeviceManager->CreateDevice(0, CComBSTR(DeviceID), &pWiaItem2Root );

          PropVariantClear(&PropVar);
          
          if(SUCCEEDED(hRes))
          {
            WIA2::CScanner* pScanner = 
              static_cast<CScanner*>( 
                CScannerFactory::Instance().Create(*this) );
            
            if(NULL != pScanner)
            {
              pScanner->SetWiaDeviceManager( m_DeviceManager );
              pScanner->SetWiaPropertyStorage( pWiaPropertyStorage );
              pScanner->SetWiaRoot( pWiaItem2Root );

              pScanner->Init();
            }
          }
        }

      }
    }

    bRet = true;
  } while (0);

  return bRet;
}

bool WIA2::CScannerManager::Cleanup()
{
  CScannerFactory::Instance().Recycle(*this);

  if(NULL != m_DeviceManager)
  {
    m_DeviceManager = NULL;
  }

  if(NULL != m_WIADeviceInfo)
  {
    m_WIADeviceInfo = NULL;
  }

  return true;
}

void* WIA2::CScannerManager::Object()
{
  return m_DeviceManager;
}


IContainerScanner & WIA2::CScannerManager::Scanners() 
{
  return m_Devices;
}

WIA2::CScannerManager & WIA2::CScannerManager::GetInstance()
{
  static CScannerManager obj;
  return obj;
}

