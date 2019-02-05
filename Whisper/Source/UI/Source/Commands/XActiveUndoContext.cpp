/*
 *  File:       XActiveUndoContext.cpp
 *  Summary:   	The undo context we're currently using.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XActiveUndoContext.cpp,v $
 *		Revision 1.3  2000/12/14 08:32:37  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:24:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XActiveUndoContext.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/14 08:32:37  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 5/06/00	JDJ		Created 
 */

#include <XWhisperHeader.h>
#include <IActiveUndoContext.h>

#include <XInterfaceMacros.h>

namespace Whisper {


// ===================================================================================
//	class XActiveUndoContext
//!		The undo context we're currently using.
// ===================================================================================
class XActiveUndoContext : public IActiveUndoContext {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XActiveUndoContext();
	
						XActiveUndoContext(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
	virtual	IUndoContextPtr Get();
			
	virtual void		Set(const IUndoContextPtr& newContext);
		
//-----------------------------------
//	Member Data
//
private:
	IUndoContextPtr	mContext;
};

DEFINE_INTERFACE_FACTORY(XActiveUndoContext)

//---------------------------------------------------------------
//
// XActiveUndoContext::~XActiveUndoContext
//
//---------------------------------------------------------------
XActiveUndoContext::~XActiveUndoContext()
{
}


//---------------------------------------------------------------
//
// XActiveUndoContext::XActiveUndoContext
//
//---------------------------------------------------------------
XActiveUndoContext::XActiveUndoContext(XBoss* boss)
{
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XActiveUndoContext::Get
//
//---------------------------------------------------------------
IUndoContextPtr XActiveUndoContext::Get()						
{
	return mContext;
}


//---------------------------------------------------------------
//
// XActiveUndoContext::Set
//
//---------------------------------------------------------------
void XActiveUndoContext::Set(const IUndoContextPtr& newContext)	
{
	mContext = newContext;
}


}	// namespace Whisper
