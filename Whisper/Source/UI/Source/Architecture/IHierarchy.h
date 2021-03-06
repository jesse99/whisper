/*
 *  File:       IHierarchy.h
 *  Summary:   	The interface used to navigate the tree of objects owned by the app.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IHierarchy.h,v $
 *		Revision 1.6  2001/10/03 02:08:46  jesjones
 *		Changed the way MatchInterface works (Pro 7 template bug).
 *		
 *		Revision 1.5  2001/01/30 07:11:43  jesjones
 *		Added CallRepeatedMethod.
 *		
 *		Revision 1.4  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/14 08:22:36  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:02:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <list>

#include <XCallbacks.h>
#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IHierarchy
//!		The interface used to navigate the tree of objects owned by the app.
/*!		The application object typically owns windows and documents. These objects own 
 *		additional objects. For example, the hierarchy could look like this:
 *				- Application
 *					- Document #1
 *						- Doc Window
 *							- Root Control
 *								- controls
 *							- Floating Window
 *								- ...
 *					- Document #2
 *						- Doc Window
 *							- ...
 *					- Regular Window
 *						- ...
 *
 *		Note that the hierarchy is used to propagate menu and key events upwards (this 
 *		is why floating windows place themselves below the frontmost document window). */
// ===================================================================================
class IHierarchy : public XUnknown {

//-----------------------------------
//	Types
//
public:
	typedef XInterfacePtr<IHierarchy> 	NodePtr;
	typedef std::list<NodePtr> 			Nodes;
	typedef Nodes::iterator 			iterator;

	typedef XCallback1<void, NodePtr>	Callback;
	typedef XCallback1<bool, NodePtr> 	Predicate;

//-----------------------------------
//	API
//
public:
	//! @name Children
	//@{
	virtual iterator 	begin() const = 0;
	virtual iterator 	end() const = 0;
						/**< Iterates over the immediate children. */
	
	virtual void 		RemoveChild(const NodePtr& child) = 0;
	virtual void 		AddChild(const NodePtr& child) = 0;

	virtual void 		RemoveChildren() = 0;
	//@}

	//! @name Parent
	//@{
	virtual NodePtr 	GetParent() const = 0;
	//@}

	//! @name Traversals
	//@{
	virtual NodePtr 	FindChild(const Predicate& predicate, bool recursive = kRecursive) const = 0;
						/**< Returns the first child for which predicate returns true.
						If recursive is false only the immediate children are checked.
						Often used like this:
							XCallback1<bool, IHierarchyPtr> predicate = MatchInterface<IScrollableControl>();
							IHierarchyPtr parent(this);
							IHierarchyPtr child = parent->FindChild(predicate);
						Note that IRootControl has a method that will search by name. */

		
	virtual void 		Traverse(const Callback& callback, const Predicate& predicate = Predicate()) const = 0;
						/**< Iterate over all the nodes and call the specified function
						for each node. Note that this is a breadth-first traversal.
						If predicate is valid all nodes are iterated, but callback
						is only called for the nodes for which predicate returns 
						true. */
	//@}

//-----------------------------------
//	Internal API
//
public:
	virtual void 		DoSetParent(const NodePtr& node) = 0;
};

typedef XInterfacePtr<IHierarchy> IHierarchyPtr;
typedef XInterfacePtr<const IHierarchy> IConstHierarchyPtr;


// ===================================================================================
//	Helpers
// ===================================================================================

//---------------------------------------------------------------
//
// GetInterface
//
// Query node for the T interface. If T isn't found check each
// parent of T until T is found. If none of the parents have a T
// return nil.
//
//---------------------------------------------------------------
template <class T, class N>
XInterfacePtr<T> GetInterface(const N& node)		
{
	XInterfacePtr<T> parent;
	IHierarchyPtr candidate = node;

	while (!parent && candidate) {
		parent = XInterfacePtr<T>(candidate);
		candidate = candidate->GetParent();
	}
	
	return parent;
}


//---------------------------------------------------------------
//
// GetConstInterface
//
// Const version of GetInterface.
//
//---------------------------------------------------------------
template <class T, class N>
XInterfacePtr<const T> GetConstInterface(const N& node)	
{
	XInterfacePtr<const T> parent;
	IConstHierarchyPtr candidate = node;

	while (!parent && candidate) {
		parent = XInterfacePtr<const T>(candidate);
		candidate = candidate->GetParent();
	}
	
	return parent;
} 


//---------------------------------------------------------------
//
// GetParent
//
// Like GetInterface except that node isn't checked for T (only
// the parents are queried).
//
//---------------------------------------------------------------
template <class T, class N>
XInterfacePtr<T> GetParent(const N& node)		// check node's parents for a T interface
{
	IConstHierarchyPtr child = node;
	XInterfacePtr<T> parent = GetInterface<T>(child->GetParent());
	
	return parent;
}


//---------------------------------------------------------------
//
// GetConstParent
//
// Const version of GetParent.
//
//---------------------------------------------------------------
template <class T, class N>
XInterfacePtr<const T> GetConstParent(const N& node)	
{
	IConstHierarchyPtr child = node;
	XInterfacePtr<const T> parent = GetConstInterface<T>(child->GetParent());
	
	return parent;
}


//---------------------------------------------------------------
//
// CallRepeatedMethod
//
// Calls the specified member function for each repeated interface
// on object.
//
//---------------------------------------------------------------
template <class I, class O>
void CallRepeatedMethod(const O* object, void (I::*method)())	
{
	XBoss* boss = object->GetBoss();
	for (XBoss::iterator iter = boss->begin(); iter != boss->end(); ++iter) {
		XInterfacePtr<I> interface(boss, iter);
		
		if (interface)
			((interface.Get())->*method)();
	}
}


//---------------------------------------------------------------
//
// CallRepeatedMethod
//
//---------------------------------------------------------------
template <class I, class O, class A>
void CallRepeatedMethod(const O* object, void (I::*method)(), A arg)	
{
	XBoss* boss = object->GetBoss();
	for (XBoss::iterator iter = boss->begin(); iter != boss->end(); ++iter) {
		XInterfacePtr<I> interface(boss, iter);
		
		if (interface)
			((interface.Get())->*method)(arg);
	}
}


//---------------------------------------------------------------
//
// MatchInterface
//
// Predicate used with IHierarchy::FindChild (see the comment in
// IHierarchy).
//
//---------------------------------------------------------------
template <class T>
bool DoMatchInterface(IHierarchyPtr node)
{
	XInterfacePtr<const T> ptr(node); 
	return ptr; 
}
										
template <class T>	// used like this: XCallback1<bool, IHierarchyPtr> predicate = MatchInterface<IScrollableControl>();
XCallback1<bool, IHierarchyPtr> MatchInterface()	// $$$ should be posible to use: XCallback1<bool, IHierarchyPtr> predicate(DoMatchInterface<IScrollableControl>); but CW 6 and 7 don't like this
{
	bool (*f)(IHierarchyPtr) = DoMatchInterface<T>;
	XCallback1<bool, IHierarchyPtr> predicate(f);
	return predicate;
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

