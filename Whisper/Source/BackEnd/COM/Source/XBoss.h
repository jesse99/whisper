/*
 *  File:       XBoss.h
 *  Summary:    A class that contains a list of interface objects (clients shouldn't
 *				normally have to deal with these).
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBoss.h,v $
 *		Revision 1.6  2001/04/17 01:43:12  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.5  2001/03/05 05:40:36  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.4  2001/02/22 07:27:38  jesjones
 *		Renamed an argument from "interface" to "face" (Microsoft name pollution...).
 *		
 *		Revision 1.3  2000/12/11 01:30:03  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:26:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	 7/15/00	JDJ		Added IsPrototype.
 *		 <5>	 5/20/00	JDJ		Moved XInterface into its own file.
 *		 <4>	 5/09/00	JDJ		Added reverse iterators.
 *		 <3>	 4/23/00	JDJ		Added XBoss::RemoveInterface.
 *		 <2>	 4/23/00	JDJ		Replaced SInterface with XInterface. Added a GetInterface
 *									method to make the class more useful when iterating.
 *		 <1>	11/27/99	JDJ		Created
 */

#pragma once

#include <vector>

#include <XInterface.h>
#include <XReferenceCounted.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XPlugin;
class XXMLElement;


// ===================================================================================
//	class XBoss
//!		A class that contains a list of interface objects (clients mostly don't have to deal with these).
// ===================================================================================
class COM_EXPORT XBoss 
#if !GARBAGE_COLLECT_COM
	: public XReferenceCountedMixin
#endif
{

//-----------------------------------
//	Types
//
public:
	typedef std::vector<XInterface> 	Interfaces;
	typedef Interfaces::iterator 		iterator;
	typedef Interfaces::const_iterator 	const_iterator;

#if MSVC >= 1100
    typedef std::reverse_iterator<const_iterator, const XInterface>	const_reverse_iterator;
    typedef std::reverse_iterator<iterator, XInterface>				reverse_iterator;
#else
    typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;
    typedef std::reverse_iterator<iterator>			reverse_iterator;
#endif

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XBoss();

						XBoss(XPlugin* plugin, const XXMLElement* elem);
						
			XBoss* 		Clone() const;

private:
						XBoss(const XBoss& rhs);
			
			XBoss& operator=(const XBoss& rhs);
			
//-----------------------------------
//	New API
//
public:
	//! @name Interface Access
	//@{
			XImplementation* GetImplementation(const char* encodedName);
						/**< Returns nil if the interface can't be found. */
						
			XImplementation* GetImplementation();
						/**< Returns an arbitrary interface on the boss. */

			XImplementation* front();
			XImplementation* back();
	//@}

	//! @name Iterators
	//@{
			const_iterator begin() const							{return const_cast<XBoss*>(this)->begin();}
			const_iterator end() const								{return const_cast<XBoss*>(this)->end();}
			
			iterator 	begin();
			iterator 	end();
	//@}
			
	//! @name Reverse Iterators
	//@{
			reverse_iterator 		rbegin()						{ASSERT(!mDeleting); return reverse_iterator(this->end());}
			const_reverse_iterator 	rbegin() const					{ASSERT(!mDeleting); return const_reverse_iterator(this->end());}

			reverse_iterator 		rend()							{return reverse_iterator(this->begin());}
			const_reverse_iterator 	rend() const					{return const_reverse_iterator(this->begin());}
	//@}

	//! @name Boss Info
	//@{
			const std::wstring& GetName() const						{return mName;}
			XPlugin* 	GetPlugin() const							{return mPlugin;}
			bool 		IsSingleton() const							{return mSingleton;}
			bool 		IsAbstract() const							{return mAbstract;}
			bool 		IsPrototype() const							{return mPrototype;}
	//@}
			
	//! @name Interface Addition and Removal
	//@{
			iterator 	AddInterface(const std::wstring& encodedName, XImplementation* implementation);

			void 		RemoveInterface(iterator iter);
	//@}

//-----------------------------------
//	Inherited API
//
protected:
			void 		Invariant() const;

#if !GARBAGE_COLLECT_COM
	virtual void 		OnLastReference();
#endif

//-----------------------------------
//	Internal API
//
public:
			void 		ReplaceInterface(const XInterface& face);

protected:
			void 		DoAddInterfaces(const XXMLElement* parent);

			void 		DoAddParent(const XXMLElement* elem);
			
			void 		DoAddInterface(const XXMLElement* elem);
			
			bool 		DoIsValidTarget(const XXMLElement* elem);
			
//-----------------------------------
//	Member Data
//
private:
	XPlugin*					mPlugin;		// will be nil if the boss resides in the app
	std::wstring				mName;
	bool						mDeleting;
	bool						mSingleton;
	bool						mAbstract;
	bool						mPrototype;		// the object model stores prototypes for all the bosses which it clones when a new boss is created, you can use this flag to see if you're being destructed as a result of the object model shutting down
	
	mutable Interfaces			mInterfaces;	// maps encoded interface names to interface pointers/factories
	mutable bool 				mUnsorted;
	
	const char*					mCachedName;
	XImplementation*			mCachedInterface;
	
	mutable XCriticalSection	mMutex;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

