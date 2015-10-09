/***********************************************************************//**
 *	Base.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/04/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_BASE_H_
#define _TFW_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "BaseTypes.h"
#include "debug/TfwDebug.h"

#if defined(_WINDOWS)
	#include <windows.h>
	#include <tchar.h>
#endif

#if defined(_DIRECTX)
	//#include <DirectX/DXUT.h>
#endif
	#define _OGL_1_0
	#if defined(_OGL_1_0)
		#if defined(_WINDOWS)
			#include <gl/gl.h>
			#include <gl/glu.h>
			#include <OpenGL/glext.h>
		#elif defined(_IOS)
			#include <OpenGLES/ES1/gl.h>
			#include <OpenGLES/ES1/glext.h>
		#elif defined(_ANDROID)
			#include <EGL/egl.h>
			#include <GLES/gl.h>
		#endif
	#endif
//#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <new>

////////////////////////////////////////////////////////////////////////////
// 定数

#define TFW_DPI						(6.2831853072f)
#define TFW_PI						(3.1415926535f)
#define TFW_HPI						(1.5707963267f)

#define TFW_COS45					(0.7071067811865475244f)

#define TFW_DISPF_RANG_MASK			(0x00000007)	// 回転マスク
#define TFW_DISPF_RANG_0			(0x00000001)	// 右回転 0° 表示
#define TFW_DISPF_RANG_90			(0x00000002)	// 右回転 90° 表示
#define TFW_DISPF_RANG_180			(0x00000003)	// 右回転 180° 表示
#define TFW_DISPF_RANG_270			(0x00000004)	// 右回転 270° 表示
///#define TFW_DISPF_WIDE				(0x00000010)	// ワイド表示
#define TFW_DISPF_ROT_COORD			(0x00000100)	// 座標系も回転
#define TFW_DISPF_DRANG_MASK		(0x00070000)	// デバイス回転マスク
#define TFW_DISPF_DRANG_0			(0x00010000)	// デバイス右回転 0° 設置中
#define TFW_DISPF_DRANG_90			(0x00020000)	// デバイス右回転 90° 設置中
#define TFW_DISPF_DRANG_180			(0x00030000)	// デバイス右回転 180° 設置中
#define TFW_DISPF_DRANG_270			(0x00040000)	// デバイス右回転 270° 設置中

////////////////////////////////////////////////////////////////////////////
// マクロ

//==========================================================================
// 定義用

#define TFW_BEGIN_NS namespace tfw {
#define TFW_END_NS }

#if defined(TFW_INTERFACE)
	#undef TFW_INTERFACE
#endif
#define TFW_INTERFACE class

//==========================================================================
// コンパイルスイッチ

#define SUPPORT_EXTLIB_ZLIB			(1)	// ZLIB サポート
#define SUPPORT_EXTLIB_LPNG			(1)	// PNG サポート
#define SUPPORT_EXTLIB_DTOA			(1)	// DTOA サポート

#define ENABLE_GETCURTFW			(0)	// 試作
#define TEST_PTCL					(0)	// パーティクルテスト

//==========================================================================
// ユーティリティ

#define TFW_IS_FLAG(v_, f_)				(((v_) & (f_)) != 0)
#define TFW_SET_FLAG(v_, f_, is_)		{ if(is_) { (v_) |= (f_); } else { (v_) &= ~(f_); } }

#define TFW_ABS(f_)						( ((f_) >= 0) ? (f_) : (-(f_)) )
#define TFW_FABS(f_)					( ((f_) >= 0.0f) ? (f_) : (-(f_)) )

#define TFW_IS_NEAR(f1_, f2_, d_)		( ( ((f2_) - (d_)) <= (f1_) ) && ( (f1_) <= ((f2_) + (d_)) ) )
#define TFW_IS_RANGE(a_, v1_, v2_)		( ((v1_) <= (a_)) && ((a_) <= (v2_)) )

#define TFW_CLAMP(v_, min_, max_)		{ if((v_) < (min_)) { (v_) = (min_); } else if((v_) > (max_)) { (v_) = (max_); } }

#define TFW_MAX(a_, b_)					( ((a_) >= (b_)) ? (a_) : (b_) )
#define TFW_MIN(a_, b_)					( ((a_) <= (b_)) ? (a_) : (b_) )

#define TFW_SWAP(a_, b_, temp_)			((temp_) = (a_), (a_) = (b_), (b_) = (temp_))

#define TFW_DEG_TO_RAD(a_)				((a_) * (TFW_PI / 180.0f))
#define TFW_RAD_TO_DEG(a_)				((a_) * (180.0f / TFW_PI))

#define TFW_BIT_ROT_L_32(a_, bits_)		(((a_) << (bits_)) | ((a_) >> (32 - (bits_))))

#define TFW_ZERO_MEMORY(p_, s_)			{ for(unsigned int ixx__ = 0; ixx__ < (unsigned int)(s_); ixx__++) { ((u8*)(p_))[ixx__] = 0; } }
#if 1
	#define TFW_COPY_MEMORY(dst_, src_, s_)	memcpy((dst_), (src_), (s_))
#else
	#define TFW_COPY_MEMORY(dst_, src_, s_)	{ for(unsigned int ixx__ = 0; ixx__ < (unsigned int)(s_); ixx__++) { ((u8*)(dst_))[ixx__] = ((u8*)(src_))[ixx__]; } }
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS
class Tfw;
TFW_END_NS

////////////////////////////////////////////////////////////////////////////
// 変数

#if ENABLE_GETCURTFW
// tfw 配下のモジュール参照用インスタンスポインタ
TFW_BEGIN_NS
static Tfw* getCurTfw();
TFW_END_NS
#endif

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_BASE_H_