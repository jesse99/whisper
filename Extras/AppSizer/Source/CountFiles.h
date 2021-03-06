/*

 *  File:       CountFiles.h

 *  Summary:	The thread used to count the numbers of files to process (so we can use a progress bar).

 *  Written by: Jesse Jones

 *

 *  Copyright � 2000 Jesse Jones. 

 *	For conditions of distribution and use, see copyright notice in XTypes.h  

 *

 *  Change History (most recent first):	

 *

 *		$Log: CountFiles.h,v $
 *		Revision 1.1.1.1  2000/12/27 03:12:50  jesjones
 *		First Imported.
 *		

 */



#pragma once



#include <XCountFiles.h>

#include <XHandle.h>



using namespace Whisper;

using namespace std;





// ===================================================================================

//	class CCountFiles

// ===================================================================================

class CCountFiles : public XCountFiles {



	typedef XCountFiles Inherited;

	

//-----------------------------------

//	Initialization/Destruction

//

public:

	virtual 			~CCountFiles();

	

  						CCountFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, const vector<XHandle>& files, ErrorCallback err);

  						

//-----------------------------------

//	Inherited API

//

protected:

	virtual void 		OnCount(const XFileSpec& spec);



//-----------------------------------

//	Member Data

//

protected:	

	vector<XHandle>	mFiles;

};



