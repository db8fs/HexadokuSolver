#ifndef DELEGATES__FEB43A16_3CBA_43B4_99AF_484519C8BF68__
#define DELEGATES__FEB43A16_3CBA_43B4_99AF_484519C8BF68__


//////////////////////////////////////////////////////////////////////////
/**
  \file     Delegates.h
  \brief    This file holds delegate implementations for convenience.
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


//////////////////////////////////////////////////////////////////////////
/**
  \interface  IDelegate
  \brief    Implementers of this interface may be invoked.
*/
//////////////////////////////////////////////////////////////////////////

struct IDelegate
{
  virtual ~IDelegate() {}

  // eigentlich müsste es Invoke(IDelegate*) sein...
  virtual void Invoke() = 0;
};


//////////////////////////////////////////////////////////////////////////
/**
  \struct    CDelegate0 
  \brief    Delegate implementation for 0-ary functions.
*/
//////////////////////////////////////////////////////////////////////////

template <class ObjectType, typename CallbackType>
struct CDelegate0 : public virtual IDelegate
{
  CDelegate<ObjectType, CallbackType>(ObjectType* obj, CallbackType cb)
    : m_Obj(obj), m_Callback(cb)
  {}

  virtual ~CDelegate<ObjectType, CallbackType>()
  {}

  //! execute the function via Interface
  virtual void Invoke()
  {
    // proxy operation
    (m_Obj->*m_Callback)();
  }

private:
  ObjectType*  m_Obj;
  CallbackType m_Callback;
};



//////////////////////////////////////////////////////////////////////////

#endif // DELEGATES__FEB43A16_3CBA_43B4_99AF_484519C8BF68__
