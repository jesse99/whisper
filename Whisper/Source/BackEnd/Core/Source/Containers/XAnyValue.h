/*
 *  File:       XAnyValue.h
 *  Summary:    A class that can contain any value type.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XAnyValue.h,v $
 *		Revision 1.5  2000/12/10 08:49:59  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.4  2000/11/20 05:33:07  jesjones
 *		Fixed a small bug in the assignment operator caught by CW 6.
 *		
 *		Revision 1.3  2000/11/13 00:54:53  jesjones
 *		Replaced the void* conversion operator with IsValid (the operator allows XAnyValue's to be incorrectly compared).
 *		
 *		Revision 1.2  2000/11/09 12:25:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	11/02/00	JDJ		Replaced IsValid with a const void* conversion operator.
 *		 <1>	 8/30/00	JDJ		Created (derived from Kevlin Henney's boost::any class)
 */

#pragma once

#include <typeinfo>

#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class XAnyValue
//!		A class that can contain any value type.
// ===================================================================================
class XAnyValue {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XAnyValue()						{delete mPtr;}

			 			XAnyValue()							{mPtr = nil;}
			 			
			 			template <typename T>
			 			XAnyValue(const T& value)			{mPtr = new Value<T>(value);}
	
						XAnyValue(const XAnyValue& rhs)		{mPtr = rhs.mPtr != nil ? rhs.mPtr->Clone() : nil;}
			
			XAnyValue& 	operator=(const XAnyValue& rhs)		{XAnyValue temp(rhs); return swap(temp);}
			
			 			template <typename T>
			XAnyValue& 	operator=(const T& rhs)				{return swap(XAnyValue(rhs));}
			
//-----------------------------------
//	API
//
public:
			bool 		IsValid() const						{return mPtr != nil;}
			
			 			template <typename T>
			bool 		Get(T& value) const					{if (const Value<T>* p = dynamic_cast<Value<T>*>(mPtr)) {value = p->mValue; return true;} return false;}
						/**< Returns false if value isn't a T. */
			
			 			template <typename T>
			const T& 	Get() const							{const Value<T>* p = dynamic_cast<Value<T>*>(mPtr); if (p == nil) throw std::bad_cast(); return p->mValue;}
						/**< Throws if value isn't a T. */
									
			const std::type_info& type() const				{ASSERT(this->IsValid()); return mPtr->type();}

			XAnyValue& 	swap(XAnyValue& rhs)				{std::swap(mPtr, rhs.mPtr); return *this;}
			
//-----------------------------------
//	Internal Types
//
private:
	class BaseValue {
	public:
		virtual 						~BaseValue() {}
		virtual BaseValue* 				Clone() const = 0;
		virtual const std::type_info& 	type() const = 0;
	};

	template <typename T>
	class Value : public BaseValue {
	public:
				 						Value(const T& value) : mValue(value) {}
		virtual BaseValue* 				Clone() const 			{return new Value(mValue);}
		virtual const std::type_info&	type() const 			{return typeid(mValue);}
	public:
		T	mValue;
	};

//-----------------------------------
//	Member Data
//
private:
	BaseValue*	mPtr;
};


}	// namespace Whisper

