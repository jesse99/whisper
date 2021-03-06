/*
 *  File:       CRegisterClasses.h
 *  Summary:   	Registers the interface and implementation classes used by the app. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CRegisterClasses.h,v $
 *		Revision 1.2  2001/02/09 04:29:58  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.1  2000/12/28 02:50:51  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:57:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */


// ===================================================================================
//	Global Functions
// ===================================================================================
void 	RegisterInterfaces();

void 	RegisterImplementations();

void 	RegisterLoaders();
