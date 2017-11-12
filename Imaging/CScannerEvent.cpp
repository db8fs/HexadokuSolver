//////////////////////////////////////////////////////////////////////////
/**
  \file     CScannerEvent.cpp
  \brief    Event wrappers for WIA Events.
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

#include "stdafx.h"
#include "CScannerEvent.h"
#include <algorithm>


//////////////////////////////////////////////////////////////////////////

WIA2::CScannerEvent::CScannerEvent( IScanner* const scanner)
  : m_Scanner(scanner), m_RefCounter(1)
{

}

WIA2::CScannerEvent::CScannerEvent( const CScannerEvent & rhs )
  : m_Scanner(rhs.m_Scanner)
{
  throw;
}

WIA2::CScannerEvent::~CScannerEvent()
{
  try
  {
    Cleanup();
  }
  catch(...)
  {}
}

const wchar_t* WIA2::CScannerEvent::GetEventID() const
{
  return m_Identifier.c_str();
}

const wchar_t* WIA2::CScannerEvent::GetName() const
{ 
  return m_Name.c_str(); 
}

void WIA2::CScannerEvent::SetName( const wchar_t* name )
{
  m_Name = name; 
}

const GUID & WIA2::CScannerEvent::GetGUID() const
{
  return m_GUID; 
}

void WIA2::CScannerEvent::SetGUID( const GUID & guid ) // build conversion macro?
{ 
  HRESULT hRes = E_FAIL;
  wchar_t* tmp;

  m_GUID = guid; 

  hRes = StringFromCLSID( m_GUID, &tmp);

  if(SUCCEEDED(hRes))
  {
    std::wstring  guid_string(tmp);
    m_Identifier = guid_string.substr(1, guid_string.length() - 2);

    CoTaskMemFree(tmp);
  }
}

const wchar_t* WIA2::CScannerEvent::GetDescription() const
{
  return m_Description.c_str();
}

void WIA2::CScannerEvent::SetDescription( const wchar_t* description )
{
  m_Description = description; 
}

void WIA2::CScannerEvent::SetType( unsigned long type )
{
  m_Type = type;
}

unsigned long WIA2::CScannerEvent::GetType() const
{
  return m_Type;
}

IScanner* const WIA2::CScannerEvent::Scanner() const
{
  return m_Scanner; 
}

void WIA2::CScannerEvent::Subscribe( const IScannerObserver & observer )
{
  m_Listener.insert( &observer);  
}

void WIA2::CScannerEvent::Unsubscribe( const IScannerObserver & observer )
{
  m_Listener.erase( &observer );
}

void WIA2::CScannerEvent::Notify() const
{
  for( std::set<const IScannerObserver* const >::const_iterator it = m_Listener.begin(); it != m_Listener.end(); ++it)
  {
    (*it)->OnScannerEvent( *this );
  }
  
}



HRESULT WINAPI WIA2::CScannerEvent::QueryInterface( const IID & iid,void ** ppInterface)
{
  HRESULT hRes = S_OK;
  *ppInterface = NULL;

  if(IsEqualIID(iid, IID_IUnknown))
  {
    *ppInterface = static_cast<IUnknown*>( this );
    hRes = S_OK;
  }
  else if( IsEqualIID(iid, IID_IWiaEventCallback ) )
  {
    *ppInterface = static_cast<IWiaEventCallback*>(this);
    hRes = S_OK;
  }
  else
  {
    hRes = E_NOINTERFACE;
  }

  if(SUCCEEDED(hRes))
  {
    ((IUnknown*) *ppInterface)->AddRef();
  }

  return hRes;
}

ULONG WINAPI WIA2::CScannerEvent::AddRef( void )
{
  return InterlockedIncrement(&m_RefCounter);
}

ULONG WINAPI WIA2::CScannerEvent::Release( void )
{
  ULONG ulRefCount = InterlockedDecrement(&m_RefCounter);

  if(0 == ulRefCount)
  {
    delete this;
  }

  return ulRefCount;
}

void WIA2::CScannerEvent::SetObject( const CComPtr<IUnknown> & event_object )
{
   m_Object = event_object;
}


HRESULT WIA2::CScannerEvent::ImageEventCallback(  const GUID *pEventGUID,
  BSTR bstrEventDescription,
  BSTR bstrDeviceID,
  BSTR bstrDeviceDescription,
  DWORD dwDeviceType,
  BSTR bstrFullItemName,
  ULONG *pulEventType,
  ULONG ulReserved
  )
{
  CString str;
  str.Format(L"ImageEventCallback - %ls", CComBSTR(bstrEventDescription) );
  OutputDebugString(str);

  if( NULL != pEventGUID)
  {
    if( IsEqualGUID( m_GUID, *pEventGUID) )
    {
      // don't forget the thread context we're in!
      Notify();

      return S_OK;      
    }
  }

  return E_FAIL;
}

void WIA2::CScannerEvent::Cleanup()
{
  m_Listener.clear();
}

//////////////////////////////////////////////////////////////////////////




WIA2::CContainerEvents::CContainerEvents()
  :  std::vector<::IScannerEvent*>(),
  m_Iterator()
{
  m_Iterator = std::vector<::IScannerEvent*>::end();
}

WIA2::CContainerEvents::~CContainerEvents()
{
  try
  {
    Clear();
  }
  catch (...)
  {
  }
}

void WIA2::CContainerEvents::First()
{
  m_Iterator = begin();
}

bool WIA2::CContainerEvents::isDone() const
{
  return (m_Iterator == end());
}

void WIA2::CContainerEvents::Next()
{
  if(!isDone())
  {
    ++m_Iterator;
  }
}

::IScannerEvent* const WIA2::CContainerEvents::CurrentElement() const
{
  if(!isDone())
  {
    return *m_Iterator;
  }

  return NULL;
}


void WIA2::CContainerEvents::Clear()
{
  clear();
  m_Iterator = end();
}

unsigned int WIA2::CContainerEvents::Count()
{
  return size();
}

//////////////////////////////////////////////////////////////////////////

IScannerEvent* WIA2::CScannerEventFactory::Create(IScanner & scanner) const
{
  IScannerEvent* evt = new CScannerEvent(&scanner);

  static_cast<CContainerEvents &>( scanner.Events() ).push_back( evt );

  return evt;
}

void WIA2::CScannerEventFactory::Recycle( IScanner & scanner) const
{
  IContainerEvent & events = scanner.Events();
  
  for(events.First(); !events.isDone(); events.Next())
  {
    IScannerEvent* const evt = events.CurrentElement();

    if(NULL != evt)
    {
      delete evt;
    }
  }

  events.Clear();
}

const WIA2::CScannerEventFactory & WIA2::CScannerEventFactory::Instance()
{
  static CScannerEventFactory obj;

  return obj;
}

WIA2::CScannerEventFactory::CScannerEventFactory()
{

}

WIA2::CScannerEventFactory::~CScannerEventFactory()
{

}
