/***********************************************************************//**
 *	PopNumEffect.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/12.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EFFECT_POP_NUM_EFFECT_H_
#define _SRCR_EFFECT_POP_NUM_EFFECT_H_

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
class StatusChangeResult;
class CharParam;
class Game;
class PcUnit;
class Unit;

/*---------------------------------------------------------------------*//**
 *	ダメージ値などの画面表示
 *
**//*---------------------------------------------------------------------*/
class PopNumEffect
{
	//======================================================================
	// 定数
public:
	enum Kind
	{
		K_NULL,
		K_ENE_DMG,
		K_ENE_RECV,
		K_PHYS_DMG,
		K_PHYS_RECV,
		K_MISS,
	};
	enum Align
	{
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER,
	};

	//======================================================================
	// クラス
private:
	class PopNum
	{
	public:
		PopNum();
		~PopNum();

	public:
		s32 _num;
		Kind _kind;
		f32 _fcnt;
		const Unit* _unitTrg;
		Vector2F* _pos;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void on(s32 num, PopNumEffect::Kind kind, const Unit* unitTrg);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PopNumEffect();
	~PopNumEffect();
	bool create();
	void destroy();
	void exec(const ExecCtx* ec);
	void draw(const RenderCtx* rc);

private:
	void drawPopNum(const RenderCtx* rc, Texture* texGui, f32 x, f32 y, u32 num, PopNumEffect::Align align);
	void drawPopNumMiss(const RenderCtx* rc, Texture* texGui, f32 x, f32 y, PopNumEffect::Align align);

	//======================================================================
	// 変数
private:
	List<PopNum*>* _listPop;
	bool _isSelfHide;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_BODY_CONDUCT_H_
