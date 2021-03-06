/*
 *  File:       XScrap.cpp
 *  Summary:   	Default IScrap implementation.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XScrap.cpp,v $
 *		Revision 1.5  2001/04/27 04:36:06  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/04/13 08:03:34  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2001/01/05 06:15:02  jesjones
 *		Refactored XScrap so that it uses an IScrapHelper interface.
 *		
 *		Revision 1.2  2000/12/25 00:38:52  jesjones
 *		Fixed some Windows compile problems.
 *		
 *		Revision 1.1  2000/12/22 09:31:39  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IScrap.h>

#include <IScrapHelper.h>
#include <XExceptions.h>
#include <XLocker.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	class XScrap
//!		Default IScrap implementation.
// ===================================================================================
class XScrap : public IScrap {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XScrap();
	
						XScrap(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Cut();
	virtual void 		Copy();
	virtual void 		Paste();
	virtual void 		Clear();

	virtual bool 		CanCut() const;
	virtual bool 		CanCopy() const;	
	virtual bool 		CanPaste() const;
	virtual bool 		CanClear() const;
	
	virtual bool 		HasSelection() const;
};

DEFINE_INTERFACE_FACTORY(XScrap)

//---------------------------------------------------------------
//
// XScrap::~XScrap
//
//---------------------------------------------------------------
XScrap::~XScrap()
{
}


//---------------------------------------------------------------
//
// XScrap::XScrap
//
//---------------------------------------------------------------
XScrap::XScrap(XBoss* boss) 
{	
	IScrap::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XScrap::Cut
//
//---------------------------------------------------------------
void XScrap::Cut()
{
	PRECONDITION(this->CanCut());
	
	this->Copy();
	this->Clear();
}


//---------------------------------------------------------------
//
// XScrap::Copy
//
// $$$ Some of the standard Windows clipboard types are not
// $$$ allocated with GlobalAlloc...
//
//---------------------------------------------------------------
void XScrap::Copy()
{
	PRECONDITION(this->CanCopy());

	IScrapHelperPtr helper(this);
	uint32 count = helper->GetCopyCount();

#if MAC
	OSStatus err = ::ClearCurrentScrap();
	ThrowIfOSErr(err);
	
	ScrapRef scrap = nil;
	err = ::GetCurrentScrap(&scrap);
	ThrowIfOSErr(err);
	
	for (uint32 index = 0; index < count; index++) {
		uint32 type = helper->GetCopyType(index);		
		XHandle data = helper->Copy(type);
			
		if (data.GetSize() > 0) {
			XLocker lock(data);
			
			err = ::PutScrapFlavor(scrap, type, kScrapFlavorMaskNone, (int32) data.GetSize(), data.GetPtr());
			ThrowIfOSErr(err);
		}
	}		

#elif WIN
	int32 succeeded = ::OpenClipboard(nil);
	ThrowIf(!succeeded);
	
	try {
		succeeded = ::EmptyClipboard();
		ThrowIf(!succeeded);
		
		for (uint32 index = 0; index < count; index++) {
			uint32 type = helper->GetCopyType(index);
			XHandle data = helper->Copy(type);
				
			if (data.GetSize() > 0) {
				HANDLE dataH = ::GlobalAlloc(GMEM_MOVEABLE + GMEM_DDESHARE, data.GetSize());
				ThrowIfBadHandle(dataH);
					
				{
				XLocker lock(data);
					BlockMoveData(data.GetPtr(), ::GlobalLock(dataH), data.GetSize());
				}
				
				HANDLE result = ::SetClipboardData(type, dataH);
				ThrowIfBadHandle(result);
			}
		}		
	
	} catch (...) {
		(void) ::CloseClipboard();
		throw;
	}

	succeeded = ::CloseClipboard();
	ThrowIf(!succeeded);
#endif
}


//---------------------------------------------------------------
//
// XScrap::Paste
//
//---------------------------------------------------------------
void XScrap::Paste()
{
	PRECONDITION(this->CanPaste());

	IScrapHelperPtr helper(this);
	uint32 count = helper->GetPasteCount();
	ASSERT(count > 0);
	
	bool pasted = false;

#if MAC
	ScrapRef scrap = nil;
	OSStatus err = ::GetCurrentScrap(&scrap);
	ThrowIfOSErr(err);
	
	for (uint32 index = 0; index < count && !pasted; index++) {
		uint32 type = helper->GetPasteType(index);
		
		int32 bytes;
		err = ::GetScrapFlavorSize(scrap, type, &bytes);
		ThrowIfOSErr(err);
		
		if (bytes > 0) {
			XHandle data(numeric_cast<uint32>(bytes));
			
			{
			XLocker lock(data);
				err = ::GetScrapFlavorData(scrap, type, &bytes, data.GetPtr());
				ThrowIfOSErr(err);
			}
			
			if (bytes > data.GetSize()) {	
				data.SetSize(numeric_cast<uint32>(bytes));
				XLocker lock(data);
				
				err = ::GetScrapFlavorData(scrap, type, &bytes, data.GetPtr());
				ThrowIfOSErr(err);
				
				ASSERT(bytes == data.GetSize());
			}
			
			helper->Paste(type, data);
			pasted = true;
		}
	}		

#elif WIN
	int32 succeeded = ::OpenClipboard(nil);
	ThrowIf(!succeeded);
	
	try {
		for (uint32 index = 0; index < count && !pasted; index++) {
			uint32 type = helper->GetPasteType(index);
			
			HANDLE dataH = ::GetClipboardData(type);
			if (dataH != nil) {
				XHandle data(::GlobalSize(dataH));
				{
				XLocker lock(data);
					BlockMoveData(::GlobalLock(dataH), data.GetPtr(), data.GetSize());
				}
													
				helper->Paste(type, data);
				pasted = true;
			}
		}		
	
	} catch (...) {
		(void) ::CloseClipboard();
		throw;
	}

	succeeded = ::CloseClipboard();
	ThrowIf(!succeeded);
#endif
}


//---------------------------------------------------------------
//
// XScrap::Clear
//
//---------------------------------------------------------------
void XScrap::Clear()
{
	PRECONDITION(this->CanClear());

	IScrapHelperPtr helper(this);
	helper->Clear();
}


//---------------------------------------------------------------
//
// XScrap::CanCut
//
//---------------------------------------------------------------
bool XScrap::CanCut() const
{
	bool can = this->CanCopy() && this->CanClear();
	
	return can;
}


//---------------------------------------------------------------
//
// XScrap::CanCopy
//
//---------------------------------------------------------------
bool XScrap::CanCopy() const
{
	IConstScrapHelperPtr helper(this);

	bool can = helper->GetCopyCount() > 0 && this->HasSelection();
	
	return can;
}


//---------------------------------------------------------------
//
// XScrap::CanPaste
//
//---------------------------------------------------------------
bool XScrap::CanPaste() const
{
	bool canPaste = false;
	
	IConstScrapHelperPtr helper(this);
	uint32 count = helper->GetPasteCount();

#if MAC
	ScrapRef scrap = nil;
	OSStatus err = ::GetCurrentScrap(&scrap);
	ASSERT(err == noErr);

	for (uint32 index = 0; index < count && !canPaste && scrap != nil; index++) {
		uint32 type = helper->GetPasteType(index);
		
		ScrapFlavorFlags flags;
		err = ::GetScrapFlavorFlags(scrap, type, &flags);
		canPaste = err == noErr;
	}		

#elif WIN
	for (uint32 index = 0; index < count && !canPaste; index++) {
		uint32 type = helper->GetPasteType(index);

		canPaste = ::IsClipboardFormatAvailable(type) != 0;
	}		
#endif
		
	return canPaste;
}


//---------------------------------------------------------------
//
// XScrap::CanClear
//
//---------------------------------------------------------------
bool XScrap::CanClear() const
{
	bool can = this->HasSelection();
	
	return can;
}

	
//---------------------------------------------------------------
//
// XScrap::HasSelection
//
//---------------------------------------------------------------
bool XScrap::HasSelection() const
{
	IConstScrapHelperPtr helper(this);

	bool has = helper->HasSelection();
	
	return has;
}


}	// namespace Whisper
