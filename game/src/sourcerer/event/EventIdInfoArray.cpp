/***********************************************************************//**
 *	EventIdInfoArray.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EventIdInfoArray.h"

// Friends
#include "matter/EventMatterCreateParam.h"

// External
#include "../../tfw/lib/XmlParser.h"
#include "../../tfw/lib/XmlParserUtils.h"

// Library

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	XML からセットアップ
**//*---------------------------------------------------------------------*/
bool EventIdInfoArray::setupFromXmlNode(const XmlNode* node, const CStringBase* nameAtrb, const EventMatterCreateParam* emcp)
{
	s32* iarrWk = 0L;
	s32 numArr = 0;
	if(XmlParserUtils::analyzeAtrb(&iarrWk, &numArr, node, nameAtrb))
	{
		// 呼び出しイベント配列を作成する
		EventIdInfo* eiiarr = new EventIdInfo[numArr];
		if(eiiarr == 0L)
		{
			// 失敗
			delete[] iarrWk;
			return false;
		}
		for(int i = 0; i < numArr; i++)
		{
			u16 evid = (u16)iarrWk[i];
			if(emcp != 0L) { evid = emcp->offsetEvid(evid); }	// イベント ID のオフセット処理
			eiiarr[i].setup(evid, 0, 0); 
		}
		delete[] iarrWk;
		_eiiarr = eiiarr;
		_numEii = numArr;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	１つだけ保持するイベント情報としてセットアップ
**//*---------------------------------------------------------------------*/
bool EventIdInfoArray::setupOneEvent(u16 evid, u32 param, u32 hint)
{
	_eiiarr = new EventIdInfo[1];
	if(_eiiarr == 0L)
	{
		// 失敗
		return false;
	}
	_eiiarr[0].setup(evid, param, hint); 
	_numEii = 1;
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EventIdInfoArray::EventIdInfoArray()
	: _eiiarr(0L)
	, _numEii(0)
{

}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EventIdInfoArray::~EventIdInfoArray()
{
	delete[] _eiiarr;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
