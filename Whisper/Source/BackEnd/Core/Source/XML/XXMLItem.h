/*
 *  File:		XXMLItem.h
 *  Summary:	Abstract base class for things that can be nested within an XML element.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLItem.h,v $
 *		Revision 1.3  2001/04/17 01:45:38  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/10 04:46:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XBinaryPersistent.h>
#include <XInvariant.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XXMLDocVisitor;


// ===================================================================================
//	class XXMLItem
// ===================================================================================
class CORE_EXPORT XXMLItem : public XBinaryPersistentMixin {

//-----------------------------------
//	Types
//
public:
	enum EType {kElement, kCharData, kProcessInstruction, kComment};

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLItem();

						XXMLItem(EType type);

private:
						XXMLItem(const XXMLItem& rhs);
						
			XXMLItem& 	operator=(const XXMLItem& rhs);
						
//-----------------------------------
//	New API
//
public:					
			EType 		GetType() const							{return mType;}		
			
	virtual bool 		HandleVisit(XXMLDocVisitor& visitor) const = 0;		
			
//-----------------------------------
//	Inherited API
//
protected:								
	virtual	void 		OnStreamOut(XOutStream& stream) const;

	virtual void 		OnStreamIn(XInStream& stream);

//-----------------------------------
//	Member Data
//
protected:
	EType	mType;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

