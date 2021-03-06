/*
 *  File:       CBinaryWriter.cpp
 *  Summary:   	Writes a the document out using a binary stream.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *		$Log: CBinaryWriter.cpp,v $
 *		Revision 1.1  2000/12/31 04:23:14  jesjones
 *		Checked in
 *		
 */

#include "AppHeader.h"
#include <IDocWriter.h>

#include <XFile.h>
#include <XFileStream.h>
#include <XStreaming.h>

#include "IDocInfo.h"


// ===================================================================================
//	class CBinaryWriter
//!		Writes a the document out using a binary stream.
// ===================================================================================
class CBinaryWriter : public IDocWriter {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CBinaryWriter();
	
  						CBinaryWriter(XBoss* boss);	

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

DEFINE_INTERFACE_FACTORY(CBinaryWriter)

//---------------------------------------------------------------
//
// CBinaryWriter::~CBinaryWriter
//
//---------------------------------------------------------------
CBinaryWriter::~CBinaryWriter()
{
}


//---------------------------------------------------------------
//
// CBinaryWriter::CBinaryWriter
//
//---------------------------------------------------------------
CBinaryWriter::CBinaryWriter(XBoss* boss)
{
	IDocWriter::DoSetBoss(boss);
}
	

//---------------------------------------------------------------
//
// CBinaryWriter::GetType
//
//---------------------------------------------------------------
XFileType CBinaryWriter::GetType() const
{
	return XFileType(L"DocSkeleton", L"dcs", 'DcSk', 'DcSD');
}

			
//---------------------------------------------------------------
//
// CBinaryWriter::CanWrite
//
//---------------------------------------------------------------
bool CBinaryWriter::CanWrite() const
{
	return true;
}


//---------------------------------------------------------------
//
// CBinaryWriter::GetPreviewType
//
//---------------------------------------------------------------
#if MAC
OSType 	CBinaryWriter::GetPreviewType() const
{
	return '????';		// we don't have a preview
}
#endif

	
//---------------------------------------------------------------
//
// CBinaryWriter::GetPreviewData
//
//---------------------------------------------------------------
#if MAC
XHandle CBinaryWriter::GetPreviewData() const
{
	return XHandle();
}
#endif


//---------------------------------------------------------------
//
// CBinaryWriter::Write
//
//---------------------------------------------------------------
void CBinaryWriter::Write(OSType type, const XFileSpec& spec) const
{
	UNUSED(type);
	
	XFile file(spec);
	file.Open('DcSk', 'DcSD', kWritePermission);
	
	{
	XOutFileStream stream(file);
		IConstDocInfoPtr doc(this);
		
		stream << doc->GetPenWidth() << doc->GetColor();
		
		uint32 count = doc->GetNumRects();
		stream << count;
		
		for (uint32 i = 0; i < count; ++i) 
			stream << doc->GetRect(i);
	}
	
	file.Trim();
	file.Close();
}
