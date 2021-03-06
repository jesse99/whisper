/*
 *  File:       MClipDrawWindow.cpp
 *  Summary:   	MDrawWindow subclass that doesn't draw over children of the root control.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MClipDrawWindow.cpp,v $
 *		Revision 1.2  2000/11/09 09:10:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 6/29/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <MDrawWindow.h>

#include <IControl.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IRootControl.h>
#include <XRegion.h>

namespace Whisper {


// ===================================================================================
//	class MClipDrawWindow
// ===================================================================================
class MClipDrawWindow : public MDrawWindow {

	typedef MDrawWindow Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~MClipDrawWindow();
	
						MClipDrawWindow(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive);

//-----------------------------------
//	Internal API
//
private:
			IHierarchyPtr DoFindRoot() const;
};

DEFINE_INTERFACE_FACTORY(MClipDrawWindow)

//---------------------------------------------------------------
//
// MClipDrawWindow::~MClipDrawWindow
//
//---------------------------------------------------------------
MClipDrawWindow::~MClipDrawWindow()
{
}


//---------------------------------------------------------------
//
// MClipDrawWindow::MClipDrawWindow
//
//---------------------------------------------------------------
MClipDrawWindow::MClipDrawWindow(XBoss* boss) : MDrawWindow(boss)
{	
}


//---------------------------------------------------------------
//
// MClipDrawWindow::OnDraw
//
//---------------------------------------------------------------
void MClipDrawWindow::OnDraw(XDrawContext& context, const XRegion& dirtyRgn, bool isActive)
{
	XRegion clip = dirtyRgn;
	
	IHierarchyPtr root = this->DoFindRoot();
	IHierarchy::iterator iter = root->begin();
	while (iter != root->end()) {
		IHierarchyPtr node = *iter;
		++iter;
		
		IControlPtr control(node);		
		IGeometryPtr geometry(control);
		if (geometry.IsValid()) {
			XRect extent = geometry->GetExtent();
			extent = control->LocalToWindow(extent);
			clip -= extent;
		}
	}
	
	Inherited::OnDraw(context, clip, isActive);
}


//---------------------------------------------------------------
//
// MClipDrawWindow::DoFindRoot
//
// Normally the root control is the first child of the window,
// but we'll play it safe and search for it.
//
//---------------------------------------------------------------
IHierarchyPtr MClipDrawWindow::DoFindRoot() const
{
	IConstHierarchyPtr node(this);
	
	XCallback1<bool, IHierarchyPtr> predicate(MatchInterface<IRootControl>());
	IHierarchyPtr child = node->FindChild(predicate);
		
	return child;
}


}	// namespace Whisper
