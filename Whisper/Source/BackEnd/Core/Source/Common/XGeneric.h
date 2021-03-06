/*
 *  File:		XGeneric.h
 *  Summary:	General purpose template functions.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XGeneric.h,v $
 *		Revision 1.4  2001/03/03 11:15:58  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.3  2000/11/09 12:37:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#if __MWERKS__ >= 0x2400
	#include <msl_utility>
#endif


//===================================================================================
//	DECLARE_POD
//		Quality STL implementations optimize their handling of types like int and
//		double. These types don't have to be destructed and can be copied in blocks
//		instead of via copy constructors. These sort of types are called Plain Old 
//		Data (POD) by the standard. In CW Pro 6 we can tell MSL about custom POD types.
//===================================================================================
#if __MWERKS__ >= 0x2400
	#define DECLARE_POD(TYPE)								\
	namespace Metrowerks {									\
		namespace details {									\
			template <>										\
			struct class_has_trivial_default_ctor<TYPE> {	\
				static const bool value = true;				\
			};												\
															\
			template <>										\
			struct class_has_trivial_copy_ctor<TYPE> {		\
				static const bool value = true;				\
			};												\
															\
			template <>										\
			struct class_has_trivial_assignment<TYPE> {		\
				static const bool value = true;				\
			};												\
															\
			template <>										\
			struct class_has_trivial_dtor<TYPE> {			\
				static const bool value = true;				\
			};												\
		}													\
	}
#else
	#define DECLARE_POD(TYPE)
#endif


