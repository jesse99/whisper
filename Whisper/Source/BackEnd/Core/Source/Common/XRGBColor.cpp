/*
 *  File:       XRGBColor.cpp
 *  Summary:	RGB color class (the components should normally be in [0, 1]).
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XRGBColor.cpp,v $
 *		Revision 1.10  2001/05/14 10:55:13  jesjones
 *		HandleColorChange traps exceptions.
 *		
 *		Revision 1.9  2001/04/27 04:35:52  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.8  2001/04/06 04:44:27  jesjones
 *		XRGBColor stream in function no longer byte swaps twice.
 *		
 *		Revision 1.7  2001/03/30 04:13:55  jesjones
 *		Added some more asserts to the HSVColor ctor.
 *		
 *		Revision 1.6  2001/03/02 11:25:10  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.5  2000/12/25 00:32:16  jesjones
 *		XRGBColor color values are no longer clamped to [0.0, 1.0]. Added Normalize.
 *		
 *		Revision 1.4  2000/12/10 04:11:21  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/27 07:14:27  jesjones
 *		Uses XCallback::IsValid.
 *		
 *		Revision 1.2  2000/11/09 12:49:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XRGBColor.h>

//#include <iostream>

#include <XARGBColor.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XHSVColor.h>
#include <XMiscUtils.h>
#include <XNumbers.h>
#include <XStreaming.h>
#include <XTextConversions.h>

#if MAC
	#include <MDialogUtils.h>
	#include <MSystemInfo.h>
#endif

namespace Whisper {


//-----------------------------------
//	Constants
//
const XRGBColor kRGBBlack	 (0.0,  0.0,  0.0);		// Values are from ODF
const XRGBColor kRGBWhite	 (1.0,  1.0,  1.0);		// if these values are changed the pen constants in WPen.cpp and MPen.cpp should also be changed
const XRGBColor kRGBRed		 (0.87, 0.0,  0.0);
const XRGBColor kRGBYellow	 (1.0,  1.0,  0.0);
const XRGBColor kRGBBlue	 (0.0,  0.0,  0.87);
const XRGBColor kRGBGreen	 (0.0,  0.93, 0.0);
const XRGBColor kRGBBrown	 (0.6,  0.4,  0.0);
const XRGBColor kRGBPurple	 (0.4,  0.0,  0.6);
const XRGBColor kRGBLightBlue(0.0,  0.6,  1.0);
const XRGBColor kRGBGray     (0.53, 0.53, 0.53);
const XRGBColor kRGBDarkGray (0.25, 0.25, 0.25);
const XRGBColor kRGBLightGray(0.75, 0.75, 0.75);

const double kColorTolerance = 1.0/256.0;


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetPickerVersion
//
//---------------------------------------------------------------
#if MAC
static int32 GetPickerVersion()
{
	static int32 version = 0;
	
	if (version == 0)
		(void) Gestalt(gestaltColorPickerVersion, &version);

	return version;
}
#endif
			

//---------------------------------------------------------------
//
// HandleColorChange
//
//---------------------------------------------------------------
#if MAC
static pascal void HandleColorChange(int32 refCon, NPMColor* color)
{
	PRECONDITION(refCon != nil);
	PRECONDITION(color != nil);
	
	try {
		XCallback1<void, XRGBColor>* callback = reinterpret_cast<XCallback1<void, XRGBColor>* >(refCon);
		if (callback && color != nil) {
			RGBColor temp;
			temp.red   = color->color.rgb.red;
			temp.green = color->color.rgb.green;
			temp.blue  = color->color.rgb.blue;
			
			(*callback)(temp);
		}
	
	} catch (...) {
		DEBUGSTR("Caught an exception in HandleColorChange");
	}
}
#endif
			
#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// operator>> (XInStream, XRGBColor)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, XRGBColor& value)
{	
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'RCLR');
		
	stream >> value.red >> value.green >> value.blue;
		
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (XOutStream, XRGBColor)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const XRGBColor& value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('RCLR');
#endif

	stream << value.red << value.green << value.blue;

	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, XRGBColor)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& inStr, XRGBColor& value)
{
	FromStr(inStr, value.red);
	
	std::wstring str = After(inStr, L" ");
	FromStr(str, value.green);
	
	str = After(str, L" ");
	FromStr(str, value.blue);
}


//---------------------------------------------------------------
//
// ToStr (XRGBColor)
//
//---------------------------------------------------------------
std::wstring ToStr(const XRGBColor& value)
{
	return ToStr(value.red, 3) + L" " + ToStr(value.green, 3) + L" " + ToStr(value.blue, 3);
}


//---------------------------------------------------------------
//
// Normalize
//
//---------------------------------------------------------------
XRGBColor Normalize(const XRGBColor& color)
{
	XRGBColor result;
	
	result.red   = MinMax(0.0f, color.red, 1.0f);
	result.green = MinMax(0.0f, color.green, 1.0f);
	result.blue  = MinMax(0.0f, color.blue, 1.0f);
	
	return result;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XRGBColor
// ===================================================================================

//---------------------------------------------------------------
//
// XRGBColor::XRGBColor (OSColor)
//
//---------------------------------------------------------------
XRGBColor::XRGBColor(const OSColor& rhs)
{
	COMPILE_CHECK(offsetof(XRGBColor, red) == 0);		// these are required by OpenGL
	COMPILE_CHECK(offsetof(XRGBColor, green) == sizeof(float));
	COMPILE_CHECK(offsetof(XRGBColor, blue) == 2*sizeof(float));
	COMPILE_CHECK(sizeof(XRGBColor) == 3*sizeof(float));
	
#if MAC
	red   = rhs.red/65535.0f;
	green = rhs.green/65535.0f;
	blue  = rhs.blue/65535.0f;

#elif WIN
	red   = rhs.rgbRed/255.0f;
	green = rhs.rgbGreen/255.0f;
	blue  = rhs.rgbBlue/255.0f;
#endif
}


//---------------------------------------------------------------
//
// XRGBColor::XRGBColor (COLORREF)
//
//---------------------------------------------------------------
#if WIN
XRGBColor::XRGBColor(COLORREF rhs)
{
	red   = (float) (GetRValue(rhs)/255.0);
	green = (float) (GetGValue(rhs)/255.0);
	blue  = (float) (GetBValue(rhs)/255.0);
}
#endif


//---------------------------------------------------------------
//
// XRGBColor::XRGBColor (ThemeBrush)
//
//---------------------------------------------------------------
#if MAC
XRGBColor::XRGBColor(ThemeBrush brush)
{
	RGBColor color;
	OSStatus err = ::GetThemeBrushAsColor(brush, 32, true, &color);
	ThrowIfOSErr(err);
	
	red   = (float) (color.red/65535.0);
	green = (float) (color.green/65535.0);
	blue  = (float) (color.blue/65535.0);
}
#endif


//---------------------------------------------------------------
//
// XRGBColor::XRGBColor (XARGBColor)
//
//---------------------------------------------------------------
XRGBColor::XRGBColor(const XARGBColor& rhs)
{
	PRECONDITION(rhs.red >= 0.0 && rhs.red <= 1.0);
	PRECONDITION(rhs.green >= 0.0 && rhs.green <= 1.0);
	PRECONDITION(rhs.blue >= 0.0 && rhs.blue <= 1.0);
	
	red   = rhs.red;
	green = rhs.green;
	blue  = rhs.blue;
}


//---------------------------------------------------------------
//
// XRGBColor::XRGBColor (XHSVColor)
//
//---------------------------------------------------------------
XRGBColor::XRGBColor(const XHSVColor& rhs)
{
	PRECONDITION(rhs.hue >= 0.0 && rhs.hue <= 1.0);
	PRECONDITION(rhs.saturation >= 0.0 && rhs.saturation <= 1.0);
	PRECONDITION(rhs.value >= 0.0 && rhs.value <= 1.0);
	
	double h = 6.0*rhs.hue;						// From Foley and Van Dam section 13.3.4
	double s = rhs.saturation;
	double v = rhs.value; 

	if (h == 6.0)
		h = 0.0;
	
	int32 i = numeric_cast<int32>(STD::floor(h)); 
	double f = h - i;
	
	float p = numeric_cast<float>(v*(1.0 - s));
	float q = numeric_cast<float>(v*(1.0 - s*f));
	float t = numeric_cast<float>(v*(1.0 - s*(1.0 - f)));
	float u = numeric_cast<float>(v);
	
	switch (i) {
		case 0: red = u; green = t; blue = p; break;
		case 1: red = q; green = u; blue = p; break;
		case 2: red = p; green = u; blue = t; break;
		case 3: red = p; green = q; blue = u; break;
		case 4: red = t; green = p; blue = u; break;
		case 5: red = u; green = p; blue = q; break;
		
		default: DEBUGSTR("Bad  sextent (", i, ") in XRGBColor(HSVColor) ctor");
	}

	POSTCONDITION(red >= 0.0 && red <= 1.0);
	POSTCONDITION(green >= 0.0 && green <= 1.0);
	POSTCONDITION(blue >= 0.0 && blue <= 1.0);
}


//---------------------------------------------------------------
//
// XRGBColor::XRGBColor (double, double, double)
//
//---------------------------------------------------------------
XRGBColor::XRGBColor(double theRed, double theGreen, double theBlue)
{
	red   = (float) theRed;
	green = (float) theGreen;
	blue  = (float) theBlue;
}


//---------------------------------------------------------------
//
// XRGBColor::XRGBColor (XRGBColor)
//
// CW 5 won't inline this...
//
//---------------------------------------------------------------
XRGBColor::XRGBColor(const XRGBColor& rhs)
{
	red   = rhs.red;
	green = rhs.green;
	blue  = rhs.blue;
}


//---------------------------------------------------------------
//
// XRGBColor::GetOSColor
//
//---------------------------------------------------------------
OSColor XRGBColor::GetOSColor() const
{
	PRECONDITION(red >= 0.0 && red <= 1.0);
	PRECONDITION(green >= 0.0 && green <= 1.0);
	PRECONDITION(blue >= 0.0 && blue <= 1.0);
	
	OSColor color;
	
#if MAC
	color.red   = numeric_cast<uint16>(red*65535.0);
	color.green = numeric_cast<uint16>(green*65535.0);
	color.blue  = numeric_cast<uint16>(blue*65535.0);

#elif WIN
	color.rgbRed   = numeric_cast<uint8>(red*255.0);
	color.rgbGreen = numeric_cast<uint8>(green*255.0);
	color.rgbBlue  = numeric_cast<uint8>(blue*255.0);

	color.rgbReserved = 0;
#endif

	return color;
}

		
//---------------------------------------------------------------
//
// XRGBColor::GetColorRef
//
//---------------------------------------------------------------
#if WIN
COLORREF XRGBColor::GetColorRef() const
{
	OSColor color = this->GetOSColor();

	COLORREF ref = RGB(color.rgbRed, color.rgbGreen, color.rgbBlue);
	
	return ref;
}
#endif


//---------------------------------------------------------------
//
// XRGBColor::operator+=
//
//---------------------------------------------------------------
XRGBColor& XRGBColor::operator+=(const XRGBColor& rhs)
{
	red   = red + rhs.red;
	green = green + rhs.green;
	blue  = blue + rhs.blue;

	return *this;
}


//---------------------------------------------------------------
//
// XRGBColor::operator-=
//
//---------------------------------------------------------------
XRGBColor& XRGBColor::operator-=(const XRGBColor& rhs)
{
	red   = red - rhs.red;
	green = green - rhs.green;
	blue  = blue - rhs.blue;

	return *this;
}


//---------------------------------------------------------------
//
// XRGBColor::operator*= 
//
//---------------------------------------------------------------
XRGBColor& XRGBColor::operator*=(double rhs)
{	
	red   = (float) (red*rhs);
	green = (float) (green*rhs);
	blue  = (float) (blue*rhs);

	return *this;
}


//---------------------------------------------------------------
//
// XRGBColor::operator/= 
//
//---------------------------------------------------------------
XRGBColor& XRGBColor::operator/=(double rhs)
{	
	red   = (float) (red/rhs);
	green = (float) (green/rhs);
	blue  = (float) (blue/rhs);

	return *this;
}


//---------------------------------------------------------------
//
// XRGBColor::operator==
//
//---------------------------------------------------------------
bool XRGBColor::operator==(const XRGBColor& rhs) const				
{
	bool equal = false;
	
	if (Equal(red, rhs.red, kColorTolerance))
		if (Equal(green, rhs.green, kColorTolerance))
			if (Equal(blue, rhs.blue, kColorTolerance))
				equal = true;

	return equal;
}


//---------------------------------------------------------------
//
// XRGBColor::GetLumenance
//
// Calculate approximate brightness using YIQ formula where
// Y = lumenance = .30R + .59G + .11B
//
//---------------------------------------------------------------
double XRGBColor::GetLumenance() const
{
	PRECONDITION(red >= 0.0 && red <= 1.0);
	PRECONDITION(green >= 0.0 && green <= 1.0);
	PRECONDITION(blue >= 0.0 && blue <= 1.0);
	
	double lumenance = 0.3*red + 0.59*green + 0.11*blue;
	
	return lumenance;
}


//---------------------------------------------------------------
//
// XRGBColor::Pick (wstring, XRGBColor*, XProcedure1<XRGBColor>)	[static]
//
//---------------------------------------------------------------
bool XRGBColor::Pick(const std::wstring& prompt, XRGBColor* color, XCallback1<void, XRGBColor> callback)
{
	PRECONDITION(color != nil);
	
	bool picked = false;
	
#if MAC
	int32 version = GetPickerVersion();
	if (version >= 0x00000210 && CheckCFMLink(NPickColor)) {
#if TARGET_CARBON
		static UserEventUPP eventFilter = NewUserEventUPP(HandleEvents);
		static NColorChangedUPP colorCallback = NewNColorChangedUPP(HandleColorChange);
#elif GENERATINGCFM
		uint32 uppEventFilterProcInfo = kPascalStackBased
			 | RESULT_SIZE(SIZE_CODE(sizeof(Boolean)))
			 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(EventRecord*)));
		
		uint32 uppColorChangeProcInfo = kPascalStackBased
			 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(int32)))
			 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(NPMColor*)));
		
		RoutineDescriptor eventDesc = BUILD_ROUTINE_DESCRIPTOR(uppEventFilterProcInfo, HandleEvents);
		RoutineDescriptor colorDesc = BUILD_ROUTINE_DESCRIPTOR(uppColorChangeProcInfo, HandleColorChange);
		
		UserEventUPP eventFilter = (ModalFilterUPP) &eventDesc;
		NColorChangedUPP colorCallback = (NColorChangedUPP) &colorDesc;
#else
		UserEventUPP eventFilter = HandleEvents;
		NColorChangedUPP colorCallback = HandleColorChange;
#endif

		NColorPickerInfo info;
		info.theColor.profile 		  = nil;
		info.theColor.color.rgb.red   = color->GetOSColor().red;
		info.theColor.color.rgb.green = color->GetOSColor().green;
		info.theColor.color.rgb.blue  = color->GetOSColor().blue;
		info.dstProfile				  = nil;
		info.flags					  = kColorPickerDialogIsMoveable + kColorPickerDialogIsModal;
		info.placeWhere				  = kDeepestColorScreen;
		info.dialogOrigin.h			  = 0;					// ignored when kDeepestColorScreen is used
		info.dialogOrigin.v			  = 0;
		info.pickerType				  = 0;					// select last picker used
		info.eventProc				  = eventFilter;
		info.colorProc				  = callback.IsValid() ? colorCallback : nil;
		info.colorProcData			  = (uint32) &callback;
		info.prompt[0]				  = '\0';
		info.mInfo.editMenuID 		  = 0;
		info.mInfo.cutItem 			  = 0;
		info.mInfo.copyItem 		  = 0;
		info.mInfo.pasteItem 		  = 0;
		info.mInfo.clearItem 		  = 0;
		info.mInfo.undoItem 		  = 0;
		info.reserved				  = 0;
		
//		if (liveTracking)
//			info.flags += kColorPickerCallColorProcLive;	// $$$ this is supposed to allow the colorProc to be called during picker drags, but none of the picker's in System 8.5.1 seem to support this...
		
		if (prompt.length() > 0)
			BlockMoveData(ToPascalStr(prompt).c_str(), info.prompt, Min(prompt.length(), 255UL) + 1);
		
		OSErr err = ::NPickColor(&info);			// requires Color Picker 2.1 (part of System 8, may be installed on System 7.5 and above)
		ThrowIfOSErr(err);
		
		if (info.newColorChosen) {
			RGBColor temp;
			temp.red   = info.theColor.color.rgb.red;
			temp.green = info.theColor.color.rgb.green;
			temp.blue  = info.theColor.color.rgb.blue;
			
			*color = temp;
			picked = true;
		}

	} else {
		RGBColor inColor = color->GetOSColor();
		RGBColor outColor;
		
		Point loc = {-1, -1};		// use deepest device
		picked = ::GetColor(loc, ToPascalStr(prompt).c_str(), &inColor, &outColor);
		if (picked)
			*color = outColor;
	}
		
#elif WIN
	UNUSED(prompt);
	UNUSED(callback);
	
	static uint32 customColors[16];
	
	CHOOSECOLOR info;
	info.lStructSize	= sizeof(info);
	info.hwndOwner		= nil;
	info.hInstance		= nil;
	info.rgbResult		= color->GetColorRef();
	info.lpCustColors	= customColors;
	info.Flags			= CC_RGBINIT;
	info.lCustData		= 0;
	info.lpfnHook		= nil;
	info.lpTemplateName = nil;
	
	picked = ::ChooseColor(&info) != 0;
	if (picked) {
		*color = XRGBColor(info.rgbResult);
	
	} else {
		uint32 err = CommDlgExtendedError();
		ThrowIf(err != 0);
	}
#endif

	return picked;
}


}	// namespace Whisper


