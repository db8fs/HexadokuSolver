#ifndef CScanner_h__
#define CScanner_h__

//////////////////////////////////////////////////////////////////////////
/**
  \file     CScanner.h
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

#include "ScannerAPI.h"
#include "CScannerEvent.h"
#include "CDocument.h"

#include <Wia.h>
#include <vector>
#include <set>

//////////////////////////////////////////////////////////////////////////

namespace WIA2 {

//////////////////////////////////////////////////////////////////////////
/**
  \class    CScanner
  \brief    This is the implementation of the scanner interface using 
        the Windows Image Acquisition API.
  \date    25.08.2013
*/
//////////////////////////////////////////////////////////////////////////

class CScanner :  public IScanner
{
  //! the unique identifier of this device
  std::wstring    m_ID;

  //! a name for this device
  std::wstring    m_Name;
    
  //! a description of this device
  std::wstring    m_Description;

  //! the device properties of this Wia device
  CComPtr<IWiaPropertyStorage> m_WiaProperties;
    
  //! the root for the Wia device items
  CComPtr<IWiaItem2>  m_WiaRoot;

  //! the device manager that acquired this device
  CComPtr<IWiaDevMgr2> m_WiaDeviceManager;
             
  //! the events belonging to this scanner device
  CContainerEvents    m_Events;
  
  //! the documents that have been enumerated for this device
  CContainerDocument    m_Documents;

  //! the device manager for this scanner
  IScannerManager &    m_DeviceManager;

  //////////////////////////////////////////////////////////////////////////

  CScanner( IScannerManager & mgr );
  CScanner( const CScanner & rhs );
  virtual ~CScanner();

  //! registers the events of this scanner
  virtual void EnumerateEvents();

  //! fills the document container fill the acquirable documents
  virtual HRESULT EnumerateDocuments(IWiaItem2* const root);

  //! read the properties
  virtual void EnumerateDeviceProperties();


public:
  friend struct CScannerFactory;

  //! initializes this object
  virtual void Init();

  //! cleans up this object
  virtual void Cleanup();

  //! sets the WIA2 device manager object
  virtual void SetWiaDeviceManager(const CComPtr<IWiaDevMgr2> & mgr );

  //! sets the wia property storage 
  virtual void SetWiaPropertyStorage( const CComPtr<IWiaPropertyStorage> & properties );

  //! sets the wia root object
  virtual void SetWiaRoot( const CComPtr<IWiaItem2> & root );

  //! gets the wia root object
  const CComPtr<IWiaItem2> & WIA2::CScanner::GetWiaRoot() const { return m_WiaRoot; }

  //! the name of the scanner device
  virtual const wchar_t* GetDeviceName() const;

  //! the identifier of the scanner device
  virtual const wchar_t* GetDeviceID() const;

  //! a description about the scanner device
  virtual const wchar_t* GetDeviceDescription() const;

  //! gets a status update for this device
  virtual void Update();

  //! the manager object responsible for this scanner
  virtual IScannerManager & Manager() const;

  //! the events belonging to this scanner device
  virtual IContainerEvent & Events();

  //! this container holds the documents belonging to the scanner device
  virtual IContainerDocument & Documents();
};



//////////////////////////////////////////////////////////////////////////
/**
  \class  CScannerFactory
  \brief  This class is responsible for creating and recycling event 
          objects and registering them with their corresponding device.
*/
//////////////////////////////////////////////////////////////////////////

struct CScannerFactory : public IScannerFactory
{
  //! creates an event belonging to the given scanner
  virtual IScanner*  Create(IScannerManager & mgr) const;

  //! releases an event
  virtual void      Recycle(IScannerManager & mgr) const; 

  //! singleton
  static const CScannerFactory & Instance();

private:
  CScannerFactory();
  CScannerFactory(const CScannerFactory &);
  ~CScannerFactory();

  // TODO: manage allocations?
};




//////////////////////////////////////////////////////////////////////////
/**
  \class  CContainerScanner
  \brief  Simple implementation of a container holding scanner devices.
*/
//////////////////////////////////////////////////////////////////////////

struct CContainerScanner :  public IContainerScanner,
              public std::vector<IScanner*>
{
  CContainerScanner();
  virtual ~CContainerScanner();

  //! sets the iterator of the event vector to the first item
  virtual void First();

  //! returns true if the end of the vector has been reached
  virtual bool isDone() const;

  //! iterates to the next available event
  virtual void Next();

  //! returns the current event of the iterator
  virtual IScanner* const CurrentElement() const;

  //! cleans container and deallocates items
  virtual void Clear();

  //! the number of items within the container
  virtual unsigned int Count() const;

private:
  std::vector<IScanner*>::const_iterator  m_Iterator;
};


//////////////////////////////////////////////////////////////////////////

} // namespace WIA

//////////////////////////////////////////////////////////////////////////

#endif // CScanner_h__