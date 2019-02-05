/*
 *  File:       XValidateTexts.cpp
 *  Summary:   	IValidateText implementations.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XValidateTexts.cpp,v $
 *		Revision 1.2  2001/01/22 02:02:24  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.1  2000/12/19 07:44:38  jesjones
 *		Checked in	
 */

#include <XWhisperHeader.h>
#include <IValidateTextHelper.h>

#include <IRange.h>
#include <IValidateText.h>
#include <XDialogUtils.h>
#include <XFloatConversions.h>
#include <XIntConversions.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XIntValidator
//!		IValidateText that requires that the text be an integer.
// ===================================================================================
class XIntValidator : public IValidateTextHelper, public IRange<int32> {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XIntValidator();
	
						XIntValidator(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual int32 		GetMin() const					{return mMin;}
	virtual void 		SetMin(int32 value)				{mMin = value;}

	virtual int32 		GetMax() const					{return mMax;}
	virtual void 		SetMax(int32 value)				{mMax = value;}

protected:
	virtual bool 		OnValid(wchar_t ch) const;

	virtual bool 		OnValidate(const std::wstring& text, bool showAlert) const;

//-----------------------------------
//	Member Data
//
protected:
	int32	mMin;
	int32	mMax;
};

DEFINE_INTERFACE_FACTORY(XIntValidator)

//---------------------------------------------------------------
//
// XIntValidator::~XIntValidator
//
//---------------------------------------------------------------
XIntValidator::~XIntValidator()
{
}


//---------------------------------------------------------------
//
// XIntValidator::XIntValidator
//
//---------------------------------------------------------------
XIntValidator::XIntValidator(XBoss* boss)
{	
	IValidateTextHelper::DoSetBoss(boss);
	IRange<int32>::DoSetBoss(boss);
	
	IValidateTextHelper* thisPtr = this;
	IValidateTextPtr validate(thisPtr);
	validate->SetMinChars(1);
	validate->SetMaxChars(11);

	mMin = LONG_MIN;
	mMax = LONG_MAX;
}


//---------------------------------------------------------------
//
// XIntValidator::OnValid
//
//---------------------------------------------------------------
bool XIntValidator::OnValid(wchar_t ch) const
{
	bool valid = IsDigit(ch) || ch == '+';
	
	if (!valid && mMin < 0)
		valid = ch == '-';

	return valid;
}


//---------------------------------------------------------------
//
// XIntValidator::OnValidate
//
//---------------------------------------------------------------
bool XIntValidator::OnValidate(const std::wstring& text, bool showAlert) const
{
	bool valid = false;
	
	try {
		int32 num = StrToInt32(text);
		
		if (num < mMin) {
			if (showAlert)
				Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"The number must be at least #1.", Int32ToStr(mMin)));
			
		} else if (num > mMax) {
			if (showAlert)
				Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"The number can't be larger than #1.", Int32ToStr(mMax)));
			
		} else
			valid = true;
				
	} catch (const std::invalid_argument&) {
		if (showAlert)
			Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"Expected an integer."));
		
	} catch (const std::range_error&) {
		if (showAlert)
			Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"The number must be within #1 and #2.", Int32ToStr(mMin), Int32ToStr(mMax)));
	}
		
	return valid;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XHexValidator
//!		IValidateText that requires that the text be a hex number.
// ===================================================================================
class XHexValidator : public IValidateTextHelper, public IRange<uint32> {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XHexValidator();
	
						XHexValidator(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual uint32 		GetMin() const					{return mMin;}
	virtual void 		SetMin(uint32 value)			{mMin = value;}

	virtual uint32 		GetMax() const					{return mMax;}
	virtual void 		SetMax(uint32 value)			{mMax = value;}

protected:
	virtual bool 		OnValid(wchar_t ch) const;

	virtual bool 		OnValidate(const std::wstring& text, bool showAlert) const;

//-----------------------------------
//	Member Data
//
protected:
	uint32	mMin;
	uint32	mMax;
};

DEFINE_INTERFACE_FACTORY(XHexValidator)

//---------------------------------------------------------------
//
// XHexValidator::~XHexValidator
//
//---------------------------------------------------------------
XHexValidator::~XHexValidator()
{
}


//---------------------------------------------------------------
//
// XHexValidator::XHexValidator
//
//---------------------------------------------------------------
XHexValidator::XHexValidator(XBoss* boss)
{	
	IValidateTextHelper::DoSetBoss(boss);
	IRange<uint32>::DoSetBoss(boss);
	
	IValidateTextHelper* thisPtr = this;
	IValidateTextPtr validate(thisPtr);
	validate->SetMinChars(1);
	validate->SetMaxChars(8);

	mMin = 0;
	mMax = ULONG_MAX;
}


//---------------------------------------------------------------
//
// XHexValidator::OnValid
//
//---------------------------------------------------------------
bool XHexValidator::OnValid(wchar_t ch) const
{
	bool valid = IsHexDigit(ch);
	
	return valid;
}


//---------------------------------------------------------------
//
// XHexValidator::OnValidate
//
//---------------------------------------------------------------
bool XHexValidator::OnValidate(const std::wstring& text, bool showAlert) const
{
	bool valid = false;
	
	try {
		uint32 num = StrToUInt32(L"$" + text);
		
		if (num < mMin) {
			if (showAlert)
				Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"The number must be at least #1.", UInt32ToHex(mMin)));
			
		} else if (num > mMax) {
			if (showAlert)
				Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"The number can't be larger than #1.", UInt32ToHex(mMax)));
			
		} else
			valid = true;
				
	} catch (const std::invalid_argument&) {
		if (showAlert)
			Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"Expected a hexadecimal number."));
		
	} catch (const std::range_error&) {
		if (showAlert)
			Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"The number must be within #1 and #2.", UInt32ToHex(mMin), UInt32ToHex(mMax)));
	}
		
	return valid;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XDoubleValidator
//!		IValidateText that requires that the text be a double.
// ===================================================================================
class XDoubleValidator : public IValidateTextHelper, public IRange<double> {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDoubleValidator();
	
						XDoubleValidator(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual double 		GetMin() const					{return mMin;}
	virtual void 		SetMin(double value)			{mMin = value;}

	virtual double 		GetMax() const					{return mMax;}
	virtual void 		SetMax(double value)			{mMax = value;}

protected:
	virtual bool 		OnValid(wchar_t ch) const;

	virtual bool 		OnValidate(const std::wstring& text, bool showAlert) const;

//-----------------------------------
//	Member Data
//
protected:
	double	mMin;
	double	mMax;
};

DEFINE_INTERFACE_FACTORY(XDoubleValidator)

//---------------------------------------------------------------
//
// XDoubleValidator::~XDoubleValidator
//
//---------------------------------------------------------------
XDoubleValidator::~XDoubleValidator()
{
}


//---------------------------------------------------------------
//
// XDoubleValidator::XDoubleValidator
//
//---------------------------------------------------------------
XDoubleValidator::XDoubleValidator(XBoss* boss)
{	
	IValidateTextHelper::DoSetBoss(boss);
	IRange<double>::DoSetBoss(boss);
	
	IValidateTextHelper* thisPtr = this;
	IValidateTextPtr validate(thisPtr);
	validate->SetMinChars(1);
	validate->SetMaxChars(40);

	mMin = -INFINITY;
	mMax = INFINITY;
}


//---------------------------------------------------------------
//
// XDoubleValidator::OnValid
//
//---------------------------------------------------------------
bool XDoubleValidator::OnValid(wchar_t ch) const
{
	ch = ConvertToUpperCase(ch);
	
	bool valid = IsDigit(ch) || ch == '+' || ch == '-' || 		// integer
				 ch == '.' || ch == 'e' || ch == 'E' ||			// float
				 ch == 'I' || ch == 'N' || ch == 'F' ||			// INF
				 ch == 'A';										// NAN
	
	return valid;
}


//---------------------------------------------------------------
//
// XDoubleValidator::OnValidate
//
//---------------------------------------------------------------
bool XDoubleValidator::OnValidate(const std::wstring& text, bool showAlert) const
{
	bool valid = false;
	
	try {
		double num = StrToDouble(text);
		
		if (num < mMin) {
			if (showAlert)
				Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"The number must be at least #1.", StripLeading(DoubleToStr(mMin), L" ")));
			
		} else if (num > mMax) {
			if (showAlert)
				Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"The number can't be larger than #1.", StripLeading(DoubleToStr(mMax), L" ")));
			
		} else
			valid = true;
				
	} catch (const std::invalid_argument&) {
		if (showAlert)
			Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"Expected a floating point number."));
		
	} catch (const std::range_error&) {
		if (showAlert)
			Whisper::DoStop(LoadWhisperString(L"Invalid Number."), LoadWhisperString(L"The number must be within #1 and #2.", StripLeading(DoubleToStr(mMin), L" "), StripLeading(DoubleToStr(mMax), L" ")));
	}
		
	return valid;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XAlphaValidator
//!		IValidateText that requires that the text be letters.
// ===================================================================================
class XAlphaValidator : public IValidateTextHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XAlphaValidator();
	
						XAlphaValidator(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnValid(wchar_t ch) const;

	virtual bool 		OnValidate(const std::wstring& text, bool showAlert) const;
};

DEFINE_INTERFACE_FACTORY(XAlphaValidator)

//---------------------------------------------------------------
//
// XAlphaValidator::~XAlphaValidator
//
//---------------------------------------------------------------
XAlphaValidator::~XAlphaValidator()
{
}


//---------------------------------------------------------------
//
// XAlphaValidator::XAlphaValidator
//
//---------------------------------------------------------------
XAlphaValidator::XAlphaValidator(XBoss* boss)
{	
	IValidateTextHelper::DoSetBoss(boss);
	
	IValidateTextHelper* thisPtr = this;
	IValidateTextPtr validate(thisPtr);
	validate->SetMinChars(1);
	validate->SetMaxChars(256);
}


//---------------------------------------------------------------
//
// XAlphaValidator::OnValid
//
//---------------------------------------------------------------
bool XAlphaValidator::OnValid(wchar_t ch) const
{
	bool valid = IsAlphabetical(ch);	// $$$ special case combining characters?

	return valid;
}


//---------------------------------------------------------------
//
// XAlphaValidator::OnValidate
//
//---------------------------------------------------------------
bool XAlphaValidator::OnValidate(const std::wstring& text, bool showAlert) const
{
	bool valid = true;	// $$$ special case combining characters
	
	for (uint32 index = 0; index < text.length() && valid; index++)
		valid = IsAlphabetical(text[index]);
		
	if (!valid)
		if (showAlert)
			Whisper::DoStop(LoadWhisperString(L"Invalid Text."), LoadWhisperString(L"Only letters are allowed."));		// should never happen
		
	return valid;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XAlphaNumValidator
//!		IValidateText that requires that the text be letters or digits.
// ===================================================================================
class XAlphaNumValidator : public IValidateTextHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XAlphaNumValidator();
	
						XAlphaNumValidator(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnValid(wchar_t ch) const;

	virtual bool 		OnValidate(const std::wstring& text, bool showAlert) const;
};

DEFINE_INTERFACE_FACTORY(XAlphaNumValidator)

//---------------------------------------------------------------
//
// XAlphaNumValidator::~XAlphaNumValidator
//
//---------------------------------------------------------------
XAlphaNumValidator::~XAlphaNumValidator()
{
}


//---------------------------------------------------------------
//
// XAlphaNumValidator::XAlphaNumValidator
//
//---------------------------------------------------------------
XAlphaNumValidator::XAlphaNumValidator(XBoss* boss)
{	
	IValidateTextHelper::DoSetBoss(boss);
	
	IValidateTextHelper* thisPtr = this;
	IValidateTextPtr validate(thisPtr);
	validate->SetMinChars(1);
	validate->SetMaxChars(256);
}


//---------------------------------------------------------------
//
// XAlphaNumValidator::OnValid
//
//---------------------------------------------------------------
bool XAlphaNumValidator::OnValid(wchar_t ch) const
{
	bool valid = IsAlphabetical(ch) || IsDigit(ch);	// $$$ special case combining characters?

	return valid;
}


//---------------------------------------------------------------
//
// XAlphaNumValidator::OnValidate
//
//---------------------------------------------------------------
bool XAlphaNumValidator::OnValidate(const std::wstring& text, bool showAlert) const
{
	bool valid = true;	// $$$ special case combining characters
	
	for (uint32 index = 0; index < text.length() && valid; index++)
		valid = IsAlphabetical(text[index]) || IsDigit(text[index]);
		
	if (!valid)
		if (showAlert)
			Whisper::DoStop(LoadWhisperString(L"Invalid Text."), LoadWhisperString(L"Only letters and numbers are allowed."));		// should never happen
		
	return valid;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPrintableValidator
//!		IValidateText that requires that the text be printable (ie no control characters).
// ===================================================================================
class XPrintableValidator : public IValidateTextHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XPrintableValidator();
	
						XPrintableValidator(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnValid(wchar_t ch) const;

	virtual bool 		OnValidate(const std::wstring& text, bool showAlert) const;
};

DEFINE_INTERFACE_FACTORY(XPrintableValidator)

//---------------------------------------------------------------
//
// XPrintableValidator::~XPrintableValidator
//
//---------------------------------------------------------------
XPrintableValidator::~XPrintableValidator()
{
}


//---------------------------------------------------------------
//
// XPrintableValidator::XPrintableValidator
//
//---------------------------------------------------------------
XPrintableValidator::XPrintableValidator(XBoss* boss)
{	
	IValidateTextHelper::DoSetBoss(boss);
	
	IValidateTextHelper* thisPtr = this;
	IValidateTextPtr validate(thisPtr);
	validate->SetMinChars(1);
	validate->SetMaxChars(256);
}


//---------------------------------------------------------------
//
// XPrintableValidator::OnValid
//
//---------------------------------------------------------------
bool XPrintableValidator::OnValid(wchar_t ch) const
{
	bool valid = IsPrintable(ch);	// $$$ special case combining characters?

	return valid;
}


//---------------------------------------------------------------
//
// XPrintableValidator::OnValidate
//
//---------------------------------------------------------------
bool XPrintableValidator::OnValidate(const std::wstring& text, bool showAlert) const
{
	bool valid = true;	// $$$ special case combining characters?
	
	for (uint32 index = 0; index < text.length() && valid; index++)
		valid = (bool) IsPrintable(text[index]);
		
	if (!valid)
		if (showAlert)
			Whisper::DoStop(LoadWhisperString(L"Invalid Text."), LoadWhisperString(L"Only printable characters are allowed."));	// should never happen
		
	return valid;
}


}	// namespace Whisper
