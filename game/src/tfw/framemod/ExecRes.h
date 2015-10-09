/***********************************************************************//**
 *  ExecRes.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FRAMEMOD_EXEC_RES_H_
#define _TFW_FRAMEMOD_EXEC_RES_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	制御結果
 *
**//*---------------------------------------------------------------------*/
class ExecRes
{
	//======================================================================
	// 定数
public:
	static const s32 F_NULL			= 0x00000000;
	static const s32 F_DONE			= 0x00000001;
	static const s32 F_DECIDE		= 0x00000010;
	static const s32 F_CANCEL		= 0x00000020;
	static const s32 F_YES			= 0x00000040;
	static const s32 F_NO			= 0x00000080;

	static const s32 F_CUSTOM_MASK	= 0xffff0000;

	//======================================================================
	// メンバ変数
protected:
	s32		_flag;
	s32		_value1;
	f32		_value2;
	void*	_obj1;
	void*	_obj2;
	bool	_extend;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 終了しているかどうかを得る
	inline bool	isDone() const		{	return (_flag & F_DONE) != 0;	}
	// 終了設定
	inline void	setDone()			{	_flag |= F_DONE;				}
	// キャンセル設定
	inline void	setCancel()			{	_flag |= F_CANCEL;				}
	// （サイズが）拡張されたクラスかどうか
	inline bool	isExtend() const	{	return _extend;					}
	
	// 値の取得
	inline s32 getGeneralValue1() const		{	return _value1;			}
	inline f32 getGeneralValue2() const		{	return _value2;			}
	inline void* getGeneralObject1() const	{	return _obj1;			}
	inline void* getGeneralObject2() const	{	return _obj2;			}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス

	// 汎用値を設定する
	void setGeneralValue1(s32 val)			{	_value1 = val;			}
	void setGeneralValue2(f32 val)			{	_value2 = val;			}
	void setGeneralObject1(void* obj)		{	_obj1 = obj;			}
	void setGeneralObject2(void* obj)		{	_obj2 = obj;			}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	ExecRes() : _flag(0), _value1(0), _value2(0.0f), _obj1(0L), _obj2(0L), _extend(false)
	{
	}

	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	ExecRes(bool extend) : _flag(0), _value1(0), _value2(0.0f), _obj1(0L), _obj2(0L), _extend(extend)
	{
	}

	/*-----------------------------------------------------------------*//**
		コピーコンストラクタ
	**//*-----------------------------------------------------------------*/
	ExecRes(const ExecRes& src)
	{
		copy(&src);
	}
	
	/*-----------------------------------------------------------------*//**
		コピー
	**//*-----------------------------------------------------------------*/
	ExecRes& copy(const ExecRes* src)
	{
		_flag = src->_flag;
		_value1 = src->_value1;
		_value2 = src->_value2;
		_obj1 = src->_obj1;
		_obj2 = src->_obj2;
		_extend = src->_extend;
		return *this;
	}
	
	/*-----------------------------------------------------------------*//**
		リセット
	**//*-----------------------------------------------------------------*/
	void reset()
	{
		_flag = 0;
		_value1 = 0;
		_value2 = 0.0f;
		_obj1 = 0L;
		_obj2 = 0L;
	}
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_FRAMEMOD_EXEC_RES_H_
