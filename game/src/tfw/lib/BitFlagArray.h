/***********************************************************************//**
 *	BitFlagArray.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2011/08/05.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_BIT_FLAG_ARRAY_H_
#define _TFW_LIB_BIT_FLAG_ARRAY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ID マネージャ クラス
 *	
**//*---------------------------------------------------------------------*/
class BitFlagArray
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline u32 getFlagNum() const { return _numFlag; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool getFlag(u16 flag) const;
	bool setFlag(u16 flag, bool on);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	BitFlagArray();
	~BitFlagArray();
	bool create(u32 numFlag);
	bool recreate(u32 numFlagNew);
	void destroy();

	//======================================================================
	// メンバ変数
protected:
	u32 _numFlag;
	u8* _arrFlag;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_LIB_BIT_FLAG_ARRAY_H_

