#include "stdafx.h"
#include "CImage.h"
#include "CImageStream.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
/**
  \file     CImageStream.cpp
  \brief    This file holds the implementations for WIA2 data streams.
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

CImageStream::CImageStream(CImage* const img)
  : m_RefCounter(1),
  m_StreamPosition(NULL),
  m_Finished(false),
  m_Image(img)
{
  
}

CImageStream::~CImageStream()
{

}



ULONG __stdcall CImageStream::AddRef()
{
  return InterlockedIncrement(&m_RefCounter);
}

ULONG __stdcall CImageStream::Release()
{
  ULONG ulRefCount = InterlockedDecrement(&m_RefCounter);

  if(0 == ulRefCount)
  {
    delete this;
  }

  return ulRefCount;
}

HRESULT __stdcall CImageStream::QueryInterface( REFIID iid, LPVOID* ppInterface )
{
  HRESULT hRes = E_POINTER;
  *ppInterface = NULL;

  if(IsEqualIID(iid, IID_IUnknown))
  {
    *ppInterface = static_cast<IUnknown*>( this );
    hRes = S_OK;}
  else if( IsEqualIID(iid, IID_IStream) )
  {
    *ppInterface = static_cast<IStream*>(this);
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

HRESULT __stdcall CImageStream::Read( void* buffer, ULONG bufSize, ULONG* bytesRead )
{
  CString str;
  str.Format(L"IStream::Read() : buf: %x, size: %d\n", buffer, bufSize );
  OutputDebugString(str);

  return S_OK;
}


//////////////////////////////////////////////////////////////////////////
/**
  \brief  Writes the received stream data into a DIBSection.
  \detail  The function received streamed data of a bitmap. This data
      may have been acquired by the Windows Image Acquisition.
      We therefore have to perform several things:
      1. Examine the header of the data
      2. Allocate memory for holding the received data
      3. Create a memory DC
      4. Create a DIBSECTION between memory DC and received data
      5. Build a synchronization mechanism to notify UI to redraw
        the received data (could be inspired by Evas_GDI)
*/
//////////////////////////////////////////////////////////////////////////

HRESULT __stdcall CImageStream::Write( const void* buffer, ULONG bufSize, ULONG* bytesWritten )
{  
  // handle start of stream
  if(bufSize == sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))
  {
    LPBITMAPFILEHEADER bfh  = (LPBITMAPFILEHEADER)(buffer);
    LPBITMAPINFOHEADER bmi  = (LPBITMAPINFOHEADER)(bfh + 1);

    if(NULL != bfh)
    {  
      if( bfh->bfType == MAKEWORD('B', 'M'))
      {
        // create new bitmap 
        BITMAPINFO bm = {0};
        unsigned long size = 0;

        memcpy( &bm.bmiHeader, bmi, sizeof(BITMAPINFOHEADER) );  

        if(bytesWritten != NULL)
        {
          *bytesWritten = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

          size = bfh->bfSize - *bytesWritten;
        }

        if(m_Image->isInitialized())
        {
          m_Image->Cleanup();
        }

        m_Image->Init( &bm, size );

        m_StreamPosition = m_Image->m_BitmapData;

        // status information
        m_Status.pwcsName = L"";
        memset( &m_Status.mtime, 0, sizeof(m_Status.mtime) );
        memset( &m_Status.ctime, 0, sizeof(m_Status.ctime) );
        memset( &m_Status.atime, 0, sizeof(m_Status.atime) );
        memset( &m_Status.clsid, 0, sizeof(m_Status.clsid) );

        m_Status.type = STGTY_STREAM;
        m_Status.cbSize.QuadPart = bfh->bfSize;
        //m_Status.grfLocksSupported = LOCK_WRITE;
        ///m_Status.grfMode =
                
        OutputDebugString(L"IStream::Write() - Bitmap header received!\n");

        return S_OK;
      }

      // other stream types that bitmaps are not supported
      return E_NOTIMPL;
    }
  }

  if(NULL != m_StreamPosition)
  {
    const unsigned char* pbuf = static_cast<const unsigned char*>(buffer);
    
    m_Image->Lock();
    std::copy<const unsigned char*, volatile unsigned char* volatile >( pbuf, pbuf + bufSize, m_StreamPosition);
    m_Image->Unlock();

    m_Image->Update();

    m_StreamPosition += bufSize ;
  }

  CString str;
  str.Format(L"IStream::Write() : buf: %x, size: %d\n", m_StreamPosition, bufSize );
  OutputDebugString(str);
  return S_OK;
}

HRESULT __stdcall CImageStream::Seek( LARGE_INTEGER bytesMove, DWORD origin, ULARGE_INTEGER* newPosition )
{
  CString str;
  str.Format(L"IStream::Seek() : bytesMove: %ld, origin: %d\n", bytesMove, origin);
  OutputDebugString(str);
  return S_OK;
}

HRESULT __stdcall CImageStream::SetSize( ULARGE_INTEGER newSize )
{
  CString str;
  str.Format(L"IStream::SetSize() : size: %ld\n", newSize);
  OutputDebugString(str);
  return E_NOTIMPL;
}

HRESULT __stdcall CImageStream::CopyTo( LPSTREAM destStream, ULARGE_INTEGER numBytes, ULARGE_INTEGER* bytesRead, ULARGE_INTEGER* bytesWritten )
{
  CString str;
  str.Format(L"IStream::CopyTo() : dest: %x, numBytes: %d\n", destStream, numBytes);
  OutputDebugString(str);
  return E_NOTIMPL;
}

HRESULT __stdcall CImageStream::Commit( DWORD commitFlags )
{

  CString str;
  str.Format(L"IStream::Commit()\n");
  OutputDebugString(str);
  return E_NOTIMPL;
}

HRESULT __stdcall CImageStream::Revert()
{
  CString str;
  str.Format(L"IStream::Revert()\n");
  OutputDebugString(str);
  return E_NOTIMPL;
}

HRESULT __stdcall CImageStream::LockRegion( ULARGE_INTEGER offset, ULARGE_INTEGER numBytes, DWORD LockType )
{
  CString str;
  str.Format(L"IStream::LockRegion(), offset: %ld, numBytes: %ld\n", offset, numBytes);
  OutputDebugString(str);

  return E_NOTIMPL;
}

HRESULT __stdcall CImageStream::UnlockRegion( ULARGE_INTEGER offset, ULARGE_INTEGER numBytes, DWORD LockType )
{
  CString str;
  str.Format(L"IStream::UnlockRegion(), offset: %ld, numBytes: %ld\n", offset, numBytes);
  OutputDebugString(str);
  return E_NOTIMPL;
}

HRESULT __stdcall CImageStream::Stat( STATSTG* status, DWORD flags )
{
  CString str;
  str.Format(L"IStream::Stat()\n");
  OutputDebugString(str);

  status = &m_Status;

  return S_OK;
}

HRESULT __stdcall CImageStream::Clone( LPSTREAM* newStreamObj )
{
  CString str;
  str.Format(L"IStream::Close()\n");
  OutputDebugString(str);
  return E_NOTIMPL;
}


//////////////////////////////////////////////////////////////////////////

} // namespace WIA2

//////////////////////////////////////////////////////////////////////////