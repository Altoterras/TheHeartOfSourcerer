/***********************************************************************//**
 *	MoveCursor.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_MOVE_CURSOR_H_
#define _SRCR_GUI_MOVE_CURSOR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Point;
typedef Point<f32> PointF32;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

class Game;
class MoveCursorExtension;

/*---------------------------------------------------------------------*//**
 *	移動カーソル
 *
**//*---------------------------------------------------------------------*/
class MoveCursor : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	// カーソルの状態
	enum State
	{
		STATE_STOP,
		STATE_WALK,
		STATE_RUN
	};

	// 有効化・無効化チャンネル
public:
	static const u16	EDCH_PC1		= 0x0010;
	static const u16	EDCH_PC2		= 0x0020;
	static const u16	EDCH_PC3		= 0x0040;
	static const u16	EDCH_EVENT		= 0x0100;
	static const u16	EDCH_GUI		= 0x1000;
protected:
	static const u16	EDCH_EXT_MASK	= 0xfff0;
	static const u16	EDCH_SELF		= 0x0001;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 有効かどうかを得る
	inline bool isEnable(u16 edch) const { return !TFW_IS_FLAG(_edchFlags, edch); }
	// タッチしているかどうかを得る
	inline bool isTouching() const { return _isTouching; }
	// カーソルの状態を得る
	inline State getState() const { return _state; }
	// X 方向タッチ位置の差分を得る
	inline s32 getTouchDiffX() const { return _xDiff; }
	// Y 方向タッチ位置の差分を得る
	inline s32 getTouchDiffY() const { return _yDiff; }
	// タッチ開始位置を得る
	inline const PointF32* getStartPt() const { return _ptTouchStart; }
	// タッチ最終位置を得る
	inline const PointF32* getLastPt() const { return _ptTouchLast; }
	// 最大長さに対するレートを得る
	inline f32 getLengthRate() const { return _rateLength; }
	// X 方向タッチ位置差分の最大値を得る
	s32 getMaxTouchDiffX() const;
	// Y 方向タッチ位置差分の最大値を得る
	s32 getMaxTouchDiffY() const;
	// エクステンションを得る
	inline const MoveCursorExtension* getExtension() const { return _ext; }
	inline MoveCursorExtension* extension() { return _ext; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setEnable(u16 edch, bool isEnable);
	void reset();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MoveCursor();
	~MoveCursor();
	bool create(Texture* texRef);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void draw(const RenderCtx* rc);				// 描画

	void notifyChangedMap();
	void notifyChangeLeader();

	//======================================================================
	// メンバ変数
protected:
	u16 _edchFlags;
	bool _isTouching;
	PointF32* _ptTouchStart;
	PointF32* _ptTouchLast;
	s32 _xDiff;
	s32 _yDiff;
	f32 _angle;
	f32 _length;
	f32 _rateLength;
	State _state;
	Texture* _texRef;
	MoveCursorExtension* _ext;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_MOVE_CURSOR_H_
