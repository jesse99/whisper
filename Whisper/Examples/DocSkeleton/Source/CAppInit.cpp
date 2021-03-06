/*
 *  File:       CAppInit.cpp
 *  Summary:   	An IInit implementation that handles the cross platform application initialization.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CAppInit.cpp,v $
 *		Revision 1.3  2001/02/17 10:24:46  jesjones
 *		Tweaked some comments.
 *		
 *		Revision 1.2  2001/02/07 02:22:14  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.1  2000/12/31 04:23:03  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include <IInit.h>

#include <IDocuments.h>
#include <XFileDialog.h>
#include <XFileFilter.h>
#include <XFileSpec.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class CFileFilter	
//!		Used to filter out files in the system open file dialogs.	
// ===================================================================================
class CFileFilter : public XFileFilter {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CFileFilter();
	
						CFileFilter();
												
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		operator()(const XFileSpec& spec) const;
};

//---------------------------------------------------------------
//
// CFileFilter::~CFileFilter
//
//---------------------------------------------------------------
CFileFilter::~CFileFilter()
{
}

	
//---------------------------------------------------------------
//
// CFileFilter::CFileFilter
//
//---------------------------------------------------------------
CFileFilter::CFileFilter()
{
}

												
//---------------------------------------------------------------
//
// CFileFilter::operator()
//
//---------------------------------------------------------------
bool CFileFilter::operator()(const XFileSpec& spec) const
{
	bool ok = false;

	if (spec.GetExtension() == L"xml" || spec.GetExtension() == L"dcs")
		ok = true;

#if MAC
	else if (spec.GetType() == 'TEXT' || spec.GetType() == 'DcSD')
		ok = true;
#endif
		
	return ok;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class CAppInit
//!		An IInit implementation that handles the cross platform application initialization.
// ===================================================================================
class CAppInit : public IInit {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CAppInit();
	
  						CAppInit(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		OnInit();
	virtual void		OnCleanup();
};

DEFINE_INTERFACE_FACTORY(CAppInit)

//---------------------------------------------------------------
//
// CAppInit::~CAppInit
//
//---------------------------------------------------------------
CAppInit::~CAppInit()
{
}


//---------------------------------------------------------------
//
// CAppInit::CAppInit
//
//---------------------------------------------------------------
CAppInit::CAppInit(XBoss* boss) 
{
	IInit::DoSetBoss(boss);
}
	
#if __MWERKS__
#pragma mark ~
#endif
	
//---------------------------------------------------------------
//
// CAppInit::OnInit		
//
//---------------------------------------------------------------
void CAppInit::OnInit()
{	
#if MAC && 0						// you might want to enable this if your app makes heavy use of timers
	IApplicationPtr app(this);
	app->SetSleepTime(20);			
#endif

	IDocumentsPtr documents(this);
	documents->SetFileFilter(new CFileFilter);
	
	XFileDialogOptions& options = documents->GetFileOptions();
	options.AddType(XFileType(L"XML", L"xml", 'DcSk', 'TEXT'));
	options.AddType(XFileType(L"DocSkeleton", L"dcs", 'DcSk', 'DcSD'));	
}


//---------------------------------------------------------------
//
// CAppInit::OnCleanup		
//
//---------------------------------------------------------------
void CAppInit::OnCleanup()
{
}
	

