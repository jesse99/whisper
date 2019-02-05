/*
 *  File:       XCallbackData.cpp
 *  Summary:   	Data interface used to initialize things like command objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCallbackData.cpp,v $
 *		Revision 1.2  2000/11/09 12:27:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/29/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <ICallbackData.h>

#include <ICallbackData.h>
#include <XInterfaceMacros.h>

namespace Whisper {


// ===================================================================================
//	class XCallbackData
// ===================================================================================
class XCallbackData : public ICallbackData {

	typedef ICallbackData Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XCallbackData();
	
						XCallbackData();

//-----------------------------------
//	Inherited API
//
public:
	virtual XCallback0<void> Get() const;
							
	virtual void 		Set(const XCallback0<void>& callback);
		
//-----------------------------------
//	Member Data
//
private:
	XCallback0<void>	mCallback;
};

DEFINE_INTERFACE_FACTORY(XCallbackData)

//---------------------------------------------------------------
//
// XCallbackData::~XCallbackData
//
//---------------------------------------------------------------
XCallbackData::~XCallbackData()
{
}


//---------------------------------------------------------------
//
// XCallbackData::XCallbackData
//
//---------------------------------------------------------------
XCallbackData::XCallbackData()					
{
}


//---------------------------------------------------------------
//
// XCallbackData::Get
//
//---------------------------------------------------------------
XCallback0<void> XCallbackData::Get() const
{
	return mCallback;
}

							
//---------------------------------------------------------------
//
// XCallbackData::Set
//
//---------------------------------------------------------------
void XCallbackData::Set(const XCallback0<void>& callback)
{
	mCallback = callback;
}


}	// namespace Whisper
