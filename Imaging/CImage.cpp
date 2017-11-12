#include "stdafx.h"
#include "CImage.h"

//////////////////////////////////////////////////////////////////////////
/**
  \file   CImage.cpp
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

WIA2::CImage::CImage()
  :  m_Initialized(false),
    m_MemDC(NULL),
    m_Bitmap(NULL),
    m_BitmapData(NULL),
    m_Size(0U)
{
}

WIA2::CImage::~CImage()
{
  try
  {
    Cleanup();
  }
  catch (...)
  {
  }
}

void WIA2::CImage::Lock()
{
  if(m_Initialized)
  {
    EnterCriticalSection(&m_Sync);
  }

}

void WIA2::CImage::Unlock()
{
  if(m_Initialized)
  {
    LeaveCriticalSection(&m_Sync);
  }
}


bool WIA2::CImage::Init( const LPBITMAPINFO info, unsigned long size )
{
  if(!m_Initialized)
  {
    InitializeCriticalSection(&m_Sync);

    m_MemDC = CreateCompatibleDC(NULL);

    m_Bitmap = CreateDIBSection(m_MemDC, info, DIB_RGB_COLORS, (void**)&m_BitmapData, NULL, 0);

    RECT rect;
    rect.left = 0;
    rect.right = info->bmiHeader.biWidth;
    rect.top = 0;
    rect.bottom = -info->bmiHeader.biHeight;

    HBRUSH brush = CreateSolidBrush( RGB(255,255,255) );

    if(NULL != brush)
    {
      SelectObject(m_MemDC, brush);
      FillRect(m_MemDC, &rect, brush);
      DeleteObject(brush);
    }
    

    m_Size = size;

    memcpy(&m_BitmapInfo, info, sizeof(BITMAPINFO));

    m_Initialized = true;
    return true;
  }

  return false;
}


bool WIA2::CImage::isInitialized() const
{
  return m_Initialized;
}


bool WIA2::CImage::Cleanup()
{
  if(m_Initialized)
  {
    // cleanup   

    DeleteCriticalSection(&m_Sync);

    if(NULL != m_MemDC)
    {
      DeleteDC(m_MemDC);
      m_MemDC = NULL;
    }
    
    if(NULL != m_Bitmap)
    {
      DeleteObject( m_Bitmap );
      m_Bitmap = NULL;
    }
    
    m_BitmapData = NULL;
    m_Size = 0U;

    m_Initialized = false;
    return true;
  }

  return false;
}

unsigned long WIA2::CImage::GetSize() const
{
  return m_Size;
}


bool WIA2::CImage::GetRawData( unsigned char** buffer, unsigned long* size ) const
{
  if(m_Initialized)
  {
    if(NULL != buffer)
    {
      *buffer = reinterpret_cast<unsigned char*>(m_Bitmap);
    }

    if(NULL != size)
    {
      *size = m_Size;
    }

    return true;
  }
  
  return false;
}


void WIA2::CImage::Update() const
{
  // notify the observers about this update
  for(std::set<const IImageObserver* const>::iterator it = m_Observers.begin(); it != m_Observers.end(); ++it)
  {
    const IImageObserver* const obj = *it;

    if(NULL != obj)
    {
      obj->OnImageEvent(*this);
    }
  }
}

void WIA2::CImage::Subscribe( const IImageObserver & img )
{
  m_Observers.insert(&img);
}

void WIA2::CImage::Unsubscribe( const IImageObserver & img )
{
  m_Observers.erase(&img);
}

void WIA2::CImage::Draw( void* dest, unsigned long destX, unsigned long destY, unsigned long destWidth, unsigned long destHeight ) const
{
  HDC outputDC = static_cast<HDC>(dest);
  
  if(NULL != outputDC)
  {
    unsigned long srcWidth = m_BitmapInfo.bmiHeader.biWidth;
    unsigned long srcHeight =  (unsigned long) fabs((float) m_BitmapInfo.bmiHeader.biHeight); 

    // only blit when dimensions are valid
    if(srcWidth > 0 && srcHeight > 0 && destWidth > 0 && destHeight > 0)
    {
      unsigned long x = destX;
      unsigned long y = destY;
      unsigned long width = destWidth;
      unsigned long height = destHeight;

      // try to fit the aspect ratio of destination
      float srcRatio = (float) srcHeight / (float)srcWidth;
      float destRatio = (float) destHeight / (float) destWidth; 
      
      if(srcRatio > 1.f && destRatio < 1.f) // destination DC uses landscape, image is portrait
      {
        width = static_cast<unsigned long>( static_cast<float>(height) / srcRatio );
        x = static_cast<unsigned short>((destWidth - width) / 2);
      }

      SelectObject(m_MemDC, m_Bitmap );
      StretchBlt(outputDC, x, y, width, height, m_MemDC, 0, 0, srcWidth, srcHeight, SRCCOPY );
    }
  }
}


void WIA2::CImage::Save( const wchar_t* const path) const
{
  FILE* fp = _wfopen(path, L"w+");

  if(NULL != fp)
  {
    BITMAPFILEHEADER bfh = {0};
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER);
    bfh.bfSize = m_Size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER);
    bfh.bfType = MAKEWORD('B', 'M');
    
    fwrite(&bfh, sizeof(bfh), 1, fp);
    fwrite(&m_BitmapInfo.bmiHeader, sizeof(m_BitmapInfo.bmiHeader), 1, fp);
    fwrite((const void*)m_BitmapData, 1, m_Size, fp );
    fclose(fp);
  }
}


//////////////////////////////////////////////////////////////////////////