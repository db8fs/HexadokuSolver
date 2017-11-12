#include "stdafx.h"
#include "CScanner.h"
#include <wia.h>

#include <algorithm>
#include "CScannerEvent.h"
#include "CDocument.h"


//////////////////////////////////////////////////////////////////////////
/**
  \file     CScanner.cpp
  \brief    WIA implementation of a scanner.
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

#pragma comment(lib, "wiaguid.lib")

//////////////////////////////////////////////////////////////////////////

WIA2::CScanner::CScanner( IScannerManager & mgr )
: m_ID(L""), 
  m_Name(L""), 
  m_Description(L""),
  m_Events(),
  m_Documents(),
  m_DeviceManager(mgr)
{
}

WIA2::CScanner::~CScanner()
{
  try
  {
    Cleanup();
  }
  catch (...)
  {
  
  }
}


void WIA2::CScanner::SetWiaDeviceManager( const CComPtr<IWiaDevMgr2> & mgr )
{
  m_WiaDeviceManager = mgr;
}

void WIA2::CScanner::SetWiaPropertyStorage( const CComPtr<IWiaPropertyStorage> & properties )
{
  m_WiaProperties = properties;
}

void WIA2::CScanner::SetWiaRoot( const CComPtr<IWiaItem2> & root )
{
  m_WiaRoot = root;
}

void WIA2::CScanner::Init()
{
  EnumerateDeviceProperties();
  EnumerateDocuments( m_WiaRoot );
  EnumerateEvents();
}


void WIA2::CScanner::Cleanup()
{
  CScannerEventFactory::Instance().Recycle(*this);
  CDocumentFactory::Instance().Recycle(*this);
  m_ID = L"";
  m_Name = L"";
  m_Description = L"";
  m_WiaDeviceManager = NULL;
  m_WiaProperties = NULL;
  m_WiaRoot = NULL;
}


const wchar_t* WIA2::CScanner::GetDeviceName() const { return m_Name.c_str(); }

const wchar_t* WIA2::CScanner::GetDeviceID() const { return m_ID.c_str(); }

const wchar_t* WIA2::CScanner::GetDeviceDescription() const { return m_Description.c_str(); }


IContainerEvent & WIA2::CScanner::Events()
{
  return m_Events;
}

IContainerDocument & WIA2::CScanner::Documents()
{
  return m_Documents;
}

IScannerManager & WIA2::CScanner::Manager() const
{
  return m_DeviceManager;
}

void WIA2::CScanner::Update()
{
  (void) EnumerateDocuments( m_WiaRoot );
}

void WIA2::CScanner::EnumerateEvents()
{
  HRESULT hRes = E_FAIL;
  
  m_Events.Clear();

  if(m_WiaDeviceManager != NULL)
  {
    if( NULL != m_WiaRoot && NULL != m_WiaDeviceManager)
    {
      CComPtr<IEnumWIA_DEV_CAPS> caps;

      hRes = m_WiaRoot->EnumDeviceCapabilities( WIA_DEVICE_EVENTS, &caps );

      if(SUCCEEDED(hRes))
      {
        ULONG count = 0;
        caps->GetCount(&count);

        if(count > 0)
        {
          WIA_DEV_CAP* cap = new WIA_DEV_CAP[count];

          if(NULL != cap)
          {
            DWORD dwNumItems = 0;

            caps->Next(count, cap, &dwNumItems);
            IUnknown* pEvent = NULL;

            for(DWORD i=0; i<dwNumItems; ++i)
            {
              CScannerEvent* evt = NULL;
                static_cast<CScannerEvent*>(
                  CScannerEventFactory::Instance().Create( *this ) );

              if(NULL != evt)
              {
                evt->SetDescription( CString( CComBSTR(cap[i].bstrDescription) ) );
                evt->SetName( CString( CComBSTR(cap[i].bstrName)) );
                evt->SetGUID( cap[i].guid );

                hRes = m_WiaDeviceManager->RegisterEventCallbackInterface(0, /*CComBSTR(m_ID)*/ NULL, &(cap[i].guid), 
                  evt, &pEvent);

                if(hRes == S_OK)
                {
                  evt->SetObject(pEvent);

                  pEvent->Release();
                }
              }
            }

            delete[] cap;
          }
        }
      }
    }
  }
}


void WIA2::CScanner::EnumerateDeviceProperties()
{
  HRESULT hRes = E_FAIL;

  if(m_WiaProperties != NULL)
  {
    // reading device properties (name, description, ...)

    PROPSPEC PropSpec[3] = {0};
    PROPVARIANT PropVar[3] = {0};

    const ULONG nPropertyCount = sizeof(PropSpec)/sizeof(PropSpec[0]);
    PropSpec[0].ulKind = PRSPEC_PROPID;
    PropSpec[0].propid = WIA_DIP_DEV_ID;

    PropSpec[1].ulKind = PRSPEC_PROPID;
    PropSpec[1].propid = WIA_DIP_DEV_NAME;

    PropSpec[2].ulKind = PRSPEC_PROPID;
    PropSpec[2].propid = WIA_DIP_DEV_DESC;

    hRes = m_WiaProperties->ReadMultiple( nPropertyCount, PropSpec, PropVar );

    if(SUCCEEDED(hRes))
    {
      if( VT_BSTR == PropVar[0].vt)
      {
        m_ID = PropVar[0].bstrVal;
      }

      if(VT_BSTR == PropVar[1].vt)
      {
        m_Name = PropVar[1].bstrVal;
      }

      if(VT_BSTR == PropVar[2].vt)
      {
        m_Description = PropVar[2].bstrVal;
      }

      FreePropVariantArray(nPropertyCount, PropVar);
    }
  }
}


HRESULT WIA2::CScanner::EnumerateDocuments( IWiaItem2* const root)
{
  LONG  lItemType = 0;
  HRESULT hRes = E_INVALIDARG;

  if(NULL != root )
  {
    m_Documents.Clear();

    hRes = root->GetItemType( &lItemType );

    if( SUCCEEDED(hRes) )
    {
      // recursion case - traverse folders
      if(lItemType & WiaItemTypeFolder || lItemType & WiaItemTypeHasAttachments)
      {
        CComPtr<IEnumWiaItem2> pEnumChildItems;

        // TODO: extend for non-flatbed scanners
        hRes = root->EnumChildItems( &WIA_CATEGORY_FLATBED, &pEnumChildItems );

        if(SUCCEEDED(hRes))
        {
          while(S_OK == hRes)
          {
            CComPtr<IWiaItem2> pChildItem;

            hRes = pEnumChildItems->Next(1, &pChildItem, NULL );

            if(S_OK == hRes)
            {
              hRes = EnumerateDocuments( pChildItem );
            }
          }

          if(S_FALSE == hRes) 
          {
            hRes = S_OK;
          }
        }
      }
      else
      {
        // trivial case - create document
        if(lItemType & WiaItemTypeImage || lItemType & WiaItemTypeFile)
        {      
#if 0
          (void) CDocumentFactory::Instance().Create(*this);
#else
          CDocument & doc = static_cast<CDocument &>( CDocumentFactory::Instance().Create(*this) );
          
          doc.Init(root);
#endif
        }

      }
    }

  }

  return hRes;
}


//////////////////////////////////////////////////////////////////////////


WIA2::CContainerScanner::CContainerScanner()
  :  std::vector<::IScanner*>(),
  m_Iterator()
{
  m_Iterator = std::vector<::IScanner*>::end();
}

WIA2::CContainerScanner::~CContainerScanner()
{
  try
  {
    Clear();
  }
  catch (...)
  {
    OutputDebugString(L"WIA2::CContainerScanner::~CContainerScanner: Error when clearing scanner container!\n");
  }
}

void WIA2::CContainerScanner::First()
{
  m_Iterator = begin();
}

bool WIA2::CContainerScanner::isDone() const
{
  return (m_Iterator == end());
}

void WIA2::CContainerScanner::Next()
{
  if(!isDone())
  {
    ++m_Iterator;
  }
}

::IScanner* const WIA2::CContainerScanner::CurrentElement() const
{
  if(!isDone())
  {
    return *m_Iterator;
  }

  return NULL;
}


void WIA2::CContainerScanner::Clear()
{
  clear();
  m_Iterator = end();
}

unsigned int WIA2::CContainerScanner::Count() const
{
  return size();
}

//////////////////////////////////////////////////////////////////////////

IScanner* WIA2::CScannerFactory::Create( IScannerManager & mgr ) const
{
  IScanner* dev = new CScanner(mgr);

  if(NULL != dev)
  {
    static_cast<CContainerScanner &>( mgr.Scanners() ).push_back(dev);

    return dev;
  }

  return NULL;
}

void WIA2::CScannerFactory::Recycle(IScannerManager & mgr) const
{
  IContainerScanner & devices = mgr.Scanners();

  for(devices.First(); !devices.isDone(); devices.Next() )
  {
    IScanner* const scanner = devices.CurrentElement();
        
    if(NULL != scanner)
    {
      delete scanner;
    }
  }

  devices.Clear();
}

const WIA2::CScannerFactory & WIA2::CScannerFactory::Instance()
{
  static WIA2::CScannerFactory factory;

  return factory;
}

WIA2::CScannerFactory::CScannerFactory()
{

}

WIA2::CScannerFactory::~CScannerFactory()
{

}
