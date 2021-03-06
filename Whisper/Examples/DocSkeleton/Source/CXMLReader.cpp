/*
 *  File:       CXMLReader.cpp
 *  Summary:   	Reads the document using an XML file.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CXMLReader.cpp,v $
 *		Revision 1.1  2000/12/31 04:24:00  jesjones
 *		Checked in
 *		
 */

#include "AppHeader.h"
#include <IDocReader.h>

#include <IReadXML.h>
#include <XAutoPtr.h>
#include <XResource.h>
#include <XXMLCallbacks.h>
#include <XXMLParser.h>

using namespace Whisper;


// ===================================================================================
//	class CXMLReader
//!		Reads the document using an XML file.
// ===================================================================================
class CXMLReader : public IDocReader {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CXMLReader();
	
  						CXMLReader(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		CanRead(OSType type, const std::wstring& extension) const;
			
	virtual void 		Read(const XFileSpec& spec);	
};

DEFINE_INTERFACE_FACTORY(CXMLReader)

//---------------------------------------------------------------
//
// CXMLReader::~CXMLReader
//
//---------------------------------------------------------------
CXMLReader::~CXMLReader()
{
}


//---------------------------------------------------------------
//
// CXMLReader::CXMLReader
//
//---------------------------------------------------------------
CXMLReader::CXMLReader(XBoss* boss)
{
	this->DoSetBoss(boss);
}
	

//---------------------------------------------------------------
//
// CXMLReader::CanRead
//
//---------------------------------------------------------------
bool CXMLReader::CanRead(OSType type, const std::wstring& extension) const
{	
#if MAC
	bool can = type == 'TEXT' || extension == L"xml";
	return can;

#else
	UNUSED(type);
	bool can = extension == L"xml";
	return can;
#endif
}

			
//---------------------------------------------------------------
//
// CXMLReader::Read
//
//---------------------------------------------------------------
void CXMLReader::Read(const XFileSpec& spec)
{
	// Parse the file and stick it into an XXMLDoc.
	XXMLDoc doc;
	{
	XAutoPtr<XResource> data(new XResource(spec));
	
	XXMLParser parser(data.Get());
	XXMLDocumentCallback builder(doc);
		parser.Parse(builder);
	}

	// Use IReadXML to give each interface on 'this' a chance to stream
	// in using a child element of the root element.
	XXMLElement* element = doc.GetRootElement();

	IReadXMLPtr reader(L"XML Persist");
	XUnknownPtr unknown(this);
	reader->ReadBoss(unknown, *element);
}


