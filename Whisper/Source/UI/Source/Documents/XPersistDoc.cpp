/*
 *  File:       XPersistDoc.cpp
 *  Summary:   	Interface used with documents that can handle multiple file formats. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XPersistDoc.cpp,v $
 *		Revision 1.4  2000/12/31 04:49:47  jesjones
 *		Allow for no previews using '????'.
 *		
 *		Revision 1.3  2000/12/14 08:36:51  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:46:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 8/29/00	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <IPersistDoc.h>

#include <IApplication.h>
#include <IDocReader.h>
#include <IDocWriter.h>
#include <XExceptions.h>
#include <XFileSpec.h>
#include <XLocker.h>
#include <XStringUtils.h>
#include <XTranscode.h>

#if MAC
	#include <Navigation.h>
	#include <MAEDesc.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XPersistDoc
//!		Interface used with documents that can handle multiple file formats. 
// ===================================================================================
class XPersistDoc : public IPersistDoc {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XPersistDoc();
	
  						XPersistDoc(XBoss* boss);
  						
//-----------------------------------
//	Inherited API
//
public:
	virtual void		HandleWrite(OSType type, const XFileSpec& spec);
	virtual void 		HandleRead(const XFileSpec& spec);
	
	virtual XSaveReply 	SaveFile(const std::wstring& prompt, const std::wstring& fileName);
	
	virtual bool 		CanReplace(const XSaveReply& reply);

#if MAC
	virtual OSType 		GetDefaultCreator() const;
	virtual OSType 		GetDefaultType() const;

	virtual void 		SavePreview(const XFileSpec& spec);
#endif
					
//-----------------------------------
//	Internal API
//
private:
			IDocWriterPtr DoFindWriter(OSType type, const std::wstring& extension);
};

DEFINE_INTERFACE_FACTORY(XPersistDoc)

//---------------------------------------------------------------
//
// XPersistDoc::~XPersistDoc
//
//---------------------------------------------------------------
XPersistDoc::~XPersistDoc()
{
}


//---------------------------------------------------------------
//
// XPersistDoc::XPersistDoc
//
//---------------------------------------------------------------
XPersistDoc::XPersistDoc(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XPersistDoc::HandleWrite
//
//---------------------------------------------------------------
void XPersistDoc::HandleWrite(OSType type, const XFileSpec& spec)
{
	IDocWriterPtr writer = this->DoFindWriter(type, spec.GetExtension());
			
	writer->Write(type, spec);
}


//---------------------------------------------------------------
//
// XPersistDoc::HandleRead
//
//---------------------------------------------------------------
void XPersistDoc::HandleRead(const XFileSpec& spec)
{
	IDocReaderPtr reader;
	
	OSType type = '????';
	std::wstring extension = spec.GetExtension();
#if MAC
	type = spec.GetType();
#endif

	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !reader) {
		IDocReaderPtr candidate(boss, iter);
		++iter;
		
		if (candidate && candidate->CanRead(type, extension))
			reader = candidate;
	}
	
	if (!reader)			// should never happen
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Can't find a load method.")));	
		
	reader->Read(spec);
}

	
//---------------------------------------------------------------
//
// XPersistDoc::SaveFile
//
//---------------------------------------------------------------
XSaveReply XPersistDoc::SaveFile(const std::wstring& prompt, const std::wstring& fileName)
{
	IApplicationPtr app(L"Application");
	
	XFileDialogOptions options;
	options.SetAppName(app->GetName());
	options.SetMessage(prompt);
	options.SetDefaultFileName(fileName);
	options.SetPrefKey('Doc ');
	
	bool setDefault = false;
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end()) {
		IDocWriterPtr writer(boss, iter);
		++iter;
		
		if (writer && writer->CanWrite()) {
			const XFileType& type = writer->GetType();
			options.AddType(type);
			
#if WIN
			if (!setDefault) {
				options.SetDefaultExtension(type.GetExtension());
				setDefault = true;
			}
#endif
		}
	}
			
	XSaveReply reply = XFileDialog::SaveFile(options);
	
	return reply;
}

	
//---------------------------------------------------------------
//
// XPersistDoc::CanReplace
//
//---------------------------------------------------------------
bool XPersistDoc::CanReplace(const XSaveReply& reply)
{
	bool can = false;
	
	XFileSpec target = reply.GetTarget();
	
	std::wstring extension = target.GetExtension();
	OSType type = '????';
#if MAC
	type = target.GetType();
#endif
	
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !can) {
		IDocWriterPtr writer(boss, iter);
		++iter;
		
		if (writer && writer->CanWrite()) {
			const XFileType& fileType = writer->GetType();
#if MAC
			can = fileType.GetType() == type;
#else
			can = fileType.Match(extension, '????') > 0;
#endif
		}
	}
		
	return can;
}


//---------------------------------------------------------------
//
// XPersistDoc::SavePreview
//
//---------------------------------------------------------------
#if MAC
void XPersistDoc::SavePreview(const XFileSpec& spec)
{
	IDocWriterPtr writer = this->DoFindWriter(spec.GetType(), spec.GetExtension());

#if UNIVERSAL_INTERFACES_VERSION >= 0x0330
	OSType type = writer->GetPreviewType();
	
	OSErr err = noErr;
	MAEDesc desc(typeFSS, &spec.GetOSSpec(), sizeof(FSSpec));
	
	if (type != 0 && type != '????') {
		XHandle data = writer->GetPreviewData();
		
		XLocker lock(data);
		err = ::NavCreatePreview(&desc, type, data.GetPtr(), (int32) data.GetSize());
	
	} else if (type != '????')
		err = ::NavCreatePreview(&desc, 0, nil, 0);		// Nav Services will look at the file and try to create a reasonable preview
	
	ThrowIfOSErr(err);
#endif
}
#endif


//---------------------------------------------------------------
//
// XPersistDoc::GetDefaultCreator
//
//---------------------------------------------------------------
#if MAC
OSType XPersistDoc::GetDefaultCreator() const
{
	OSType creator = '????';
	
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && creator == '????') {
		IConstDocWriterPtr writer(boss, iter);
		++iter;
		
		if (writer && writer->CanWrite()) {
			const XFileType& fileType = writer->GetType();
			creator = fileType.GetCreator();
		}
	}
		
	return creator;
}
#endif


//---------------------------------------------------------------
//
// XPersistDoc::GetDefaultType
//
//---------------------------------------------------------------
#if MAC
OSType XPersistDoc::GetDefaultType() const
{
	OSType type = '????';
	
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && type == '????') {
		IConstDocWriterPtr writer(boss, iter);
		++iter;
		
		if (writer && writer->CanWrite()) {
			const XFileType& fileType = writer->GetType();
			type = fileType.GetType();
		}
	}
		
	return type;
}
#endif

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XPersistDoc::DoFindWriter
//
//---------------------------------------------------------------
IDocWriterPtr XPersistDoc::DoFindWriter(OSType type, const std::wstring& extension)
{
	IDocWriterPtr writer;
	
	uint32 weight = 0;
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end()) {
		IDocWriterPtr candidate(boss, iter);
		++iter;
		
		if (candidate && candidate->CanWrite()) {
			const XFileType& fileType = candidate->GetType();
			uint32 w = fileType.Match(extension, type);
		
			if (w > weight) {
				writer = candidate;
				weight = w;
			}
		}
	}
	
	if (!writer)			// should never happen
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Can't find a write method.")));	

	return writer;
}


}	// namespace Whisper
