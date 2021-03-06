/*
 *  File:       XCreateDoc.cpp
 *  Summary:   	Default ICreateDoc implementation.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCreateDoc.cpp,v $
 *		Revision 1.2  2001/04/02 23:35:51  jesjones
 *		Tweaked for CW Pro 5.3
 *		
 *		Revision 1.1  2001/02/07 02:32:20  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <ICreateDoc.h>

#include <IDocument.h>
#include <IHierarchy.h>
#include <IReadXML.h>
#include <XURI.h>

namespace Whisper {


// ===================================================================================
//	class XCreateDoc
//!		Default ICreateDoc implementation.
// ===================================================================================
class XCreateDoc : public ICreateDoc {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XCreateDoc();
	
						XCreateDoc(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual IDocumentPtr OnCreate(IHierarchyPtr& parent);
};

DEFINE_INTERFACE_FACTORY(XCreateDoc)

//---------------------------------------------------------------
//
// XCreateDoc::~XCreateDoc
//
//---------------------------------------------------------------
XCreateDoc::~XCreateDoc()
{
}


//---------------------------------------------------------------
//
// XCreateDoc::XCreateDoc
//
//---------------------------------------------------------------
XCreateDoc::XCreateDoc(XBoss* boss)
{	
	ICreateDoc::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XCreateDoc::OnCreate
//
//---------------------------------------------------------------
IDocumentPtr XCreateDoc::OnCreate(IHierarchyPtr& parent)
{
	// Create the doc
	IDocumentPtr doc(L"Document");
	IHierarchy::NodePtr node(doc);		// need this for Pro 5.3
	parent->AddChild(node);
		
	// Create the window
	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	(void) persist->CreateBoss(uri, L"Document_Window", node);
	
	return doc;
}


}	// namespace Whisper
