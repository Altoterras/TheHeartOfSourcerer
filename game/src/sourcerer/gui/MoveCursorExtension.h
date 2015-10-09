/***********************************************************************//**
 *	MoveCursorExtension.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/18.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_MOVE_CURSOR_EXTENSION_H_
#define _SRCR_GUI_MOVE_CURSOR_EXTENSION_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
template<class TYPE> class Point;
typedef Point<f32> PointF32;

TFW_END_NS

SRCR_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	移動カーソル拡張
 *
**//*---------------------------------------------------------------------*/
class MoveCursorExtension
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 操作ユニットを得る
	inline const Unit* getHandledUnit() const { return _unitHandled; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void registerHandlingableUnit(const Unit* unit);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MoveCursorExtension();
	virtual ~MoveCursorExtension();
	bool create();
	void destroy();

	void onTapStart(const PointF32* ptTouch);
	void onTapEnd();

	void notifyChangeLeader();

	//======================================================================
	// メンバ変数
private:
	List<const Unit*>* _listUnit;
	const Unit* _unitHandled;
	const Unit* _unitDefault;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_MOVE_CURSOR_EXTENSION_H_
