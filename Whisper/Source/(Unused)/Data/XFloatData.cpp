/*
 *  File:       XFloatData.cpp
 *  Summary:   	Data interface used to initialize things like command objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFloatData.cpp,v $
 *		Revision 1.2  2000/11/09 12:36:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/29/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IFloatData.h>

#include <XInterfaceMacros.h>

namespace Whisper {


// ===================================================================================
//	class XFloatData
// ===================================================================================
class XFloatData : public IFloatData {

	typedef IFloatData Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XFloatData();
	
						XFloatData();

//-----------------------------------
//	Inherited API
//
public:
	virtual double 		Get() const;
							
	virtual void 		Set(double value);
		
//-----------------------------------
//	Member Data
//
private:
	double	mValue;
};

DEFINE_INTERFACE_FACTORY(XFloatData)

//---------------------------------------------------------------
//
// XFloatData::~XFloatData
//
//---------------------------------------------------------------
XFloatData::~XFloatData()
{
}


//---------------------------------------------------------------
//
// XFloatData::XFloatData
//
//---------------------------------------------------------------
XFloatData::XFloatData()					
{
}


//---------------------------------------------------------------
//
// XFloatData::Get
//
//---------------------------------------------------------------
double XFloatData::Get() const
{
	return mValue;
}

							
//---------------------------------------------------------------
//
// XFloatData::Set
//
//---------------------------------------------------------------
void XFloatData::Set(double value)
{
	mValue = value;
}


}	// namespace Whisper
