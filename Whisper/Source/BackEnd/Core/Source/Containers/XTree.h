/*
 *  File:		XTree.h
 *  Summary:	Templatized tree class allowing an arbitrary number of children.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTree.h,v $
 *		Revision 1.5  2001/04/17 01:45:05  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/03/05 05:44:19  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:57:03  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:57:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <list>

#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	Traversal Functions
// ===================================================================================
template <class TREE, class FUNCTOR>
void 		PostOrderTraversal(TREE& tree, FUNCTOR functor);
			// Depth first traversal over children and then the node. functor needs
			// to have a single argument that's compatible with a TTree reference.
			// $$ It'd be nice if functor was a reference, but CW Pro1 gave me an error
			// $$Êwhen I tried to use a C function.

template <class TREE, class FUNCTOR>
void 		PreOrderTraversal(TREE& tree, FUNCTOR functor);	
			// Depth first traversal over node and then the children.

template <class TREE, class FUNCTOR>
void 		InOrderTraversal(TREE& tree, FUNCTOR functor);	
			// Depth first traversal over left child, node, and then right child. 
			// (Only valid for binary trees).

template <class TREE, class FUNCTOR>
void 		BreadthTraversal(TREE& tree, FUNCTOR functor);	
			// Breadth first traversal.


// ===================================================================================
//	class XTree
//!		Templatized tree class allowing an arbitrary number of children.
// ===================================================================================
template <class T> class XTree {
	
//-----------------------------------
//	Types
//
public:
	typedef std::list<XTree<T>*> 		Children;
	
	typedef T							value_type;
	typedef value_type& 				reference;
    typedef const value_type&			const_reference;    
    typedef value_type*					pointer;
    typedef const value_type*			const_pointer;

	typedef STD::size_t					size_type;
	typedef STD::ptrdiff_t				difference_type;

	typedef Children::iterator			iterator;
	typedef Children::const_iterator	const_iterator;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual					~XTree();

							XTree();

							XTree(const T& node);

private:
							XTree(const XTree& rhs);
							
			XTree& 			operator=(const XTree& rhs);
												
//-----------------------------------
//	API
//
public:
	//! @name Size
	//@{
			uint32 			size() const							{return (uint32) mCount;}
			
			bool 			empty() const							{return mCount == 0;}
			
			void 			clear();
	//@}

	//! @name Root
	//@{
			XTree* 			GetRoot();

			const XTree* 	GetRoot() const;
			
			bool 			IsRoot() const							{return mParent == nil;}
	//@}

	//! @name Node
	//@{
			XTree* 			GetParent()								{return mParent;}

			const XTree* 	GetParent() const						{return mParent;}

			bool 			HasNode() const							{return mNode != nil;}

			T& 				GetNode()								{ASSERT(mNode != nil); return mNode->data;}
			
			const T&		GetNode() const							{ASSERT(mNode != nil); return mNode->data;}
			
			void			SetNode(const T& node);
			
			bool 			IsLeaf() const							{return mNode != nil ? mNode->children.empty() : true;}
	//@}

	//! @name Siblings
	//@{
			XTree* 			GetLeftSibling();	
							// Returns nil if 'this' is the leftmost sibling.

			const XTree* 	GetLeftSibling() const;	

			XTree* 			GetRightSibling();	

			const XTree* 	GetRightSibling() const;	

			bool 			IsSibling(const XTree* tree) const;
	//@}

	//! @name Children Getters
	//@{
			uint32 			GetNumChildren() const					{return mNode != nil ? mNode->children.size() : 0;}

			bool 			HasChildren() const						{return mNode != nil ? !mNode->children.empty() : false;}
			
			bool 			IsChild(const XTree* tree) const;

			XTree* 			GetLeftChild()							{ASSERT(this->HasChildren()); return mNode->children.front();}

			const XTree* 	GetLeftChild() const					{ASSERT(this->HasChildren()); return mNode->children.front();}

			XTree* 			GetRightChild()							{ASSERT(this->HasChildren()); return mNode->children.back();}

			const XTree* 	GetRightChild() const					{ASSERT(this->HasChildren()); return mNode->children.back();}

			const Children& GetChildren() const						{ASSERT(this->HasChildren()); return mNode->children;}
							/**< Returns the immediate children. */
	//@}

	//! @name Children Setters
	//@{
			void 			AddChildToLeft(XTree* tree);
							/**< Merge a subtree into 'this'. */
			
			void 			AddChildToRight(XTree* tree);
			
			void 			RemoveChild(XTree* tree);
							/**< Removes tree from 'this' (ie after this call there 
							will be two disjoint trees). */
	//@}
							
	//! @name Iterating (over immediate children)
	//@{
			iterator 		begin()									{return mNode != nil ? mNode->children.begin() : iterator();}
			const_iterator 	begin() const							{return mNode != nil ? mNode->children.begin() : iterator();}
			
			iterator 		end()									{return mNode != nil ? mNode->children.end() : iterator();}
			const_iterator 	end() const								{return mNode != nil ? mNode->children.end() : iterator();}
	//@}

	//! @name Comparisons
	//@{
			bool 			operator==(const XTree& rhs) const;
			
			bool 			operator!=(const XTree& rhs) const		{return !(this->operator==(rhs));}
	//@}

//-----------------------------------
//	Internal API
//
protected:
			void 			Invariant() const;

			void 			DoAdjustCounts(int32 delta);
		
//-----------------------------------
//	Internal Types
//
protected:
	struct SNode {
		T			data;
		Children	children;
							
					SNode(const T& x) : data(x)	{}
					
		bool 		operator==(const SNode& rhs) const	{return data == rhs.data;}

		bool 		operator!=(const SNode& rhs) const	{return data != rhs.data;}
	};
	
//-----------------------------------
//	Member Data
//
protected:
	XTree*	mParent;
	int32	mCount;
	
	SNode*	mNode;
};


}	// namespace Whisper


#include <XTree.inc>
