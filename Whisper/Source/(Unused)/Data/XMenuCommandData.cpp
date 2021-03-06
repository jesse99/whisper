/*
 *  File:       XMenuCommandData.cpp
 *  Summary:   	Data interface used to initialize things like command objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMenuCommandData.cpp,v $
 *		Revision 1.2  2000/11/09 12:42:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/29/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IMenuCommandData.h>

#include <IMenuCommandData.h>
#include <XInterfaceMacros.h>

namespace Whisper {


// ===================================================================================
//	class XMenuCommandData
// ===================================================================================
class XMenuCommandData : public IMenuCommandData {

	typedef IMenuCommandData Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XMenuCommandData();
	
						XMenuCommandData();

//-----------------------------------
//	Inherited API
//
public:
	virtual MenuCommand Get() const;
							
	virtual void 		Set(const MenuCommand& command);
		
//-----------------------------------
//	Member Data
//
private:
	MenuCommand	mCommand;
};

DEFINE_INTERFACE_FACTORY(XMenuCommandData)

//---------------------------------------------------------------
//
// XMenuCommandData::~XMenuCommandData
//
//---------------------------------------------------------------
XMenuCommandData::~XMenuCommandData()
{
}


//---------------------------------------------------------------
//
// XMenuCommandData::XMenuCommandData
//
//---------------------------------------------------------------
XMenuCommandData::XMenuCommandData()					
{
}


//---------------------------------------------------------------
//
// XMenuCommandData::Get
//
//---------------------------------------------------------------
MenuCommand XMenuCommandData::Get() const
{
	return mCommand;
}

							
//---------------------------------------------------------------
//
// XMenuCommandData::Set
//
//---------------------------------------------------------------
void XMenuCommandData::Set(const MenuCommand& command)
{
	mCommand = command;
}


}	// namespace Whisper
