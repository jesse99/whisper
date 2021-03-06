/*
 *  File:       MAEList.h
 *  Summary:   	An AEDesc that contains a sequence of values.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAEList.h,v $
 *		Revision 1.3  2000/12/10 08:48:06  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:09:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: MAEList.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:48:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	 7/19/99	JDJ		Replaced the Handle ctor with one that takes a void*.
 *		 <1>	 9/13/98	JDJ		Created
 */

#pragma once

#include <MAEDesc.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class MAEList
//!		An AEDesc that contains a sequence of values.
// ===================================================================================
class CORE_EXPORT MAEList : public MAEDesc {

	typedef MAEDesc Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~MAEList();
						
						MAEList(bool isRecord = false);
					
						MAEList(DescType type, const void* data, uint32 bytes);

						MAEList(const AEDescList& desc, bool copyDesc = true);

//-----------------------------------
//	New API
//
public:
	//! @name Getters
	//@{
			uint32 		GetCount() const;
						/**< Note that indices are zero-based! */

			DescType 	GetItemType(uint32 index) const;
			uint32 		GetItemBytes(uint32 index) const;

			OSErr 		GetItem(uint32 index, DescType desiredType, AEDesc* desc) const;
			OSErr 		GetItem(uint32 index, DescType desiredType, void* data, uint32 bytes) const;
			OSErr 		GetItem(uint32 index, DescType desiredType, void* data, uint32 bytes, DescType* typeCode, uint32* actualBytes) const;
						/**< If the parameter is not desiredType it will be coerced (if
						possible). If desiredType is typeWildCard no coercion is performed
						and typeCode contains the parameter type. */
	//@}

	//! @name Mutators
	//@{
			void 		SetItem(uint32 index, DescType typeCode, const void* data, uint32 bytes);	
			void 		SetItem(uint32 index, const AEDesc& desc);	
			
			void 		Append(DescType typeCode, const void* data, uint32 bytes);
			void 		Append(const AEDesc& desc);

			void 		DeleteItem(uint32 index);
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
