/***********************************************************************//**
 *  CamAnim.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/25.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_CAM_ANIM_H_
#define _TFW_G3D_CAM_ANIM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class CamAnimFrame;

/*---------------------------------------------------------------------*//**
 *	カメラアニメ クラス
 *
**//*---------------------------------------------------------------------*/
class CamAnim
{
	//======================================================================
	// 定数
public:
	static const u16		CAFIDX_INVALID		= U16_MAX;		// 無効な CamAnimFrame インデックス

	// advNextFrame の返り値
	enum
	{
		ADVRES_NULL			= 0,
		ADVRES_LOOP			= 1,
		ADVRES_END			= 2,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在のフレーム番号を取得する
	inline f32			getFrame() const				{	return _frame;				}
	// キーフレーム数を取得する
	inline u16			getKeyFrameNum() const			{	return _numKeyFrame;		}
	// フレーム情報を取得する
	const CamAnimFrame*	getAnimFrame(u16 icaf) const;
	// ループするかを設定する
	void				setEnableLoop(bool isEnable)	{	_isEnableLoop = isEnable;	}
	// fov 値を得る
	inline f32			getFovY() const					{	return _fovy;				}
	// ニアクリップ値を得る
	inline f32			getNearClip() const				{	return _cnear;				}
	// ファークリップ値を得る
	inline f32			getFarClip() const				{	return _cfar;				}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void			resetHeadFrame(u16* idxCaf);
	u32				advNextFrame(u16* idxCafCur, u16* idxCafNext, f32 frameAdv);
	s32				getMaxFrame() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CamAnim();
	~CamAnim();
	bool			create(CamAnimFrame* arrcafEntr, u16 numKeyFrame, f32 fov, f32 cnear, f32 cfar);
	void			destroy();

	//======================================================================
	// メンバ変数
private:
	CamAnimFrame*			_arrcaf;
	u16						_numKeyFrame;
	f32						_fovy;
	f32						_cnear;
	f32						_cfar;

	bool					_isEnableLoop;
	f32						_frame;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_CAM_ANIM_H_