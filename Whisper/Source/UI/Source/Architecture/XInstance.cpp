/*
 *  File:       XInstance.cpp
 *  Summary:   	IInstance implementation.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInstance.cpp,v $
 *		Revision 1.1  2000/12/22 09:31:33  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <IInstance.h>

namespace Whisper {


// ===================================================================================
//	class XInstance
//!		IInstance implementation.
// ===================================================================================
class XInstance : public IInstance {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XInstance();
	
						XInstance(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual XUnknownPtr Get() const;

	virtual void 		Set(const XUnknownPtr& instance);

//-----------------------------------
//	Member Data
//
private:
	XUnknownPtr	mPtr;
};

DEFINE_INTERFACE_FACTORY(XInstance)

//---------------------------------------------------------------
//
// XInstance::~XInstance
//
//---------------------------------------------------------------
XInstance::~XInstance()
{
}


//---------------------------------------------------------------
//
// XInstance::XInstance
//
//---------------------------------------------------------------
XInstance::XInstance(XBoss* boss) 
{	
	IInstance::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XInstance::Get
//
//---------------------------------------------------------------
XUnknownPtr XInstance::Get() const
{
	return mPtr;
}


//---------------------------------------------------------------
//
// XInstance::Set
//
//---------------------------------------------------------------
void XInstance::Set(const XUnknownPtr& instance)
{
	mPtr = instance;
}


}	// namespace Whisper
