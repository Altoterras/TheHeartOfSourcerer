/***********************************************************************//**
 *	FieldItemUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_ITEM_FIELD_ITEM_UNIT_H_
#define _SRCR_UNIT_ITEM_FIELD_ITEM_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "ItemUnit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class CharUnitSaveStructure;
class CircleShadow;
class PlacementObj;

/*---------------------------------------------------------------------*//**
 *	マップに置かれたアイテム ユニット
 *
**//*---------------------------------------------------------------------*/
class FieldItemUnit : public ItemUnit
{
	//======================================================================
	// 定数
private:
	// パーティクル表示間隔
	static const s32 FRAME_BLINK_MIN	= 15;
	static const s32 FRAME_BLINK_MAX	= 4 * 30;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 配置オブジェクト情報を得る
	inline PlacementObj* getPlacementObj() const { return _pobjRef; }
	// 制御用イベント ID を得る
	virtual u16 getCtrlEvid() const;

	// 配置オブジェクト情報を設定する
	inline void setPlacementObj(PlacementObj* pobj) { _pobjRef = pobj; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FieldItemUnit();
	~FieldItemUnit();
	bool create(u16 itmdid, const EleneStat* eestatDefault, bool isHasCircleShadow);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);			// 描画

	bool storeToStructure(CharUnitSaveStructure* svst) const;
	bool restoreFromStructure(const CharUnitSaveStructure* svst);

	//======================================================================
	// メンバ変数
protected:
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// モデル表示系変数
	s32					_idxCurGcolPlane;		// 現在の地面コリジョン平面インデックス
	f32					_fcntShow;				// 表示フレームカウンタ

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 論理動作系変数
	EleneStat*			_eestat;				// エネルギー状態
	PlacementObj*		_pobjRef;				// 配置オブジェクト情報
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_ITEM_FIELD_ITEM_UNIT_H_
