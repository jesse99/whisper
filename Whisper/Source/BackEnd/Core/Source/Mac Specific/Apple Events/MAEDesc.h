/*
 *  File:       MAEDesc.h
 *  Summary:   	Base class for AEDesc wrappers.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAEDesc.h,v $
 *		Revision 1.3  2000/12/10 08:48:02  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:08:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: MAEDesc.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:48:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	 7/19/99	JDJ		Removed GetDataHandle (can't implement on Carbon).
 *									Removed MAEDesc::MAEDesc (DescType, Handle).
 *		 <1>	 9/13/98	JDJ		Created
 */

#pragma once

#include <AERegistry.h>

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class MAEDesc
//!		Base class for AEDesc wrappers.
// ===================================================================================
class CORE_EXPORT MAEDesc : public AEDesc {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~MAEDesc();
						
						MAEDesc();

						MAEDesc(DescType type, const void* data, uint32 bytes);
					
						MAEDesc(const AEDesc& desc, bool copyDesc = true);
						
						MAEDesc(const MAEDesc& desc);
						/**< Makes a copy of the desc. */
					
			MAEDesc& 	operator=(const MAEDesc& desc);

//-----------------------------------
//	API
//
public:
	//! @name Accessors
	//@{
			DescType 	GetDescriptorType() const			{return descriptorType;}

//			Handle 		GetDataHandle() const				{return dataHandle;}

#if TARGET_CARBON
			uint32 		GetDataSize() const					{return (uint32) (dataHandle != nil ? ::AEGetDescDataSize(this) : 0);}
#else
			uint32 		GetDataSize() const					{return (uint32) (dataHandle != nil ? ::GetHandleSize(dataHandle) : 0);}
#endif
	//@}

	//! @name Extracting Data
	//@{
			bool 		CanCoerceTo(DescType toType) const;

			MAEDesc 	CoerceTo(DescType toType) const;
			OSErr 		CoerceTo(DescType toType, AEDesc* desc) const;

			bool 		GetBoolean() const;
						/**< All these Get methods will try to coerce the desc if it's not
						already the correct type. If the coercion fails they will throw. */

			int16 		GetShort() const;
			int32 		GetLong() const;
			float 		GetFloat() const;
			double 		GetDouble() const;

			std::wstring GetString() const;
			Rect 		GetRect() const;
			RGBColor 	GetRGBColor() const;
			uint32 		GetDateTime() const;
			ProcessSerialNumber GetPSN() const;
			FSSpec 		GetFSSpec() const;
	
			AliasHandle GetAlias() const;
						/**< When calling GetAlias you're responsible for disposing of the AliasHandle. */
	
			DescType 	GetType() const;
			DescType 	GetEnum() const;
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
