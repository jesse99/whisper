/*
 *  File:       X3DObject.cpp
 *  Summary:	Base class for QuickDraw 3D objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DObject.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:49  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:30:09  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:18:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	  4/14/00	JDJ		Got rid of XRegister3DObject and X3DObject::Create(TQ3Object).
 *		 <2>	 12/03/97	JDJ		Added ConfirmType and X3DBadObjectException.
 *		 <1>	  1/13/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X3DObject.h>

#include <X3DUtils.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DObject
// ===================================================================================

//---------------------------------------------------------------
//
// X3DObject::~X3DObject
//
//---------------------------------------------------------------
X3DObject::~X3DObject()
{
	TQ3Status status = Q3Object_Dispose(mObject);	// for shared objects this will decrement the ref count
	ASSERT(status == kQ3Success);
}


//---------------------------------------------------------------
//
// X3DObject::X3DObject
//
//---------------------------------------------------------------
X3DObject::X3DObject(TQ3Object object)
{
	ThrowIfNil(object);
	
	mObject = object;
}


//---------------------------------------------------------------
//
// X3DObject::Clone
//
//---------------------------------------------------------------
X3DObject* X3DObject::Clone() const
{
	TQ3Object newObject = Q3Object_Duplicate(mObject);
	ThrowIfNil(newObject);
	
	X3DObject* object = new X3DObject(newObject);
	
	return object;
}


//---------------------------------------------------------------
//
// X3DObject::IsWritable
//
//---------------------------------------------------------------
bool X3DObject::IsWritable(TQ3FileObject file) const
{
	PRECONDITION(file != nil);
	
	bool is = Q3Object_IsWritable(mObject, file);
	
	return is;
}


//---------------------------------------------------------------
//
// X3DObject::ConfirmType
//
//---------------------------------------------------------------
void X3DObject::ConfirmType(TQ3ObjectType type) const
{
	if (!this->IsType(type)) {
		TQ3ObjectType leaf = this->GetLeafType();
		
		std::wstring text = L"Internal Error: QD 3D object isn't a '" + IDToStr((uint32) type) + L"'.";
		text += L" It's a '" + IDToStr((uint32) leaf) + L"'.";
		
		throw std::invalid_argument(ToUTF8Str(text));
	}
}


}	// namespace Whisper
