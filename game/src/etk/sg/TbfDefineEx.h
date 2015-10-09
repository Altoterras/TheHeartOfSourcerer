/***********************************************************************//**
 *	TbfDefineEx.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/08/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_SG_TBF_DEFINE_EX_H_
#define _ETK_SG_TBF_DEFINE_EX_H_

////////////////////////////////////////////////////////////////////////////
// クラス

namespace etk
{

class TbfDefineEx
{
	//======================================================================
	// 定数
public:
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// モデル拡張フラグ

	static const u16	MEXF_ATRBFLAG_NCC		= 0x0001;	// ニアカメラ変更モデル

	static const u16	MEXF_BG					= 0x0100;	// 背景
	static const u16	MEXF_CHAR				= 0x0200;	// キャラクタ
	static const u16	MEXF_PTCL				= 0x0400;	// パーティクル
	static const u16	MEXF_RTMV				= 0x0800;	// リアルタイムムービー
};

}	// namespace etk

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_SG_TBF_DEFINE_EX_H_
