/***********************************************************************//**
 *	Unit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_UNIT_H_
#define _SRCR_UNIT_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"

#include "UnitType.h"
#include "UnitArray.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Model;

TFW_END_NS

SRCR_BEGIN_NS

class StatusChangeResult;
class Confront;
class Elene;
class EleneStat;
class Game;
class Spirit;
class TransStat;
class UnitSaveStructure;

/*---------------------------------------------------------------------*//**
 *	ユニット基本クラス
 *		※	ユニットは、画面に３Ｄ配置されるゲーム的論理単位である．
 *			モデルよりも粒度が荒く、ユーザーがユニット単位でなんらかの
 *			操作・干渉・フォーカスができるものとする．
 *			原則として、それ以外の例外的論理単位は MagicCluster のみとする．
 *
**//*---------------------------------------------------------------------*/
class Unit : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	// ユニットフラグ（生成後は固定）
	static const u16		UNITFLAG_CHAR					= 0x0010;	// キャラクタ
	static const u16		UNITFLAG_PC						= 0x0020;	// プレイヤーキャラクタ（味方キャラ）
	static const u16		UNITFLAG_NPC					= 0x0040;	// ノンプレイヤーキャラクタ
	static const u16		UNITFLAG_ITEM					= 0x0100;	// アイテム
	static const u16		UNITFLAG_EQUIPMENT_ITEM			= 0x0200;	// 装備アイテム
	static const u16		UNITFLAG_FIELD_ITEM				= 0x0400;	// フィールドアイテム
	static const u16		UNITFLAG_PUT_ITEM				= 0x0800;	// 置かれたアイテム
	static const u16		UNITFLAG_CFTRG					= 0x1000;	// 対決対象の非敵
	static const u16		UNITFLAG_TRICK					= 0x2000;	// 仕掛け
	static const u16		UNITFLAG_DEPEND_MAP				= 0x0001;	// マップ依存のユニット
	static const u16		UNITFLAG_CREATION_FOCUSABLE		= 0x0002;	// 生成時、フォーカス可能なユニット（実値は AF_FOCUSABLE）
	static const u16		UNITFLAG_CREATION_HIDDEN		= 0x0004;	// 生成時、隠し（実値は AF_HIDDEN）
	static const u16		UNITFLAG_CREATION_HIDN_CLICK	= 0x0008;	// 生成時、画面上にクリック対象を描画しない（実値は AF_HIDN_CLICK）

	// カテゴリ
	enum UnitCategory
	{
		UNITCAT_NULL,
		UNITCAT_CHAR,
		UNITCAT_ITEM,
	};

	// 創造できる元素エネルギー（getEvokableElene）の取得値カテゴリ
	enum EvokableElneCategory
	{
		EVECAT_TOTAL,		// すべて
		EVECAT_SELF,		// 自分自身からの影響値
		EVECAT_EQUIP,		// 装備アイテムからの影響値
		EVECAT_PARTY,		// パーティメンバー（自分自身と支援モンスターは含まない）からの影響値
		EVECAT_HELPER,		// 支援モンスターからの影響値
		EVECAT_MAP,			// マップからの影響値
	};

private:
	// 動作フラグ（生成後も動的）
	static const u16		AF_ENABLE		= 0x0001;	// 有効である
	static const u16		AF_GUI_FOCUSED	= 0x0002;	// GUI フォーカスされている
	static const u16		AF_FOCUSABLE	= 0x0004;	// フォーカス可能なユニット
	static const u16		AF_HIDDEN		= 0x0008;	// 隠し（画面上に表示・選択できない；メニューやソウマからは選択できる）
	static const u16		AF_HIDN_CLICK	= 0x0010;	// 画面上にクリック対象を描画しない

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ユニットクラスを得る
	inline UnitType getUnitType() const { return (UnitType)_unittype; }
	// ユニットフラグを得る
	inline u16 getUnitFlags() const { return _unitflags; }
	// ID を得る
	inline u16 getUnitId() const { return _unitid; }
	// 位置を得る
	inline const Vector3F* getPosition() const { return _pos; }
	// 中心位置を得る
	virtual const Vector3F* getCenterPos() const { return _pos; }	// デフォルトでは getPosition と同じ
	// 方向を得る
	virtual const Vector3F* getDirection() const { return 0L; }
	// 半径を得る
	inline f32 getRadius() const { return _radius; }
	// モデル数を得る
	inline s32 getModelNum() const { return _numMdl; }
	// モデルを取得する
	inline Model* getModel(int index) const { return _mdlarrRef[index]; }
	// フレンド関係にあるユニットの配列を得る
	virtual UnitArray getFriends() const { return UnitArray(); }	// デフォルトでは長さゼロのユニット配列を返す
	// 有効かどうかを得る
	inline bool isEnable() const { return TFW_IS_FLAG(_aflags, AF_ENABLE); }
	// 現在 GUI フォーカスされているかどうかを得る
	inline bool isGuiFocused() const { return TFW_IS_FLAG(_aflags, AF_GUI_FOCUSED); }
	// フォーカス可能かを得る
	inline bool isEnableFocus() const { return TFW_IS_FLAG(_aflags, AF_FOCUSABLE); }
	// 隠し属性かどうかを得る
	inline bool isHidden() const { return TFW_IS_FLAG(_aflags, AF_HIDDEN); }
	// クリック対象隠し属性かどうかを得る
	inline bool isHiddenClick() const { return TFW_IS_FLAG(_aflags, AF_HIDN_CLICK); }
	// マップ依存ユニットかを得る
	inline bool isMapDepend() const { return TFW_IS_FLAG(_unitflags, UNITFLAG_DEPEND_MAP); }
	// 表示中かどうかを得る
	bool isShow() const;
	// 指定カテゴリかどうかを得る
	bool isCategory(UnitCategory ucat) const;
	// 味方かどうかを得る
	bool isFriendsSide(const Unit* unitTrg) const;
	// 制御用イベント ID を得る
	virtual u16 getCtrlEvid() const { return 0; }
	// スピリットを得る（無い場合は NULL が返る）
	inline virtual const Spirit* getSpirit() const { return 0L; }
	inline virtual Spirit* spirit() const { return 0L; }
	// 元素エネルギーを得る（無い場合は NULL が返る）
	virtual const EleneStat* getEleneStat() const {	return 0L; }
	virtual EleneStat* eleneStat() { return 0L; }
	// オーナーユニットを得る（アイテムなどの従属ユニットのみ．無い場合は NULL が返る）
	virtual Unit* getOwnerUnit() const { return 0L; }
	// 代表名を得る（無い場合は NULL が返る）
	virtual const VcString* getMainName() const {	return 0L; }

	// 一度に呼び出せる元素エネルギー量を得る（ソウマを使えるユニットのみの実装）
	virtual bool getEvokableElene(Elene* elene, EvokableElneCategory evecat) const { return false; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setEnable(bool isEnable);
	virtual void setPosition(const Vector3F* pos);
	virtual void setGuiFocused(bool isFocused) { TFW_SET_FLAG(_aflags, AF_GUI_FOCUSED, isFocused); }
	virtual void setEnableFocus(bool isFocusable) { TFW_SET_FLAG(_aflags, AF_FOCUSABLE, isFocusable); }
	virtual void setHidden(bool isHidden) { TFW_SET_FLAG(_aflags, AF_HIDDEN, isHidden); }
	virtual void setHiddenClick(bool isHiddenClick) { TFW_SET_FLAG(_aflags, AF_HIDN_CLICK, isHiddenClick); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Unit(UnitType unittype);
	virtual ~Unit();
	virtual bool create(Model* mdlRef, Spirit* sprtRef, u16 unitflags);
	virtual bool create(Model** mdlarrRef, s32 numModel, Spirit* sprtRef, u16 unitflags);
	virtual void destroy();
	void exec(ExecRes* res, const ExecCtx* ec)	{}	// フレーム制御
	void render(const RenderCtx* rc)			{}	// 描画
	virtual void notifyChangeMapBefore();
	virtual void notifyChangedMap();
	virtual void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition);
	virtual void notifyChangedEventMode(bool isEventMode);
	virtual void notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat);

	bool storeToStructure(UnitSaveStructure* svst) const;
	bool restoreFromStructure(const UnitSaveStructure* svst);

private:
	void setShow(bool isShow);

	//======================================================================
	// 変数
protected:
	u8			_unittype;			// ユニットタイプ（UNITTYPE_*）
	u8			__pad_0010[ 1];
	u16			_unitid;			// ユニット ID
	u16			_unitflags;			// ユニットフラグ
	u16			_aflags;			// 動作フラグ

	Model**		_mdlarrRef;			// モデル参照配列
	s32			_numMdl;			// モデル数

	Vector3F*	_pos;				// 位置
	f32			_radius;			// モデル半径
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_UNIT_H_
