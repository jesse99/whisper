/*
 *  File:       XInterfaceListData.cpp
 *  Summary:   	Data interface that contains a list of interface pointers.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInterfaceListData.cpp,v $
 *		Revision 1.3  2001/04/27 04:33:38  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:39:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IInterfaceListData.h>

#include <XInterfaceMacros.h>

namespace Whisper {


// ===================================================================================
//	class XInterfaceListData
// ===================================================================================
class XInterfaceListData : public IInterfaceListData {

	typedef IInterfaceListData Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XInterfaceListData();
	
						XInterfaceListData();

//-----------------------------------
//	Inherited API
//
public:
	virtual uint32 		GetCount() const;
	
	virtual XUnknownPtr& Get(uint32 index);
	virtual const XUnknownPtr& Get(uint32 index) const;
	
	virtual void 		Append(const XUnknownPtr& ptr);
	virtual void 		Remove(uint32 index);
		
//-----------------------------------
//	Member Data
//
private:
	std::vector<XUnknownPtr>	mPtrs;
};

DEFINE_INTERFACE_FACTORY(XInterfaceListData)

//---------------------------------------------------------------
//
// XInterfaceListData::~XInterfaceListData
//
//---------------------------------------------------------------
XInterfaceListData::~XInterfaceListData()
{
}


//---------------------------------------------------------------
//
// XInterfaceListData::XInterfaceListData
//
//---------------------------------------------------------------
XInterfaceListData::XInterfaceListData()					
{
}


//---------------------------------------------------------------
//
// XInterfaceListData::GetCount
//
//---------------------------------------------------------------
uint32 XInterfaceListData::GetCount() const
{
	uint32 count = mPtrs.size();
	
	return count;
}

	
//---------------------------------------------------------------
//
// XInterfaceListData::Get
//
//---------------------------------------------------------------
XUnknownPtr& XInterfaceListData::Get(uint32 index)
{
	PRECONDITION(index < mPtrs.size());
	
	XUnknownPtr& ptr = mPtrs[index];
	
	return ptr;
}


//---------------------------------------------------------------
//
// XInterfaceListData::Get const
//
//---------------------------------------------------------------
const XUnknownPtr& XInterfaceListData::Get(uint32 index) const
{
	PRECONDITION(index < mPtrs.size());
	
	const XUnknownPtr& ptr = mPtrs[index];
	
	return ptr;
}

	
//---------------------------------------------------------------
//
// XInterfaceListData::Append
//
//---------------------------------------------------------------
void XInterfaceListData::Append(const XUnknownPtr& ptr)
{
	mPtrs.push_back(ptr);
}


//---------------------------------------------------------------
//
// XInterfaceListData::Remove
//
//---------------------------------------------------------------
void XInterfaceListData::Remove(uint32 index)
{
	PRECONDITION(index < mPtrs.size());
	
	mPtrs.erase(mPtrs.begin() + index);
}


}	// namespace Whisper
