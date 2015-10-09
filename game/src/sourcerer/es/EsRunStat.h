/***********************************************************************//**
 *	EsRunStat.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2010/02/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_RUN_STAT_H_
#define _ES_ES_RUN_STAT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スクリプト実行状態
 *
**//*---------------------------------------------------------------------*/
class EsRunStat
{
	//======================================================================
	// 定数
protected:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 実行継続中かどうかを得る
	inline bool isRunning() const { return _isRunning; }
	// エラーが起きたかどうか
	inline bool isErrorOccur() const { return _isError; }
	// 実行中ソースコードの開始インデックスを得る
	inline u32 getBeginCodeIndex() const { return _idxBegin; }
	// 実行中ソースコードの終了インデックスを得る
	inline u32 getEndCodeIndex() const { return _idxEnd; }

	// 実行継続中かどうかを設定する
	inline void setRunning(bool isRunning, bool isError)
	{
		_isRunning = isRunning;
		_isError = isError;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// コンストラクタ
	EsRunStat()
	{
		clear();
	}

	// 値のクリア
	void clear()
	{
		_isRunning = false;
		_isError = false;
		_idxBegin = 0;
		_idxEnd = 0;
	}

	// 実行中ソースのインデックスを設定する
	inline void setCodeIndex(u32 idxBegin, u32 idxEnd)
	{
		_idxBegin = idxBegin;
		_idxEnd = idxEnd;
	}

	//======================================================================
	// 変数
private:
	bool		_isRunning;	// 実行継続中かどうか
	bool		_isError;	// エラーが起きたかどうか
	u32			_idxBegin;	// 実行中ソースの開始インデックス
	u32			_idxEnd;	// 実行中ソースの終了インデックス
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_RUN_STAT_H_
