/*
 *  File:       XRestoreDocWindowPos.cpp
 *  Summary:   	Saves and restores doc window positions in the file's meta-data.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRestoreDocWindowPos.cpp,v $
 *		Revision 1.3  2000/12/14 08:37:11  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/25 01:53:05  jesjones
 *		Restore calls HasMetaData.
 *		
 *		Revision 1.1  2000/11/20 00:56:40  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <IRestoreWindowPosition.h>

#include <IDocument.h>
#include <IDocView.h>
#include <IGeometry.h>
#include <IHierarchy.h>

namespace Whisper {


// ===================================================================================
//	class XRestoreDocWindowPos
//!		Saves and restores doc window positions in the file's meta-data.
// ===================================================================================
class XRestoreDocWindowPos : public IRestoreWindowPosition {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XRestoreDocWindowPos();
	
						XRestoreDocWindowPos(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Save();

	virtual void 		Restore();
};

DEFINE_INTERFACE_FACTORY(XRestoreDocWindowPos)

//---------------------------------------------------------------
//
// XRestoreDocWindowPos::~XRestoreDocWindowPos
//
//---------------------------------------------------------------
XRestoreDocWindowPos::~XRestoreDocWindowPos()
{
}


//---------------------------------------------------------------
//
// XRestoreDocWindowPos::XRestoreDocWindowPos
//
//---------------------------------------------------------------
XRestoreDocWindowPos::XRestoreDocWindowPos(XBoss* boss)
{	
	IRestoreWindowPosition::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XRestoreDocWindowPos::Save
//
//---------------------------------------------------------------
void XRestoreDocWindowPos::Save()
{
	IDocViewPtr view = GetInterface<IDocView>(this);
	IDocumentPtr doc = view->GetDoc();
	
	if (doc->IsOnDisk()) {
		XFileSpec spec = doc->GetFile();
		
		try {
			IGeometryPtr geometry(this);
			
			XRect frame = geometry->GetFrame();
			spec.SetMetaData(L"Window Position", &frame, sizeof(frame));	// meta-data isn't portable so padding and bytes-ordering shouldn't be much of an issue...
						
		} catch (...) {
			// eat exceptions
		}
	} 
}


//---------------------------------------------------------------
//
// XRestoreDocWindowPos::Restore
//
//---------------------------------------------------------------
void XRestoreDocWindowPos::Restore()
{	
	IDocViewPtr view = GetInterface<IDocView>(this);
	IDocumentPtr doc = view->GetDoc();
	
	if (doc->IsOnDisk()) {
		XFileSpec spec = doc->GetFile();
		
		try {
			if (spec.HasMetaData(L"Window Position")) {	// get rid of a throw if we're running on FAT
				XRect frame;
				spec.GetMetaData(L"Window Position", &frame, sizeof(frame));
				
				IGeometryPtr geometry(this);
				geometry->SetFrame(frame);
			}
						
		} catch (...) {
			// eat exceptions
		}
	} 
}


}	// namespace Whisper
