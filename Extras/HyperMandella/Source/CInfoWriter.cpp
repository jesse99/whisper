/*
 *  File:       CInfoWriter.cpp
 *  Summary:   	Exports a fractal as an XML file.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CInfoWriter.cpp,v $
 *		Revision 1.2  2001/04/27 04:18:02  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.1  2000/12/28 02:49:53  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.3  2000/11/25 01:43:02  jesjones
 *		Trims the file after writing it.
 *		
 *		Revision 1.2  2000/11/09 08:55:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include <IDocWriter.h>

#include <IWriteXML.h>
#include <XAutoPtr.h>
#include <XExceptions.h>
#include <XFile.h>
#include <XTranscode.h>
#include <XXMLDocVisitors.h>
#include <XXMLItems.h>

#if MAC
	#include <MPicture.h>
#endif

#include "IDocInfo.h"
#include "IFractalFunction.h"


// ===================================================================================
//	class CInfoWriter
// ===================================================================================
class CInfoWriter : public IDocWriter {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CInfoWriter();
	
  						CInfoWriter(XBoss* boss);	

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
};

DEFINE_INTERFACE_FACTORY(CInfoWriter)

//---------------------------------------------------------------
//
// CInfoWriter::~CInfoWriter
//
//---------------------------------------------------------------
CInfoWriter::~CInfoWriter()
{
}


//---------------------------------------------------------------
//
// CInfoWriter::CInfoWriter
//
//---------------------------------------------------------------
CInfoWriter::CInfoWriter(XBoss* boss)
{
	this->DoSetBoss(boss);
}
	

//---------------------------------------------------------------
//
// CInfoWriter::GetType
//
//---------------------------------------------------------------
XFileType CInfoWriter::GetType() const
{
	return XFileType(L"XML", L"xml", 'HypM', 'TEXT');
}

			
//---------------------------------------------------------------
//
// CInfoWriter::CanWrite
//
//---------------------------------------------------------------
bool CInfoWriter::CanWrite() const
{
	return true;
}


//---------------------------------------------------------------
//
// CInfoWriter::Write
//
//---------------------------------------------------------------
void CInfoWriter::Write(OSType type, const XFileSpec& spec) const
{
	// Create an XML element representing the document
	IWriteXMLPtr writer(L"XML Persist");
	XAutoPtr<XXMLElement> element(writer->CreateElement(this->GetBoss(), L"Fractal"));	
	
	// Convert the XML element into a string
	std::wstring text;
	text += L"<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n";
	text += L"<!DOCTYPE Fractal SYSTEM \"resource://Fractal.dtd\">\n";

	XXMLPrintDoc printer(text);
	printer.HandleVisit(*element);
	
	// Write the string to disk. Note that we'll use meta-data if
	// we're writing to something like a PNG file.
	UTF8String str = ToUTF8Str(text);
	
	if (type == 'TEXT' || spec.GetExtension() == L"xml") {
		XFile file(spec);
		file.Open('HypM', type, kWritePermission);
		file.Write(str.c_str(), str.length());
		file.Trim();
		file.Close();
	 
	} else {
		XFileSpec temp = spec;
		temp.SetMetaData(L"Fractal Info", str.c_str(), str.length() + 1);
	}
}


//---------------------------------------------------------------
//
// CInfoWriter::GetPreviewType
//
//---------------------------------------------------------------
#if MAC
OSType CInfoWriter::GetPreviewType() const
{
//	return 'TEXT';
	return 'PICT';
}
#endif

	
//---------------------------------------------------------------
//
// CInfoWriter::GetPreviewData
//
//---------------------------------------------------------------
#if MAC
XHandle CInfoWriter::GetPreviewData() const
{
#if 1
	IConstDocInfoPtr info(this);			// PICT
	XPixMapPtr image = info->CreateThumbnail(kMaxThumbnailSize);
		
	MPicture pict(*image);
	
	Handle hand = (Handle) (PicHandle) pict;
	OSErr err = HandToHand(&hand);
	ThrowIfOSErr(err);
	
	return hand;

#else
	IConstDocInfoPtr info(this);			// TEXT
	IConstFractalFunctionPtr function = info->GetFractalFunction();
	std::wstring formula = function->GetFormula();
	
	PlatformString str = ToPlatformStr(formula + L" Fractal");
	
	XHandle data(strlen(str));
	BlockMoveData(str, data.GetUnsafePtr(), data.GetSize());
	
	return data;
#endif
}
#endif




	
	
	

