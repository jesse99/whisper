/*
 *  File:		XTree.inc
 *  Summary:	Templatized tree class allowing an arbitrary number of children.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTree.inc,v $
 *		Revision 1.3  2001/04/17 01:45:10  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 12:57:37  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 6/30/99	JDJ		Fixed the mCount check in the gIntenseDebugging section of the Invariant.
 *									RemoveChild subtracts instead of adding delta to mCount.
 *		 <4>	 6/18/99	JDJ		Made StackSpace checks #if MAC.
 *		 <3>	 6/17/99	JDJ		AddChildToLeft, AddChildToRight, and RemoveChild correctly
 *									update mCount when the subtree has more than one node.
 *		 <2>	 6/17/99	JDJ		Fixed several syntax errors.
 *		 <1>	11/28/97	JDJ		Created (from Raven)
 */

#include <XExceptions.h>

#include <algorithm>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// DoPushBack
//
//---------------------------------------------------------------
template <class COLLECTION, class ITERATOR>
void DoPushBack(COLLECTION& c, ITERATOR first, ITERATOR last)
{
    while (first != last)
    	c.push_back(*first++);
}


//---------------------------------------------------------------
//
// DoCompareTree
//
//---------------------------------------------------------------
template <class T>
bool DoCompareTree(XTree<T> const*& lhs, XTree<T> const*& rhs)
{
	ASSERT(lhs != nil);
	ASSERT(rhs != nil);
	
	return *lhs == *rhs;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	ZTraverseHelper
//		Functor used by the traversal functions when calling for_each.
// ===================================================================================
template <class TREE, class TRAVERSE, class FUNCTOR>
class ZTraverseHelper {

public:
			ZTraverseHelper(TRAVERSE t, FUNCTOR& f) : mTraverseFunction(t), mFunctor(f)	{}
	
	void 	operator()(TREE* tree) const	{ASSERT(tree != nil); mTraverseFunction(*tree, mFunctor);}
    
private:
	TRAVERSE mTraverseFunction;
	FUNCTOR& mFunctor;
};

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// PostOrderTraversal
//
// Note that we call for_each instead of doing the iteration
// ourselves because we don't know which kind of iterator to use
// (TREE may be const or non-const). Also we don't use bind2nd
// because it expects the function to be a functor descending
// from binary_function (it's much less hassle to just use
// ZTraverseHelper).
//
//---------------------------------------------------------------
template <class TREE, class FUNCTOR>
void PostOrderTraversal(TREE& tree, FUNCTOR functor)
{
	typedef void (*TraversalProcPtr)(TREE&, FUNCTOR);

#if MAC
	if (StackSpace() < 1024)
		throw TRuntimeException("Internal Error: Ran out of stack space in PostOrderTraversal.");
#endif

	if (!tree.empty()) {
		ZTraverseHelper<TREE, TraversalProcPtr, FUNCTOR> traverse(PostOrderTraversal, functor);
		for_each(tree.begin(), tree.end(), traverse);
	
		functor(tree);
	}
}


//---------------------------------------------------------------
//
// PreOrderTraversal
//
//---------------------------------------------------------------
template <class TREE, class FUNCTOR>
void PreOrderTraversal(TREE& tree, FUNCTOR functor)
{
	typedef void (*TraversalProcPtr)(TREE&, FUNCTOR);
	
#if MAC
	if (StackSpace() < 1024)
		throw TRuntimeException("Internal Error: Ran out of stack space in PreOrderTraversal.");
#endif
	
	if (!tree.empty()) {
		functor(tree);

		ZTraverseHelper<TREE, TraversalProcPtr, FUNCTOR> traverse(PreOrderTraversal, functor);
		for_each(tree.begin(), tree.end(), traverse);
	}
}
	

//---------------------------------------------------------------
//
// InOrderTraversal
//
//---------------------------------------------------------------
template <class TREE, class FUNCTOR>
void InOrderTraversal(TREE& tree, FUNCTOR functor)
{
#if MAC
	if (StackSpace() < 1024)
		throw TRuntimeException("Internal Error: Ran out of stack space in InOrderTraversal.");
#endif
	
	if (tree.HasChildren()) {
		ASSERT(tree.GetNumChildren() == 2);
		
		InOrderTraversal(*(tree.GetLeftChild()), functor);
		
		functor(tree);

		InOrderTraversal(*(tree.GetRightChild()), functor);
	}
}
	

//---------------------------------------------------------------
//
// BreadthTraversal
//
//---------------------------------------------------------------
template <class TREE, class FUNCTOR>
void BreadthTraversal(TREE& tree, FUNCTOR functor)
{	
	if (!tree.empty()) {
		std::list<TREE*> nodes;
		
		nodes.push_back(&tree);
		
		while (!nodes.empty()) {
			TREE* subTree = nodes.front();
			nodes.pop_front();
			
			functor(*subTree);
			
#ifdef MSIPL_MEMBER_TEMPLATE
			nodes.insert(nodes.end(), subTree->begin(), subTree->end());
#else
			DoPushBack(nodes, subTree->begin(), subTree->end());	// have to use a function since we don't know if TREE is const or not
#endif
		}
	}
}	

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XTree
// ===================================================================================

//---------------------------------------------------------------
//
// XTree::~XTree
//
//---------------------------------------------------------------
template <class T>
XTree<T>::~XTree()
{
	if (mParent != nil)
		mParent->RemoveChild(this);
		
	if (mNode != nil) {		
		while (!mNode->children.empty()) {
			XTree* subtree = mNode->children.back();
			
			delete subtree;
		}

		delete mNode;
	}
}


//---------------------------------------------------------------
//
// XTree::XTree ()
//
//---------------------------------------------------------------
template <class T>
XTree<T>::XTree()
{
	mParent = nil;
	mCount  = 0;
	mNode   = nil;
}


//---------------------------------------------------------------
//
// XTree::XTree (T)
//
//---------------------------------------------------------------
template <class T>
XTree<T>::XTree(const T& node)
{
	mParent = nil;
	mCount  = 1;
	mNode   = new SNode(node);
}


//---------------------------------------------------------------
//
// XTree::clear
//
//---------------------------------------------------------------
template <class T>
void XTree<T>::clear()	
{
	PRECONDITION(true);
	
	if (mNode != nil) {
		delete mNode;
		mNode = nil;
			
		this->DoAdjustCounts(-mCount);
	}

	POSTCONDITION(this->IsEmpty());
}


//---------------------------------------------------------------
//
// XTree::GetRoot
//
//---------------------------------------------------------------
template <class T>
XTree<T>* XTree<T>::GetRoot()
{
	XTree* root = this;
	
	while (root->mParent != nil)
		root = root->mParent;
		
	return root;
}


//---------------------------------------------------------------
//
// XTree::GetRoot const
//
//---------------------------------------------------------------
template <class T>
const XTree<T>* XTree<T>::GetRoot() const
{
	const XTree* root = this;
	
	while (root->mParent != nil)
		root = root->mParent;
		
	return root;
}


//---------------------------------------------------------------
//
// XTree::SetNode
//
//---------------------------------------------------------------
template <class T>
void XTree<T>::SetNode(const T& node)
{
	PRECONDITION(true);
	
	if (mNode != nil)
		mNode->data = node;
		
	else {
		mNode = new SNode(node);
				
		this->DoAdjustCounts(+1);
	}		

	POSTCONDITION(mNode->data == node);
}


//---------------------------------------------------------------
//
// XTree::GetLeftSibling
//
//---------------------------------------------------------------
template <class T>
XTree<T>* XTree<T>::GetLeftSibling()
{
	ASSERT(mParent != nil);
	ASSERT(mParent->mNode != nil);
	
	XTree* sibling = nil;
	
	Children& siblings = mParent->mNode->children;
	
	Children::iterator iter = std::find(siblings.begin(), siblings.end(), this);
	ASSERT(iter != siblings.end());
	
	if (iter != siblings.begin()) {
		--iter;
		
		sibling = *iter;
	}
	
	return sibling;
}
	

//---------------------------------------------------------------
//
// XTree::GetLeftSibling const
//
//---------------------------------------------------------------
template <class T>
const XTree<T>* XTree<T>::GetLeftSibling() const
{
	ASSERT(mParent != nil);
	ASSERT(mParent->mNode != nil);
	
	const XTree* sibling = nil;
	
	const Children& siblings = mParent->mNode->children;
	
	Children::const_iterator iter = std::find(siblings.begin(), siblings.end(), this);
	ASSERT(iter != siblings.end());
	
	if (iter != siblings.begin()) {
		--iter;
		
		sibling = *iter;
	}
	
	return sibling;
}
	

//---------------------------------------------------------------
//
// XTree::GetRightSibling
//
//---------------------------------------------------------------
template <class T>
XTree<T>* XTree<T>::GetRightSibling()
{
	ASSERT(mParent != nil);
	ASSERT(mParent->mNode != nil);
	
	XTree* sibling = nil;
	
	Children& siblings = mParent->mNode->children;
	
	Children::iterator iter = std::find(siblings.begin(), siblings.end(), this);
	ASSERT(iter != siblings.end());
	
	++iter;
	
	if (iter != siblings.end())
		sibling = *iter;
	
	return sibling;
}
	

//---------------------------------------------------------------
//
// XTree::GetRightSibling const
//
//---------------------------------------------------------------
template <class T>
const XTree<T>* XTree<T>::GetRightSibling() const
{
	ASSERT(mParent != nil);
	ASSERT(mParent->mNode != nil);
	
	const XTree* sibling = nil;
	
	const Children& siblings = mParent->mNode->children;
	
	Children::const_iterator iter = std::find(siblings.begin(), siblings.end(), this);
	ASSERT(iter != siblings.end());
	
	++iter;
	
	if (iter != siblings.end())
		sibling = *iter;
	
	return sibling;
}
	

//---------------------------------------------------------------
//
// XTree::IsSibling
//
//---------------------------------------------------------------
template <class T>
bool XTree<T>::IsSibling(const XTree* tree) const
{
	ASSERT(tree != nil);
	ASSERT(mParent != nil);
	ASSERT(mParent->mNode != nil);
		
	bool is = false;
	
	if (tree->mParent != nil) {		
		const Children& siblings = mParent->mNode->children;
		
		Children::const_iterator iter = std::find(siblings.begin(), siblings.end(), tree);
		is = iter != siblings.end();
	}
		
	return is;
}


//---------------------------------------------------------------
//
// XTree::IsChild
//
//---------------------------------------------------------------
template <class T>
bool XTree<T>::IsChild(const XTree* tree) const
{
	ASSERT(tree != nil);
	
	bool is = false;
	
	if (mNode != nil && tree->mParent != nil) {		
		Children::const_iterator iter = std::find(this->begin(), this->end(), tree);
		is = iter != this->end();
	}
		
	return is;
}


//---------------------------------------------------------------
//
// XTree::AddChildToLeft
//
//---------------------------------------------------------------
template <class T>
void XTree<T>::AddChildToLeft(XTree* tree)	
{
	PRECONDITION(tree != nil);
	ASSERT(tree->mParent == nil);
	ASSERT(mNode != nil);				// need to have a valid node before adding children!
	
	mNode->children.push_front(tree);
	tree->mParent = this;
	
	this->DoAdjustCounts(tree->mCount);

	POSTCONDITION(this->IsChild(tree));
}


//---------------------------------------------------------------
//
// XTree::AddChildToRight
//
//---------------------------------------------------------------
template <class T>
void XTree<T>::AddChildToRight(XTree* tree)	
{
	PRECONDITION(tree != nil);
	ASSERT(tree->mParent == nil);
	ASSERT(mNode != nil);				// need to have a valid node before adding children!
	
	mNode->children.push_back(tree);
	tree->mParent = this;
	
	this->DoAdjustCounts(tree->mCount);

	POSTCONDITION(this->IsChild(tree));
}


//---------------------------------------------------------------
//
// XTree::RemoveChild
//
//---------------------------------------------------------------
template <class T>
void XTree<T>::RemoveChild(XTree* tree)	
{
	PRECONDITION(tree != nil);
	ASSERT(tree->mParent == this);
	ASSERT(mNode != nil);
		
	Children::iterator iter = std::find(this->begin(), this->end(), tree);
	ASSERT(iter != this->end());

	int32 count = tree->mCount;
	mNode->children.erase(iter);
	tree->mParent = nil;
	
	this->DoAdjustCounts(-count);

	POSTCONDITION(!this->IsChild(tree));
}

	
//---------------------------------------------------------------
//
// XTree::operator==
//
//---------------------------------------------------------------
template <class T>
bool XTree<T>::operator==(const XTree<T>& rhs) const
{
#if MAC
	if (StackSpace() < 1024)
		throw TRuntimeException("Internal Error: Ran out of stack space in XTree::operator==.");
#endif
		
	bool equals = mCount == rhs.mCount;
	
	if (equals && mNode != nil) {
		equals = mNode->data == rhs.mNode->data;
		
		if (equals)
			equals = equal(this->begin(), this->end(), rhs.begin(), DoCompareTree);
	}
	
	return equals;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XTree::Invariant
//
//---------------------------------------------------------------
template <class T>
void XTree<T>::Invariant() const
{	
	if (mNode == nil)
		ASSERT(mCount == 0);
	else
		ASSERT(mCount >= 1);

	if (mParent != nil) {
		ASSERT(mParent->mCount > mCount);
		ASSERT(mParent->mNode != nil);
	}

#if DEBUG	
	if (gIntenseDebugging) {
		if (mParent != nil) 
			ASSERT(std::find(mParent->begin(), mParent->end(), this) != mParent->end());

		if (mNode != nil) {
			int32 count = 1;
			const_iterator iter = this->begin();
			while (iter != this->end()) {
				XTree* subtree = *iter++;
				
				if (subtree->mNode != nil)
					count += subtree->mCount;
			}
			ASSERT(count == mCount);
		}
	}
#endif
}


//---------------------------------------------------------------
//
// XTree::DoAdjustCounts
//
//---------------------------------------------------------------
template <class T>
void XTree<T>::DoAdjustCounts(int32 delta)
{
	ASSERT(delta != 0);
	
	XTree* parent = this;
	
	do {
		parent->mCount += delta;
		
		parent = parent->mParent;
	} while (parent != nil);
}


}	// namespace Whisper

