/*
 *  File:       XIntData.cpp
 *  Summary:   	Data interface used to initialize things like command objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XIntData.cpp,v $
 *		Revision 1.2  2000/11/09 12:39:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/29/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IIntData.h>

#include <XInterfaceMacros.h>

namespace Whisper {


// ===================================================================================
//	class XIntData
// ===================================================================================
class XIntData : public IIntData {

	typedef IIntData Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XIntData();
	
						XIntData();

//-----------------------------------
//	Inherited API
//
public:
	virtual int32 		Get() const;
							
	virtual void 		Set(int32 value);
		
//-----------------------------------
//	Member Data
//
private:
	int32	mValue;
};

DEFINE_INTERFACE_FACTORY(XIntData)

//---------------------------------------------------------------
//
// XIntData::~XIntData
//
//---------------------------------------------------------------
XIntData::~XIntData()
{
}


//---------------------------------------------------------------
//
// XIntData::XIntData
//
//---------------------------------------------------------------
XIntData::XIntData()					
{
}


//---------------------------------------------------------------
//
// XIntData::Get
//
//---------------------------------------------------------------
int32 XIntData::Get() const
{
	return mValue;
}

							
//---------------------------------------------------------------
//
// XIntData::Set
//
//---------------------------------------------------------------
void XIntData::Set(int32 value)
{
	mValue = value;
}


}	// namespace Whisper
