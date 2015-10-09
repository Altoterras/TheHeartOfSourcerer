/***********************************************************************//**
 *	Conduct.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BODY_CONDUCT_H_
#define _SRCR_BODY_CONDUCT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
template<class TYPE> class List;
template<class TYPE> class Vector2;
typedef Color<u8> ColorU8;
typedef Vector2<f32> Vector2F;
class ExecCtx;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

class GameActMsgWindow;
class CharParam;
class Game;
class PcUnit;
class PopNumEffect;
class Unit;
class StatusChangeResult;

/*---------------------------------------------------------------------*//**
 *	行為の伝達・表示・換算
 *		※	GameActMsgWindow への表示
 *
**//*---------------------------------------------------------------------*/
class Conductor
{
	//======================================================================
	// クラス
private:
	class Msg
	{
	public:
		Msg();
		~Msg();

	public:
		VcString* _strMsg;
		ColorU8* _color;
		f32 _frameRest;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void on(const VcString* strMsg, const Unit* unitActor, s32 frameDelay);
	void on(const VcString* strMsg, const ColorU8* color, s32 frameDelay);
	void on(const StatusChangeResult* scres, s32 frameDelay);
	void onExperienceNow(u16 experience);
	void on(s32 num, s32 kind, const Unit* unitTrg);

	static void getSysRewardColor(ColorU8* color);
	static void getUserSsDebugColor(ColorU8* color);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Conductor();
	~Conductor();
	bool create();
	void destroy();
	void exec(const ExecCtx* ec);
	void draw(const RenderCtx* rc);

private:
	void onCharStatResultExp(const StatusChangeResult* scres, s32 frameDelay);
	void onCharStatResult(const StatusChangeResult* scres, s32 frameDelay);
	bool onVeilResult(const StatusChangeResult* scres, const VcString* nameTrg, s32 iveil, bool isStren, s32 frameDelay);
	static void getEleneAtrbString(VcString* str, u32 ff);
	void onExperience(u16 experience, s32 frameDelay);
	bool onExperiencePc(CharParam* cprmOldOut, CharParam* cprmNewOut, PcUnit* unitPc, s32 experience, s32 frameDelay);

	//======================================================================
	// 変数
private:
	List<Msg*>* _listMsg;
	PopNumEffect* _popnef;
	GameActMsgWindow* _wndActMsgRef;
	f32 _fcntChainBonus;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_BODY_CONDUCT_H_
