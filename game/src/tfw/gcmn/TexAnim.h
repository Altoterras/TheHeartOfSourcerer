/***********************************************************************//**
 *	TexAnim.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/05/27.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_GCMN_TEX_ANIM_H_
#define _TFW_GCMN_TEX_ANIM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector2;
typedef Vector2<f32> Vector2F;
class TexAnimCtx;

/*---------------------------------------------------------------------*//**
 *	テクスチャ アニメーション
 *
**//*---------------------------------------------------------------------*/
class TexAnim
{
	//======================================================================
	// クラス
private:
	class KeyFrame
	{
	public:
		u32			_frame;
		f32			_trs[2];

		KeyFrame() : _frame(0)
		{
			_trs[0] = 0.0f;
			_trs[1] = 0.0f;
		}
		void copy(KeyFrame* src)
		{
			_frame = src->_frame;
			_trs[0] = src->_trs[0];
			_trs[1] = src->_trs[1];
		}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// フレーム数を得る
	inline u32		getFrameNum() const { return _numFrame; }
	// 現在の平行移動値を得る
	void			getCurrentTranslate(TexAnimCtx* ctx, Vector2F* trs) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void			advNextFrame(TexAnimCtx* ctx, f32 frameAdv);
	void			resetTexAnimFrame(TexAnimCtx* ctx);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TexAnim();
	virtual ~TexAnim();
	bool			create(u32 numFrame, u32 numKeyFrame);
	bool			createKeyFrame(s32 idxKeyFrame, u32 frame, f32 s, f32 t);
	void			destroy();
	bool			createContext(TexAnimCtx* ctx);
	void			destroyContext(TexAnimCtx* ctx);

private:

	//======================================================================
	// メンバ変数
private:
	u32				_numFrame;
	u32				_numKeyFrame;
	KeyFrame*		_kfarrKeyFrame;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_GCMN_TEX_ANIM_H_