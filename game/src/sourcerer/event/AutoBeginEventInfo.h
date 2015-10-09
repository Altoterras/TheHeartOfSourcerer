/***********************************************************************//**
 *	AutoBeginEventInfo.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/17.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_AUTO_BEGIN_EVENT_INFO_H_
#define _SRCR_EVENT_AUTO_BEGIN_EVENT_INFO_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class XmlNode;

TFW_END_NS

SRCR_BEGIN_NS

class EventIdInfoArray;
class EventMatterCreateParam;

/*---------------------------------------------------------------------*//**
 *	自動起動イベント情報
 *
**//*---------------------------------------------------------------------*/
class AutoBeginEventInfo
{
	//======================================================================
	// 定数
public:
	// 種別
	enum Kind
	{
		KIND_NULL,
		KIND_STORY_START,
		KIND_MAP_START,
		KIND_LOAD_INTVEV_START,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 種類を得る
	inline Kind getKind() const { return _kind;	}
	// 呼び出しイベント情報を得る
	inline const EventIdInfoArray* getCallEvi() const { return _eiiarr; }
	inline EventIdInfoArray* callEvi() { return _eiiarr; }
	// マップ ID を得る
	inline s32 getMapId() const { return _mapid; }
	// true のとき実行する条件セーブフラグ ID を得る
	inline s32 getPositiveConditionSaveFlagsId() const { return _sfidCondP; }
	// false のとき実行する条件セーブフラグ ID を得る
	inline s32 getNegativeConditionSaveFlagsId() const { return _sfidCondN; }
	// 実行後に true を書き込むセーブフラグ ID を得る
	inline s32 getPositiveWriteSaveFlagsId() const { return _sfidWriteP; }
	// 実行後に false を書き込むセーブフラグ ID を得る
	inline s32 getNegativeWriteSaveFlagsId() const { return _sfidWriteN; }

	// 識別キーを得る
	inline u32 getDescKey() const { return _desckey; }
	// 現在のイベント情報インデックスを得る
	inline s32 getActiveCallEviIndex() const { return _idxCallEvi; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool setupFromXmlNode(const XmlNode* node, const EventMatterCreateParam* emcp);
	bool setupForStoryStart(u16 evid);
	bool setupForLoadIntervalEventStart(u16 evid);

	void setDescKey(u32 desckey) { _desckey = desckey; }

	void nextCallEvi(bool isAutoRewind);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	AutoBeginEventInfo() 
		: _kind(KIND_NULL)
		, _eiiarr(0)
		, _mapid(0)
		, _sfidCondP(0)
		, _sfidCondN(0)
		, _sfidWriteP(0)
		, _sfidWriteN(0)
		, _desckey(0)
		, _idxCallEvi(0)
	{
	}
	~AutoBeginEventInfo();

	//======================================================================
	// 変数
private:
	// 基本情報
	Kind _kind;					// 種別
	EventIdInfoArray* _eiiarr;	// 起動イベント 情報 配列
	s32 _mapid;					// マップ ID
	s32 _sfidCondP;				// true のとき実行する条件セーブフラグ ID
	s32 _sfidCondN;				// false のとき実行する条件セーブフラグ ID
	s32 _sfidWriteP;			// 実行後に true を書き込むセーブフラグ ID
	s32 _sfidWriteN;			// 実行後に false を書き込むセーブフラグ ID

	// 作業変数
	u32 _desckey;				// 識別キー
	s32 _idxCallEvi;			// 現在の起動イベント（※セーブされない点に注意）
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EVENT_AUTO_BEGIN_EVENT_INFO_H_
