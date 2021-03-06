/*
 *  File:       XHierarchy.cpp
 *  Summary:   	The interface used to navigate the tree of objects owned by the app.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XHierarchy.cpp,v $
 *		Revision 1.9  2001/04/27 04:33:09  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.8  2001/03/06 07:32:47  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.7  2001/02/23 10:09:04  jesjones
 *		Traverse hits 'this' even if there are no children.
 *		
 *		Revision 1.6  2001/02/09 04:34:04  jesjones
 *		Renamed IChildNode methods.
 *		
 *		Revision 1.5  2001/01/30 07:25:20  jesjones
 *		Uses CallRepeatedMethod.
 *		
 *		Revision 1.4  2000/12/14 08:36:00  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/20 05:47:26  jesjones
 *		XCallback again uses IsValid.
 *		
 *		Revision 1.2  2000/11/09 12:38:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IHierarchy.h>

#include <IChildNode.h>
#include <XInterfaceMacros.h>
#include <XObjectModel.h>

#if GARBAGE_COLLECT_COM
	#include <XGarbageCollector.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XHierarchy
//!		The interface used to navigate the tree of objects owned by the app.
// ===================================================================================
class XHierarchy : public IHierarchy {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XHierarchy();
	
						XHierarchy(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual NodePtr 	GetParent() const;
	virtual void 		DoSetParent(const NodePtr& node);
							
	virtual iterator 	begin() const;
	virtual iterator 	end() const;

	virtual void 		RemoveChild(const NodePtr& node);
	virtual void 		AddChild(const NodePtr& node);

	virtual void 		RemoveChildren();

	virtual NodePtr 	FindChild(const Predicate& predicate, bool recursive = kRecursive) const;
	virtual void 		Traverse(const Callback& callback, const Predicate& predicate = Predicate()) const;
	
//-----------------------------------
//	Member Data
//
private:
	IHierarchyPtr	mParent;
	Nodes			mChildren;
};

DEFINE_INTERFACE_FACTORY(XHierarchy)

//---------------------------------------------------------------
//
// XHierarchy::~XHierarchy
//
//---------------------------------------------------------------
XHierarchy::~XHierarchy()
{
#if !GARBAGE_COLLECT_COM
	PRECONDITION(mChildren.empty());
#endif
}


//---------------------------------------------------------------
//
// XHierarchy::XHierarchy
//
//---------------------------------------------------------------
XHierarchy::XHierarchy(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XHierarchy::GetParent
//
//---------------------------------------------------------------
XHierarchy::NodePtr XHierarchy::GetParent() const
{
	return mParent;
}

							
//---------------------------------------------------------------
//
// XHierarchy::DoSetParent
//
//---------------------------------------------------------------
void XHierarchy::DoSetParent(const NodePtr& node)
{
	mParent = node;
}


//---------------------------------------------------------------
//
// XHierarchy::begin
//
//---------------------------------------------------------------
XHierarchy::iterator XHierarchy::begin() const
{
	XHierarchy* thisPtr = const_cast<XHierarchy*>(this);
	
	return thisPtr->mChildren.begin();
}


//---------------------------------------------------------------
//
// XHierarchy::end
//
//---------------------------------------------------------------
XHierarchy::iterator XHierarchy::end() const
{
	XHierarchy* thisPtr = const_cast<XHierarchy*>(this);
	
	return thisPtr->mChildren.end();
}


//---------------------------------------------------------------
//
// XHierarchy::FindChild
//
//---------------------------------------------------------------
IHierarchyPtr XHierarchy::FindChild(const Predicate& predicate, bool recursive) const
{
	IHierarchyPtr child;
	
	std::list<IHierarchyPtr> nodes(this->begin(), this->end());
		
	while (!nodes.empty() && !child) {
		IHierarchyPtr candidate = nodes.front();
		nodes.pop_front();
		
		if (predicate(candidate))
			child = candidate;

		else if (recursive)
			nodes.insert(nodes.end(), candidate->begin(), candidate->end());
	}
	
	return child;
}


//---------------------------------------------------------------
//
// XHierarchy::Traverse
//
//---------------------------------------------------------------
void XHierarchy::Traverse(const Callback& callback, const Predicate& predicate) const
{
	std::list<IHierarchyPtr> nodes;
	
	XHierarchy* thisPtr = const_cast<XHierarchy*>(this);
	nodes.push_back(thisPtr);
	
	while (!nodes.empty()) {
		IHierarchyPtr node = nodes.front();
		nodes.pop_front();
		
		if (!predicate.IsValid() || predicate(node))
			callback(node);
		
		nodes.insert(nodes.end(), node->begin(), node->end());
	}
}


//---------------------------------------------------------------
//
// XHierarchy::RemoveChild
//
//---------------------------------------------------------------
void XHierarchy::RemoveChild(const NodePtr& node)
{
	PRECONDITION(node);
	
	CallRepeatedMethod(node.Get(), &IChildNode::OnOrphaned);

	iterator iter2 = std::find(mChildren.begin(), mChildren.end(), node);
	PRECONDITION(iter2 != mChildren.end());
	
#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->RemoveInterface(this->GetBoss(), iter2->GetPtr());
#endif
	
	mChildren.erase(iter2);
	
	IHierarchy* nilChild = nil;
	node->DoSetParent(nilChild);
}


//---------------------------------------------------------------
//
// XHierarchy::AddChild
//
//---------------------------------------------------------------
void XHierarchy::AddChild(const NodePtr& node)
{
#if DEBUG
	PRECONDITION(node);
	
	iterator iter = std::find(mChildren.begin(), mChildren.end(), node);
	PRECONDITION(iter == mChildren.end());
#endif

	mChildren.push_back(node);
	
	node->DoSetParent(this);
	
	XBoss* boss = nil;
#if GARBAGE_COLLECT_COM
	boss = this->GetBoss();
	XGarbageCollector::Instance()->AssignInterface(boss, mChildren.back().GetPtr());
#endif
	
	CallRepeatedMethod(node.Get(), &IChildNode::OnAdopted);
}


//---------------------------------------------------------------
//
// XHierarchy::RemoveChildren
//
//---------------------------------------------------------------
void XHierarchy::RemoveChildren()
{
	while (!mChildren.empty()) {
		IHierarchyPtr child = mChildren.front();
		
		child->RemoveChildren();
		this->RemoveChild(child);
	}
}


}	// namespace Whisper

