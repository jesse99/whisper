/*
 *  File:       XWriteXML.cpp
 *  Summary:   	Interface used to convert interfaces and bosses into XML.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XWriteXML.cpp,v $
 *		Revision 1.4  2001/04/27 04:38:09  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2000/12/14 08:38:10  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:59:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IWriteXML.h>

#include <XAutoPtr.h>
#include <XInterfaceMacros.h>
#include <XObjectModel.h>
#include <XXMLItems.h>

namespace Whisper {


// ===================================================================================
//	class XWriteXML
//!		Interface used to convert interfaces and bosses into XML.
// ===================================================================================
class XWriteXML : public IWriteXML {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XWriteXML();
	
						XWriteXML(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual XXMLElement* CreateElement(const XBoss* boss, const std::wstring& name = L"Boss");	
	
	virtual XXMLElement* CreateElement(const XBoss* boss, XBoss::const_iterator iter);	

	virtual void 		AddWriter(const std::wstring& interfaceName, WriteProc proc);

//-----------------------------------
//	Types
//
private:
	typedef std::map<std::wstring, WriteProc> WriterTable;

//-----------------------------------
//	Member Data
//
private:
	WriterTable		mWriters;
};

DEFINE_INTERFACE_FACTORY(XWriteXML)

//---------------------------------------------------------------
//
// XWriteXML::~XWriteXML
//
//---------------------------------------------------------------
XWriteXML::~XWriteXML()
{
}


//---------------------------------------------------------------
//
// XWriteXML::XWriteXML
//
//---------------------------------------------------------------
XWriteXML::XWriteXML(XBoss* boss)
{
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XWriteXML::CreateElement (XBoss*, wstring)
//
//---------------------------------------------------------------
XXMLElement* XWriteXML::CreateElement(const XBoss* boss, const std::wstring& name)
{
	PRECONDITION(boss != nil);
	
	XAutoPtr<XXMLElement> element(new XXMLElement(name));
	
	XBoss::const_iterator iter = boss->begin();
	while (iter != boss->end()) {
		XXMLElement* child = this->CreateElement(boss, iter);
		++iter;
		
		if (child != nil)
			element->AppendItem(child);
	}
	
	return element.Release();
}

	
//---------------------------------------------------------------
//
// XWriteXML::CreateElement (const XBoss*, XBoss::const_iterator)
//
//---------------------------------------------------------------
XXMLElement* XWriteXML::CreateElement(const XBoss* boss, XBoss::const_iterator iter)
{
	PRECONDITION(boss != nil);
	
	XXMLElement* element = nil;
	
	const std::wstring& name = iter->GetEncodedName();
	
	WriterTable::iterator iter2 = mWriters.find(name);
	if (iter2 != mWriters.end()) {
		WriteProc writer = iter2->second;
		element = writer(boss, iter);					
	} 
		
	return element;
}


//---------------------------------------------------------------
//
// XWriteXML::AddWriter
//
//---------------------------------------------------------------
void XWriteXML::AddWriter(const std::wstring& interfaceName, WriteProc proc)
{
	PRECONDITION(interfaceName.length() > 0);
	PRECONDITION(proc != nil);
	
	std::wstring encodedName = XObjectModel::Instance()->GetEncodedName(interfaceName);
	ASSERT(encodedName.length() > 0);
	
	(void) mWriters.insert(WriterTable::value_type(encodedName, proc));	// allow overwriting existing writers
}


}	// namespace Whisper
