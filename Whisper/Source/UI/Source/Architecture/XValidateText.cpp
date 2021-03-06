/*
 *  File:       XValidateText.cpp
 *  Summary:   	IValidateText implementation that uses a IValidateTextHelper.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XValidateText.cpp,v $
 *		Revision 1.4  2001/03/28 09:47:39  jesjones
 *		Fixed the code to give the helper a chance to initialize things.
 *		
 *		Revision 1.3  2001/02/12 09:51:40  jesjones
 *		Added a work around to ensure that 'this' is initialized.
 *		
 *		Revision 1.2  2001/01/22 02:02:18  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.1  2000/12/19 07:44:31  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IValidateText.h>

#include <IValidateTextHelper.h>
#include <XDialogUtils.h>
#include <XIntConversions.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XValidateText
//!		IValidateText implementation that uses a IValidateTextHelper.
// ===================================================================================
class XValidateText : public IValidateText {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XValidateText();
	
						XValidateText(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		IsValid(wchar_t ch) const;
	virtual bool 		IsValid(const std::wstring& text) const;
						
	virtual bool 		Validate(const std::wstring& text) const;

	virtual uint32 		GetMinChars() const				{if (mMaxChars == 0) (void) IConstValidateTextHelperPtr(this); return mMinChars;}
	virtual uint32 		GetMaxChars() const				{if (mMaxChars == 0) (void) IConstValidateTextHelperPtr(this); return mMaxChars;}
	
	virtual void 		SetMinChars(uint32 min)			{mMinChars = min;}
	virtual void 		SetMaxChars(uint32 max)			{mMaxChars = max;}
	
//-----------------------------------
//	Member Data
//
private:
	uint32		mMinChars;
	uint32		mMaxChars;
};

DEFINE_INTERFACE_FACTORY(XValidateText)

//---------------------------------------------------------------
//
// XValidateText::~XValidateText
//
//---------------------------------------------------------------
XValidateText::~XValidateText()
{
}


//---------------------------------------------------------------
//
// XValidateText::XValidateText
//
//---------------------------------------------------------------
XValidateText::XValidateText(XBoss* boss) 
{	
	IValidateText::DoSetBoss(boss);

	mMinChars = 0;					// subclasses need to set these
	mMaxChars = 0;
}


//---------------------------------------------------------------
//
// XValidateText::IsValid (wchar_t)
//
//---------------------------------------------------------------
bool XValidateText::IsValid(wchar_t ch) const
{
	IConstValidateTextHelperPtr helper(this);
	bool valid = helper->OnValid(ch);
	
	return valid;
}


//---------------------------------------------------------------
//
// XValidateText::IsValid (std::wstring)
//
//---------------------------------------------------------------
bool XValidateText::IsValid(const std::wstring& text) const
{
	bool valid = false;
	
	uint32 length = text.length();
	
	IConstValidateTextHelperPtr helper(this);
	if (length >= mMinChars && length <= mMaxChars)
		valid = helper->OnValidate(text, false);
		
	return valid;
}

						
//---------------------------------------------------------------
//
// XValidateText::Validate
//
//---------------------------------------------------------------
bool XValidateText::Validate(const std::wstring& text) const
{
	bool valid = false;
	
	uint32 length = text.length();
	
	IConstValidateTextHelperPtr helper(this);	
	if (length >= mMinChars) {
		if (length <= mMaxChars) 
			if (length > 0) 
				valid = helper->OnValidate(text, true);
			else
				valid = true;
				
		else
			Whisper::DoStop(LoadWhisperString(L"Invalid Text."), LoadWhisperString(L"Text must be less than #1 characters.", UInt32ToStr(mMaxChars + 1)));

	} else
		Whisper::DoStop(LoadWhisperString(L"Invalid Text."), LoadWhisperString(L"Text must be greater than #1 characters.", UInt32ToStr(mMinChars - 1)));
			
	return valid;
}


}	// namespace Whisper
