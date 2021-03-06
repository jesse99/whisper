/*
 *  File:		XXMLPersistent.h
 *  Summary:	Mixin for objects that can be flattened onto XML objects.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLPersistent.h,v $
 *		Revision 1.5  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.4  2000/12/10 08:57:34  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/12/10 04:12:45  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/10 04:47:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 6/25/99	JDJ		Tweaked the code in the Usage comment.
 *		 <2>	 6/22/99	JDJ		HandleStreamOut and OnStreamOut XXMLDoc argument is no longer const.
 *		 <1>	 3/20/99	JDJ		Created
 */

#pragma once

#include <XPersistent.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XXMLCharData;
class XXMLDoc;
class XXMLElement;
class XXMLProcessInstruction;


// ===================================================================================
//	class XXMLPersistentMixin
//!		Mixin for objects that can be flattened onto XML objects.
/*!		XXMLPersistentMixin saves objects using a parent element named using XPersistentMixin::GetClassName. 
 *		OnStreamOut should call Inherited::OnStreamOut and then append a new element to 
 *		the parent element. This results in the parent element having a child element 
 *		for each class in the inheritance graph that streams data out. When the object 
 *		is streamed in HandleStreamIn passes each child element to OnStreamIn.
 *				
 *		This results in an XML document that looks something like: \code
 *			<XTextBoxControl>
 *				<ControlInfo
 *					name 	  = "Date"
 *					enabled   = "true"
 *					help_mesg = "Enter a date as mm/dd/yy."
 *					loc 	  = "100 25"
 *					size	  = "80 20"
 *				/>
 *					
 *				<TextBoxInfo
 *					text = ""
 *				/>
 *			</XTextBoxControl> \endcode
 *				
 * 		The above XML would be created using something like this: \code
 *			void XTextBoxControl::OnStreamOut(XXMLDoc& doc, XXMLElement& parent) const
 *			{
 *				Inherited::OnStreamOut(doc, parent);
 *				
 *				XAutoPtr<XXMLElement> element(new XXMLElement(L"TextBoxInfo"));
 *				element->AppendAttribute(XXMLAttribute(L"text", mText));
 *						
 *				parent.AppendItem(element.Release());
 *			}
 *				
 *			void XTextBoxControl::OnStreamIn(const XXMLDoc& doc, const XXMLElement& parent, const XXMLElement& element) const
 *			{
 *				if (element.GetName() == L"TextBoxInfo") {
 *					ASSERT(element.item_begin() == element.item_end());		// we don't have any child elements
 *					ASSERT(element.attr_begin() != element.attr_end());		// we do have required attributes
 *						
 *					XXMLElement::attr_iterator iter = element.attr_begin();	
 *					while (iter != element.attr_end()) {
 *						const XXMLAttribute& attr = *iter;
 *						++iter;
 *							
 *						const std::wstring& name = attr.GetName();
 *						const std::wstring& value = attr.GetValue();
 *							
 *						if (name == L"text")				
 *							mText = value;
 *						else
 *							DEBUGSTR(L"%ls isn't a valid attribute of TextBoxInfo!", name.c_str());
 *					}
 *				
 *				} else
 *					Inherited::OnStreamIn(doc, parent, element);
 *			} \endcode 
 *
 *		Simpler objects can get by with something like: \code
 *			void XFont::OnStreamOut(XXMLDoc& doc, XXMLElement& element) const
 *			{				
 *				UNUSED(doc);
 *						
 *				element.AppendAttribute(XXMLAttribute(L"font", mName));
 *				element.AppendAttribute(XXMLAttribute(L"size", mSize));
 *				element.AppendAttribute(XXMLAttribute(L"style", (uint32) mStyle));
 *				element.AppendAttribute(XXMLAttribute(L"color", mColor));
 *			}
 *				
 *			void XFont::OnStreamIn(const XXMLDoc& doc, const XXMLElement& parent, const XXMLElement& element) const
 *			{
 *				ASSERT(element.item_begin() == element.item_end());	// we don't have any child elements
 *				ASSERT(element.attr_begin() != element.attr_end());	// we do have attributes
 *				ASSERT(element.GetName() == element.GetName());		// element doesn't have any child elements so this should never fire
 *				UNUSED(doc);										// doc is provided in case IDREF attributes are used
 *				UNUSED(parent);
 *						
 *				XXMLElement::const_attr_iterator iter = element.attr_begin();	
 *				while (iter != element.attr_end()) {
 *					XXMLAttribute attr = *iter;
 *					++iter;
 *							
 *					const std::wstring& name = attr.GetName();
 *						
 *					if (name == L"font")				
 *						this->SetName(attr.GetValue());
 *					else if (name == L"size")				
 *						mSize = attr.GetInt32Value();
 *					else if (name == L"style")				
 *						this->SetStyle(numeric_cast<uint16>(attr.GetUInt32Value()));
 *					else if (name == L"color")				
 *						this->SetColor(attr.GetColorValue());
 *					else
 *						DEBUGSTR(L"%ls isn't a valid attribute of XFont!", name.c_str());
 *				}
 *			} \endcode */
// ===================================================================================
class CORE_EXPORT XXMLPersistentMixin : public XPersistentMixin {
	
	typedef XPersistentMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XXMLPersistentMixin();

						XXMLPersistentMixin(bool persistant = kPersistant);
						
//-----------------------------------
//	New API
//
public:
	static	void 		Flatten(XXMLDoc& doc, XXMLElement& parent, const XXMLPersistentMixin* object);
						/**< Appends to parent a new XXMLElement representing the object. 
						Unlike XBinaryPersistentMixin this *does not* automatically 
						handle circular object graphs. */
	
	static	XXMLPersistentMixin* Unflatten(const XXMLDoc& doc, const XXMLElement& element, XCreateByNameMixin* parent);
						/**< Creates the object using the object factory and then calls
						HandleStreamIn. */

	virtual	void 		HandleStreamOut(XXMLDoc& doc, XXMLElement& element) const;

	virtual void 		HandleStreamIn(const XXMLDoc& doc, const XXMLElement& element);

protected:
	virtual	void 		OnStreamOut(XXMLDoc& doc, XXMLElement& parent) const = 0;

	virtual void 		OnStreamIn(const XXMLDoc& doc, const XXMLElement& parent, const XXMLElement& element) = 0;
	virtual void 		OnStreamIn(const XXMLDoc& doc, const XXMLElement& parent, const XXMLCharData& data);
	virtual void 		OnStreamIn(const XXMLDoc& doc, const XXMLElement& parent, const XXMLProcessInstruction& pi);
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

