/*
 *  File:       CBinaryReader.cpp
 *  Summary:   	Reads the document in using a binary stream.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CBinaryReader.cpp,v $
 *		Revision 1.1  2000/12/31 04:23:10  jesjones
 *		Checked in
 *		
 */

#include "AppHeader.h"
#include <IDocReader.h>

#include <XFile.h>
#include <XFileStream.h>
#include <XStreaming.h>

#include "IDocInfo.h"


// ===================================================================================
//	class CBinaryReader
//!		Reads a the document in using a binary stream.
// ===================================================================================
class CBinaryReader : public IDocReader {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CBinaryReader();
	
  						CBinaryReader(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		CanRead(OSType type, const std::wstring& extension) const;
			
	virtual void 		Read(const XFileSpec& spec);	
};

DEFINE_INTERFACE_FACTORY(CBinaryReader)

//---------------------------------------------------------------
//
// CBinaryReader::~CBinaryReader
//
//---------------------------------------------------------------
CBinaryReader::~CBinaryReader()
{
}


//---------------------------------------------------------------
//
// CBinaryReader::CBinaryReader
//
//---------------------------------------------------------------
CBinaryReader::CBinaryReader(XBoss* boss)
{
	this->DoSetBoss(boss);
}
	

//---------------------------------------------------------------
//
// CBinaryReader::CanRead
//
//---------------------------------------------------------------
bool CBinaryReader::CanRead(OSType type, const std::wstring& extension) const
{
#if MAC
	UNUSED(extension);
	bool can = type == 'DcSD';
	return can;

#else
	UNUSED(type);
	bool can = extension == L"dcs";
	return can;
#endif
}

			
//---------------------------------------------------------------
//
// CBinaryReader::Read
//
//---------------------------------------------------------------
void CBinaryReader::Read(const XFileSpec& spec)
{
	XFile file(spec);
	file.Open(kReadPermission);
	
	{
	XInFileStream stream(file);
		IDocInfoPtr doc(this);
		
		int32 width;
		stream >> width;
		doc->SetPenWidth(width, false);		// no need to dirty the doc here (XDocument will handle setting the dirtyFlag and broadcasting change messages)
		
		XRGBColor color;
		stream >> color;
		doc->SetColor(color, false);
		
		uint32 count;
		stream >> count;
		
		doc->RemoveRects(false);
		for (uint32 i = 0; i < count; ++i) {
			XRect rect;
			stream >> rect;
			doc->AddRect(rect, false);
		}
	}
	
	file.Close();
}








