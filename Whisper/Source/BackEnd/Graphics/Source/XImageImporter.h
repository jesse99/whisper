/*
 *  File:       XImageImporter.h
 *  Summary:	Abstract base class that can be used to initialize XBaseImage's.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first): 
 *
 *		$Log: XImageImporter.h,v $
 *		Revision 1.5  2001/04/17 01:43:57  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/03/05 05:42:03  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:53:23  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:38:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <vector>

#include <XCriticalSection.h>
#include <XInvariant.h>
#include <XRect.h>
#include <XReferenceCounted.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XColorTable;
class XBaseImage;
class XResource;
class XURI;


// ===================================================================================
//	class XImageImporter
//!		Abstract base class that can be used to initialize XBaseImage's.
// ===================================================================================
class GRAPHICS_EXPORT XImageImporter : public XReferenceCountedMixin {

	typedef XReferenceCountedMixin Inherited;

//-----------------------------------
//	Types
//
public:
	typedef XImageImporter* (*CreatorProcPtr)(XResource* takeData);

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XImageImporter();
						
						XImageImporter();
						
private:
						XImageImporter(const XImageImporter& rhs);

			XImageImporter& operator=(const XImageImporter& rhs);
						
//-----------------------------------
//	New API
//
public:
	//! @name Importing
	//@{
	virtual void 		Unpack(XBaseImage* sink, const XPoint& where = kZeroPt) const = 0;
						/**< Unpacks the image into the sink starting at where. Note that
						the image depth and size should already be set up: this method
						won't do any clipping or color mapping. */
	//@}
						
	//! @name Accessors
	//@{
			int32 		GetWidth() const								{return mWidth;}
			int32 		GetHeight() const								{return mHeight;}
			XSize 		GetSize() const									{return XSize(mWidth, mHeight);}
		
			XRect 		GetExtent() const								{return XRect(0, 0, mWidth, mHeight);}
		
			int32 		GetDepth() const								{return mDepth;}
			
			XColorTable* GetColors() const								{return mColors;}
	//@}

	//! @name Importer Registration
	//@{
	static	void 		Register(const std::wstring& extension, CreatorProcPtr createProc);
	static	void 		Unregister(const std::wstring& extension);
						/**< Used to register functions that create XImageImporter objects
						using the specified file extension. Case is ignored. */
	//@}

	//! @name Importer Factory
	//@{
	static	XImageImporter* Create(const XURI& uri);
						/**< Creates an XImageImporter object using the uri's extension.
						Will throw if the extension cannot be found in the table. */
	//@}

//-----------------------------------
//	Inherited API
//
protected:
			void		Invariant() const;

//-----------------------------------
//	Inherited Types
//
protected:
	struct SEntry {
		std::wstring 	extension;
		CreatorProcPtr	creator;
		
				SEntry(const std::wstring& e = L"", CreatorProcPtr c = nil) : extension(e), creator(c) {}
				SEntry(const SEntry& rhs);
				
		bool 	operator==(const SEntry& rhs) const		{return extension == rhs.extension;}
	};
	
	typedef std::vector<SEntry> ExtTable;			// there shouldn't be more than a handful of extensions so a vector will work at least as well as a map

//-----------------------------------
//	Member Data
//
protected:
	int32					mWidth;
	int32					mHeight;
	int32					mDepth;
	XColorTable*			mColors;
	
	static ExtTable			msExtensionTable;
	static XCriticalSection	msExtensionMutex;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

