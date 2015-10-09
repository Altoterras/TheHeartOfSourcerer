/***********************************************************************//**
 *	TfwTest.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2012/12/07.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TfwTest.h"

// Friends
#include "../string/VcString.h"
#include "../string/VwString.h"
#include "../txt/implement/SjisTxt.h"
#include "../txt/implement/UnicodeTxt.h"
#include "../txt/implement/Utf8Txt.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 静的変数

#if defined(_DEBUG) && 0
static TfwTest s_test;
#endif

////////////////////////////////////////////////////////////////////////////
// 静的関数

template <class TYPE>
TYPE* strrstr_test(const TYPE* string, const TYPE* pattern)
{
	const TYPE* last = 0L;
	for(const TYPE* p = string; 0 != (p = wcsstr(p, pattern)); ++p)
	{
		last = p;
		if('\0' == *p)
		{
			return (TYPE*)last;
		}
	}
	return (TYPE*)last;
}

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TfwTest メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	テスト 00
**//*---------------------------------------------------------------------*/
void TfwTest::test00()
{
#if defined(_DEBUG) && defined(_WINDOWS)
	::srand(100);
	for(int icnt = 0; icnt < 10000; icnt++)
	{
		u16 buf1[10];
		u16 buf2[10];
		for(int i = 0; i < 10; i++)
		{
			buf1[i] = ::rand() & 0xf;
			buf2[i] = ::rand() & 0xf;
		}
		u16 c = ::rand() & 0x1f;

		buf1[0] = 2;
		buf1[1] = 9;
		buf2[0] = 2;
		buf2[1] = 0;

		const u16* rs1 = wcschr(buf1, c);
		const u16* rs2 = wcsrchr(buf1, c);
		const u16* rs3 = wcsstr(buf1, buf2);
		const u16* rs4 = strrstr_test(buf1, buf2);
		const u16* rc1 = VStringBase<u16, WStringBase>::findStrByChar(buf1, c);
		const u16* rc2 = VStringBase<u16, WStringBase>::findLastStrByChar(buf1, c);
		const u16* rc3 = VStringBase<u16, WStringBase>::findStrByStr(buf1, buf2);
		const u16* rc4 = VStringBase<u16, WStringBase>::findLastStrByStr(buf1, buf2);

		/*
		WcvString str1(buf1);
		int i = str1.findReverse(buf2);
		rs4 = (i >= 0) ? &buf1[i] : 0L;
		*/

		if((rs1 != rc1) && (c != 0))
		{
			TRACE("!1 %x, %x, buf1={ ", rs1, rc1);
			for(int i = 0; i < 10; i++) { TRACE("%d, ", buf1[i]); }
			TRACE("} c=%d\n", c);
		}
		if((rs2 != rc2) && (c != 0))
		{
			TRACE("!2 %x, %x, buf1={ ", rs2, rc2);
			for(int i = 0; i < 10; i++) { TRACE("%d, ", buf1[i]); }
			TRACE("} c=%d\n", c);
		}
		if((rs3 != rc3) && (buf2[0] != 0))
		{
			TRACE("!3 %x, %x, buf1={ ", rs3, rc3);
			for(int i = 0; i < 10; i++) { TRACE("%d, ", buf1[i]); }
			TRACE("} buf2={ ");
			for(int i = 0; i < 10; i++) { TRACE("%d, ", buf2[i]); }
			TRACE("}\n");
		}
		if((rs4 != rc4) && (buf2[0] != 0))
		{
			TRACE("!4 %x, %x, buf1={ ", rs4, rc4);
			for(int i = 0; i < 10; i++) { TRACE("%d, ", buf1[i]); }
			TRACE("} buf2={ ");
			for(int i = 0; i < 10; i++) { TRACE("%d, ", buf2[i]); }
			TRACE("}\n");
		}

		if((rs1 != 0) || (rs2 != 0) || (rs3 != 0) || (rs4 != 0))
		{
			TRACE("# %x=%x, %x=%x, %x=%x, %x=%x\n", rs1, rc1, rs2, rc2, rs3, rc3, rs4, rc4);
		}
	}


	VwString wstr1;
	wstr1.add(0x5c71);	// 山
	wstr1.add(0x5e7b);	// 幻
	wstr1.add(0x6238);	// 戸
	UnicodeTxt wtxt1;
	wtxt1.setString(&wstr1, UnicodeTxt::F_REF_STRING);
	VwString wstr2;
//	wstr2.add(0x65e5);	// 日
	wstr2.add(0x41);	// A
	wstr2.add(0x42);	// B
	UnicodeTxt wtxt2;
	wtxt2.setWideString(&wstr2);

	VcString sstr1;
	sstr1.add(0x41);	// A
	Utf8Txt stxt1;
	stxt1.setSingleString(&sstr1);

	TxtPos tpos;
	wtxt1.getHearPos(&tpos);
	wtxt1.nextPos(&tpos);
	wtxt1.nextPos(&tpos);
//	wtxt1.setAt(&tpos, &wtxt2.iterator().cur());
	wtxt1.setAt(&tpos, &stxt1.iterator().cur());
#if 0
	wtxt1.nextPos(&tpos);
	wtxt1.removeAll();
	UnicodeTxt* utxt3;
	wtxt1.subtext((Txt**)&utxt3, &tpos);
	TxtPosArray tpa;
	wtxt1.remove(&tpa, &tpos);
	wtxt1.trimLeft(&tpa);
	TxtChar tc; 
	wtxt1.trim(&tpa, &tc);
#endif


	ASSERT(false);	// 要デバッグコード削除
#endif
}

/*---------------------------------------------------------------------*//**
	テスト 01
**//*---------------------------------------------------------------------*/
void TfwTest::test01()
{
}

/*---------------------------------------------------------------------*//**
	テスト 99
**//*---------------------------------------------------------------------*/
void TfwTest::test99()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TfwTest::TfwTest()
{
	test00();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TfwTest::~TfwTest()
{
	test99();
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
