/*
 *  File:       IWindowMgr.h
 *  Summary:   	Window manager desktop functions.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IWindowMgr.h,v $
 *		Revision 1.4  2000/12/14 08:26:15  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/26 03:33:43  jesjones
 *		Added a DoAddWindows method to XWindowIterator.
 *		
 *		Revision 1.2  2000/11/09 09:08:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 8/05/00	JDJ		Moved AutoPosition and SendToBack to IWindow. 
 *		 <3>	 5/20/99	JDJ		Renamed kCenterOnParentWindow kCenterOnTopWindow, kAlertPositionOnParentWindow 
 *									kAlertPositionOnTopWindow, and kStaggerOnParentWindow kStaggerOnTopWindow.
 *		 <2>	 5/11/99	JDJ		Implemented SendToBack on Windows.
 *		 <1>	10/12/98	JDJ		Created (from Raven)
 */
										
#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class IWindow;
typedef XInterfacePtr<IWindow> IWindowPtr;


//-----------------------------------
//	Types and Constants
//
enum EWindowLayer {kModalLayer = 1, kFloatingLayer = 2, kRegularLayer = 4};
					

// ===================================================================================
//	class XWindowIter
//!		Used to iterate over windows.
// ===================================================================================
class UI_EXPORT XWindowIter : public std::iterator<std::forward_iterator_tag, IWindowPtr> {

//-----------------------------------
//	Types
//
public:
	typedef value_type& 		reference;
    typedef const value_type&	const_reference;    
    typedef value_type*			pointer;
    typedef const value_type*	const_pointer;

//-----------------------------------
//	Initialization/Destruction
//
public:
						XWindowIter();

						XWindowIter(bool visibleOnly, OSWindow wptr);
						
						XWindowIter(const XWindowIter& rhs);
						
		XWindowIter& 	operator=(const XWindowIter& rhs);
						
//-----------------------------------
//	API
//
public:	
        IWindowPtr& 	operator*() const							{return *mIter;}
        IWindowPtr 		operator->() const							{return *mIter;}
        
        XWindowIter& 	operator++()								{++mIter; return *this;}
        XWindowIter 	operator++(int)								{XWindowIter temp = *this; ++mIter; return temp;}
              
        XWindowIter& 	operator--()								{--mIter; return *this;}
        XWindowIter 	operator--(int)								{XWindowIter temp = *this; --mIter; return temp;}
              
		bool 			operator==(const XWindowIter& rhs) const	{return mIter == rhs.mIter || (mIter == mWindows.end() && rhs.mIter == rhs.mWindows.end());}	
		bool 			operator!=(const XWindowIter& rhs) const	{return !this->operator==(rhs);}

private:
#if WIN
		void 			DoAddWindows(bool visibleOnly, OSWindow windH);
#endif

//-----------------------------------
//	Internal Types
//
protected:
	typedef std::vector<IWindowPtr>	Windows;

//-----------------------------------
//	Member Data
//
private:
	Windows				mWindows;
	Windows::iterator	mIter;
};


// ===================================================================================
//	class IWindowMgr
//!		Window manager desktop functions.
// ===================================================================================
class IWindowMgr : public XUnknown {

//-----------------------------------
//	Types
//
public:
	typedef XWindowIter 					iterator;	
    typedef std::reverse_iterator<iterator> reverse_iterator;

//-----------------------------------
//	API
//
public:
	//! @name Accessors
	//@{
	virtual IWindowPtr GetTop(EWindowLayer layer) const = 0;

	virtual IWindowPtr GetTopRegular() const = 0;	
	virtual IWindowPtr GetBottomRegular() const = 0;
		
	virtual IWindowPtr GetTopFloater() const = 0;
	virtual IWindowPtr GetBottomFloater() const = 0;
						/**< Bottom floater may be invisible. */
	
	virtual IWindowPtr GetTopModal() const = 0;
	virtual IWindowPtr GetBottomModal() const = 0;
						/**< Bottom dialog may be invisible. */
	//@}
	
	//! @name Iterating
	//@{
	virtual iterator 	begin(bool visibleOnly = true, OSWindow wptr = nil) const = 0;						
	virtual iterator 	end() const = 0;
						/**< Iterates over all top level windows starting with the
						frontmost.  */

	virtual reverse_iterator rbegin() const = 0;						
	virtual reverse_iterator rend(bool visibleOnly = true, OSWindow wptr = nil) const = 0;
	//@}

	//! @name Misc
	//@{
#if MAC
	virtual void 		Suspend() = 0;
	virtual void 		Resume() = 0;
#endif

#if DEBUG
	virtual void 		DumpWindows() const = 0;
#endif
	//@}
};

typedef XInterfacePtr<IWindowMgr> IWindowMgrPtr;
typedef XInterfacePtr<const IWindowMgr> IConstWindowMgrPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
