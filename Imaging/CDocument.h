#ifndef CDOCUMENT__8F4DF0F1_12BA_4516_9149_12315E59F5B0__
#define CDOCUMENT__8F4DF0F1_12BA_4516_9149_12315E59F5B0__

//////////////////////////////////////////////////////////////////////////

#include "ScannerAPI.h"
#include <vector>
#include <Wia.h>
#include "CImage.h"

//////////////////////////////////////////////////////////////////////////
/**
  \file     CDocument.h
  \brief    This file holds the WIA2 implementations for image acquisition.
  \author   Falk Schilling <falk.schilling.de (at) ieee.org >
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

namespace WIA2 {

//////////////////////////////////////////////////////////////////////////

class CDocumentEvent;

//////////////////////////////////////////////////////////////////////////
/**
  \class    CDocument
  \brief    This is the implementation of a document that can be read
            by a scanner.
  \author   DB8FS
  \date     10.11.2013
*/
//////////////////////////////////////////////////////////////////////////

class CDocument :  public ::IDocument,
          public IWiaTransferCallback
{ 
  CComPtr<IWiaItem2>  m_WiaItem;
  volatile ULONG m_RefCounter;
  
  CImage      m_Image;
  CImageStream*  m_Stream;

  IScanner &    m_Scanner;

//  CContainerEvents*  m_Events;

  CDocument(IScanner & scanner);
  CDocument(const CDocument & rhs);
  virtual ~CDocument();

public:
  //! downloads the item and blocks the current thread
  virtual bool Download();
  
  //! the image scanned in this document
  virtual IImage & Image();

  //! the scanner belonging to this document
  virtual IScanner & Scanner() const;

  //! the events belonging to this document
  //virtual IDocumentEvents* const Events() const;

  virtual void Init(const CComPtr<IWiaItem2> & item);

  // -- IWiaTransferCallback -- //
  
  virtual HRESULT __stdcall TransferCallback(  LONG lFlags, WiaTransferParams *pWiaTransferParams);
  virtual HRESULT __stdcall GetNextStream( LONG lFlags, BSTR bstrItemName, BSTR bstrFullItemName, IStream **ppDestination);


  // -- IUnknown provided for IWiaTransferCallback -- //

  virtual HRESULT __stdcall QueryInterface(const IID &,void **);
  virtual ULONG  __stdcall AddRef(void);
  virtual ULONG  __stdcall Release(void);

  friend class CDocumentFactory;

};



//////////////////////////////////////////////////////////////////////////
/**
  \class    CDocumentFactory
  \brief    This class is responsible for creating and recycling document
            objects and registering them with their corresponding device.
*/
//////////////////////////////////////////////////////////////////////////

class CDocumentFactory : public IDocumentFactory
{
  explicit CDocumentFactory();
  explicit CDocumentFactory(const CDocumentFactory &) throw (...);
  ~CDocumentFactory();

public:
  //! creates a document belonging to the given scanner
  virtual ::IDocument &  Create(IScanner & scanner) const;

  //! releases a document
  virtual void      Recycle(::IScanner & scanner) const;

  //! singleton
  static const CDocumentFactory & Instance();

  // TODO: manage allocations?
};



//////////////////////////////////////////////////////////////////////////
/**
  \struct   CContainerDocument
  \brief    This class may hold a set of documents.
  \date     31.10.2013
*/
//////////////////////////////////////////////////////////////////////////

class CContainerDocument : public IContainerDocument,
              public std::vector<::IDocument*> 
{
  std::vector<::IDocument*>::const_iterator    m_Iterator;
  
public:
  //! construction
  CContainerDocument();
  
  //! prohibit copys (not implemented)
  CContainerDocument( const CContainerDocument & ); 

  //! destruction
  virtual ~CContainerDocument();

  //! sets the iterator of the document container to the first item
  virtual void First();

  //! returns true if the end of the container has been reached
  virtual bool isDone() const;

  //! iterates to the next available event
  virtual void Next();

  //! returns the current item of the container
  virtual ::IDocument* const CurrentElement() const;

  //! clears this container
  virtual void Clear();

  //! returns the number of items
  virtual unsigned int Count();


};


//////////////////////////////////////////////////////////////////////////

} // namespace WIA

//////////////////////////////////////////////////////////////////////////

#endif // CDOCUMENT__8F4DF0F1_12BA_4516_9149_12315E59F5B0__
