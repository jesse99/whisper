/*
 *  File:       X3DVertexList.cpp
 *  Summary:	Encapsulates a TQ3Vertex3D vector.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DVertexList.cpp,v $
 *		Revision 1.4  2001/04/21 03:39:02  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:33:40  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:24:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DVertexList.h>

#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class ZBase3DVertexList
// ===================================================================================

//---------------------------------------------------------------
//
// ZBase3DVertexList::DoDestroy
//
//---------------------------------------------------------------
void ZBase3DVertexList::DoDestroy(TQ3Vertex3D* vertices, uint32 count)	
{
	for (uint32 i = 0; i < count; ++i) {
		if (vertices[i].attributeSet != nil) {
			TQ3Status status = Q3Object_Dispose(vertices[i].attributeSet);
			ASSERT(status == kQ3Success);
		}
	}
}


//---------------------------------------------------------------
//
// ZBase3DVertexList::DoGetAttribute
//
//---------------------------------------------------------------
X3DAttributeSet ZBase3DVertexList::DoGetAttribute(const TQ3Vertex3D* inVertices, uint32 count, uint32 i) const				
{
	PRECONDITION(i < count); 
	
	TQ3Vertex3D* vertices = const_cast<TQ3Vertex3D*>(inVertices);
	
	if (vertices[i].attributeSet == nil) {
		vertices[i].attributeSet = Q3AttributeSet_New(); 
		ThrowIfNil(vertices[i].attributeSet);
	}
	
	return vertices[i].attributeSet;
}


//---------------------------------------------------------------
//
// ZBase3DVertexList::DoSetAttribute
//
//---------------------------------------------------------------
void ZBase3DVertexList::DoSetAttribute(TQ3Vertex3D* vertices, uint32 count, uint32 i, TQ3AttributeSet attr)	
{
	PRECONDITION(i < count); 
	
	if (attr != vertices[i].attributeSet) {
		if (vertices[i].attributeSet != nil) {
			TQ3Status status = Q3Object_Dispose(vertices[i].attributeSet); 
			ThrowIf3DError(status);
		}
		
		vertices[i].attributeSet = attr;

		if (vertices[i].attributeSet != nil) 
			Q3Shared_GetReference(vertices[i].attributeSet); 
	}
}


//---------------------------------------------------------------
//
// ZBase3DVertexList::DoDestroyAttributes
//
//---------------------------------------------------------------
void ZBase3DVertexList::DoDestroyAttributes(TQ3AttributeSet* attributes, uint32 count)	
{
	for (uint32 i = 0; i < count; ++i) {
		if (attributes[i] != nil) {
			TQ3Status status = Q3Object_Dispose(attributes[i]);
			ASSERT(status == kQ3Success);
		}
	}
}


//---------------------------------------------------------------
//
// ZBase3DVertexList::DoGetAttribute
//
//---------------------------------------------------------------
X3DAttributeSet ZBase3DVertexList::DoGetAttribute(const TQ3AttributeSet* inAttributes, uint32 count, uint32 i) const			
{
	PRECONDITION(i < count); 
	
	TQ3AttributeSet* attributes = const_cast<TQ3AttributeSet*>(inAttributes);
	
	if (attributes[i] == nil) {
		attributes[i] = Q3AttributeSet_New(); 
		ThrowIfNil(attributes[i]);
	}
	
	return attributes[i];
}


//---------------------------------------------------------------
//
// ZBase3DVertexList::DoSetAttribute
//
//---------------------------------------------------------------
void ZBase3DVertexList::DoSetAttribute(TQ3AttributeSet* attributes, uint32 count, uint32 i, TQ3AttributeSet attr)	
{
	PRECONDITION(i < count); 
	
	if (attr != attributes[i]) {
		if (attributes[i] != nil) {
			TQ3Status status = Q3Object_Dispose(attributes[i]); 
			ThrowIf3DError(status);
		}
		
		attributes[i] = attr;

		if (attributes[i] != nil) 
			Q3Shared_GetReference(attributes[i]); 
	}
}


}	// namespace Whisper
