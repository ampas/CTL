///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Academy of Motion Picture Arts and Sciences 
// ("A.M.P.A.S."). Portions contributed by others as indicated.
// All rights reserved.
// 
// A worldwide, royalty-free, non-exclusive right to copy, modify, create
// derivatives, and use, in source and binary forms, is hereby granted, 
// subject to acceptance of this license. Performance of any of the 
// aforementioned acts indicates acceptance to be bound by the following 
// terms and conditions:
//
//  * Copies of source code, in whole or in part, must retain the 
//    above copyright notice, this list of conditions and the 
//    Disclaimer of Warranty.
//
//  * Use in binary form must retain the above copyright notice, 
//    this list of conditions and the Disclaimer of Warranty in the
//    documentation and/or other materials provided with the distribution.
//
//  * Nothing in this license shall be deemed to grant any rights to 
//    trademarks, copyrights, patents, trade secrets or any other 
//    intellectual property of A.M.P.A.S. or any contributors, except 
//    as expressly stated herein.
//
//  * Neither the name "A.M.P.A.S." nor the name of any other 
//    contributors to this software may be used to endorse or promote 
//    products derivative of or based on this software without express 
//    prior written permission of A.M.P.A.S. or the contributors, as 
//    appropriate.
// 
// This license shall be construed pursuant to the laws of the State of 
// California, and any disputes related thereto shall be subject to the 
// jurisdiction of the courts therein.
//
// Disclaimer of Warranty: THIS SOFTWARE IS PROVIDED BY A.M.P.A.S. AND 
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, 
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT ARE DISCLAIMED. IN NO 
// EVENT SHALL A.M.P.A.S., OR ANY CONTRIBUTORS OR DISTRIBUTORS, BE LIABLE 
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, RESITUTIONARY, 
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.
//
// WITHOUT LIMITING THE GENERALITY OF THE FOREGOING, THE ACADEMY 
// SPECIFICALLY DISCLAIMS ANY REPRESENTATIONS OR WARRANTIES WHATSOEVER 
// RELATED TO PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS IN THE ACADEMY 
// COLOR ENCODING SYSTEM, OR APPLICATIONS THEREOF, HELD BY PARTIES OTHER 
// THAN A.M.P.A.S., WHETHER DISCLOSED OR UNDISCLOSED.
///////////////////////////////////////////////////////////////////////////


#ifndef INCLUDED_CTL_RC_PTR_H
#define INCLUDED_CTL_RC_PTR_H

//-----------------------------------------------------------------------
//
//	class RcObject -- a base class for reference-counted
//	objects.  Reference-counting pointers can point only
//	to objects defived from RcObject.
//
//	template clsss RcPtr<T> -- reference-counting pointers
//	that automatically delete the pointed-to objects if the
//	objects' reference counters reach zero.
//
//-----------------------------------------------------------------------

#include <mutex>

namespace Ctl {


class RcObject
{
  public:

    RcObject (): _n (0) {}
    RcObject (const RcObject &ro) {}
    virtual ~RcObject();
    const RcObject & operator = (const RcObject &ro) {return *this;}

  private:

    template <class T> friend class RcPtr;

    unsigned long	_n;
};


template <class T>
class RcPtr
{
  public:

    //-------------------------------------------------
    // Constructor:
    //
    // T pointer p is stored in the new RcPtr, and if p
    // is not 0, the reference counter in the object
    // to which p points is incremented by 1.
    //-------------------------------------------------

    RcPtr (T *p = 0);


    //--------------------------------------------------
    // Copy constructor:
    //
    // RcPtr rp is copied, and if rp contains a non-zero
    // T pointer, the reference counter in the object to
    // which rp points is incremented by 1.
    //--------------------------------------------------

    RcPtr (const RcPtr &rp);

    template <class S>
    RcPtr (const RcPtr <S> &rp);


    //------------------------------------------------------
    // Destructor:
    //
    // If the RcPtr to be destroyed contains a non-zero T
    // pointer, the reference counter in the corresponding
    // object is decremented by 1.  If the reference counter
    // is 0 after decrementing, then the object to which the
    // pointer points is deleted.
    //------------------------------------------------------

    ~RcPtr ();


    //------------------------------------------------------
    // Assignment -- RcPtr to RcPtr:
    //
    // If the left-hand and right-hand side RcPtr point
    // to the same object, then assignment is a no-op.
    // Otherwise, if the left-hand side RcPtr contains a
    // non-zero T pointer, the reference counter in the
    // corresponding object is decremented by 1.  If the
    // reference counter is 0 after decrementing, the object
    // to which the left-hand side RcPtr points is deleted.
    // Then, RcPtr rp is copied into the left-hand side
    // RcPtr, and if rp contains a non-zero T pointer the
    // reference counter in the object to which rp points
    // is incremented by 1.
    //------------------------------------------------------

    const RcPtr &	operator = (const RcPtr &rp);

    template <class S>
    const RcPtr &	operator = (const RcPtr <S> &rp);


    //----------------------------------------------------------------
    // Dynamic cast:
    //
    // t.cast() returns a new reference-counted pointer, s, of type
    // RcPtr<S,RA>.  The pointer stored in s is computed by evaluating
    // the expression dynamic_cast<S*>(t.pointer()).
    //----------------------------------------------------------------

    template <class S>
    RcPtr <S>		cast () const;


    //--------------------------------------------------------------
    // Return a reference to the T object, to which the RcPtr points
    // (the effect of this method is undefined if the RcPtr contains
    // a zero T pointer).
    //--------------------------------------------------------------

    T &			operator * () const;


    //------------------------------------------
    // Return the T pointer stored in the RcPtr.
    //------------------------------------------

    T *			operator -> () const;
    T *			pointer () const;


    //--------------------------------------------------------
    // Return the value of the reference counter in the object
    // to which the RcPtr points (zero if the RcPtr contains a
    // zero T pointer).
    //--------------------------------------------------------

    unsigned long	refcount () const;


    //---------------------------------
    // Test wether the pointer is null.
    //---------------------------------

    			operator bool () const;
    bool                operator ! () const;

   private:

    void		ref ();
    void		unref ();

    template <class S> friend class RcPtr;

    template <class S>
    RcPtr (const RcPtr <S> &rp, int);

    T *			_p;
};


void throwRcPtrExc (const RcObject *lhs, const RcObject *rhs);
std::mutex &rcPtrMutex (RcObject *ptr);


//---------------
// Implementation
//---------------

template <class T>
inline void	
RcPtr<T>::ref ()
{
    if (_p)
    {
      std::lock_guard<std::mutex> lock (rcPtrMutex ((RcObject*)_p));
	(_p->_n)++;
    }
}


template <class T>
inline void
RcPtr<T>::unref ()
{
    if (_p)
    {
	unsigned long n;

	{
        std::lock_guard<std::mutex> lock(rcPtrMutex ((RcObject*)_p));
	    n = --(_p->_n);
	}

	if (n == 0)
	{
	    delete _p;
	    _p = 0;
	}
    }
}


template <class T>
inline
RcPtr<T>::RcPtr (T *p):
    _p (p)
{
    ref();
}


template <class T>
inline
RcPtr<T>::RcPtr (const RcPtr &rp):
    _p (rp._p)
{
    ref();
}


template <class T>
template <class S>
inline
RcPtr<T>::RcPtr (const RcPtr<S> &rp)
{
    if (rp._p == 0)
    {
	_p = 0;
    }
    else if (T * tp = dynamic_cast <T *> (rp._p))
    {
	_p = tp;
	ref();
    }
    else
    {
	throwRcPtrExc (0, rp._p);
    }
}


template <class T>
template <class S>
inline
RcPtr<T>::RcPtr (const RcPtr<S> &rp, int)
{
    if (T * tp = dynamic_cast <T *> (rp._p))
    {
	_p = tp;
	ref();
    }
    else
    {
	_p = 0;
    }
}


template <class T>
inline
RcPtr<T>::~RcPtr ()
{
    unref();
}


template <class T>
inline const RcPtr<T> &
RcPtr<T>::operator = (const RcPtr &rp)
{
    if (rp._p != _p)
    {
	unref();
	_p = rp._p;
	ref();
    }

    return *this;
}
    

template <class T>
template <class S>
inline const RcPtr<T> &
RcPtr<T>::operator = (const RcPtr<S> &rp)
{
    if (rp._p == 0)
    {
	unref();
	_p = 0;
    }
    else if (T * tp = dynamic_cast <T *> (rp._p))
    {
	if (tp != _p)
	{
	    unref();
	    _p = tp;
	    ref();
	}
    }
    else
    {
	throwRcPtrExc (_p, rp._p);
    }

    return *this;
}


template <class T>
template <class S>
RcPtr <S>
RcPtr<T>::cast () const
{
    return RcPtr <S> (*this, 0);
}
    

template <class T>
inline T &
RcPtr<T>::operator * () const
{
    return *_p;
}


template <class T>
inline T *
RcPtr<T>::operator -> () const
{
    return _p;
}


template <class T>
inline T *
RcPtr<T>::pointer () const
{
    return _p;
}


template <class T>
inline unsigned long
RcPtr<T>::refcount () const
{
    return _p? _p->_n: 0;
}


template <class T>
inline
RcPtr<T>::operator bool () const
{
    return _p != 0;
}


template <class T>
inline bool
RcPtr<T>::operator ! () const
{
    return _p == 0;
}

} // namespace Ctl

#endif
