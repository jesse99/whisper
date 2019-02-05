/*
 *  File:       X3DDrawContexts.h
 *  Summary:	QuickDraw 3D draw contexts.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DMacDrawContext		- Renders into a CGrafPort.
 *				X3DWin32DCDrawContext	- Renders into a Window's DC.
 *				X3DPixmapDrawContext	- Renders into an offscreen pixmap.
 *
 *  Copyright © 1997-1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DDrawContexts.h,v $
 *		Revision 1.3  2000/12/13 08:51:20  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:15:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DDrawContexts.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:51:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	  8/08/98	JDJ		Added Windows context.
 *		 <1>	  2/17/97	JDJ		Created.
 */

#pragma once

#include <X3DBitmaps.h>
#include <X3DDrawContext.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	struct SMacDrawContextData
//!		TQ3MacDrawContextData wrapper.
// ===================================================================================
#if TARGET_OS_MAC
struct QUESA_EXPORT SMacDrawContextData : public TQ3MacDrawContextData {

public:						
	SMacDrawContextData();

	SMacDrawContextData(const TQ3MacDrawContextData& data);

	SMacDrawContextData(OSWindow window, const X3DColorARGB& clearColor, bool doubleBuffer = true);

	SMacDrawContextData(OSWindow window, const X2DRect& paneArea, bool doubleBuffer = true);

	SMacDrawContextData(OSWindow window, const X3DColorARGB& clearColor, const X2DRect& paneArea, bool doubleBuffer = true);
};
#endif	// TARGET_OS_MAC


// ===================================================================================
//	struct SWin32DCDrawContextData
//!		TQ3Win32DCDrawContextData wrapper.
// ===================================================================================
#if TARGET_OS_WIN32
struct QUESA_EXPORT SWin32DCDrawContextData : public TQ3Win32DCDrawContextData {

public:						
	SWin32DCDrawContextData();

	SWin32DCDrawContextData(const TQ3Win32DCDrawContextData& data);

	SWin32DCDrawContextData(HDC dc, const X3DColorARGB& clearColor, bool doubleBuffer = true);

	SWin32DCDrawContextData(HDC dc, const X2DRect& paneArea, bool doubleBuffer = true);

	SWin32DCDrawContextData(HDC dc, const X3DColorARGB& clearColor, const X2DRect& paneArea, bool doubleBuffer = true);
};
#endif	// TARGET_OS_WIN32


// ===================================================================================
//	struct SPixmapDrawContextData
//!		TQ3PixmapDrawContextData wrapper.
// ===================================================================================
struct QUESA_EXPORT SPixmapDrawContextData : public TQ3PixmapDrawContextData {

public:						
	SPixmapDrawContextData();

	SPixmapDrawContextData(const TQ3PixmapDrawContextData& data);

	SPixmapDrawContextData(const SPixmap& pixmap, const X3DColorARGB& clearColor, bool doubleBuffer = true);

	SPixmapDrawContextData(const SPixmap& pixmap, const X2DRect& paneArea, bool doubleBuffer = true);

	SPixmapDrawContextData(const SPixmap& pixmap, const X3DColorARGB& clearColor, const X2DRect& paneArea, bool doubleBuffer = true);
};


// ===================================================================================
//	class X3DMacDrawContext
//!		Used when rendering to a Mac GrafPort.
// ===================================================================================
#if TARGET_OS_MAC
class QUESA_EXPORT X3DMacDrawContext : public X3DDrawContext {

	typedef X3DDrawContext Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DMacDrawContext();
						
						X3DMacDrawContext(const SMacDrawContextData& data);

						X3DMacDrawContext(TQ3DrawContextObject object);
						
	virtual X3DMacDrawContext* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Window
	//@{
			OSWindow 	GetWindow() const;
			
			void 		SetWindow(OSWindow window);
	//@}

	//! @name 2D Library
	//@{
			TQ3MacDrawContext2DLibrary Get2DLibrary() const;
			
			void 		Set2DLibrary(TQ3MacDrawContext2DLibrary library);
	//@}

	//! @name Data
	//@{
			SMacDrawContextData GetData() const;

			void 		SetData(const SMacDrawContextData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DMacDrawContext. */
	//@}
};
#endif	// TARGET_OS_MAC


// ===================================================================================
//	class X3DWin32DCDrawContext
//!		Used when rendering to a Windows DC.
// ===================================================================================
#if TARGET_OS_WIN32
class QUESA_EXPORT X3DWin32DCDrawContext : public X3DDrawContext {

	typedef X3DDrawContext Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DWin32DCDrawContext();
						
						X3DWin32DCDrawContext(const SWin32DCDrawContextData& data);

						X3DWin32DCDrawContext(TQ3DrawContextObject object);
						
	virtual X3DWin32DCDrawContext* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Window
	//@{
			HDC 		GetWindow() const;
			
			void 		SetWindow(HDC window);
	//@}

	//! @name Data
	//@{
			SWin32DCDrawContextData GetData() const;

			void 		SetData(const SWin32DCDrawContextData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DMacDrawContext. */
	//@}
};
#endif	// TARGET_OS_WIN32


// ===================================================================================
//	class X3DPixmapDrawContext
//!		Used when rendering to an offscreen pixmap (this will often not be accelerated).
// ===================================================================================
class QUESA_EXPORT X3DPixmapDrawContext : public X3DDrawContext {

	typedef X3DDrawContext Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPixmapDrawContext();
						
						X3DPixmapDrawContext(const SPixmapDrawContextData& data);

						X3DPixmapDrawContext(TQ3DrawContextObject object);
						
	virtual X3DPixmapDrawContext* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Pixmap
	//@{
			SPixmap 	GetPixmap() const;
			
			void 		SetPixmap(const SPixmap& pixmap);
	//@}

	//! @name Data
	//@{
			SPixmapDrawContextData GetData() const;

			void 		SetData(const SPixmapDrawContextData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPixmapDrawContext. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

