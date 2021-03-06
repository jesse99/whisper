/*
 *  File:       XFont.h
 *  Summary:	An object used to describe how text should be drawn.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFont.h,v $
 *		Revision 1.6  2001/04/17 01:43:49  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.5  2001/03/16 10:35:46  jesjones
 *		Exports the iterator.
 *		
 *		Revision 1.4  2001/03/05 05:41:49  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:52:50  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:37:01  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <vector>

#include <XDrawAttribute.h>
#include <XInvariant.h>
#include <XRGBColor.h>
#include <XSize.h>
#include <XXMLPersistent.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Constants
//
extern GRAPHICS_EXPORT const std::wstring kSystemFont; 			// Standard Font Names
extern GRAPHICS_EXPORT const std::wstring kAppFont;
extern GRAPHICS_EXPORT const std::wstring kHelveticaFont;
extern GRAPHICS_EXPORT const std::wstring kTimesFont;
extern GRAPHICS_EXPORT const std::wstring kCourierFont;
extern GRAPHICS_EXPORT const std::wstring kPalatinoFont;
							
extern GRAPHICS_EXPORT const std::wstring kBigSystemFont; 		// 12-point kSystemFont (but note that these can change with the theme on Macs)
extern GRAPHICS_EXPORT const std::wstring kSmallSystemFont; 	// 10-point kAppFont
extern GRAPHICS_EXPORT const std::wstring kSmallBoldSystemFont; 


// ===================================================================================
//	struct SFontMetrics
// ===================================================================================
struct GRAPHICS_EXPORT SFontMetrics {
	int32	ascent;
	int32	descent;
	int32	widMax;
	int32	leading;						//!< external leading
};


// ===================================================================================
//	class XFont
//!		An object used to describe how text should be drawn.
// ===================================================================================
class GRAPHICS_EXPORT XFont : public XDrawAttribute, public XXMLPersistentMixin {

	typedef XDrawAttribute Inherited;

//-----------------------------------
//	Types
//
public:
	class GRAPHICS_EXPORT iterator : public std::iterator<std::forward_iterator_tag, std::wstring> {
	
		friend class XFont;

	protected:
					iterator(OSDrawContext dc, int dummy);
							
					iterator(OSDrawContext dc);

	public:
		std::wstring operator*() const;

		iterator& 	operator++();
		iterator 	operator++(int)								{iterator temp = *this; ++*this; return temp;}
				
		bool 		operator==(const iterator& rhs) const;
		bool 		operator!=(const iterator& rhs) const		{return !this->operator==(rhs);}
		
	protected:
#if WIN
		static int CALLBACK DoEnum(const LOGFONT* logicalData, const TEXTMETRIC* physicalData, DWORD type, LPARAM refCon);
#endif

	protected:
		OSDrawContext 				mDC;
		uint32						mIndex;
		std::vector<std::wstring>	mNames;
	};

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XFont();		
						
						XFont(const std::wstring& name, int32 pointSize = 12);
						/**< pointSize is ignored if kBigSystemFont, kSmallSystemFont, or
						kSmallBoldSystemFont is used. */
						
						XFont(const XFont& rhs);
			
			XFont& 		operator=(const XFont& rhs);
						
//-----------------------------------
//	API
//
public:
	//! @name Font Name
	//@{
			std::wstring GetName() const							{return mName;}
						
			void 		SetName(const std::wstring& name);
	//@}

	//! @name Point Size
	//@{
			int32 		GetSize() const							{return mSize;}
			
			void 		SetSize(int32 pointSize);
			
			bool 		HasSize(int32 pointSize) const;
						/**< Returns true if the font can be displayed (nicely) at the given size. */
	//@}

	//! @name Style
	//@{
			uint16 		GetStyle() const						{return mStyle;}
						/**< Defaults to kPlainStyle */
						
			void 		SetStyle(uint16 style);
						/**< Style can be a sum of kPlainStyle, kBoldStyle, kItalicStyle, 
						and kUnderlineStyle. */
	//@}

	//! @name Color
	//@{
			XRGBColor 	GetColor() const						{return mColor;}
						/**< Defaults to kRGBBlack */
						
			void 		SetColor(const XRGBColor& color);
	//@}

	//! @name Metrics
	//@{
			SFontMetrics GetMetrics() const;
			
			XSize 		GetStringSize(const std::wstring& text) const;
						/**< Returns the dimensions of the text. Note that this does not
						include leading. The font must be selected into a draw context. */
	//@}

	//! @name Font Iterating
	//@{
	static	iterator 	begin(OSDrawContext dc)					{return iterator(dc, 0);}
						/**< Iterates over all font names. */

	static	iterator 	end(OSDrawContext dc)					{return iterator(dc);}
	//@}

	//! @name Comparisons
	//@{
			bool 		operator==(const XFont& rhs) const;

			bool 		operator!=(const XFont& rhs) const		{return !this->operator==(rhs);}
	//@}
			
	//! @name Misc
	//@{
#if MAC
			int16 		GetFontNumber() const					{return mFontNum;}
#endif
	//@}

//-----------------------------------
//	Inherited API
//
public:
	virtual	void 		DoSelect(OSDrawContext context) const;

	virtual void 		DoDeSelect(OSDrawContext context) const;

protected:
			void 		Invariant() const;
	
	virtual	void 		OnStreamOut(XXMLDoc& doc, XXMLElement& root) const;

	virtual void 		OnStreamIn(const XXMLDoc& doc, const XXMLElement& root, const XXMLElement& element);

//-----------------------------------
//	Internal API
//
public:
#if MAC
			ControlFontStyleRec GetFontStyleRecord() const;
#elif WIN
			HFONT 		GetFont() const;
#endif

protected:
			void 		DoInit(OSDrawContext context);

//-----------------------------------
//	Member Data
//
protected:
	class ZFont*			mFont;

	std::wstring			mName;
	int32					mSize;
	uint16					mStyle;
	XRGBColor				mColor;
	
	mutable SFontMetrics	mInfo;

#if MAC
	int16					mFontNum;	
	mutable bool			mValidInfo;
	
	mutable int16			mOldFontNum;
	mutable int16			mOldSize;
	mutable Style			mOldStyle;
	
#elif WIN
	mutable HFONT			mOldFont;
	mutable HDC				mDC;
#endif

	mutable XRGBColor		mOldColor;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

