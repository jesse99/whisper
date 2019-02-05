/*
 *  File:		XXMLAttribute.h
 *  Summary:	Encapsulates an XML attribute.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLAttribute.h,v $
 *		Revision 1.6  2001/03/16 10:37:35  jesjones
 *		Minor reformatting.
 *		
 *		Revision 1.5  2001/03/02 11:25:36  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.4  2001/03/01 11:46:14  jesjones
 *		Template Get and Set methods use helper routines to avoid generating gobs of code.
 *		
 *		Revision 1.3  2000/12/19 08:00:35  jesjones
 *		Template Get and Set methods now throw in the event of an error.
 *		
 *		Revision 1.2  2000/11/09 12:59:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XDebug.h>
#include <XTextConversions.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XInStream;
class XOutStream;


// ===================================================================================
//	class XXMLAttribute
// ===================================================================================
class CORE_EXPORT XXMLAttribute {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XXMLAttribute();

						XXMLAttribute();
						// Not for public use (it's only here so we can put these in an 
						// STL collection).

						XXMLAttribute(const wchar_t* name, const wchar_t* value, bool implied = false);
						XXMLAttribute(const wchar_t* name, const std::wstring& value, bool implied = false);
						XXMLAttribute(const std::wstring& name, const wchar_t* value, bool implied = false);
						XXMLAttribute(const std::wstring& name, const std::wstring& value, bool implied = false);
						// Need the char* versions so we're not ambiguous with the template ctor.

						template <typename T>
						XXMLAttribute(const std::wstring& name, const T& value) : mName(name), mImplied(false)	{this->Set(value);}

						XXMLAttribute(const XXMLAttribute& rhs);

//-----------------------------------
//	API
//
public:
	// ----- Name and Value -----
			const std::wstring& GetName() const							{return mName;}
						
			bool 		IsImplied() const								{return mImplied;}
						// Returns true if the attribute is #IMPLIED and not specified.
						// Note that it's a programmer error to call GetValue if IsImplied 
						// returns true.
			
			const std::wstring& GetValue() const						{ASSERT(!mImplied); return mValue;}
			void 		SetValue(const std::wstring& value);
						// Note that certain value types have multiple values (eg NMTOKENS).
						// In this case GetValue will return all of the values at once with
						// an intervening space. (Attribute values are normalized with all 
						// whitespace converted to spaces. If the type is not CDATA leading 
						// and trailing spaces are stripped and runs of spaces are converted 
						// to a single space).
												
	// ----- Helpers -----
			template <typename T>
			void 		Get(T& value) const								{FromStr(mValue, value);}
						// Uses a reference instead of returning the value to allow the
						// compiler to deduce the type.
			
			template <typename T>
			T 			Get() const										{T value; FromStr(mValue, value); return value;}
						// It's usually more convenient to use the getter above because
						// this one requires clients to explicitly indicate the type.
				
			template <typename T>
			void 		Get(T min, T& value, T max) const				{FromStr(mValue, mName, min, max, value);}
						// Throws if the value is outside the specified range.
			
			template <typename T>
			void 		Set(const T& value)								{mValue = ToStr(value);}
			
	// ----- Streaming -----
	friend 	XInStream& 	operator>>(XInStream& stream, XXMLAttribute& value);
	
	friend	XOutStream& operator<<(XOutStream& stream, const XXMLAttribute& value);

//-----------------------------------
//	Member Data
//
protected:
	std::wstring	mName;
	std::wstring	mValue;
	bool			mImplied;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

