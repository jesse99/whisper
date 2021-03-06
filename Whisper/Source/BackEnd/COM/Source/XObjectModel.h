/*
 *  File:       XObjectModel.h
 *  Summary:    Singleton that manages bosses and interfaces.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XObjectModel.h,v $
 *		Revision 1.6  2001/03/06 10:07:42  jesjones
 *		Added a method to XObjectModel to write the bosses and interfaces to a log file.
 *		
 *		Revision 1.5  2001/03/05 05:42:45  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.4  2000/12/11 01:30:48  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/09 12:45:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <map>
#include <set>

#include <XAutoPtr.h>
#include <XBoss.h>
#include <XCriticalSection.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XInterface;
class XLogFile;
class XPlugin;
class XURI;
class XXMLDoc;
class XXMLElement;


// ===================================================================================
//	class XObjectModel
//!		Singleton that manages bosses and interfaces.
// ===================================================================================
class COM_EXPORT XObjectModel {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XObjectModel();

						XObjectModel();
						
	static XObjectModel* Instance();
	
			void 		Shutdown();

private:
						XObjectModel(const XObjectModel& rhs);
						
			XObjectModel& operator=(const XObjectModel& rhs);

//-----------------------------------
//	API
//
public:
			XBoss* 		CreateBoss(const std::wstring& name) const;
						/**< Throws if the boss can't be found. */
									
			void 		LoadBosses(XPlugin* plugin, const XURI& uri);
			void 		LoadBosses(XPlugin* plugin, const XXMLDoc& doc);
			void 		UnloadBosses(XPlugin* plugin);
						/**< Uri should point to a Bosses XML element. Plugin may be nil for 
						bosses that reside in the app. */
									
			void 		AddInterfaceFactory(const std::wstring& implementation, InterfaceFactory factory);
			void 		RemoveInterfaceFactory(const std::wstring& implementation);
						/**< Registers factories allowing us to create interfaces when given an
						implementation name. */

			InterfaceFactory GetInterfaceFactory(const std::wstring& implementation) const;
						/**< Returns nil if a factory cannot be found. */

			void 		AddInterfaceName(const std::wstring& encodedInterfaceName, const std::wstring& interfaceName);
						/**< EncodedInterfaceName is what type_info::name returns. It will include
						namespaces and possibly other stuff. InterfaceName is the name you
						use when defining bosses in the XML. */

			void 		WriteBosses(const char* fileName = "Bosses.txt");
						/**< Uses XLogFile to write out all the bosses and interfaces defined by
						the app. */

//-----------------------------------
//	Internal Types
//
private:
	typedef std::map<std::wstring, XBoss*> 			 BossTable;
	typedef std::map<std::wstring, InterfaceFactory> FactoryTable;
	typedef std::map<std::wstring, std::wstring> 	 InterfaceTable;

	typedef std::set<XBoss*> BossList;

//-----------------------------------
//	Internal API
//
public:
			std::wstring GetEncodedName(const std::wstring& interfaceName) const;

			const XBoss* GetBoss(const std::wstring& name) const;

#if DEBUG
#if !GARBAGE_COLLECT_COM
			void 		DoDeletingBoss(XBoss* boss);
#endif
			void 		DoWriteBoss(XLogFile& log, const XBoss* boss);
			void 		DoWriteInterfaces(XLogFile& log, const InterfaceTable& interfaces);
#endif

private:
			void 		DoAddBosses(XPlugin* plugin, const XXMLElement* root);

			void 		DoAddIn(XBoss* boss, const XBoss* addIn);
			
//-----------------------------------
//	Member Data
//
private:
	BossTable						mBossTable;			// mapping from boss name to boss prototypes (these are cloned when a new boss has to be created)
	FactoryTable					mFactoryTable;		// mapping from interface implementation names to interface factories
	InterfaceTable					mInterfaceTable;	// mapping from interface names to interface typeid names
	mutable XCriticalSection		mMutex;

#if DEBUG
	bool							mTerminated;
#if !GARBAGE_COLLECT_COM
	mutable BossList				mBossList;			// list of instantiated bosses for leak checking
#endif
#endif

	static XAutoPtr<XObjectModel>	msInstance;			
	static XCriticalSection			msCreateMutex;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

