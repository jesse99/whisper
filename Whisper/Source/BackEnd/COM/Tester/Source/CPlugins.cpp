/*
 *  File:       CPlugins.cpp
 *  Summary:   	Interface class for plugins.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPlugins.cpp,v $
 *		Revision 1.3  2001/04/21 02:24:49  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 08:56:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include "IPlugins.h"

#include <XInterfaceMacros.h>


// ===================================================================================
//	class CPlugins
// ===================================================================================
class CPlugins : public IPlugins {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CPlugins();

						CPlugins(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Register(const std::wstring& boss);
	virtual void 		UnRegister(const std::wstring& boss);

	virtual uint32 		GetNumPlugins() const;
	virtual const std::wstring& GetBoss(uint32 index) const;

//-----------------------------------
//	Member Data
//
private:
	vector<std::wstring>	mBosses;
};

DEFINE_INTERFACE_FACTORY(CPlugins)

//---------------------------------------------------------------
//
// CPlugins::~CPlugins
//
//---------------------------------------------------------------
CPlugins::~CPlugins()
{
}


//---------------------------------------------------------------
//
// CPlugins::CPlugins
//
//---------------------------------------------------------------
CPlugins::CPlugins(XBoss* boss)
{
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CPlugins::Register
//
//---------------------------------------------------------------
void CPlugins::Register(const std::wstring& boss)
{
	PRECONDITION(find(mBosses.begin(), mBosses.end(), boss) == mBosses.end());
	PRECONDITION(boss.length() > 0);
	
	mBosses.push_back(boss);
}


//---------------------------------------------------------------
//
// CPlugins::UnRegister
//
// Plugins are normally unloaded in reverse order so this shouldn't
// be too inefficient.
//
//---------------------------------------------------------------
void CPlugins::UnRegister(const std::wstring& boss)
{
	PRECONDITION(boss.length() > 0);
	
	vector<std::wstring>::iterator iter = find(mBosses.begin(), mBosses.end(), boss);
	PRECONDITION(iter != mBosses.end());
	
	mBosses.erase(iter);
}


//---------------------------------------------------------------
//
// CPlugins::GetNumPlugins
//
//---------------------------------------------------------------
uint32 CPlugins::GetNumPlugins() const
{
	uint32 count = mBosses.size();
	
	return count;
}


//---------------------------------------------------------------
//
// CPlugins::GetBoss
//
//---------------------------------------------------------------
const std::wstring& CPlugins::GetBoss(uint32 index) const
{
	PRECONDITION(index < mBosses.size());
	
	const std::wstring& boss = mBosses[index];
	
	return boss;
}
