/***********************************************************************//**
 *	TxtPosArray.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2011/12/02.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TXT_TXT_POS_ARRAY_H_
#define _TFW_STRING_TXT_TXT_POS_ARRAY_H_

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{

class TxtPos;

/*---------------------------------------------------------------------*//**
	テキスト位置配列
**//*---------------------------------------------------------------------*/
class TxtPosArray
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字列取得
	inline TxtPos* getPos(int index) const { return _tposarr[index]; }
	// 文字列数を得る
	inline int getCount() const { return _num; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TxtPosArray();
	TxtPosArray(TxtPos** strarrEntr, int num);
	TxtPosArray(const TxtPosArray& src);
	explicit TxtPosArray(TxtPos* tpos1);
	TxtPosArray(TxtPos* tpos1, TxtPos* tpos2);
	TxtPosArray(TxtPos* tpos1, TxtPos* tpos2, TxtPos* tpos3);
	TxtPosArray(TxtPos* tpos1, TxtPos* tpos2, TxtPos* tpos3, TxtPos* tpos4);
	TxtPosArray(TxtPos* tpos1, TxtPos* tpos2, TxtPos* tpos3, TxtPos* tpos4, TxtPos* tpos5);
	~TxtPosArray();
	TxtPosArray& operator=(const TxtPosArray& src);
	void copy(const TxtPosArray& src);
	void clear();
	void set(TxtPos* tpos1, TxtPos* tpos2, TxtPos* tpos3, TxtPos* tpos4, TxtPos* tpos5, TxtPos* tpos6, TxtPos* tpos7, TxtPos* tpos8, TxtPos* tpos9, TxtPos* tpos10);
	void set(TxtPos** strarr, int num);

	//======================================================================
	// メンバ変数
private:
	TxtPos**	_tposarr;
	int			_num;
};

}	// namespace tfw

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_TXT_TXT_POS_ARRAY_H_
