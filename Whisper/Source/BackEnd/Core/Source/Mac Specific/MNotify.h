/*
 *  File:       MNotify.h
 *  Summary:   	A wrapper around the Notification Manager.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1996 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MNotify.h,v $
 *		Revision 1.4  2001/03/05 05:39:24  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:48:44  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:16:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Notification.h>
#include <Resources.h>

#include <XListener.h>
#include <XStateBroadcaster.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class MNotify
//!		A wrapper around the Notification Manager.
// ===================================================================================
class CORE_EXPORT MNotify : public XListenerMixin<SStateMessage> {

	typedef XListenerMixin<SStateMessage> Inherited;

//-----------------------------------
//	Initialization/Destruction
//
protected:
	virtual 			~MNotify();
						/**< Protected to prevent allocation on the stack. */
	
public:
  						MNotify(ResID iconID, ResID soundID = 0, bool useMark = true);
  						/**< Defaults to the "polite" notification: an icon blinking in
  						the menu bar with the app's name marked. If iconID is 0 no
  						icon is displayed. If iconID is greater than 0 an 'ics8' or
  						'ics4' resource is used. If soundID is -1 the system alert 
  						sound is played. If soundID is greater than 0 a 'snd ' 
  						resource is played. */

  						MNotify(const std::wstring& mesg, ResID iconID, ResID soundID = -1, bool useMark = true);
						/**< The obnoxious notification: an alert is displayed in whatever
						app happens to be frontmost. This should be used sparingly. */
						
  						MNotify(const std::wstring& mesg, ResID soundID = 0);
  						/**< Displays an alert without messing with the menu bar. This
  						can be used when you want to display an alert in things like
  						Drag Manager callbacks (note that you cannot use this in a 
  						VBL or Time Manager callback because the class doesn't use 
  						an interrupt safe version of operator new). */
  						  						
  	virtual void 		Post();
  						/**< To use a MNotify object first create it on the heap and then
  						call Post. The object will be deleted automatically. */
  						
private:
						MNotify(const MNotify& rhs);
						
			MNotify& 	operator=(const MNotify& rhs);

//-----------------------------------
//	New API
//
public:
	virtual void 		HandleCallback();
	
protected:
	virtual void 		OnCallback()							{}
						/**< This will be called after the sound has played or the user 
						dismisses the alert. Note that Inside Mac says that this should 
						not cause anything to be drawn or otherwise affect the interface. */
				
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnBroadcast(const SStateMessage& mesg);
						/**< Deletes the object if message is kResumingApp. */

//-----------------------------------
//	Internal API
//
private:
	static	pascal void DoCallback(NMRec* record);
	
			void 		DoInit(const std::wstring& mesg, ResID iconID, ResID soundID, bool useMark);
			
//-----------------------------------
//	Member Data
//
protected:
	NMRec			mRecord;	
	std::wstring	mText;
	
	bool			mCalledFromForeground;
	bool			mInstalled;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
