/***********************************************************************//**
 *	MotionController.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2013/07/28.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_UI_MOTION_CONTROLLER_H_
#define _TFW_UI_MOTION_CONTROLLER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "Ui.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector2;
typedef Vector2<s32> Vector2S32;
template<class TYPE> class Point;
typedef Point<f32> PointF32;
class Pad;
class View;

/*---------------------------------------------------------------------*//**
 *	タッチパネル
 *
**//*---------------------------------------------------------------------*/
class MotionController : public Ui
{
	//======================================================================
	// 定数
public:
	enum Action
	{
		ACT_NULL = 0,
		ACT_SHAKE,
		ACT_CANCEL,
	};

private:
	enum EventCase
	{
		EVC_BEGIN,
		EVC_END,
	};
	
	static const s32 TCNT_NOTHING		= -1;
	static const s32 TCNT_ON_END		= -101;
	static const s32 TCNT_END			= -100;
	static const s32 TCNT_ON_CANCEL		= -201;
	static const s32 TCNT_CANCEL		= -200;
	static const s32 TCNT_ON_BEGIN		= 0;
	static const s32 TCNT_BEGIN			= 1;

	//======================================================================
	// クラス
public:
	class Motion
	{
	public:
		Motion()			{}
		inline void set()	{}
	};

private:
	class Mi
	{
	public:
		bool		_isMotion;
		
		Mi();
		~Mi();
		void reset();
	};

	class Res
	{
	public:
		bool		_action;
		Action		_act;

		Res();
		~Res();
		void reset();
		void set(Action act);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// モーション操作中かどうかの判定
	bool isMotion() const;
	// モーション終了判定
	bool isMotionEnd() const;
	
	Action getLastAction() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MotionController();
	virtual ~MotionController();
	bool create();
	void destroy();
	virtual void exec();

	void beganMotion(Motion* motion);
	void endedMotion(Motion* motion);
	void cancelledMotion(Motion* motion);
	
private:
	void convMotion(Motion* motion, EventCase evc);

	//======================================================================
	// 変数
private:
	s32			_cntMotionFrame;	// モーション継続フレームカウンタ
	Mi			_mi;				// モーション情報
	Res			_res;				// タッチリザルト
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_UI_MOTION_CONTROLLER_H_