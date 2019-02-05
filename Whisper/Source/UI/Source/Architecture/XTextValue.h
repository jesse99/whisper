/*
 *  File:       XTextValue.h
 *  Summary:   	Templatized IValue implementations that gets the value using an IText interface.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTextValue.h,v $
 *		Revision 1.3  2001/03/02 11:25:32  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.2  2001/02/28 09:55:50  jesjones
 *		Added IFloatValue.
 *		
 *		Revision 1.1  2000/12/19 07:44:28  jesjones
 *		Checked in
 */

#pragma once

#include <IText.h>
#include <IValue.h>
#include <XTextConversions.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XTextValue
//!		Templatized IValue implementations that gets the value using an IText interface.
// ===================================================================================
template <typename T>
class XTextValue : public IValue<T> {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XTextValue()			{}
	
						XTextValue(XBoss* boss)	{IValue<T>::DoSetBoss(boss);}
						
//-----------------------------------
//	Inherited API
//
public:
	virtual T 			Get() const
						{
							IConstTextPtr text(this);
							std::wstring str = text->GetText();

							T value;
							FromStr(str, value);
							
							return value;
						}
	
	virtual void 		Set(T value, bool redraw = kRedraw)
						{
							std::wstring str = ToStr(value);
								
							ITextPtr text(this);
							text->SetText(str, redraw);
						}
};

typedef XTextValue<bool> XTextBoolValue;
typedef XTextValue<int32> XTextInt32Value;
typedef XTextValue<uint32> XTextUInt32Value;
typedef XTextValue<float> XTextFloatValue;
typedef XTextValue<double> XTextDoubleValue;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
