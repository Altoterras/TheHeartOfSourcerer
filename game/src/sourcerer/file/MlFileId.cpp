/***********************************************************************//**
 *	MlFileId.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MlFileId.h"

// Friends

// External
#include "../../tfw/string/StringUtils.h"

// Library

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MlFileId メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ファイル ID を得る
**//*---------------------------------------------------------------------*/
u32 MlFileId::getFileId(u32 lang) const
{
	// 合致するものを探す
	for(int i = 0; i < _numDat; i++)
	{
		if(_datarr[i]._lang == lang)
		{
			return _datarr[i]._fileid;
		}
	}

	// 全ての言語対応で探す
	for(int i = 0; i < _numDat; i++)
	{
		if(_datarr[i]._lang == NUM_LANGID)
		{
			return _datarr[i]._fileid;
		}
	}

	return 0;	// 見つからなかった
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MlFileId::MlFileId()
	: _datarr(0L)
	, _numDat(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MlFileId::~MlFileId()
{
	ASSERT(_datarr == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MlFileId::create(const VcString* strFormat)
{
	VcStringArray strarrSemi = StringUtils::split(strFormat, ",");
	if(strarrSemi.getCount() > 0)
	{
		_numDat = strarrSemi.getCount();
		_datarr = new Data[_numDat];
		if(_datarr == 0L)	{	return false;	}

		for(int i = 0; i < strarrSemi.getCount(); i++)
		{
			VcStringArray strarrColon = StringUtils::split(strarrSemi.get(i), ":");
			if(strarrColon.getCount() >= 2)
			{
				const VcString* strLang = strarrColon.get(0);
				if(strLang->equals("en"))		{	_datarr[i]._lang = LANGID_EN;	}
				else if(strLang->equals("jp"))	{	_datarr[i]._lang = LANGID_JP;	}

				_datarr[i]._fileid = StringUtils::toInteger(strarrColon.get(1));
			}
			else
			{
				_datarr[i]._lang = NUM_LANGID;	// 全ての言語対応
				_datarr[i]._fileid = StringUtils::toInteger(strarrSemi.get(i));
			}
		}
	}
	else
	{
		ASSERT(false);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MlFileId::create(u32 fileid)
{
	// 全ての言語対応の一つのファイル ID を作成
	_numDat = 1;
	_datarr = new Data[_numDat];
	if(_datarr == 0L)	{	return false;	}
	_datarr[0]._lang = NUM_LANGID;
	_datarr[0]._fileid = fileid;
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MlFileId::destroy()
{
	delete[] _datarr;
	_datarr = 0L;
	_numDat = 0;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void MlFileId::copy(const MlFileId* src)
{
	delete[] this->_datarr;

	this->_numDat = src->_numDat;

	this->_datarr = new Data[this->_numDat];
	for(int i = 0; i < this->_numDat; i++)
	{
		this->_datarr[i]._lang = src->_datarr[i]._lang;
		this->_datarr[i]._fileid = src->_datarr[i]._fileid;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
