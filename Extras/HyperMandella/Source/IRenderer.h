/*
 *  File:       IRenderer.h
 *  Summary:   	Interface used to draw a fractal into a pixmap.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IRenderer.h,v $
 *		Revision 1.4  2001/04/04 04:53:58  jesjones
 *		Added some cheesy methods to support anti-aliasing.
 *		
 *		Revision 1.3  2001/03/26 04:23:32  jesjones
 *		Changed the depth argument from an int16 to an int32.
 *		
 *		Revision 1.2  2001/03/25 06:08:41  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:38  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 09:06:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XColorTable.h>
#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


//-----------------------------------
//	Forward References
//
class IDocInfo;
typedef XInterfacePtr<IDocInfo> IDocInfoPtr;

namespace Whisper {
	class XPixMap;
	class XSize;
}


// ===================================================================================
//	class IRenderer
//!		Interface used to draw a fractal into a pixmap.
// ===================================================================================
class IRenderer : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		IsDone() const = 0;
	
	virtual int32 		Render(MilliSecond delay) = 0;
						/**< Call this until IsDone returns true. Ticks is the amount of
						time to spend drawing. Returns the percentage of the fractal
						that has been completed as a number from 0 to 100. */
						
	virtual void 		SetResolution(const XSize& resolution, int32 depth) = 0;
						/**< Call this to override the doc info resolution and pixel depth 
						(eg if you want to draw a thumbnail). */

	virtual const XPixMap* GetImage() const = 0;

	virtual void 		Reset(const IDocInfoPtr& doc) = 0;
	virtual void 		Reset() = 0;
						/**< Used when anti-aliasing. */ 
};

typedef XInterfacePtr<IRenderer> IRendererPtr;
typedef XInterfacePtr<const IRenderer> IConstRendererPtr;

