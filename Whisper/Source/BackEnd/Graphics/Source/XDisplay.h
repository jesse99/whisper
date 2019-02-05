/*
 *	File:		XDisplay.h
 *	Function:	Encapsulates a display device (typically a monitor).
 *	Written by:	Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDisplay.h,v $
 *		Revision 1.4  2001/03/05 05:41:09  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:51:38  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 08:50:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#if MAC
	#include <Video.h>
#endif

#include <XRect.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
struct ZModeInfo;

class ZDisplayIterator;


//-----------------------------------
//	Types
//
#if MAC
	typedef VDSwitchInfoRec OSDisplayState;			// cookie used to identify a particular monitor configuration
#elif WIN
	typedef DEVMODE OSDisplayState;
#endif


// ===================================================================================
//	class XDisplayState
//!		Returned while iterating over a display's modes.
// ===================================================================================
class GRAPHICS_EXPORT XDisplayState {

//-----------------------------------
//	Initialization/Destruction
//
public:	
						XDisplayState();

#if MAC
						XDisplayState(OSDisplay display, const ZModeInfo& info, uint32 depthIndex);
#elif WIN
						XDisplayState(OSDisplay display, const DEVMODE& mode);
#endif

//-----------------------------------
//	API
//
public:
	//! @name Getters
	//@{
			XSize 		GetResolution() const						{return mResolution;}
			
			int32 		GetDepth() const							{return mDepth;}
						
			double		GetRefreshRate() const						{return mRefreshRate;}
						/**< Vertical refresh rate in Hz.
						This returns 0.0 on my system when running Win 98
						(although NT works fine). The documentation says
						that this should work on Win 98 so it may be a
						driver issue... */
						
			OSDisplay 	GetDisplay() const							{return mDisplay;}
						
			OSDisplayState GetState() const							{return mState;}
	//@}

	//! @name Comparisons
	//@{
			bool 		operator==(const XDisplayState& rhs) const	{ASSERT(mDisplay == rhs.mDisplay); return mResolution == rhs.mResolution && mDepth == rhs.mDepth && mRefreshRate == rhs.mRefreshRate;}	
			
			bool 		operator!=(const XDisplayState& rhs) const	{return !this->operator==(rhs);}
	//@}

//-----------------------------------
//	Member Data
//
protected:
	XSize			mResolution;
	double			mRefreshRate;
	int32			mDepth;
	
	OSDisplay		mDisplay;
	OSDisplayState	mState;	
};


// ===================================================================================
//	class XDisplay
//!		Encapsulates a display device (typically a monitor).
// ===================================================================================
class GRAPHICS_EXPORT XDisplay {

//-----------------------------------
//	Types
//
public:
	class iterator {

	public:
						~iterator();				
						
						iterator();				
						iterator(const XDisplay* display);	
						iterator(const iterator& rhs);	
				
		iterator& 		operator=(const iterator& rhs);

		XDisplayState 	operator*() const;
		iterator& 		operator++();
		
		bool 			operator==(const iterator& rhs) const;	
		bool 			operator!=(const iterator& rhs) const		{return !this->operator==(rhs);}

	private:
#if MAC
		ZDisplayIterator*	mIter;	
#elif WIN
		OSDisplay			mDisplay;
		DWORD				mIndex;
		DEVMODE				mMode;		
#endif
	};

//-----------------------------------
//	Initialization/Destruction
//
public:	
						XDisplay(const OSDisplay& display);

//-----------------------------------
//	API
//
public:
	//! @name Conversion Operators
	//@{
						operator OSDisplay() const					{return mDisplay;}
	//@}

	//! @name Getters
	//@{
			XPoint 		GetLocation() const;
			
			XSize 		GetResolution() const;
			
			XRect 		GetFrame() const							{return XRect(this->GetLocation(), this->GetResolution());}

			XRect 		GetClientArea() const;
						/**< Returns the frame minus the menubar (Mac) or task bar (Win). */
			
			int32 		GetDepth() const;
			
			bool 		IsColor() const;
	//@}

	//! @name States
	//@{
			OSDisplayState GetState() const;
						/**< Use this to save the state of a display. */

			bool 		SetState(const OSDisplayState& state);
			bool 		SetState(const XDisplayState& state)		{return this->SetState(state.GetState());}
						/**< Use this to change a display to one of the states returned by 
						begin(). Returns false if the display couldn't be changed (this
						can happen on Windows with some video drivers). */
	//@}

	//! @name XDisplayState Iterating
	//@{
			iterator 	begin() const;
						/**< Note that some video cards on the Mac wait for the VBL interval
						before returning a new state. This can make iterating over all
						the modes of a multi-synch monitor rather slow... */
			
			iterator	end() const;
	//@}

	//! @name Comparison Operators
	//@{
			bool 		operator==(const XDisplay& rhs) const		{return mDisplay == rhs.mDisplay;}

			bool 		operator!=(const XDisplay& rhs) const		{return mDisplay != rhs.mDisplay;}
	//@}

//-----------------------------------
//	Member Data
//
protected:
	OSDisplay		mDisplay;
	
#if WIN
	OSDisplayState	mCurrentState;
#endif			
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
