#ifndef CIMAGESTREAM__9CBC7F53_BDD6_4A31_B893_D1EBA4CB1667__
#define CIMAGESTREAM__9CBC7F53_BDD6_4A31_B893_D1EBA4CB1667__

//////////////////////////////////////////////////////////////////////////
/**
  \file     CImageStream.h
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

class CImage;

//////////////////////////////////////////////////////////////////////////
/**
  \struct   CImageStream
  \brief    This implementation may receive a data stream from WIA
            and holds it in a DIBSection.
  \author   DB8FS
*/
/////////////////////////////////////////////////////////////////////////

class CImageStream :  public IStream
{
  volatile ULONG  m_RefCounter;
  volatile unsigned char*  volatile m_StreamPosition;  // pointer will change, content will change

  volatile bool  m_Finished;
  STATSTG      m_Status;

  CImage*  const  m_Image;

public:
  //! construction
  CImageStream(CImage* const img);

  //! copy construction
  CImageStream( const CImageStream & ); // not impl.

  //! destruction
  virtual ~CImageStream();

  // -- IUnknown, inherited from IStream --
  virtual ULONG __stdcall AddRef();
  virtual ULONG __stdcall Release();
  virtual HRESULT __stdcall QueryInterface(REFIID iid, LPVOID* ppInterface);

  // -- IStream --

  HRESULT __stdcall Read(void* buffer, ULONG bufSize, ULONG* bytesRead);
  HRESULT __stdcall Write(const void* buffer, ULONG bufSize, ULONG* bytesWritten);
  HRESULT __stdcall Seek(LARGE_INTEGER bytesMove, DWORD origin, ULARGE_INTEGER* newPosition);
  HRESULT __stdcall SetSize(ULARGE_INTEGER newSize);
  HRESULT __stdcall CopyTo(LPSTREAM destStream, ULARGE_INTEGER numBytes, ULARGE_INTEGER* bytesRead, ULARGE_INTEGER* bytesWritten);
  HRESULT __stdcall Commit(DWORD commitFlags);
  HRESULT __stdcall Revert();
  HRESULT __stdcall LockRegion(ULARGE_INTEGER offset, ULARGE_INTEGER numBytes, DWORD LockType);
  HRESULT __stdcall UnlockRegion(ULARGE_INTEGER offset, ULARGE_INTEGER numBytes, DWORD LockType);
  HRESULT __stdcall Stat(STATSTG* status, DWORD flags);
  HRESULT __stdcall Clone(LPSTREAM* newStreamObj);

};

//////////////////////////////////////////////////////////////////////////

} // namespace WIA2

//////////////////////////////////////////////////////////////////////////

#endif // CIMAGESTREAM__9CBC7F53_BDD6_4A31_B893_D1EBA4CB1667__
