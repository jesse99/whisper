/*
 *	File:		XResource.h
 *	Function:	Abstraction for data from an arbitrary source.
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XResource.h,v $
 *		Revision 1.5  2001/04/17 01:44:46  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/03/05 05:43:12  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:55:09  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:49:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 12/23/99	JDJ		Windows ctors take an optional module handle.
 *		 <3>	  7/14/99	JDJ		Added GetHandle.
 *		 <2>	  2/06/99	JDJ		Uses an XURI instead of a name.
 *		 <1>	 12/08/98	JDJ		Created
 */

#pragma once

#include <XHandle.h>
#include <XURI.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileSpec;


// ===================================================================================
//	class XResource
//!		Abstraction for data from an arbitrary source.
// ===================================================================================
class FILES_EXPORT XResource : public XLockableMixin, public XReferenceCountedMixin {
			
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual			   	~XResource();
	
						XResource(const XFileSpec& spec);

#if MAC
						XResource(ResType type, ResID id);
						XResource(ResType type, const std::wstring& name);
#elif WIN
						XResource(const TCHAR* type, ResID id, HINSTANCE moduleH = nil);
						XResource(const TCHAR* type, const std::wstring& name, HINSTANCE moduleH = nil);
						/**< Can't use a real string for type because of Microsoft's propensity
						for type unsafe interfaces (eg constants like RT_RCDATA are invalid
						string pointers). */
#endif

						XResource(const XURI& uri, const XHandle& data);

protected:
						XResource();
						/**< For use by subclasses */
						
private:
						XResource(const XResource& rhs);
						
			XResource&	operator=(const XResource& rhs);
	
//-----------------------------------
//	New API 
//
public:
			XHandle 	 GetHandle() const						{return mData;}
			const uint8* GetPtr() const							{return mData.GetPtr();}
			const uint8* GetUnsafePtr() const					{return mData.GetUnsafePtr();}
						
			uint32 		GetSize() const							{return mData.GetSize();}

			const XURI& GetURI() const							{return mURI;}
						/**< This is usually used for error reporting, but may also be used
						to load a child or sibling resource. */
						
			bool 		IsFromFile() const						{return mURI.GetScheme() == L"file";}
						/**< This is here for lame data types like PICT and BMP that include
						special headers when they're inside files, but not when they're
						inside resources. */
	
//-----------------------------------
//	Inherited API
//
protected:
			void 		Invariant() const;
	
	virtual void 		OnLock(bool moveHigh);
						/**< Subclasses can override these to do things like demand loading. */

	virtual void 		OnUnlock();

//-----------------------------------
//	Member Data
//
protected:
	XURI		mURI;
	XHandle		mData;			//!< may be invalid if unlocked
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
