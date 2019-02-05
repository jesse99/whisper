/*
 *  File:       XInterface.h
 *  Summary:    The object used to create interfaces.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInterface.h,v $
 *		Revision 1.6  2001/03/17 12:25:34  jesjones
 *		Outlined XImplementation ctors. XImplementation::GetName is no longer debug only.
 *		
 *		Revision 1.5  2001/03/16 10:36:33  jesjones
 *		Exported the classes.
 *		
 *		Revision 1.4  2001/03/05 05:42:10  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/11 01:30:21  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:39:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 8/12/00	JDJ		Added a grouped flag to XInterface.
 *		 <4>	 8/11/00	JDJ		XImplementation has the implementation name in debug builds.
 *		 <3>	 7/07/00	JDJ		XImplementation will assert if someone tries to query
 *									for an interface as it's being constructed.
 *		 <2>	 5/21/00	JDJ		Made some changes to support interface aggregation.
 *		 <1>	 5/20/00	JDJ		Created (from XBoss.h)
 */

#pragma once

#include <XInterfaceMacros.h>
#include <XReferenceCounted.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XImplementation
//!		Internal class representing an interface implementation object.
//		Interfaces may be aggregated so multiple XInterface's can share the same
//		XImplementation object.
// ===================================================================================
class COM_EXPORT XImplementation : public XReferenceCountedMixin {

public:					
						~XImplementation();
					
						XImplementation();

						XImplementation(InterfaceFactory f, bool a, const std::wstring& n);

			XImplementation* Clone() const;
						
			const XUnknown* GetInterface(XBoss* boss) const		{return const_cast<XImplementation*>(this)->GetInterface(boss);}
			XUnknown* 	GetInterface(XBoss* boss);
						/**< Creates the interface if it's nil. */
		
			const XUnknown* FindInterface() const				{return mInterface;}
			XUnknown* 	FindInterface()							{return mInterface;}
			
			bool 		IsAggregated() const					{return mAggregated;}
						/**< Returns true if the implementation class is composed of more
						than one interface. */
		
			bool 		IsAbstract() const						{return mFactory == nil;}
						
			const std::wstring& GetName() const					{return mName;}

private:
						XImplementation(const XImplementation& rhs);
			XImplementation& operator=(const XImplementation& rhs);

private:
	mutable XUnknown*	mInterface;		
	InterfaceFactory	mFactory;
	bool 				mAggregated;
	bool 				mConstructing;			// true while the interface ctor is running
	std::wstring		mName;

};


// ===================================================================================
//	class XInterface
//!		Internal class representing an interface.
// ===================================================================================
class COM_EXPORT XInterface {

//-----------------------------------
//	Initialization/Destruction
//
public:	
					~XInterface();
					
					XInterface() : mImplementation(nil)	{}
					
					XInterface(const std::wstring& encodedName, XImplementation* implementation, bool grouped);
					
					XInterface(const XInterface& rhs);
					
			XInterface& operator=(const XInterface& rhs);
				
//-----------------------------------
// 	API
//
public:
	//! @name Accessors
	//@{
			const std::wstring& GetEncodedName() const			{return mEncodedName;}
					
			const XImplementation* GetImplementation() const	{ASSERT(mImplementation != nil); return mImplementation;}
			XImplementation* GetImplementation()				{ASSERT(mImplementation != nil); return mImplementation;}
			
			bool 		IsGrouped() const						{return mGrouped;}
	//@}

	//! @name Comparisons
	//@{
			bool 		operator==(const XInterface& rhs) const	{return mEncodedName == rhs.mEncodedName;}
			bool 		operator<(const XInterface& rhs) const	{return mEncodedName < rhs.mEncodedName;}
		
			bool 		operator==(const char* rhs) const		{return this->DoCompare(rhs) == 0;}
			bool 		operator!=(const char* rhs) const		{return this->DoCompare(rhs) != 0;}
			
			bool 		operator<(const char* rhs) const		{return this->DoCompare(rhs) < 0;}
	//@}
		
//-----------------------------------
// 	Internal API
//
private:
			int 		DoCompare(const char* rhs) const;
		
//-----------------------------------
//	Member Data
//
private:
	std::wstring		mEncodedName;
	XImplementation*	mImplementation;
	bool				mGrouped;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

