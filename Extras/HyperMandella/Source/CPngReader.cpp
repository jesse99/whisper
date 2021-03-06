/*
 *  File:       CPngReader.cpp
 *  Summary:   	Attempts to import a PNG file as a HyperMandella document.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPngReader.cpp,v $
 *		Revision 1.1  2000/12/28 02:50:24  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:56:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 9/06/00	JDJ		Created
 */

#include "AppHeader.h"
#include <IDocReader.h>

using namespace Whisper;


// ===================================================================================
//	class CPngReader
// ===================================================================================
class CPngReader : public IDocReader {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CPngReader();
	
  						CPngReader(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		CanRead(OSType type, const std::wstring& extension) const;
			
	virtual void 		Read(const XFileSpec& spec);	

//-----------------------------------
//	Internal API
//
private:
			IDocReaderPtr DoGetInfoReader();
};

DEFINE_INTERFACE_FACTORY(CPngReader)

//---------------------------------------------------------------
//
// CPngReader::~CPngReader
//
//---------------------------------------------------------------
CPngReader::~CPngReader()
{
}


//---------------------------------------------------------------
//
// CPngReader::CPngReader
//
//---------------------------------------------------------------
CPngReader::CPngReader(XBoss* boss)
{
	this->DoSetBoss(boss);
}
	

//---------------------------------------------------------------
//
// CPngReader::CanRead
//
//---------------------------------------------------------------
bool CPngReader::CanRead(OSType type, const std::wstring& extension) const
{
#if MAC
	UNUSED(extension);
	bool can = type == 'PNGf';
	return can;

#else
	UNUSED(type);
	bool can = extension == L"png";
	return can;
#endif
}

			
//---------------------------------------------------------------
//
// CPngReader::Read
//
//---------------------------------------------------------------
void CPngReader::Read(const XFileSpec& spec)
{
	IDocReaderPtr reader = this->DoGetInfoReader();
	reader->Read(spec);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CPngReader::DoGetInfoReader
//
//---------------------------------------------------------------
IDocReaderPtr CPngReader::DoGetInfoReader()
{
	IDocReaderPtr reader;
	
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !reader) {
		IDocReaderPtr candidate(boss, iter);
		if (candidate && candidate->CanRead('TEXT', L"xml"))
			reader = candidate;
		++iter;
	}
	
	return reader;
}







