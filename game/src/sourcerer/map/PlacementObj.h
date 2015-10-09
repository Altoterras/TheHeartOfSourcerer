/***********************************************************************//**
 *	PlacementObj.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/15.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAP_PLACEMENT_OBJ_H_
#define _SRCR_MAP_PLACEMENT_OBJ_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../status/EleneStat.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Serializer;
class XmlNode;

TFW_END_NS

SRCR_BEGIN_NS

class EventIdInfoArray;
class EventMatterCreateParam;
class Game;

/*---------------------------------------------------------------------*//**
 *	配置オブジェクト
 *
**//*---------------------------------------------------------------------*/
class PlacementObj
{
	//======================================================================
	// 定数
public:
	// オブジェクトの種類 ※連番注意（if 判定に利用している）
	enum Kind
	{
		KIND_NULL,
		KIND_EXTRA,
		KIND_ENEMY,
		KIND_MAPOBJ,
		KIND_DROPITEM,
		KIND_PICKUPITEM,
	};

	// 使用種別
	static const u8 UCASE_STORY		= 0x01;
	static const u8 UCASE_QUEST		= 0x02;
	static const u8 UCASE_MAP_TMP	= 0x03;
	static const u8 UCASE_MAP_CONF	= 0x04;

	// 対決オプション設定のデフォルト
	static const u16 BGM_ID_DEFAULT = 0;
	static const u16 EVID_ATER_WIN_DEFAULT = 0;
	static const u16 EVID_ATER_LOSE_DEFAULT = 0;
	static const bool IS_ALLOW_ESCAPE_DEFAULT = true;		// デフォルトは逃げる許可

	// 配置フラグ
	static const u8 PLACEF_NO_SAVE		=	0x01;	// 配置されてもセーブしない
	static const u8 PLACEF_NO_Y_COLL	=	0x02;	// Y 当たりを取らない

private:
	// 作成フラグ
	static const u8 MASK_F_CREATE		=	0x0f;	// 作成フラグのマスク
	static const u8	F_DISABLE			=	0x01;	// 無効
	static const u8	F_HIDDEN			=	0x02;	// 隠し
	static const u8	F_DISABLE_FOCUS		=	0x04;	// フォーカス無効
	// 状態フラグ
	static const u8 MASK_F_STATE		=	0xf0;	// 状態フラグのマスク
	static const u8	F_PLACED			=	0x10;	// 配置された
	static const u8	F_NOPLACE_PROB		=	0x20;	// 確率によって配置されなかった
	// 敵フラグ
	static const u8 ENEF_ALLOW_ESCAPE	=	0x01;	// 逃げる許可
	// モジュールフラグ
	static const u16 MF_QUEST_ACCEPTING	=	0x0001;	// クエスト受託中のみの配置

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	配置オブジェクトのセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class SaveStructure
	{
		//==================================================================
		// 変数
	public:
		// [  0]
		u8 _ucase;								// [  1] 使用種別
		u8 _flags;								// [  1] フラグ
		u8 _kind;								// [  1] オブジェクトの種類
		u8 _eneflags;							// [  1] 敵フラグ
		u16 _did;								// [  2] キャラクタ / アイテム 定義 ID
		u16 _evidCtrl;							// [  2] 制御イベント ID
		EleneStat::SaveStructure _eestat;		// [ 96] 元素バランス
		f32 _posX;								// [  4] 位置 X
		f32 _posY;								// [  4] 位置 Y
		f32 _posZ;								// [  4] 位置 Z
		u16 _bgmid;								//［  2］敵設定．BGM ID
		u16 _evidAfterWin;						//［  2］敵設定．勝利後に実行するイベント ID
		u16 _evidAfterLose;						//［  2］敵設定．敗北後に実行するイベント ID
		u16 _flagsMod;							// [  2] モジュールフラグ
		u8 _subid;								// [  1] 種別内 ID
		u8 __pad_0010[ 3 ];						// [  3]

		// [ 128]
		static const s32 SIZE_THIS = 128;
		SaveStructure() { TFW_ZERO_MEMORY(this, sizeof(SaveStructure)); }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 種類を得る
	inline Kind getKind() const { return (Kind)_kind;	}
	// 使用種別を得る
	inline u8 getUseCase() const { return _ucase; }
	// 種別内 ID を得る
	inline u8 getSubId() const { return _subid; }
	// 有効・無効を得る
	inline bool isEnable() const { return !TFW_IS_FLAG(_flags, F_DISABLE); }
	// 隠しかどうかを得る
	inline bool isHidden() const { return TFW_IS_FLAG(_flags, F_HIDDEN); }
	// フォーカス有効・無効を得る
	inline bool isEnableFocus() const { return !TFW_IS_FLAG(_flags, F_DISABLE_FOCUS); }
	// 配置されたかを得る
	inline bool isPlaced() const { return TFW_IS_FLAG(_flags, F_PLACED); }
	// 確率によって配置されなかったかを得る
	inline bool isNoPlaceByProbability() const { return TFW_IS_FLAG(_flags, F_NOPLACE_PROB); }
	// 初期状態かどうかを得る
	inline bool isNullState() const { return !TFW_IS_FLAG(_flags, MASK_F_STATE); }
	// キャラクタ定義 ID を得る
	inline u16 getCharDefId() const { return ((KIND_EXTRA <= _kind) && (_kind <= KIND_MAPOBJ)) ? _did : 0; }
	// アイテム定義 ID を得る
	inline u16 getItemDefId() const { return ((KIND_DROPITEM <= _kind) && (_kind <= KIND_PICKUPITEM)) ? _did : 0; }
	// マップ ID を得る
	inline s32 getMapId() const { return _mapid; }
	// 配置位置を示すイベント ID を得る
	inline u16 getPositionEvid() const { return _evidPos; }
	// 配置位置を得る
	inline Vector3F* getPosition() const { return _pos; }
	// 配置位置乱数半径を得る
	inline f32 getPositionRandomRadius() const { return _rRandomPos; }
	// 配置向きを得る
	inline Vector3F* getDirection() const { return _dir; }
	// 制御用イベント ID を得る
	inline u16 getCtrlEvid() const { return _evidCtrl; }
	// 呼び出しイベント情報を得る
	inline const EventIdInfoArray* getCallEvi() const { return _eiiarr; }
	inline EventIdInfoArray* callEvi() { return _eiiarr; }
	// 起動イベントを配置時から進めるセーブフラグ ID を得る
	inline s32 getDefaultNextCallEvSfid() const { return _sfidNextCallEv; }
	// 出現確率を得る
	inline u8 getAppearProbability() const { return _probability; }
	// 元素バランスを得る
	inline EleneStat* eleneStat() { return &_eestat; }
	// 歩行スピードを得る
	inline f32 getWalkingSpeed() const { return _walkspeed; }
	// BGM ID を得る
	inline u16 getBgmId() const { return _bgmid; }
	// 勝利後に実行するイベント ID を得る
	inline u16 getAfterWinEvent() const { return _evidAfterWin; }
	// 敗北後に実行するイベント ID を得る
	inline u16 getAfterLoseEvent() const { return  _evidAfterLose; }
	// 配置フラグを得る
	inline u8 getPlaceFlags() const { return _placeflags; }
	// 対決時に逃げるが許可されるかを得る
	inline bool isAllowConfrontEscape() const { return TFW_IS_FLAG(_eneflags, ENEF_ALLOW_ESCAPE); }
	// クエスト受託中のみの配置かどうかを得る
	inline bool isQuestAcceptingOnly() const { return TFW_IS_FLAG(_flagsMod, MF_QUEST_ACCEPTING); }
	// 識別キーを得る
	inline u16 getDescKey() const { return _desckey; }
	// 現在のイベント情報インデックスを得る
	inline s32 getActiveCallEviIndex() const { return _idxCallEvi; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool setupFromXmlNode(const XmlNode* node, u8 ucase, u8 subid, const EventMatterCreateParam* emcp);
	bool setup(Kind kind, u8 ucase, u8 subid, u16 chrdid, u16 itmdid, s32 mapid, u16 evidPos, u8 probability, EventIdInfoArray* eiiarr, const Vector3F* pos);

	void setCtrlEvid(u16 evidCtrl) { _evidCtrl = evidCtrl; }
	void setPlaced(bool isPlaced) { TFW_SET_FLAG(_flags, F_PLACED, isPlaced); }
	void setNoPlaceByProbability(bool isNoPlace) { TFW_SET_FLAG(_flags, F_NOPLACE_PROB, isNoPlace); }
	void setDescKey(u32 desckey) { _desckey = desckey; }

	void nextCallEvi(bool isAutoRewind);

	bool isNewPlacedCondition() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PlacementObj();
	~PlacementObj();
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);
	bool storeToStructure(SaveStructure* svst) const;
	bool restoreFromStructure(const SaveStructure* svst);

	//======================================================================
	// 変数
private:
	// 基本情報
	u8 _kind;					// ［必須］種類
	u8 _flags;					// ［オプション］フラグ
	u8 _ucase;					// ［オプション］使用種別
	u8 _subid;					// ［オプション］種別内 ID
	u8 _probability;			// ［オプション］出現確率（デフォルトは 100%）
	u16 _did;					// ［オプション］キャラクタ ／ アイテム 定義 ID
	u16 _evidPos;				// ［オプション］配置イベント ID
	u16 _evidCtrl;				// ［オプション］制御イベント ID
	s32 _mapid;					// ［オプション］出現マップ ID
	s32 _storyidMin;			// ［オプション］出現ストーリ ID 最小値（デフォルトは 0 で、最小値制限なし）
	s32 _storyidMax;			// ［オプション］出現ストーリ ID 最大値（デフォルトは 0 で、最大値制限なし）
	EventIdInfoArray* _eiiarr;	// ［オプション］起動イベント ID 配列
	s32 _sfidNextCallEv;		// ［オプション］起動イベントを配置時から進めるセーブフラグ ID
	Vector3F* _pos;				// ［オプション］位置
	Vector3F* _dir;				// ［オプション］向き（ラジアン）
	EleneStat _eestat;			// ［オプション］元素バランス（初期値の場合はデフォルト設定を使用する）
	f32 _walkspeed;				// ［オプション］歩行速度（デフォルトは 0 で、歩かない；現状ランダム歩行のみ対応）
	f32 _rRandomPos;			// ［オプション］位置乱数半径（デフォルトは 0 で、乱数なし）
	u16 _bgmid;					// ［オプション］敵設定．BGM ID
	u16 _evidAfterWin;			// ［オプション］敵設定．勝利後に実行するイベント ID
	u16 _evidAfterLose;			// ［オプション］敵設定．敗北後に実行するイベント ID
	u8 _eneflags;				// ［オプション］敵フラグ
	u8 _placeflags;				// ［オプション］配置フラグ
	u16 _flagsMod;				// ［オプション］モジュールフラグ

	// 作業変数
	u32 _desckey;				// 識別キー
	s32 _idxCallEvi;			// 現在の起動イベント（※セーブされない点に注意）
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_MAP_PLACEMENT_OBJ_H_
