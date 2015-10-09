/***********************************************************************//**
 *	GameMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/06/29.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _DIMZ_MODE_GAME_MODE_H_
#define _DIMZ_MODE_GAME_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "GameModeBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

DIMZ_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ゲームモード クラス
 *
**//*---------------------------------------------------------------------*/
class GameMode : public GameModeBase
{
	//======================================================================
	// 定数
public:
	// モード
	enum ModeKind
	{
		MODE_NULL = 0,

		// 大域ゲームモード
		MODE_GAME__START_ = 1,
		MODE_GAME_STARTGAME,		// ( 2) ゲーム開始
		MODE_GAME_INGAME,			// ( 3) インゲーム
		MODE_GAME_TITLE,			// ( 4) タイトル
		MODE_GAME_ENDING,			// ( 5) エンディング
		MODE_GAME_MOVIE,			// ( 6) ムービー
		MODE_GAME_MODELVW,			// ( 7) モデルビューワー
		MODE_GAME_BOOKVW,			// ( 8) ブックビューワー
		MODE_GAME_TXTEDITOR,		// ( 9) テキストエディタ
		MODE_GAME_PUREPUZZLE,		// (10) パズル
		MODE_GAME_TESTVIEWTERM,		// (11) テストビュー終端
		MODE_GAME__END_,

			// インゲームモード
			MODE_GAME_INGAME__START_ = 20,
			MODE_GAME_INGAME_WALKING,		// (21) 移動中
			MODE_GAME_INGAME_CONFRONT,		// (22) 対決
			MODE_GAME_INGAME_EVENT,			// (23) イベント
			MODE_GAME_INGAME_MENU,			// (24) メニュー
			MODE_GAME_INGAME_WORLDMAP,		// (25) 地図
			MODE_GAME_INGAME__END_,

				// 移動中モード
				MODE_GAME_INGAME_WALKING__START_ = 30,
				MODE_GAME_INGAME_WALKING_NORMAL,		// (31) 通常（移動）
				MODE_GAME_INGAME_WALKING_MAGICTYPING,	// (32) 魔法入力
				MODE_GAME_INGAME_WALKING_ITEMUSING,		// (33) アイテム使用
				MODE_GAME_INGAME_WALKING__END_,

				// 対決中モード
				MODE_GAME_INGAME_CONFRONT__START_ = 40,
				MODE_GAME_INGAME_CONFRONT_NORMAL,		// (41) 通常（移動）
				MODE_GAME_INGAME_CONFRONT_MAGICTYPING,	// (42) 魔法入力
				MODE_GAME_INGAME_CONFRONT_ITEMUSING,	// (44) アイテム使用
				MODE_GAME_INGAME_CONFRONT__END_,

			// パズルモード
			MODE_GAME_PRPZL__START_ = 50,
			MODE_GAME_PRPZL_MENU,			// (51) パズルメニュー
			MODE_GAME_PRPZL_PLAY,			// (52) パズルプレイ
			MODE_GAME_PRPZL__END_,

		// テンプレート用のダミーモード
		MODE_TEMPLATE__START_ = 90,
		MODE_TEMPLATE_DUMMY_1,
		MODE_TEMPLATE_DUMMY_2,
		MODE_TEMPLATE__END_,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:
	// ゲームモード種別を得る
	inline ModeKind getCurrentGameModeKind() const	{	return (ModeKind)getCurrentModeId();				}
	// 末端のゲームモード種別を取得する
	inline ModeKind getChildEndModeKind() const		{	return (_modeCurChildEnd != 0L) ? (ModeKind)_modeCurChildEnd->getSelfModeId() : MODE_NULL;	}
	// 末端のモードを取得する
	inline Mode* getChildEndMode() const			{	return _modeCurChildEnd;							}
	// 末端のゲームモード種別を予約されたものも評価して取得する
	ModeKind getChildEndModeKindConsiderReserved() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool changeEndMode(ModeKind modek);
	bool changePrevEndMode();

private:
	void setMode(short modeid, bool isReset)	{	Mode::setMode(modeid, isReset);			}
	void setPrevMode()							{	Mode::setPrevMode();					}
	void reserveMode(short modeid)				{	Mode::reserveMode(modeid);				}
	void reserveModeCancel(short modeidCancel)	{	Mode::reserveModeCancel(modeidCancel);	}
	void setFromReservedMode()					{	Mode::setFromReservedMode();			}

public:
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
	GameMode();
	~GameMode();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);

private:
	Mode* getModeDive(Mode* mode, ModeKind modek) const;
	Mode* getChildEndModeDive(const Mode* mode) const;

	void notifyChildModeChanged(short modeid, void* objParamRef);

	//======================================================================
	// メンバ変数
private:
	Mode* _modeCurChildEnd;
	ModeKind _modekindCurChildEnd;
	ModeKind _modekindReserveEnd;
	GameModeParam _param;
};

DIMZ_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _DIMZ_MODE_GAME_MODE_H_
