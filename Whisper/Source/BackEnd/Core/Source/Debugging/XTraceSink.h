/*
 *  File:       XTraceSink.h
 *  Summary:    Abstract base class for TRACE destinations.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTraceSink.h,v $
 *		Revision 1.4  2001/03/05 05:44:12  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:56:53  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:57:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XTraceSink
//!		Abstract base class for TRACE destinations.
// ===================================================================================
class CORE_EXPORT XTraceSink {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XTraceSink();
						
						XTraceSink();

private:
						XTraceSink(const XTraceSink& rhs);
						
			XTraceSink& operator=(const XTraceSink& rhs);
						
//-----------------------------------
//	API
//
public:
			void 		HandlePrint(const wchar_t* str);
				
			const wchar_t* GetName() const 					{return this->OnGetName();}
			
			bool 		IsEnabled() const					{return mEnabled;}
			
			void 		Enable(bool enable = true)			{mEnabled = enable;}

protected:
	virtual void 		OnPrint(const wchar_t* str) = 0;
						/**< Note that this should not do any buffering. */
						
	virtual const wchar_t* OnGetName() const = 0;

//-----------------------------------
//	Member Data
//
private:
	bool mEnabled;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG