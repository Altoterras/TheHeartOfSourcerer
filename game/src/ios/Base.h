//
//  Base.h
//  Sourcerer01
//
//  Created by 岸 亮太郎 on 2015/05/15.
//  Copyright (c) 2015年 Ryoutarou Kishi. All rights reserved.
//

#ifndef Sourcerer01_Base_h
#define Sourcerer01_Base_h

////////////////////////////////////////////////////////////////////////////
// マクロ

#if !defined(TRACEERR)
	#if defined(_DEBUG) && 0
		#define TRACEERR(s_)			::fprintf(stderr, s_)
		#define TRACEERRV(s_, ...)		::fprintf(stderr, s_, __VA_ARGS__)
	#else
		#define TRACEERR(s_)			//::fprintf(stderr, s_)
		#define TRACEERRV(s_, ...)		//::fprintf(stderr, s_, __VA_ARGS__)
	#endif
#endif

#if !defined(TRACELOG)
	#if defined(_DEBUG)
		#define TRACELOG(s_)			::fprintf(stderr, s_)
		#define TRACELOGV(s_, ...)		::fprintf(stderr, s_, __VA_ARGS__)
	#else
		#define TRACELOG(s_)			//::fprintf(stderr, s_)
		#define TRACELOGV(s_, ...)		//::fprintf(stderr, s_, __VA_ARGS__)
	#endif
#endif

////////////////////////////////////////////////////////////////////////////

#endif
