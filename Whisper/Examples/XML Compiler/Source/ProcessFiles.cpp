/*
 *  File:       ProcessFiles.cpp
 *  Summary:   	The thread used to process files.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ProcessFiles.cpp,v $
 *		Revision 1.4  2001/04/21 03:29:57  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/01/21 00:37:39  jesjones
 *		Added GetNumFiles.
 *		
 *		Revision 1.2  2000/11/09 12:02:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "ProcessFiles.h"

#include <cctype>

#include <XError.h>
#include <XFile.h>
#include <XFileSystem.h>
#include <XFolderSpec.h>
#include <XIntConversions.h>
#include <XSaveXML.h>
#include <XStringUtils.h>
#include <XXMLParser.h>

#if WIN
	#include <objbase.h>
#endif


// ===================================================================================
//	class CProcessFiles
// ===================================================================================

//---------------------------------------------------------------
//
// CProcessFiles::~CProcessFiles
//
//---------------------------------------------------------------
CProcessFiles::~CProcessFiles()
{
}


//---------------------------------------------------------------
//
// CProcessFiles::CProcessFiles
//
//---------------------------------------------------------------
CProcessFiles::CProcessFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, ErrorCallback err) : XProcessFiles(fileFilter, folderFilter, specs, err, 128*1024L)
{
	mNumFiles = 0;
	mMesgPrefix = LoadAppString(L"Compiling: ");
}

						
//---------------------------------------------------------------
//
// CProcessFiles::OnRun
//
//---------------------------------------------------------------
void CProcessFiles::OnRun()
{
#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM) 		// need to initialize COM from each thread that resolves shortcuts...
	HRESULT result = CoInitializeEx(nil, COINIT_SPEED_OVER_MEMORY);
#elif WIN
	HRESULT result = CoInitialize(nil);
#endif

	Inherited::OnRun();
}


//---------------------------------------------------------------
//
// CProcessFiles::OnProcessFile
//
//---------------------------------------------------------------
void CProcessFiles::OnProcessFile(const XFileSpec& inSpec)	
{
#if MAC
	PRECONDITION(inSpec.GetType() == 'TEXT');
#endif

	XFileSpec outSpec(inSpec.GetParent(), inSpec.GetPrefix() + L".bXML");

	Whisper::Save(inSpec, outSpec);

	++mNumFiles;
}


//---------------------------------------------------------------
//
// CProcessFiles::OnException
//
//---------------------------------------------------------------
void CProcessFiles::OnException(const std::exception* e)
{
	std::wstring summary;
	
	const XXMLException* ee = dynamic_cast<const XXMLException*>(e);
	
	if (ee != nil)
		summary = LoadAppString(L"There was an error processing '#1'.", ee->GetURI().GetFile());
	else if (mSpec.GetName().length() == 0)
		summary = LoadWhisperString(L"There was an error processing the files.");
	else if (XFileSystem::IsFolder(mSpec))
		summary = LoadWhisperString(L"There was an error processing '#1'.", XFolderSpec(mSpec).GetName());
	else
		summary = LoadWhisperString(L"There was an error processing '#1'.", mSpec.GetName());

	std::wstring narrative = XError::Instance()->GetText(e);	
	if (ee != nil)
		narrative += LoadAppString(L" Line #1.", UInt32ToStr(ee->GetLine()));
	
	this->OnError(summary, narrative);
}



