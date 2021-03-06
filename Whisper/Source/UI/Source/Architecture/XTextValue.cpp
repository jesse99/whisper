/*
 *  File:       XTextValue.cpp
 *  Summary:   	Templatized IValue implementations that gets the value using an IText interface.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTextValue.cpp,v $
 *		Revision 1.2  2001/02/28 09:55:47  jesjones
 *		Added IFloatValue.
 *		
 *		Revision 1.1  2000/12/19 07:44:25  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <XTextValue.h>

namespace Whisper {


DEFINE_INTERFACE_FACTORY(XTextBoolValue)
DEFINE_INTERFACE_FACTORY(XTextInt32Value)
DEFINE_INTERFACE_FACTORY(XTextUInt32Value)
DEFINE_INTERFACE_FACTORY(XTextFloatValue)
DEFINE_INTERFACE_FACTORY(XTextDoubleValue)


}	// namespace Whisper
