/*
 *  File:       X3DSet.h
 *  Summary:	QuickDraw 3D set object.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DSet.h,v $
 *		Revision 1.4  2001/04/14 07:31:40  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:53:56  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:20:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaSet.h>

#include <X3DSharedObject.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DSet
//!		Quesa set class.
// ===================================================================================
class QUESA_EXPORT X3DSet : public X3DSharedObject {

	typedef X3DSharedObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DSet();
						
						X3DSet();

						X3DSet(TQ3SetObject object);
						
	virtual X3DSet* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3SetObject() const				{return mObject;}
	//@}

	//! @name Getters
	//@{
			void 		Get(TQ3ElementType type, void* data) const;
	
			bool 		Contains(TQ3ElementType type) const;
						
			TQ3ElementType GetNextType(TQ3ElementType lastType) const;
						/**< Set lastType to kQ3ElementTypeNone to get the first type.
						Returns kQ3ElementTypeNone when there are no more types. */
	//@}

	//! @name Setters
	//@{
			void 		Add(TQ3ElementType type, const void* data);
			
			void 		Clear(TQ3ElementType type);
						/**< Removes an element. */

			void 		Empty();
						/**< Removes every element. */
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DSet. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
