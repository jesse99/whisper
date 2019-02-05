/*
 *  File:		XStreaming.h
 *  Summary:	Streaming operators for some common types.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStreaming.h,v $
 *		Revision 1.2  2000/11/09 12:52:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <6>	12/11/99	JDJ		Added XFileSystemSpec streaming methods.
 *		 <5>	 4/07/99	JDJ		Added XTinyVector streaming methods.
 *		 <4>	 3/23/99	JDJ		Added XArray streaming methods.
 *		 <3>	 2/19/99	JDJ		Added XDate streaming methods.
 *		 <2>	12/17/98	JDJ		Added XSparseArray and XSet streaming methods.
 *		 <2>	12/17/98	JDJ		Added XSparseArray and XSet streaming methods.
 *		 <1>	 1/11/98	JDJ		Created (from Raven)
 */

#pragma once

#include <list>
#include <map>
#include <set>
#include <vector>

#include <XArray.h>
#include <XSet.h>
#include <XSparseArray.h>
#include <XStream.h>
#include <XTinyVector.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XDate;
class XHandle;
class XRect;


// ========================================================================================
//	Input Streaming Operators (usage is like this: stream >> ch)
// ========================================================================================
CORE_EXPORT XInStream& operator>>(XInStream& stream, bool& value);		
CORE_EXPORT XInStream& operator>>(XInStream& stream, char& value);					
CORE_EXPORT XInStream& operator>>(XInStream& stream, wchar_t& value);					
CORE_EXPORT XInStream& operator>>(XInStream& stream, short& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, int& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, long& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, int64& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, float& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, double& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, unsigned char& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, unsigned short& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, unsigned int& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, unsigned long& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, uint64& value);

CORE_EXPORT XInStream& operator>>(XInStream& stream, std::wstring& value);	
CORE_EXPORT XInStream& operator>>(XInStream& stream, XPoint& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, XSize& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, XRect& value);

CORE_EXPORT XInStream& operator>>(XInStream& stream, XHandle& value);
CORE_EXPORT XInStream& operator>>(XInStream& stream, XDate& value);

template <class T>
XInStream& operator>>(XInStream& stream, std::vector<T>& collection);

template <class T>
XInStream& operator>>(XInStream& stream, std::list<T>& collection);

template <class T, class COMP>
XInStream& operator>>(XInStream& stream, std::set<T, COMP>& collection);

template <class KEY, class VALUE, class COMP>
XInStream& operator>>(XInStream& stream, std::map<KEY, VALUE, COMP>& collection);


// ========================================================================================
//	Output Streaming Operators (usage is like this: stream << ch)
// ========================================================================================
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, bool value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, char value);				
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, wchar_t value);				
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, short value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, int value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, long value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, int64 value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, float value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, double value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, unsigned char value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, unsigned short value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, unsigned int value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, unsigned long value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, uint64 value);

CORE_EXPORT XOutStream& operator<<(XOutStream& stream, const std::wstring& value);	
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, const XPoint& value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, const XSize& value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, const XRect& value);

CORE_EXPORT XOutStream& operator<<(XOutStream& stream, const XHandle& value);
CORE_EXPORT XOutStream& operator<<(XOutStream& stream, const XDate& value);

template <class T>
XOutStream& operator<<(XOutStream& stream, const std::vector<T>& collection);

template <class T>
XOutStream& operator<<(XOutStream& stream, const std::list<T>& collection);

template <class T, class COMP>
XOutStream& operator<<(XOutStream& stream, const std::set<T, COMP>& collection);

template <class KEY, class VALUE, class COMP>
XOutStream& operator<<(XOutStream& stream, const std::map<KEY, VALUE, COMP>& collection);


// ========================================================================================
//	Inlines
//		It would be possible to merge some of these into a more generic template function
//		but that would often lead to compiler ambiguities.
// ========================================================================================
template <class T> XInStream& operator>>(XInStream& stream, XTinyVector<T>& collection)
{
	uint32 size;
	stream >> size;
	
	if (size != collection.size())		// tiny vectors aren't resizable
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Tiny vector sizes don't match.")));
		
	for (uint32 i = 0; i < size; ++i) {
		T value;
		stream >> value;
		
		collection[i] = value;
	}
	
	return stream;
}

template <class T> XInStream& operator>>(XInStream& stream, std::vector<T>& collection)
{
	uint32 size;
	stream >> size;
	
	collection.clear();
	collection.reserve(size);
	
	while (size--) {
		T value;
		stream >> value;
		
		collection.push_back(value);
	}
	
	return stream;
}

template <class T> XInStream& operator>>(XInStream& stream, std::list<T>& collection)
{
	uint32 size;
	stream >> size;
	
	collection.clear();
	while (size--) {
		T value;
		stream >> value;
		
		collection.push_back(value);
	}
	
	return stream;
}

template <class T, class COMP> XInStream& operator>>(XInStream& stream, std::set<T, COMP>& collection)
{
	uint32 size;
	stream >> size;
	
	collection.clear();
	while (size--) {
		T value;
		stream >> value;
		
		collection.insert(value);
	}
	
	return stream;
}

template <class KEY, class VALUE, class COMP> XInStream& operator>>(XInStream& stream, std::map<KEY, VALUE, COMP>& collection)
{
	uint32 size;
	stream >> size;
	
	collection.clear();
	while (size--) {
		KEY key;							// can't use value_type because the KEY is const
		VALUE value;

		stream >> key >> value;
		
		collection.insert(std::map<KEY, VALUE, COMP>::value_type(key, value));
	}
	
	return stream;
}


template <class T> XOutStream& operator<<(XOutStream& stream, const XTinyVector<T>& collection)
{
	uint32 size = collection.size();
	stream << size;
	
	std::vector<T>::const_iterator iter = collection.begin();
	while (iter != collection.end()) {
		const T& value = *iter++;

		stream << value;
	}
	
	return stream;
}

template <class T> XOutStream& operator<<(XOutStream& stream, const std::vector<T>& collection)
{
	uint32 size = collection.size();
	stream << size;
	
	std::vector<T>::const_iterator iter = collection.begin();
	while (iter != collection.end()) {
		const T& value = *iter++;

		stream << value;
	}
	
	return stream;
}

template <class T> XOutStream& operator<<(XOutStream& stream, const std::list<T>& collection)
{
	uint32 size = collection.size();
	stream << size;
	
	std::list<T>::const_iterator iter = collection.begin();
	while (iter != collection.end()) {
		const T& value = *iter++;

		stream << value;
	}
	
	return stream;
}

template <class T, class COMP> XOutStream& operator<<(XOutStream& stream, const std::set<T, COMP>& collection)
{	
	uint32 size = collection.size();
	stream << size;
	
	std::set<T, COMP>::const_iterator iter = collection.begin();
	while (iter != collection.end()) {
		const T& value = *iter++;
		
		stream << value;
	}
		
	return stream;
}

template <class KEY, class VALUE, class COMP> XOutStream& operator<<(XOutStream& stream, const std::map<KEY, VALUE, COMP>& collection)
{
	uint32 size = collection.size();
	stream << size;
	
	std::map<KEY, VALUE, COMP>::const_iterator iter = collection.begin();
	while (iter != collection.end()) {
		const std::map<KEY, VALUE, COMP>::value_type& value = *iter++;
		
		stream << value.first << value.second;
	}
		
	return stream;
}


template <class T, class C>
XInStream& operator>>(XInStream& stream, XSet<T, C>& c)				
{
	return (stream >> c.mElements);
}

template <class T, class C>
XOutStream& operator<<(XOutStream& stream, const XSet<T, C>& c)		
{
	return (stream << c.mElements);
}


template <class T>
XInStream& operator>>(XInStream& stream, XSparseArray<T>& c)			
{
	return (stream >> c.mShape >> c.mElements >> c.mDefaultValue);	
}

template <class T>
XOutStream& operator<<(XOutStream& stream, const XSparseArray<T>& c)	
{
	return (stream << c.mShape << c.mElements << c.mDefaultValue);
}


template <class T>
XInStream& operator>>(XInStream& stream, XArray<T>& c)			
{
	XRect shape;
	stream >> shape;
	c.SetShape(shape);
	
	for (uint32 index = 0; index < c.mShape.GetArea(); ++index)
		stream >> c.mElements[index];
	
	return stream;
}

template <class T>
XOutStream& operator<<(XOutStream& stream, const XArray<T>& c)	
{
	stream << c.mShape;
	
	for (uint32 index = 0; index < c.mShape.GetArea(); ++index)
		stream << c.mElements[index];
	
	return stream;
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
