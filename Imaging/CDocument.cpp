#include "stdafx.h"
#include "CDocument.h"
#include <algorithm>
#include <WinGDI.h>
#include "CImageStream.h"
#include "CImage.h"
#include "CScanner.h"

//////////////////////////////////////////////////////////////////////////
/**
  \file     CDocument.cpp
  \brief    This file contains information for reading data using WIA2.
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

const CString wiaFormatBMP = L"{B96B3CAB-0728-11D3-9D7B-0000F81EF32E}";
const CString wiaFormatGIF = L"{B96B3CB0-0728-11D3-9D7B-0000F81EF32E}";
const CString wiaFormatJPEG = L"{B96B3CAE-0728-11D3-9D7B-0000F81EF32E}";
const CString wiaFormatPNG = L"{B96B3CAF-0728-11D3-9D7B-0000F81EF32E}";
const CString wiaFormatTIFF = L"{B96B3CB1-0728-11D3-9D7B-0000F81EF32E}";
const CString wiaFormatRAW = L"{B96B3CA9-0728-11D3-9D7B-0000F81EF32E}";

//////////////////////////////////////////////////////////////////////////

namespace WIA2 {

//////////////////////////////////////////////////////////////////////////

CDocument::CDocument(IScanner & scanner)
  :  m_WiaItem(),
    m_RefCounter(1),
    m_Stream(NULL),
    m_Scanner(scanner),
    m_Image()
{
}

CDocument::~CDocument()
{
  if(m_Stream)
  {
    delete m_Stream;
    m_Stream = NULL;
  }
}

void CDocument::Init( const CComPtr<IWiaItem2> & item )
{
  HRESULT hRes = S_FALSE;

  m_WiaItem = item;

  // enumerate document properties

  CComPtr<IWiaPropertyStorage> pPropertyStorage;

  hRes = m_WiaItem->QueryInterface( IID_IWiaPropertyStorage, (void**) &pPropertyStorage );

  if(SUCCEEDED(hRes))
  {
    PROPVARIANT PropVar;
    PROPSPEC PropSpec;

    PropSpec.ulKind = PRSPEC_PROPID;
    PropSpec.propid = WIA_IPS_CUR_INTENT;

    pPropertyStorage->ReadMultiple(1, &PropSpec, &PropVar);

    PropSpec.ulKind = WIA_INTENT_IMAGE_TYPE_COLOR;

    hRes = pPropertyStorage->WriteMultiple(1, &PropSpec, &PropVar, PropSpec.propid);

    PropVariantClear( &PropVar );
    
  }
}


IImage & CDocument::Image() 
{
  return m_Image;
}

bool CDocument::Download()
{
  HRESULT hRes = E_FAIL;
  
  if(NULL != m_WiaItem)
  {
    long lType = 0;
    hRes = m_WiaItem->GetItemType(&lType);

    if(lType & WiaItemTypeTransfer)
    {
      if(lType & WiaItemTypeFile)
      {
        CComPtr<IWiaTransfer> transfer;
        hRes = m_WiaItem->QueryInterface( IID_IWiaTransfer, (void**) &transfer);
        
        if( SUCCEEDED(hRes) )
        {
          if(transfer)
          {
            IEnumWIA_FORMAT_INFO* pFmtInfo = NULL;
            hRes = transfer->EnumWIA_FORMAT_INFO(&pFmtInfo);

            if( hRes == S_OK)
            {
              ULONG size = 0UL;

              hRes = pFmtInfo->GetCount(&size);

              if( hRes == S_OK)
              {
                ULONG rcvd = 0UL;
                WIA_FORMAT_INFO* info = new WIA_FORMAT_INFO[size];

                hRes = pFmtInfo->Next(size, info, &rcvd);

                if( hRes == S_OK ) 
                {
                  for( int i=0; i<rcvd; ++i)
                  {
                    GUID guid = {0};

                    CLSIDFromString((LPCTSTR)wiaFormatBMP, &guid);

                    if( guid == info[i].guidFormatID )
                    {
                      OutputDebugString(L"RECEIVING BMP\n");

                    }
                  }
                }

                delete[] info;
              }
            }

            hRes = transfer->Download( 0, this);
          }
        }
      }
    }
  }
  
  return SUCCEEDED(hRes);
}


// -- snip, IWiaTransferCallback --

HRESULT __stdcall CDocument::TransferCallback( LONG lFlags, WiaTransferParams *pWiaTransferParams )
{

  if(NULL != pWiaTransferParams)
  {
    switch(pWiaTransferParams->lMessage)
    {
    case WIA_TRANSFER_MSG_STATUS:
      break;
    case WIA_TRANSFER_MSG_END_OF_STREAM:
      break;
    case WIA_TRANSFER_MSG_END_OF_TRANSFER:
      
      break;
    }

  }

  return S_OK;
}

HRESULT __stdcall CDocument::GetNextStream( LONG lFlags, BSTR bstrItemName, BSTR bstrFullItemName, IStream **ppDestination )
{
  CString str;
  str.Format(L" Flags: %d ", lFlags);
  OutputDebugString(L"GetNextStream: ");
  
  OutputDebugString( str );
  OutputDebugString(L" Item: ");
  OutputDebugString(CComBSTR(bstrItemName));
  OutputDebugString(L" FullItem: ");
  OutputDebugString(CComBSTR(bstrFullItemName));
  OutputDebugString(L"\n");
  
  if(NULL != ppDestination)
  {
#if 0
    HRESULT hRes;
    IStream* str;
    
    /*hRes = SHCreateStreamOnFileEx(L"c:\\temp\\123.bmp", 
      STGM_READWRITE | STGM_CREATE | STGM_DIRECT, 
      CREATE_ALWAYS | FILE_ATTRIBUTE_NORMAL, TRUE, NULL, &str);
    */

    hRes = SHCreateStreamOnFile(L"c:\\temp\\123.bmp", STGM_CREATE, ppDestination);

    //*ppDestination = str;
#else
    *ppDestination = new CImageStream( &m_Image );
#endif
    return S_OK;
  }
  
  return S_FALSE;
}

// -- snap, IWiaTransferCallback --

// -- snip, IUnknown --

HRESULT __stdcall CDocument::QueryInterface( const IID & iid,void ** ppInterface)
{
  HRESULT hRes = E_POINTER;
  
  if(NULL != ppInterface)
  {
    hRes = S_OK;
    *ppInterface = NULL;

    if(IsEqualIID(iid, IID_IUnknown))
    {
      *ppInterface = static_cast<IUnknown*>( this );
    }
    else if( IsEqualIID(iid, IID_IWiaTransferCallback) )
    {
      *ppInterface = static_cast<IWiaTransferCallback*>(this);
    }
    else
    {
      hRes = E_NOINTERFACE;
    }

    if(hRes == S_OK)
    {
      ((IUnknown*) *ppInterface)->AddRef();
    }
  }

  return hRes;
}

ULONG  __stdcall CDocument::AddRef( void )
{
  return InterlockedIncrement(&m_RefCounter);
}

// stolen from MSDN
ULONG  __stdcall CDocument::Release( void )
{
  ULONG ulRefCount = InterlockedDecrement(&m_RefCounter);

  if(0 == ulRefCount)
  {
    delete this;
  }

  return ulRefCount;
}

IScanner & CDocument::Scanner() const
{
  return m_Scanner;
}



// -- snap, IUnknown

//////////////////////////////////////////////////////////////////////////


CContainerDocument::CContainerDocument()
  :  std::vector<::IDocument*>(),
    m_Iterator()
{
  m_Iterator = std::vector<::IDocument*>::end();
}

CContainerDocument::~CContainerDocument()
{
  try
  {
    Clear();
  }
  catch(...)
  {}
}

void CContainerDocument::First()
{
  m_Iterator = begin();
}

bool CContainerDocument::isDone() const
{
  return (m_Iterator == end());
}

void CContainerDocument::Next()
{
  if(!isDone())
  {
    ++m_Iterator;
  }
}

::IDocument* const CContainerDocument::CurrentElement() const
{
  if(!isDone())
  {
    return *m_Iterator;
  }

  return NULL;
}


void CContainerDocument::Clear()
{
  clear();
  m_Iterator = end();
}

unsigned int CContainerDocument::Count()
{
  return size();
}

//////////////////////////////////////////////////////////////////////////

::IDocument & WIA2::CDocumentFactory::Create(IScanner & scanner) const
{
  WIA2::CDocument* doc = new WIA2::CDocument(scanner);

  if(NULL != doc)
  {
    static_cast<CContainerDocument &>( scanner.Documents() ).push_back( doc );

    return *doc;
  }

  throw std::runtime_error("Out of memory");
}

void WIA2::CDocumentFactory::Recycle( ::IScanner & scanner ) const
{
  IContainerDocument & documents = scanner.Documents();

  for( documents.First(); !documents.isDone(); documents.Next() )
  {
    ::IDocument* const doc = documents.CurrentElement();

    if(NULL != doc)
    {
      delete doc;
    }
  }

  documents.Clear();
}

const CDocumentFactory & WIA2::CDocumentFactory::Instance()
{
  static CDocumentFactory obj;

  return obj;
}

WIA2::CDocumentFactory::CDocumentFactory()
{

}

WIA2::CDocumentFactory::~CDocumentFactory()
{

}

//////////////////////////////////////////////////////////////////////////

} // namespace WIA

//////////////////////////////////////////////////////////////////////////