/*
 *  File:       CXMLWriter.cpp
 *  Summary:   	Writes the document out using an XML file.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CXMLWriter.cpp,v $
 *		Revision 1.2  2001/04/27 04:18:27  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.1  2000/12/31 04:24:03  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include <IDocWriter.h>

#include <IWriteXML.h>
#include <XAutoPtr.h>
#include <XFile.h>
#include <XTranscode.h>
#include <XXMLDocVisitors.h>
#include <XXMLItems.h>

#include "IDocInfo.h"


// ===================================================================================
//	class CXMLWriter
//!		Writes the document out using an XML file.
// ===================================================================================
class CXMLWriter : public IDocWriter {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CXMLWriter();
	
  						CXMLWriter(XBoss* boss);	

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

DEFINE_INTERFACE_FACTORY(CXMLWriter)

//---------------------------------------------------------------
//
// CXMLWriter::~CXMLWriter
//
//---------------------------------------------------------------
CXMLWriter::~CXMLWriter()
{
}


//---------------------------------------------------------------
//
// CXMLWriter::CXMLWriter
//
//---------------------------------------------------------------
CXMLWriter::CXMLWriter(XBoss* boss)
{
	this->DoSetBoss(boss);
}
	

//---------------------------------------------------------------
//
// CXMLWriter::GetType
//
//---------------------------------------------------------------
XFileType CXMLWriter::GetType() const
{
	return XFileType(L"XML", L"xml", 'DcSk', 'TEXT');
}

			
//---------------------------------------------------------------
//
// CXMLWriter::CanWrite
//
//---------------------------------------------------------------
bool CXMLWriter::CanWrite() const
{
	return true;
}


//---------------------------------------------------------------
//
// CXMLWriter::GetPreviewType
//
//---------------------------------------------------------------
#if MAC
OSType CXMLWriter::GetPreviewType() const
{
	return '????';		// we don't have a preview
}
#endif

	
//---------------------------------------------------------------
//
// CXMLWriter::GetPreviewData
//
//---------------------------------------------------------------
#if MAC
XHandle CXMLWriter::GetPreviewData() const
{
	return XHandle();
}
#endif


//---------------------------------------------------------------
//
// CXMLWriter::Write
//
//---------------------------------------------------------------
void CXMLWriter::Write(OSType type, const XFileSpec& spec) const
{
	// Use IWriteXML to give each interface on 'this' a chance to 
	// append a new element to the XXMLDoc.
	IWriteXMLPtr writer(L"XML Persist");
	XAutoPtr<XXMLElement> element(writer->CreateElement(this->GetBoss(), L"Doc"));	
	
	// Convert the XML element into a string.
	std::wstring text;
	text += L"<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n";
	text += L"<!DOCTYPE Doc SYSTEM \"resource://Doc.dtd\">\n";

	XXMLPrintDoc printer(text);
	printer.HandleVisit(*element);
	
	// Write the string to disk. 
	UTF8String str = ToUTF8Str(text);
	
	XFile file(spec);
	file.Open('DcSk', type, kWritePermission);
	file.Write(str.c_str(), str.length());
	file.Trim();
	file.Close();
}


