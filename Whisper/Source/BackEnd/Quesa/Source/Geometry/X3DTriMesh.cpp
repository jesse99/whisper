/*
 *  File:       X3DTriMesh.cpp
 *  Summary:	Efficient mesh with uniform attributes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTriMesh.cpp,v $
 *		Revision 1.4  2001/04/21 03:38:48  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:33:14  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:23:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DTriMesh.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DTriMesh
// ===================================================================================

//---------------------------------------------------------------
//
// X3DTriMesh::~X3DTriMesh
//
//---------------------------------------------------------------
X3DTriMesh::~X3DTriMesh()
{
}


//---------------------------------------------------------------
//
// X3DTriMesh::X3DTriMesh (TQ3TriMeshData)
//
//---------------------------------------------------------------
X3DTriMesh::X3DTriMesh(const TQ3TriMeshData& data) : X3DGeometry(Q3TriMesh_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriMesh::X3DTriMesh (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DTriMesh::X3DTriMesh(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeTriMesh);
}


//---------------------------------------------------------------
//
// X3DTriMesh::Clone
//
//---------------------------------------------------------------
X3DTriMesh* X3DTriMesh::Clone() const
{
	X3DTriMesh* object = new X3DTriMesh(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DTriMesh::GetData
//
//---------------------------------------------------------------
TQ3TriMeshData X3DTriMesh::GetData() const
{
	TQ3TriMeshData data;
	
	TQ3Status status = Q3TriMesh_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DTriMesh::SetData
//
//---------------------------------------------------------------
void X3DTriMesh::SetData(const TQ3TriMeshData& data)
{
	TQ3Status status = Q3TriMesh_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriMesh::Submit (TQ3TriMeshData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DTriMesh::Submit(const TQ3TriMeshData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3TriMesh_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriMesh::IsCompatible								[static]
//	
//---------------------------------------------------------------
bool X3DTriMesh::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeTriMesh);
	
	return compatible;
}


}	// namespace Whisper
