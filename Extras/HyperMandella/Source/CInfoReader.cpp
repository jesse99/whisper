/*
 *  File:       CInfoReader.cpp
 *  Summary:   	Attempts to import an XML file as a HyperMandella document.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CInfoReader.cpp,v $
 *		Revision 1.2  2000/12/31 04:18:00  jesjones
 *		Accepts .xml files even if the type isn't 'TEXT'.
 *		
 *		Revision 1.1  2000/12/28 02:49:49  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.3  2000/11/25 01:43:36  jesjones
 *		Don't call XFileSpec::GetType on Windows.
 *		
 *		Revision 1.2  2000/11/09 08:55:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 9/13/00	JDJ		Created
 */

#include "AppHeader.h"
#include <IDocReader.h>

#include <IReadXML.h>
#include <XAutoPtr.h>
#include <XFileSpec.h>
#include <XResource.h>
#include <XXMLCallbacks.h>
#include <XXMLParser.h>

using namespace Whisper;


// ===================================================================================
//	class CInfoReader
// ===================================================================================
class CInfoReader : public IDocReader {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CInfoReader();
	
  						CInfoReader(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		CanRead(OSType type, const std::wstring& extension) const;
			
	virtual void 		Read(const XFileSpec& spec);	
};

DEFINE_INTERFACE_FACTORY(CInfoReader)

//---------------------------------------------------------------
//
// CInfoReader::~CInfoReader
//
//---------------------------------------------------------------
CInfoReader::~CInfoReader()
{
}


//---------------------------------------------------------------
//
// CInfoReader::CInfoReader
//
//---------------------------------------------------------------
CInfoReader::CInfoReader(XBoss* boss)
{
	this->DoSetBoss(boss);
}
	

//---------------------------------------------------------------
//
// CInfoReader::CanRead
//
//---------------------------------------------------------------
bool CInfoReader::CanRead(OSType type, const std::wstring& extension) const
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
// CInfoReader::Read
//
//---------------------------------------------------------------
void CInfoReader::Read(const XFileSpec& spec)
{
	XXMLDoc doc;
	{
	XAutoPtr<XResource> data;
#if MAC
	if (spec.GetType() == 'TEXT' || spec.GetExtension() == L"xml")
		data.Reset(new XResource(spec));
#else
	if (spec.GetExtension() == L"xml")
		data.Reset(new XResource(spec));
#endif
	else
		data.Reset(new XResource(spec, spec.GetMetaData(L"Fractal Info")));
	
	XXMLParser parser(data.Get());
	XXMLDocumentCallback builder(doc);
		parser.Parse(builder);
	}

	XXMLElement* element = doc.GetRootElement();

	IReadXMLPtr reader(L"XML Persist");
	XUnknownPtr unknown(this);
	reader->ReadBoss(unknown, *element);
}


