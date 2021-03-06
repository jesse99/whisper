/*
 *  File:       IPlugins.h
 *  Summary:   	Interface class for effect plugins.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IPlugins.h,v $
 *		Revision 1.2  2000/11/09 09:05:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	12/04/99	JDJ		Created
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class IPlugins
// ===================================================================================
class IPlugins : public XUnknown {

//-----------------------------------
//	New API
//
public:
	virtual void 		Register(const std::wstring& boss) = 0;
	virtual void 		UnRegister(const std::wstring& boss) = 0;

	virtual uint32 		GetNumPlugins() const = 0;
	virtual const std::wstring& GetBoss(uint32 index) const = 0;
};

typedef XInterfacePtr<IPlugins> IPluginsPtr;
typedef XInterfacePtr<const IPlugins> IConstPluginsPtr;
