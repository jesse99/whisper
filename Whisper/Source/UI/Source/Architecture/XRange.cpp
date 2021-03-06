/*
 *  File:       XRange.cpp
 *  Summary:   	Template interface for an inclusive range of values.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRange.cpp,v $
 *		Revision 1.1  2001/03/23 10:51:22  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <IRange.h>

namespace Whisper {

typedef XRange<int32> XInt32Range;
typedef XRange<uint32> XUInt32Range;
typedef XRange<float> XFloatRange;
typedef XRange<double> XDoubleRange;

DEFINE_INTERFACE_FACTORY(XInt32Range)
DEFINE_INTERFACE_FACTORY(XUInt32Range)
DEFINE_INTERFACE_FACTORY(XFloatRange)
DEFINE_INTERFACE_FACTORY(XDoubleRange)

}	// namespace Whisper
