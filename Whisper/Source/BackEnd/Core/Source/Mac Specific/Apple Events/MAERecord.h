/*
 *  File:       MAERecord.h
 *  Summary:   	An AEList with each item being tagged with a keyword.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAERecord.h,v $
 *		Revision 1.4  2001/04/27 04:18:41  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.3  2000/12/10 08:48:12  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:09:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <MAEList.h>
#include <XTranscode.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class MAERecord
//!			An AEList with each item being tagged with a keyword.
// ===================================================================================
class CORE_EXPORT MAERecord : public MAEList {

	typedef MAEList Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~MAERecord();
						
						MAERecord();
					
						MAERecord(DescType type, const void* data, uint32 bytes);

						MAERecord(const AERecord& desc, bool copyDesc = true);

//-----------------------------------
//	New API
//
public:
	//! @name Getters
	//@{
			bool 		HasKey(AEKeyword keyword, DescType desiredType = typeWildCard) const;

			DescType 	GetKeyType(AEKeyword keyword) const;
			uint32 		GetKeyBytes(AEKeyword keyword) const;

			OSErr 		GetItem(uint32 index, DescType desiredType, void* data, uint32 bytes, 
								DescType* keyword, DescType* typeCode, uint32* actualBytes) const;

			OSErr 		GetKey(AEKeyword keyword, DescType desiredType, AEDesc* desc) const;
			OSErr 		GetKey(AEKeyword keyword, DescType desiredType, void* data, uint32 bytes) const;
			OSErr 		GetKey(AEKeyword keyword, DescType desiredType, DescType* typeCode,
							   void* data, uint32 bytes, uint32* actualBytes) const;
						/**< If the parameter is not desiredType it will be coerced (if
						possible). If desiredType is typeWildCard no coercion is performed
						and typeCode contains the parameter type. */
	//@}
	
	//! @name Mutators
	//@{
			void 		SetKey(AEKeyword keyword, DescType typeCode, const void* data, uint32 bytes);
			void 		SetKey(AEKeyword keyword, const AEDesc& desc);
						
			void 		Append(AEKeyword keyword, DescType typeCode, const void* data, uint32 bytes);
			void 		Append(AEKeyword keyword, const AEDesc& desc);

			void 		DeleteKey(AEKeyword keyword);
	//@}

	//! @name Standard Values
	//@{
			bool 		GetBoolean(AEKeyword key) const;
			void 		SetBoolean(AEKeyword key, bool value);
						/**< All these Get methods will try to coerce the value if it's not
						already the correct type. If the coercion fails they will throw. */

			int16 		GetShort(AEKeyword key) const;
			void 		SetShort(AEKeyword key, int16 value)					{this->SetKey(key, typeShortInteger, &value, sizeof(value));}

			int32 		GetLong(AEKeyword key) const;
			void 		SetLong(AEKeyword key, int32 value)						{this->SetKey(key, typeLongInteger, &value, sizeof(value));}

			float 		GetFloat(AEKeyword key) const;
			void 		SetFloat(AEKeyword key, float value)					{this->SetKey(key, typeIEEE32BitFloatingPoint, &value, sizeof(value));}

			double 		GetDouble(AEKeyword key) const;
			void 		SetDouble(AEKeyword key, double value)					{this->SetKey(key, typeIEEE64BitFloatingPoint, &value, sizeof(value));}

			Rect 		GetRect(AEKeyword key) const;
			void 		SetRect(AEKeyword key, const Rect& value)				{this->SetKey(key, typeQDRectangle, &value, sizeof(value));}

			std::wstring GetString(AEKeyword key) const;
			void 		SetString(AEKeyword key, const std::wstring& value)		{this->SetKey(key, typeChar, ToPlatformStr(value).c_str(), value.length());}

			uint32 		GetDateTime(AEKeyword key) const;
			void 		SetDateTime(AEKeyword key, uint32 value);

			RGBColor 	GetColor(AEKeyword key) const;
			void 		SetColor(AEKeyword key, const RGBColor& value)			{this->SetKey(key, typeRGBColor, &value, sizeof(value));}

			ProcessSerialNumber GetPSN(AEKeyword key) const;
			void 		SetPSN(AEKeyword key, const ProcessSerialNumber& value)	{this->SetKey(key, typeProcessSerialNumber, &value, sizeof(value));}

			FSSpec 		GetFSSpec(AEKeyword key) const;
			void 		SetFSSpec(AEKeyword key, const FSSpec& value)			{this->SetKey(key, typeFSS, &value, sizeof(value));}

			AliasHandle GetAlias(AEKeyword key) const;
			void 		SetAlias(AEKeyword key, AliasHandle value);
						/**< When calling GetAliasKey you're responsible for disposing of the AliasHandle. */

			DescType 	GetEnum(AEKeyword key) const;
			void 		SetEnum(AEKeyword key, DescType value)					{this->SetKey(key, typeEnumeration, &value, sizeof(value));}

			MAEDesc 	GetDesc(AEKeyword key, DescType desiredType) const;
			void 		SetDesc(AEKeyword key, const MAEDesc& value);	
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
