/*
 *  File:       XInputTargetData.cpp
 *  Summary:   	Data interface used to initialize things like command objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInputTargetData.cpp,v $
 *		Revision 1.2  2000/11/09 12:39:01  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/29/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IInputTargetData.h>

#include <IInputTargetData.h>
#include <XInterfaceMacros.h>

namespace Whisper {


// ===================================================================================
//	class XInputTargetData
// ===================================================================================
class XInputTargetData : public IInputTargetData {

	typedef IInputTargetData Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XInputTargetData();
	
						XInputTargetData();

//-----------------------------------
//	Inherited API
//
public:
	virtual IInputTargetPtr Get() const;
							
	virtual void 		Set(const IInputTargetPtr& target);
		
//-----------------------------------
//	Member Data
//
private:
	IInputTargetPtr	mTarget;
};

DEFINE_INTERFACE_FACTORY(XInputTargetData)

//---------------------------------------------------------------
//
// XInputTargetData::~XInputTargetData
//
//---------------------------------------------------------------
XInputTargetData::~XInputTargetData()
{
}


//---------------------------------------------------------------
//
// XInputTargetData::XInputTargetData
//
//---------------------------------------------------------------
XInputTargetData::XInputTargetData()					
{
}


//---------------------------------------------------------------
//
// XInputTargetData::Get
//
//---------------------------------------------------------------
IInputTargetPtr XInputTargetData::Get() const
{
	return mTarget;
}

							
//---------------------------------------------------------------
//
// XInputTargetData::Set
//
//---------------------------------------------------------------
void XInputTargetData::Set(const IInputTargetPtr& target)
{
	mTarget = target;
}


}	// namespace Whisper
