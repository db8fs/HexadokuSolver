#ifndef CScannerEvent_h__
#define CScannerEvent_h__

//////////////////////////////////////////////////////////////////////////
/**
  \file     CScannerEvent.h
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

#include "ScannerAPI.h"
#include <comdef.h>
#include <vector>
#include <string>
#include <set>
#include <Wia.h>

//////////////////////////////////////////////////////////////////////////

namespace WIA2 {

//////////////////////////////////////////////////////////////////////////
/**
  \class  CScannerEvent
  \brief  This is a simple event wrapper for scanners.
*/
//////////////////////////////////////////////////////////////////////////

struct CScannerEvent :  public IScannerEvent,
            public IWiaEventCallback
{
  virtual void Cleanup();

  //! gets the event identifier string
  const wchar_t* GetEventID() const;
  
  //! gets the name of the event
  virtual const wchar_t* GetName() const;

  //! sets the name of the event
  virtual void SetName( const wchar_t* name);

  virtual const GUID & GetGUID() const;

  //! sets the global unique identifier for this event -> build macro?
  virtual void SetGUID( const GUID & guid);

  //! description of the event
  virtual const wchar_t* GetDescription() const;

  //! sets the description of the event
  virtual void SetDescription(const wchar_t* description);

  //! set the type of the event
  virtual void SetType(unsigned long type);

  //! the type of the event
  virtual unsigned long GetType() const;

  //! sets the COM event object
  virtual void SetObject(const CComPtr<IUnknown> & event_object);

  //! the scanner belonging to this event
  virtual IScanner* const Scanner() const;

  //! subscribe for receiving events (observers won't be deleted!)
  virtual void Subscribe( const IScannerObserver & observer );

  //! unsubscribe from receiving events
  virtual void Unsubscribe( const IScannerObserver & observer );
  
  //! notifies the listeners about this event
  virtual void Notify() const;


  // -- IWiaEventCallback -- //

  //! callback for receiving WIA hardware events and forwarding them to the subscribers
  virtual HRESULT __stdcall ImageEventCallback(  const GUID *pEventGUID,
    BSTR bstrEventDescription,
    BSTR bstrDeviceID,
    BSTR bstrDeviceDescription,
    DWORD dwDeviceType,
    BSTR bstrFullItemName,
    ULONG *pulEventType,
    ULONG ulReserved
    );

  // -- IUnknown provided for IWiaEventCallback -- //

  virtual HRESULT WINAPI QueryInterface(const IID &,void **);

  virtual ULONG WINAPI AddRef(void);

  virtual ULONG WINAPI Release(void);

  friend struct CScannerEventFactory;

private:
  CScannerEvent(IScanner* const scanner);
  CScannerEvent(const CScannerEvent & rhs);
  virtual ~CScannerEvent();

  IScanner* const    m_Scanner;
  std::wstring    m_Name;
  std::wstring    m_Description;
  unsigned long    m_Type;
  std::wstring    m_Identifier;
  GUID        m_GUID;
  CComPtr<IUnknown>  m_Object;

  std::set<const IScannerObserver* const>  m_Listener;

  // IUnknown reasons...
  volatile ULONG m_RefCounter;
};


//////////////////////////////////////////////////////////////////////////
/**
  \class  CScannerEventFactory
  \brief  This class is responsible for creating and recycling event 
      objects and registering them with their corresponding device.
*/
//////////////////////////////////////////////////////////////////////////

struct CScannerEventFactory : public IScannerEventFactory
{
  //! creates an event belonging to the given scanner
  virtual IScannerEvent*  Create(IScanner & scanner) const;

  //! releases an event
  virtual void      Recycle(IScanner & scanner) const;

  //! singleton
  static const CScannerEventFactory & Instance();

private:
  CScannerEventFactory();
  CScannerEventFactory(const CScannerEventFactory &);
  ~CScannerEventFactory();

  // TODO: manage allocations?
};



//////////////////////////////////////////////////////////////////////////
/**
  \class  CContainerEvents
  \brief  Simple implementation of a container using events.
*/
//////////////////////////////////////////////////////////////////////////

struct CContainerEvents : public IContainerEvent,
              public std::vector<::IScannerEvent*> 
{
  //! sets the iterator of the document container to the first item
  virtual void First();

  //! returns true if the end of the container has been reached
  virtual bool isDone() const;

  //! iterates to the next available event
  virtual void Next();

  //! returns the current item of the container
  virtual ::IScannerEvent* const CurrentElement() const;

  //! clears this container
  virtual void Clear();

  //! returns the number of items
  virtual unsigned int Count();

  //! construction
  CContainerEvents();

  //! destruction
  virtual ~CContainerEvents();

private:
  std::vector<::IScannerEvent*>::const_iterator    m_Iterator;
};


//////////////////////////////////////////////////////////////////////////

} // namespace WIA

//////////////////////////////////////////////////////////////////////////

#endif // CScannerEvent_h__

