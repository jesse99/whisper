/*
 *  File:       CPngWriter.cpp
 *  Summary:   	Writes a HyperMandella document out as a PNG file.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPngWriter.cpp,v $
 *		Revision 1.2  2000/12/31 04:18:43  jesjones
 *		Fixed a comment.
 *		
 *		Revision 1.1  2000/12/28 02:50:28  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:56:55  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 9/06/00	JDJ		Created
 */

#include "AppHeader.h"
#include <IDocWriter.h>

#include <IHierarchy.h>
#include <XExceptions.h>
#include <XImageExporters.h>
#include <XPixMap.h>

#if MAC
	#include <MPicture.h>
#endif

#include "IDocInfo.h"
#include "IRenderer.h"


// ===================================================================================
//	class CPngWriter
// ===================================================================================
class CPngWriter : public IDocWriter {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CPngWriter();
	
  						CPngWriter(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
public:
	virtual XFileType 	GetType() const;
			
	virtual bool 		CanWrite() const;

	virtual void 		Write(OSType type, const XFileSpec& spec) const;
	
#if MAC
	virtual OSType 		GetPreviewType() const;
	
	virtual XHandle 	GetPreviewData() const;
#endif

//-----------------------------------
//	Internal API
//
private:
			IConstRendererPtr DoGetRenderer() const;
			
			IConstDocWriterPtr DoGetInfoWriter() const;
};

DEFINE_INTERFACE_FACTORY(CPngWriter)

//---------------------------------------------------------------
//
// CPngWriter::~CPngWriter
//
//---------------------------------------------------------------
CPngWriter::~CPngWriter()
{
}


//---------------------------------------------------------------
//
// CPngWriter::CPngWriter
//
//---------------------------------------------------------------
CPngWriter::CPngWriter(XBoss* boss)
{
	this->DoSetBoss(boss);
}
	

//---------------------------------------------------------------
//
// CPngWriter::GetType
//
//---------------------------------------------------------------
XFileType CPngWriter::GetType() const
{
	return XFileType(L"PNG", L"png", 'HypM', 'PNGf');
}

			
//---------------------------------------------------------------
//
// CPngWriter::CanWrite
//
//---------------------------------------------------------------
bool CPngWriter::CanWrite() const
{
	IConstRendererPtr renderer = this->DoGetRenderer();
	const XPixMap* pixels = renderer->GetImage();
	
	bool can = pixels->GetDepth() != 16;
	
	return can;
}


//---------------------------------------------------------------
//
// CPngWriter::Write
//
//---------------------------------------------------------------
void CPngWriter::Write(OSType type, const XFileSpec& spec) const
{
	UNUSED(type);
		
	// Write the image
	IConstRendererPtr renderer = this->DoGetRenderer();
	const XPixMap* pixels = renderer->GetImage();
	
	XPNGExporter exporter;
	exporter.Export(spec, pixels, 'HypM', 'PNGf');	
	
	// Write the fractal info
	try {
		IConstDocWriterPtr writer = this->DoGetInfoWriter();
		writer->Write(type, spec);
	
	} catch (...) {
		// eat exceptions
	}
}

	
//---------------------------------------------------------------
//
// CPngWriter::GetPreviewType
//
//---------------------------------------------------------------
#if MAC
OSType CPngWriter::GetPreviewType() const
{
	return 'PICT';
}
#endif

	
//---------------------------------------------------------------
//
// CPngWriter::GetPreviewData
//
//---------------------------------------------------------------
#if MAC
XHandle CPngWriter::GetPreviewData() const
{
	IConstDocInfoPtr info(this);
	XPixMapPtr image = info->CreateThumbnail(kMaxThumbnailSize);
		
	MPicture pict(*image);
	
	Handle hand = (Handle) (PicHandle) pict;
	OSErr err = HandToHand(&hand);
	ThrowIfOSErr(err);
	
	return hand;
}
#endif

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CPngWriter::DoGetRenderer
//
//---------------------------------------------------------------
IConstRendererPtr CPngWriter::DoGetRenderer() const
{
	IConstHierarchyPtr node(this);	// $$ This is a bit cheesy...

	XCallback1<bool, IHierarchyPtr> predicate(MatchInterface<IRenderer>());
	IConstRendererPtr renderer = node->FindChild(predicate);
	
	return renderer;
}


//---------------------------------------------------------------
//
// CPngWriter::DoGetInfoWriter
//
//---------------------------------------------------------------
IConstDocWriterPtr CPngWriter::DoGetInfoWriter() const
{
	IConstDocWriterPtr writer;
	
	const XBoss* boss = this->GetBoss();
	XBoss::const_iterator iter = boss->begin();
	while (iter != boss->end() && !writer) {
		IConstDocWriterPtr candidate(boss, iter);
		if (candidate && candidate->GetType().GetType() == 'TEXT')
			writer = candidate;
		++iter;
	}
	
	return writer;
}


