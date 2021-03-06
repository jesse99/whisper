/*
 *  File:       XDrawExtensible.cpp
 *  Summary:   	Default IDrawExtensible implementation.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDrawExtensible.cpp,v $
 *		Revision 1.5  2001/04/27 04:32:25  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/02/07 04:57:58  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:35:10  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:33:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDrawExtensible.h>

#include <IDrawExtension.h>
#include <XInterfaceMacros.h>
#include <XMouseEvents.h>
#include <XObjectModel.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class XDrawExtensible
//!		Default IDrawExtensible implementation.
// ===================================================================================
class XDrawExtensible : public IDrawExtensible {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDrawExtensible();
	
						XDrawExtensible(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		AddExtension(const IDrawExtensionPtr& extension);
	virtual IDrawExtensionPtr RemoveExtension(const std::wstring& name);
	virtual void		RemoveExtensions();

	virtual bool		HasExtension(const std::wstring& name) const;
	virtual IDrawExtensionPtr GetExtension(const std::wstring& name) const;

	virtual	bool		HandleDraw(XDrawContext& canvas, const XRect& ownerExtent, bool before, bool enabled);
						
//-----------------------------------
//	Member Data
//
private:
	std::vector<IDrawExtensionPtr>	mExtensions;
};

DEFINE_INTERFACE_FACTORY(XDrawExtensible)

//---------------------------------------------------------------
//
// XDrawExtensible::~XDrawExtensible
//
//---------------------------------------------------------------
XDrawExtensible::~XDrawExtensible()
{
}


//---------------------------------------------------------------
//
// XDrawExtensible::XDrawExtensible
//
//---------------------------------------------------------------
XDrawExtensible::XDrawExtensible(XBoss* boss) 
{	
	IDrawExtensible::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDrawExtensible::AddExtension
//
//---------------------------------------------------------------
void XDrawExtensible::AddExtension(const IDrawExtensionPtr& extension)
{
	PRECONDITION(extension);
		
	mExtensions.push_back(extension);
}


//---------------------------------------------------------------
//
// XDrawExtensible::RemoveExtension
//
//---------------------------------------------------------------
IDrawExtensionPtr XDrawExtensible::RemoveExtension(const std::wstring& name)
{
	PRECONDITION(this->HasExtension(name));
	
	IDrawExtensionPtr extension;
	
	std::vector<IDrawExtensionPtr>::iterator iter;
	for (iter = mExtensions.begin(); iter != mExtensions.end() && !extension; ++iter) {		
		IDrawExtensionPtr candidate = *iter;
		
		if (candidate->GetName() == name)
			extension = candidate;
	}
	
	PRECONDITION(iter != mExtensions.end());
	mExtensions.erase(iter);
		
	POSTCONDITION(!this->HasExtension(name));
	
	return extension;
}


//---------------------------------------------------------------
//
// XDrawExtensible::RemoveExtensions
//
//---------------------------------------------------------------
void XDrawExtensible::RemoveExtensions()
{		
	mExtensions.clear();
}


//---------------------------------------------------------------
//
// XDrawExtensible::HasExtension
//
//---------------------------------------------------------------
bool XDrawExtensible::HasExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	bool has = false;
	
	for (std::vector<IDrawExtensionPtr>::const_iterator iter = mExtensions.begin(); iter != mExtensions.end() && !has; ++iter) {		
		IDrawExtensionPtr extension = *iter;

		if (extension->GetName() == name)
			has = true;
	}
	
	return has;
}


//---------------------------------------------------------------
//
// XDrawExtensible::GetExtension
//
//---------------------------------------------------------------
IDrawExtensionPtr XDrawExtensible::GetExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	IDrawExtensionPtr extension;
	
	for (std::vector<IDrawExtensionPtr>::const_iterator iter = mExtensions.begin(); iter != mExtensions.end() && !extension; ++iter) {		
		IDrawExtensionPtr extension = *iter;

		if (extension->GetName() == name)
			extension = *iter;
	}
	
	return extension;
}


//---------------------------------------------------------------
//
// XDrawExtensible::HandleDraw
//
//---------------------------------------------------------------
bool XDrawExtensible::HandleDraw(XDrawContext& canvas, const XRect& ownerExtent, bool before, bool enabled)
{	
	bool drawOwner = true;
	
	for (std::vector<IDrawExtensionPtr>::iterator iter = mExtensions.begin(); iter != mExtensions.end(); ++iter) {		
		IDrawExtensionPtr extension = *iter;

		if (before == extension->DrawsBeforeOwner())
			if (!extension->Draw(canvas, ownerExtent, enabled))
				drawOwner = false;
	}
				
	return !drawOwner;
}


}	// namespace Whisper

