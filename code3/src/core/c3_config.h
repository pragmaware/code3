#ifndef _c3_config_h_
#define _c3_config_h_
//=============================================================================
//
//   File : c3_config.h
//   Creation Date : mar 20 ott 2015 00:03:46
//   Project : Code 3
//   Author : Szymon Tomasz Stefanek <sts at pragmaware dot net>
//
//   This file is part of the Code 3 Editor distribution
//   Copyright (C) 2015-2019 Szymon Tomasz Stefanek <sts at pragmaware dot net>
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 3
//   of the License, or (at your option) any later version.
//
//=============================================================================


#include "c3_sysconfig.h"

#define __utf8(_text) QString::fromUtf8(_text)
#define __ascii(_text) QString::fromUtf8(_text)

#define __tr(_text) __utf8(_text)

#define C3_COMPILE_DEBUG_MODE

#ifdef __GNUC__
	#define C3_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
	#define C3_PRETTY_FUNCTION __FUNCTION__
#endif

#include <qdebug.h>

#ifdef C3_COMPILE_DEBUG_MODE

	#define C3_ASSERT(__condition__) \
			do { \
				if(!(__condition__)) \
				{ \
					qDebug("[ASSERT FAILED] (" # __condition__ ") in %s at %s:%u",C3_PRETTY_FUNCTION,__FILE__,__LINE__); \
					qFatal("Assert failed: aborting"); \
				} \
			} while(0)
	#define C3_ASSERT_MSG(__condition__,__message__) \
			do { \
				if(!(__condition__)) \
				{ \
					qDebug("[ASSERT FAILED] (" # __condition__ ") in %s at %s:%u",C3_PRETTY_FUNCTION,__FILE__,__LINE__); \
					qDebug("[ASSERT FAILED] " # __message__); \
					qFatal("Assert failed: aborting"); \
				} \
			} while(0)

	#include "C3DebugContext.h"

	#define C3_TRACE_HACK_TOKENPASTE_2(x,y) x ## y
	#define C3_TRACE_HACK_TOKENPASTE_1(x,y) C3_TRACE_HACK_TOKENPASTE_2(x,y)

	#ifdef __GNUC__
		#define C3_TRACE_FUNCTION \
				C3DebugContext C3_TRACE_HACK_TOKENPASTE_1(ctx,__LINE__)(C3_PRETTY_FUNCTION)

		#define C3_TRACE_BLOCK(_szBlockDescription) \
				C3DebugContext C3_TRACE_HACK_TOKENPASTE_1(ctx,__LINE__)("%s - %s",C3_PRETTY_FUNCTION,_szBlockDescription)
		
		#define C3_TRACE(_szFmt,arg...) C3DebugContext::trace(_szFmt,##arg)

	#else //!C3_COMPILER_GCC
		#define C3_TRACE_FUNCTION \
				C3DebugContext C3_TRACE_HACK_TOKENPASTE_1(ctx,__LINE__)(C3_PRETTY_FUNCTION)

		#define C3_TRACE_BLOCK(_szBlockDescription) \
				C3DebugContext C3_TRACE_HACK_TOKENPASTE_1(ctx,__LINE__)("%s - %s",C3_PRETTY_FUNCTION,_szBlockDescription)

		#define C3_TRACE(_szFmt,...) C3DebugContext::trace(_szFmt,__VA_ARGS__)

	#endif //!C3_COMPILER_GCC

#else //!C3_COMPILE_DEBUG_MODE

	#define C3_ASSERT(__condition__) do { } while(0)
	#define C3_ASSERT_MSG(__condition__,__message__) do { } while(0)

	#define C3_TRACE_FUNCTION \
			do { } while(0)

	#define C3_TRACE_BLOCK(_szBlockDescription) \
			do { } while(0)

	#ifdef C3_COMPILER_GCC
		#define C3_TRACE(_szFmt,arg...) do { } while(0)
	#else
		#define C3_TRACE(_szFmt,...) do { } while(0)
	#endif 
#endif //!C3_COMPILE_DEBUG_MODE



#endif //!_c3_config_h_