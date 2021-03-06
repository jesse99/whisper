/*
 *  File:       XErrorData.cpp
 *  Summary:   	Data interface used to initialize things like command objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XErrorData.cpp,v $
 *		Revision 1.2  2000/11/09 12:34:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/29/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IErrorData.h>

#include <XError.h>
#include <XInterfaceMacros.h>

namespace Whisper {


// ===================================================================================
//	class XErrorData
// ===================================================================================
class XErrorData : public IErrorData {

	typedef IErrorData Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XErrorData();
	
						XErrorData();

//-----------------------------------
//	Inherited API
//
public:
	virtual std::wstring GetSummary() const;
	virtual std::wstring GetNarrative() const;
							
	virtual void 		SetSummary(const std::wstring& text);
	virtual void 		SetNarrative(const std::wstring& text);
	virtual void 		SetNarrative(const std::exception* e);
	virtual void 		SetNarrative(OSError err);
		
//-----------------------------------
//	Member Data
//
private:
	std::wstring	mSummary;
	std::wstring	mNarrative;
};

DEFINE_INTERFACE_FACTORY(XErrorData)

//---------------------------------------------------------------
//
// XErrorData::~XErrorData
//
//---------------------------------------------------------------
XErrorData::~XErrorData()
{
}


//---------------------------------------------------------------
//
// XErrorData::XErrorData
//
//---------------------------------------------------------------
XErrorData::XErrorData()					
{
}


//---------------------------------------------------------------
//
// XErrorData::GetSummary
//
//---------------------------------------------------------------
std::wstring XErrorData::GetSummary() const
{
	return mSummary;
}


//---------------------------------------------------------------
//
// XErrorData::GetNarrative
//
//---------------------------------------------------------------
std::wstring XErrorData::GetNarrative() const
{
	return mNarrative;
}

							
//---------------------------------------------------------------
//
// XErrorData::SetSummary
//
//---------------------------------------------------------------
void XErrorData::SetSummary(const std::wstring& text)
{
	mSummary = text;
}


//---------------------------------------------------------------
//
// XErrorData::SetNarrative (wstring)
//
//---------------------------------------------------------------
void XErrorData::SetNarrative(const std::wstring& text)
{
	mNarrative = text;
}


//---------------------------------------------------------------
//
// XErrorData::SetNarrative (std::exception*)
//
//---------------------------------------------------------------
void XErrorData::SetNarrative(const std::exception* e)
{
	mNarrative = XError::Instance()->GetText(e);
}


//---------------------------------------------------------------
//
// XErrorData::SetNarrative (OSError)
//
//---------------------------------------------------------------
void XErrorData::SetNarrative(OSError err)
{
	mNarrative = XError::Instance()->GetText(err);
}


}	// namespace Whisper
