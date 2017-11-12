#ifndef Scanner_h__
#define Scanner_h__

//////////////////////////////////////////////////////////////////////////
/**
  \file     ScannerAPI.h
  \brief    Simple API interface definitions for scanner access.
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

class IImageObserver;
class IImage;
class IDocument;
class IScanner;
class IScannerEvent;
class IScannerObserver;
class IScannerManager;
class IContainerEvent;

//////////////////////////////////////////////////////////////////////////
/**
  \interface  IScannerEvent
  \brief      Implementers of this interface will provide information 
              about scanner events that occured.
*/
//////////////////////////////////////////////////////////////////////////

class IScannerEvent
{
public:
  virtual ~IScannerEvent() {}

  //! gets the identifier of the event
  virtual const wchar_t* GetEventID() const = 0;

  //! gets the name of the event
  virtual const wchar_t* GetName() const = 0;

  //! description of the event
  virtual const wchar_t* GetDescription() const = 0;
  
  //! the type of the event
  virtual unsigned long GetType() const = 0;

  //! subscribes for being notified about this event
  virtual void Subscribe( const IScannerObserver & observer ) = 0;

  //! unsubscribes from being notified about this event
  virtual void Unsubscribe( const IScannerObserver & observer ) = 0;

  //! the scanner belonging to this event
  virtual IScanner* const Scanner() const = 0;
};


//////////////////////////////////////////////////////////////////////////
/**
  \interface  IScannerEventFactory
  \brief      Implementers of this interface are able to create 
              event objects.  
*/
//////////////////////////////////////////////////////////////////////////

class IScannerEventFactory
{
public:
  virtual ~IScannerEventFactory() {}

  //! creates a single scanner event and registers it at the scanner
  virtual IScannerEvent*  Create(IScanner & scanner) const = 0;

  //! recycles and deregisters all scanner events 
  virtual void      Recycle(IScanner & scanner) const = 0;
};


//////////////////////////////////////////////////////////////////////////
/**
  \interface  IContainerEvent
  \brief      Implementers of this interface may group events 
*/
//////////////////////////////////////////////////////////////////////////

class IContainerEvent
{
public:
  virtual ~IContainerEvent() {}

  //! clears this container
  virtual void Clear() = 0;

  //! sets the iterator of the event vector to the first item
  virtual void First() = 0;

  //! returns true if the end of the vector has been reached
  virtual bool isDone() const = 0;

  //! iterates to the next available event
  virtual void Next() = 0;

  //! returns the current item of the container
  virtual IScannerEvent* const CurrentElement() const = 0;
};



//////////////////////////////////////////////////////////////////////////
/**
  \interface  IScannerObserver
  \brief      Implementers of this interface may register for receiving
              scanner events.
*/
//////////////////////////////////////////////////////////////////////////

class IScannerObserver
{
public:
  virtual ~IScannerObserver() {}

  //! the notification function
  virtual void OnScannerEvent(const IScannerEvent& obj) const  = 0;
};


//////////////////////////////////////////////////////////////////////////
/**
  \interface  IImageObserver
  \brief    Implementers of this interface may be notified about
            image updates
*/
//////////////////////////////////////////////////////////////////////////

class IImageObserver
{
public:
  virtual ~IImageObserver() {}

  //! notifies about updates about this image
  virtual void OnImageEvent(const IImage & img) const = 0;
};


//////////////////////////////////////////////////////////////////////////
/**
  \interface  IImage
  \brief    Implementers of this interface are representing images.
  \todo    Should be adapted to be convenient to use with OpenCV,
           Leptonica or stuff like that.        
*/
//////////////////////////////////////////////////////////////////////////

class IImage
{
public:
  //! destruction
  virtual ~IImage() {}

  //! draws the image to the destination surface
  virtual void Draw(  void* dest, 
            unsigned long x, 
            unsigned long y, 
            unsigned long width, 
            unsigned long height) const = 0;

  //! locks the mutex
  virtual void Lock() = 0;
  
  //! unlocks the mutex
  virtual void Unlock() = 0;

  //! subscribe for receiving updates about this image
  virtual void Subscribe(const IImageObserver & img) = 0;

  //! unsubscribe from receiving updates about this image
  virtual void Unsubscribe(const IImageObserver & img) = 0;

  //! saves the image 
  virtual void Save( const wchar_t* path) const = 0;

  //! gets access to the raw data buffer
  virtual bool GetRawData(unsigned char** buffer, unsigned long* size) const = 0; 
};


//////////////////////////////////////////////////////////////////////////
/**
  \interface  IDocument
  \brief    Implementers of this interface are representing documents
            that are processed by scanners.
*/
//////////////////////////////////////////////////////////////////////////

class IDocument
{
public:
  virtual ~IDocument() {}
  
  //! the image scanned into this document
  virtual IImage & Image() = 0;

  //! acquires the item
  virtual bool Download() = 0;
  
  //! the scanner belonging to this document
  virtual IScanner & Scanner() const = 0;
};


//////////////////////////////////////////////////////////////////////////
/**
  \interface  IDocumentFactory
  \brief      Implementers of this interface are able to create 
              document objects for a given scanner.
*/
//////////////////////////////////////////////////////////////////////////

class IDocumentFactory
{
public:
  virtual ~IDocumentFactory() {}

  //! creates a single document belonging to a scanner
  virtual ::IDocument &   Create(IScanner & scanner) const = 0;

  //! recycles and deregisters all scanner documents for the given scanner
  virtual void      Recycle(IScanner & scanner) const = 0;
};


//////////////////////////////////////////////////////////////////////////
/**
  \interface  IContainerDocument
  \brief      Implementers of this interface are able to holds a set 
              of documents.
*/
//////////////////////////////////////////////////////////////////////////

class IContainerDocument
{
public:
  virtual ~IContainerDocument() {}
  
  //! clears this container
  virtual void Clear() = 0;

  //! sets the iterator of the document container to the first item
  virtual void First() = 0;

  //! returns true if the end of the container has been reached
  virtual bool isDone() const = 0;

  //! iterates to the next available event
  virtual void Next() = 0;

  //! returns the current item of the container
  virtual ::IDocument* const CurrentElement() const = 0;
};


//////////////////////////////////////////////////////////////////////////
/**
  \interface  IScanner
  \brief    Implementers of this interface have the basic means for
            acquiring image data from a scanner device.
*/
//////////////////////////////////////////////////////////////////////////

class IScanner
{
public:
  virtual ~IScanner() {}

  //! updates the status of this device
  virtual void Update() = 0;

  //! the name of the scanner device
  virtual const wchar_t* GetDeviceName() const = 0;

  //! the identifier of the scanner device
  virtual const wchar_t* GetDeviceID() const = 0;

  //! a description about the scanner device
  virtual const wchar_t* GetDeviceDescription() const = 0;
  
  //! the events belonging to this scanner device
  virtual IContainerEvent & Events() = 0;

  //! this container holds the documents belonging to the scanner device
  virtual IContainerDocument & Documents() = 0;

  //! the manager object responsible for this scanner
  virtual IScannerManager & Manager() const = 0;

};


//////////////////////////////////////////////////////////////////////////
/**
  \interface  IScannerFactory
  \brief      Implementers of this interface are able to create 
              and destroy scanner objects.
*/
//////////////////////////////////////////////////////////////////////////

class IScannerFactory
{
public:
  virtual ~IScannerFactory() {}
  
  //! creates a single scanner object and registers it at the manager
  virtual IScanner*  Create(IScannerManager & mgr) const = 0;

  //! recycles all scanner objects belonging to the given manager object
  virtual void    Recycle(IScannerManager & mgr) const = 0;
};



//////////////////////////////////////////////////////////////////////////
/**
  \interface  IContainerScanner
  \brief      Implementers of this interface are able to holds a set 
              of scanners.
*/
//////////////////////////////////////////////////////////////////////////

class IContainerScanner
{
public:
  virtual ~IContainerScanner() {}
  
  //! clears this container
  virtual void Clear() = 0;

  //! the number of items within the container
  virtual unsigned int Count() const = 0;

  //! sets the iterator of the document container to the first item
  virtual void First() = 0;

  //! returns true if the end of the container has been reached
  virtual bool isDone() const = 0;

  //! iterates to the next available event
  virtual void Next() = 0;

  //! returns the current item of the container
  virtual ::IScanner* const CurrentElement() const = 0;
};


//////////////////////////////////////////////////////////////////////////
/**
  \interface  IScannerManager
  \brief      Implementers of this interface are able to enumerate
              scanning devices
*/
//////////////////////////////////////////////////////////////////////////

class IScannerManager
{
public:
  virtual ~IScannerManager() {}

  //! initializes the scanner manager
  virtual bool Init() = 0;

  //! cleans up allocated resources
  virtual bool Cleanup() = 0;

  //! the device that are available
  virtual IContainerScanner & Scanners()  = 0;

  //! generic object handler
  virtual void* Object() = 0;
};

//////////////////////////////////////////////////////////////////////////


#endif // Scanner_h__
