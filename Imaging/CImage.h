#ifndef CIMAGE__2B23EF0F_1523_4E8C_BFB4_29B85A86BC53__
#define CIMAGE__2B23EF0F_1523_4E8C_BFB4_29B85A86BC53__

//////////////////////////////////////////////////////////////////////////

#include "ScannerAPI.h"
#include <WinGDI.h>
#include <set>

//////////////////////////////////////////////////////////////////////////
/**
  \file   CImage.h
  \author Falk Schilling <falk.schilling.de (at) ieee.org >
  \license GPLv3

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
/**
  \struct CImage
  \brief  This class wraps Device-Independent Bitmaps.
*/
//////////////////////////////////////////////////////////////////////////

class CImage : public IImage
{
  HDC                   m_MemDC;
  HBITMAP               m_Bitmap;
  volatile unsigned char*      m_BitmapData; // content is volatile, pointer address not 
  unsigned long         m_Size;
  bool                  m_Initialized;
  CRITICAL_SECTION      m_Sync;
  BITMAPINFO            m_BitmapInfo;
  std::set<const IImageObserver* const>    m_Observers;

public:
  friend class CImageStream;

  CImage();
  CImage( const CImage & ); // not impl.
  virtual ~CImage();

  //! initializes the image 
  virtual bool Init(const LPBITMAPINFO info, unsigned long size);
  
  //! cleans up the reserved data
  virtual bool Cleanup();

  //! true if image is ready to use
  virtual bool isInitialized() const;

  //! the size of the raw data buffer
  virtual unsigned long  GetSize() const;

  //! thread synchronizing lock
  virtual void Lock();

  //! thread synchronizing unlock
  virtual void Unlock();
  
  //! subscribe for receiving updates about this image
  virtual void Subscribe(const IImageObserver & img);

  //! unsubscribe from receiving updates about this image
  virtual void Unsubscribe(const IImageObserver & img);

  //! notify that this image has modified
  virtual void Update() const;

  //! draws the image to the given surface
  virtual void Draw(  void* dest, unsigned long x, unsigned long y, 
            unsigned long width, unsigned long height) const;


  //! gets access to the raw data buffer
  virtual bool GetRawData(unsigned char** buffer, unsigned long* size) const; 

  //! saves the image 
  virtual void Save( const wchar_t* path) const;

};


//////////////////////////////////////////////////////////////////////////

} // namespace WIA2

//////////////////////////////////////////////////////////////////////////

#endif // CIMAGE__2B23EF0F_1523_4E8C_BFB4_29B85A86BC53__
