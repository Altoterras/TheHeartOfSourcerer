/***********************************************************************//**
 *	EventIdInfoArray.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/17.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_EVENT_ID_INFO_ARRAY_H_
#define _SRCR_EVENT_EVENT_ID_INFO_ARRAY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class XmlNode;

TFW_END_NS

SRCR_BEGIN_NS

class EventMatterCreateParam;

/*---------------------------------------------------------------------*//**
 *	イベント ID 情報配列
 *
**//*---------------------------------------------------------------------*/
class EventIdInfoArray
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	イベント ID 情報
	 *
	**//*-----------------------------------------------------------------*/
	class EventIdInfo
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// イベント ID を得る
		inline u16 getEvid() const { return _evid; }
		// イベント汎用パラメータを得る
		inline u32 getEventParam() const { return _param; }
		// ヒントを得る
		inline u32 getHint() const { return _hint; }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 外部サービス
	public:

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		EventIdInfo() : _evid(0), _param(0), _hint(0)
		{
		}

		void setup(u16 evid, u32 param, u32 hint)
		{
			_evid = evid;
			_param = param;
			_hint = hint;
		}

		//==================================================================
		// 変数
	private:
		u16 _evid;
		u32 _param;
		u32 _hint;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// イベント ID 情報配列の数を得る
	inline s32 getInfoNum() const { return _numEii; }
	// イベント ID 情報配列を得る
	inline const EventIdInfo* getInfo(s32 index) const { return ((_eiiarr != 0L) && (0 <= index) && (index < _numEii)) ? &_eiiarr[index] : 0L; }
	inline EventIdInfo* info(s32 index) { return ((_eiiarr != 0L) && (0 <= index) && (index < _numEii)) ? &_eiiarr[index] : 0L; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool setupFromXmlNode(const XmlNode* node, const CStringBase* nameAtrb, const EventMatterCreateParam* emcp);
	bool setupOneEvent(u16 evid, u32 param, u32 hint);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EventIdInfoArray();
	~EventIdInfoArray();

	//======================================================================
	// 変数
private:
	EventIdInfo* _eiiarr;	// イベント ID 情報配列
	s32 _numEii;			// イベント ID 情報配列の要素数
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EVENT_EVENT_ID_INFO_ARRAY_H_
