/*
 *  File:       XQueue.inc
 *  Summary:    Fast light weight queue class that's optimized for a relatively
 *				small number of elements.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XQueue.inc,v $
 *		Revision 1.2  2000/11/09 12:48:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */
 
#include <XTypes.h>

namespace Whisper {


// ===================================================================================
//	class XQueue
// ===================================================================================

//---------------------------------------------------------------
//
// XQueue::~XQueue
//
//---------------------------------------------------------------
template <class TYPE, int INITIAL_SIZE>
XQueue<TYPE, INITIAL_SIZE>::~XQueue()
{
	if (mHeapBuffer != nil)			// dorky micro-optimization
		delete [] mHeapBuffer;
}


//---------------------------------------------------------------
//
// XQueue::XQueue ()
//
//---------------------------------------------------------------
template <class TYPE, int INITIAL_SIZE>
XQueue<TYPE, INITIAL_SIZE>::XQueue()
{
	mBuffer   = mInitialBuffer;
	mCount     = 0;
	mCapacity = INITIAL_SIZE;
	
	mHeapBuffer = nil;
}


//---------------------------------------------------------------
//
// XQueue::XQueue (XQueue)
//
//---------------------------------------------------------------
template <class TYPE, int INITIAL_SIZE>
XQueue<TYPE, INITIAL_SIZE>::XQueue(const XQueue<TYPE, INITIAL_SIZE>& rhs)
{	
	mBuffer     = mInitialBuffer;
	mCount       = 0;
	mCapacity   = INITIAL_SIZE;	
	mHeapBuffer = nil;

	if (rhs.size() > mCapacity)
		this->DoExpandBuffer(rhs.size());
		
	std::copy(rhs.begin(), rhs.end(), this->begin());

	mCount = rhs.mCount;
}



//---------------------------------------------------------------
//
// XQueue::operator=
//
//---------------------------------------------------------------
template <class TYPE, int INITIAL_SIZE>
XQueue<TYPE, INITIAL_SIZE>& XQueue<TYPE, INITIAL_SIZE>::operator=(const XQueue<TYPE, INITIAL_SIZE>& rhs)
{	
	if (this != &rhs) {
		if (rhs.size() > mCapacity)
			this->DoExpandBuffer(rhs.size());
			
		STD::copy(rhs.begin(), rhs.end(), this->begin());

		mCount = rhs.mCount;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XQueue::push_back
//
//---------------------------------------------------------------
template <class TYPE, int INITIAL_SIZE>
void XQueue<TYPE, INITIAL_SIZE>::push_back(const TYPE& x)
{
	if (mCount + 1 > mCapacity)
		this->DoExpandBuffer(2*mCount);
		
	mBuffer[mCount] = x;
	mCount++;
}


//---------------------------------------------------------------
//
// XQueue::push_front
//
//---------------------------------------------------------------
template <class TYPE, int INITIAL_SIZE>
void XQueue<TYPE, INITIAL_SIZE>::push_front(const TYPE& x)
{	
	if (mCount + 1 > mCapacity)
		this->DoExpandBuffer(2*mCount);
		
	STD::copy_backward(this->begin(), this->end(), this->end() + 1);
		
	mBuffer[0] = x;
	mCount++;
}


//---------------------------------------------------------------
//
// XQueue::pop_front
//
//---------------------------------------------------------------
template <class TYPE, int INITIAL_SIZE>
void XQueue<TYPE, INITIAL_SIZE>::pop_front()
{	
	ASSERT(mCount > 0);
	
	STD::copy(this->begin() + 1, this->end(), this->begin());
	
	mCount--;	
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XQueue::DoExpandBuffer
//
//---------------------------------------------------------------
template <class TYPE, int INITIAL_SIZE>
void XQueue<TYPE, INITIAL_SIZE>::DoExpandBuffer(uint32 newSize)
{	
	ASSERT(newSize >= mCapacity);
	
	TYPE* newBuffer = new TYPE[newSize];
	
	std::copy(this->begin(), this->end(), newBuffer);

	delete [] mHeapBuffer;
	mHeapBuffer = newBuffer;
	
	mBuffer = mHeapBuffer;
	mCapacity = newSize;
}


}	// namespace Whisper

