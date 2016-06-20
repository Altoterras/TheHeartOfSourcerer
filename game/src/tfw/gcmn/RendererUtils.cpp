/***********************************************************************//**
 *	RendererUtils.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "RendererUtils.h"

// Friends
#include "Renderer.h"
#include "View.h"
#include "../lib/Color.h"
#include "../lib/Rect.h"
#include "../lib/Vector.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 静的関数

// テクスチャの有効／無効を gl ステイト関数により行うかどうか
// 最適化のオプションとして残す
#define INVALID_TEXTURE_BY_STATE	1

/*---------------------------------------------------------------------*//**
	テクスチャを有効化
**//*---------------------------------------------------------------------*/
inline void validTexture(Renderer* rdr)
{
	/* TEST-TextureOptimize-
	#if INVALID_TEXTURE_BY_STATE
		rdr->setEnableTexture(true);	// テクスチャを有効に
	#endif
	*/
}

/*---------------------------------------------------------------------*//**
	テクスチャを無効化
**//*---------------------------------------------------------------------*/
inline void invalidTexture(Renderer* rdr)
{
	#if INVALID_TEXTURE_BY_STATE
		rdr->setEnableTexture(false);	// テクスチャを無効に
	#else
		rdr->bindTexture(0L);	// テクスチャなし
	#endif
}

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// RendererUtils メソッド

/*---------------------------------------------------------------------*//**
	2D 座標の線を描画する
**//*---------------------------------------------------------------------*/
void RendererUtils::draw2dLine(Renderer* rdr, f32 vx1, f32 vy1, f32 vx2, f32 vy2)
{
#if 0
	static GLfloat vtxs[4];
	vtxs[0] = vx1;		vtxs[1] = vy1;
	vtxs[2] = vx2;		vtxs[3] = vy2;
#else
	GLfloat vtxs[] =
	{
		vx1,		vy1,
		vx2,		vy2,
	};
#endif

	invalidTexture(rdr);	// テクスチャを無効化
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(2, GL_FLOAT, 0, vtxs);
	::glDrawArrays(GL_LINES, 0, 2);
	::glDisableClientState(GL_VERTEX_ARRAY);
	validTexture(rdr);		// テクスチャを有効化
}

/*---------------------------------------------------------------------*//**
	2D の円塗りつぶし描画
		※ 角度は -π～＋π の範囲であるとする
**//*---------------------------------------------------------------------*/
void RendererUtils::draw2dFillArc(Renderer* rdr, f32 x, f32 y, f32 r, f32 thetaStart, f32 thetaEnd, f32 thetaInc)
{
	if(thetaEnd < thetaStart)
	{
		thetaEnd += TFW_DPI;
	}
	s32 n = (s32)((thetaEnd - thetaStart) / thetaInc);
	ASSERT(n >= 0);
	n += 3;
	s32 s = n * 2;
	GLfloat* vtxs = new GLfloat[s];

	vtxs[0] = x;
	vtxs[1] = y;

	f32 x1, y1, x2, y2, t1, t2;
	t1 = thetaStart;
	x1 = x + (r * ::cos(t1));
	y1 = y + (r * ::sin(t1));
	vtxs[2] = x1;
	vtxs[3] = y1;
	s32 i = 4;
	for(t2 = thetaStart + thetaInc; t2 <= thetaEnd; t2 += thetaInc)
	{
		x2 = x + (r * ::cos(t2));
		y2 = y + (r * ::sin(t2));
		ASSERT((i + 1) < s);
		vtxs[i + 0] = x2;
		vtxs[i + 1] = y2;
		i += 2;
		t1 = t2;
		x1 = x2;
		y1 = y2;
	}
	if(TFW_IS_NEAR(t1, thetaEnd, 0.01f))
	{
		n --;
	}
	else
	{
		x2 = x + (r * ::cos(thetaEnd));
		y2 = y + (r * ::sin(thetaEnd));
		ASSERT((i + 1) < s);
		vtxs[i + 0] = x2;
		vtxs[i + 1] = y2;
	}

	invalidTexture(rdr);	// テクスチャを無効化
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(2, GL_FLOAT, 0, vtxs);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, n);
	::glDisableClientState(GL_VERTEX_ARRAY);
	validTexture(rdr);		// テクスチャを有効化

	delete[] vtxs;
}

/*---------------------------------------------------------------------*//**
	2D 座標の矩形を描画する
**//*---------------------------------------------------------------------*/
void RendererUtils::draw2dRect(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh)
{
#if 0
	static GLfloat vtxs[8];
	vtxs[0] = vx;		vtxs[1] = vy;
	vtxs[2] = vx + vw;	vtxs[3] = vy;
	vtxs[4] = vx;		vtxs[5] = vy + vh;
	vtxs[6] = vx + vw;	vtxs[7] = vy + vh;
#else
	GLfloat vtxs[] =
	{
		vx,			vy,
		vx + vw,	vy,
		vx,			vy + vh,
		vx + vw,	vy + vh,
	};
#endif

	invalidTexture(rdr);	// テクスチャなし
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(2, GL_FLOAT, 0, vtxs);
	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	::glDisableClientState(GL_VERTEX_ARRAY);
	validTexture(rdr);		// テクスチャを有効化
}

/*---------------------------------------------------------------------*//**
	2D 座標の矩形線を描画する
**//*---------------------------------------------------------------------*/
void RendererUtils::draw2dRectLine(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh)
{
	GLfloat vtxs[] =
	{
		vx,		vy,
		vx + vw,	vy,
		vx + vw,	vy + vh,
		vx,		vy + vh,
		vx,		vy,
	};

	invalidTexture(rdr);	// テクスチャを無効化
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(2, GL_FLOAT, 0, vtxs);
	::glDrawArrays(GL_LINE_STRIP, 0, 5);
	::glDisableClientState(GL_VERTEX_ARRAY);
	validTexture(rdr);		// テクスチャを有効化
}

/*---------------------------------------------------------------------*//**
	2D 座標のテクスチャ付きの矩形を描画する
	@param	rdr レンダラー
	@param	vx 矩形頂点 X 座標
	@param	vy 矩形頂点 Y 座標
	@param	vw 矩形幅
	@param	vw 矩形高さ
	@param	tu テクスチャ U 座標
	@param	tv テクスチャ V 座標
	@param	tw テクスチャ幅
	@param	th テクスチャ高さ
**//*---------------------------------------------------------------------*/
void RendererUtils::draw2dTextureRect(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh, f32 tu, f32 tv, f32 tw, f32 th)
{
	tv = 1.0f - tv;
#if 0
	static GLfloat vtxs[8];
	static GLfloat uvs[8];
	vtxs[0] = vx;		vtxs[1] = vy;
	vtxs[2] = vx + vw;	vtxs[3] = vy;
	vtxs[4] = vx;		vtxs[5] = vy + vh;
	vtxs[6] = vx + vw;	vtxs[7] = vy + vh;
#if 0
	uvs[0] = tu;		uvs[1] = tv + th;
	uvs[2] = tu + tw;	uvs[3] = tv + th;
	uvs[4] = tu;		uvs[5] = tv;
	uvs[6] = tu + tw;	uvs[7] = tv;
#endif
	uvs[0] = tu;		uvs[1] = tv;
	uvs[2] = tu + tw;	uvs[3] = tv;
	uvs[4] = tu;		uvs[5] = tv - th;
	uvs[6] = tu + tw;	uvs[7] = tv - th;
#else
	GLfloat vtxs[] =
	{
		vx,			vy,
		vx + vw,	vy,
		vx,			vy + vh,
		vx + vw,	vy + vh,
	};
	GLfloat uvs[] =
	{
		tu,			tv,
		tu + tw,	tv,
		tu,			tv - th,
		tu + tw,	tv - th,
	};
#endif

	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(2, GL_FLOAT, 0, vtxs);

	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, uvs);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisableClientState(GL_VERTEX_ARRAY);
}

/*---------------------------------------------------------------------*//**
	2D 座標のテクスチャ付きの回転指定矩形を描画する
	@param	rdr レンダラー
	@param	vx 矩形頂点 X 座標
	@param	vy 矩形頂点 Y 座標
	@param	vw 矩形幅
	@param	vw 矩形高さ
	@param	tu テクスチャ U 座標
	@param	tv テクスチャ V 座標
	@param	tw テクスチャ幅
	@param	th テクスチャ高さ
	@param	rot 回転指定
**//*---------------------------------------------------------------------*/
void RendererUtils::draw2dTextureRect(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh, f32 tu, f32 tv, f32 tw, f32 th, u32 rot)
{
	tv = 1.0f - tv;
	GLfloat vtxs[8];
	GLfloat uvs[8];
	#define SETARR8(v_, f1_, f2_, f3_, f4_, f5_, f6_, f7_, f8_) { (v_)[0] = (f1_); (v_)[1] = (f2_); (v_)[2] = (f3_); (v_)[3] = (f4_); (v_)[4] = (f5_); (v_)[5] = (f6_); (v_)[6] = (f7_); (v_)[7] = (f8_); }
	if(rot == TFW_DISPF_RANG_0)
	{
		SETARR8(vtxs,
			vx,			vy,
			vx + vw,	vy,
			vx,			vy + vh,
			vx + vw,	vy + vh	);
	}
	else if(rot == TFW_DISPF_RANG_90)
	{
		SETARR8(vtxs,
			vx + vw,	vy,
			vx + vw,	vy + vh,
			vx,			vy,
			vx,			vy + vh	);
	}
	else if(rot == TFW_DISPF_RANG_180)
	{
		SETARR8(vtxs,
			vx + vw,	vy + vh,
			vx,			vy + vh,
			vx + vw,	vy,
			vx,			vy		);
	}
	else
	{
		SETARR8(vtxs,
			vx,			vy + vh,
			vx,			vy,
			vx + vw,	vy + vh,
			vx + vw,	vy		);
	}
	SETARR8(uvs,
		tu,			tv,
		tu + tw,	tv,
		tu,			tv - th,
		tu + tw,	tv - th	);
	#undef SETARR8

	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(2, GL_FLOAT, 0, vtxs);

	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, uvs);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisableClientState(GL_VERTEX_ARRAY);
}

/*---------------------------------------------------------------------*//**
	2D 座標の頂点カラー付きの矩形を描画する
**//*---------------------------------------------------------------------*/
void RendererUtils::draw2dColorRect(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh, const ColorU8* colLeftTop, const ColorU8* colRightTop, const ColorU8* colLeftBottom, const ColorU8* colRightBottom)
{
	GLfloat vtxs[] =
	{
		vx,			vy,
		vx + vw,	vy,
		vx,			vy + vh,
		vx + vw,	vy + vh,
	};
	GLubyte cols[16];
	::memcpy(&cols[ 0], colLeftTop->ptr(), sizeof(u8) * 4);
	::memcpy(&cols[ 4], colRightTop->ptr(), sizeof(u8) * 4);
	::memcpy(&cols[ 8], colLeftBottom->ptr(), sizeof(u8) * 4);
	::memcpy(&cols[12], colRightBottom->ptr(), sizeof(u8) * 4);

	invalidTexture(rdr);	// テクスチャなし

	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(2, GL_FLOAT, 0, vtxs);

	::glEnableClientState(GL_COLOR_ARRAY);
	::glColorPointer(4, GL_UNSIGNED_BYTE, 0, cols);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	::glDisableClientState(GL_COLOR_ARRAY);
	::glDisableClientState(GL_VERTEX_ARRAY);

	validTexture(rdr);		// テクスチャを有効化

#if 0	// 環境依存か！？
	rdr->setSolidColorMemoryOnly(cols[12], cols[13], cols[14], cols[15]);	// 使用色を記憶する
#endif
}

/*---------------------------------------------------------------------*//**
	2D 座標のテクスチャ、カラー付きの矩形を描画する
**//*---------------------------------------------------------------------*/
void RendererUtils::draw2dColorTextureRect(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh, f32 tu, f32 tv, f32 tw, f32 th, const ColorU8* colLeftTop, const ColorU8* colRightTop, const ColorU8* colLeftBottom, const ColorU8* colRightBottom)
{
	tv = 1.0f - tv;
	GLfloat vtxs[] =
	{
		vx,			vy,
		vx + vw,	vy,
		vx,			vy + vh,
		vx + vw,	vy + vh,
	};
	GLfloat uvs[] =
	{
		tu,			tv,
		tu + tw,	tv,
		tu,			tv - th,
		tu + tw,	tv - th,
	};
	GLubyte cols[16];
	::memcpy(&cols[ 0], colLeftTop->ptr(), sizeof(u8) * 4);
	::memcpy(&cols[ 4], colRightTop->ptr(), sizeof(u8) * 4);
	::memcpy(&cols[ 8], colLeftBottom->ptr(), sizeof(u8) * 4);
	::memcpy(&cols[12], colRightBottom->ptr(), sizeof(u8) * 4);

	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(2, GL_FLOAT, 0, vtxs);

	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, uvs);

	::glEnableClientState(GL_COLOR_ARRAY);
	::glColorPointer(4, GL_UNSIGNED_BYTE, 0, cols);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	::glDisableClientState(GL_COLOR_ARRAY);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisableClientState(GL_VERTEX_ARRAY);

	rdr->setSolidColorMemoryOnly(cols[12], cols[13], cols[14], cols[15]);	// 使用色を記憶する
}

/*---------------------------------------------------------------------*//**
	3D 座標の線を描画する
**//*---------------------------------------------------------------------*/
void RendererUtils::render3dLines(Renderer* rdr, const Vector3F* vtxarr, s32 num, LineKind linekind)
{
	GLenum mode = GL_LINES;
	switch(linekind)
	{
	case LINES:				mode = GL_LINES;			break;
	case LINE_LOOP:			mode = GL_LINE_LOOP;		break;
	case LINE_STRIP:		mode = GL_LINE_STRIP;		break;
	}

	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(3, GL_FLOAT, 0, vtxarr);

	::glDrawArrays(mode, 0, num);

	::glDisableClientState(GL_VERTEX_ARRAY);
}

/*---------------------------------------------------------------------*//**
	3D 座標の三角形を描画する
**//*---------------------------------------------------------------------*/
void RendererUtils::render3dTriangles(Renderer* rdr, const Vector3F* vtxarr, const Vector3F* nrmarr, const Vector2F* texarr, const ColorU8* colarr, s32 num, TriangleKind trikind)
{
	GLenum mode = GL_TRIANGLE_STRIP;
	switch(trikind)
	{
	case TRIANGLES:			mode = GL_TRIANGLES;		break;
	case TRIANGLE_STRIP:	mode = GL_TRIANGLE_STRIP;	break;
	case TRIANGLE_FAN:		mode = GL_TRIANGLE_FAN;		break;
	}

	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, vtxarr);
	}
	// 法線
	if(nrmarr != 0)
	{
		::glEnableClientState(GL_NORMAL_ARRAY);
		::glNormalPointer(GL_FLOAT, 0, nrmarr);
	}
	// テクスチャ
	if(texarr != 0)
	{
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glTexCoordPointer(2, GL_FLOAT, 0, texarr);
	}
	// 頂点カラー
	if(colarr != 0)
	{
		::glEnableClientState(GL_COLOR_ARRAY);
		::glColorPointer(4, GL_UNSIGNED_BYTE, 0, colarr);
		rdr->setSolidColorMemoryOnly(colarr);	// 使用色を記憶する
	}

	::glDrawArrays(mode, 0, num);

	if(colarr != 0)
	{
		::glDisableClientState(GL_COLOR_ARRAY);
	}
	if(texarr != 0)
	{
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	if(nrmarr != 0)
	{
		::glDisableClientState(GL_NORMAL_ARRAY);
	}
	{
		::glDisableClientState(GL_VERTEX_ARRAY);
	}
}

/*---------------------------------------------------------------------*//**
	線による球の描画
	※２０面体（Icosahedron）による球の描画
**//*---------------------------------------------------------------------*/
void RendererUtils::render3dLineSphere(Renderer* rdr, f32 r, s32 subdiv)
{
	#define X 0.525731112119133606f
	#define Z 0.850650808352039932f
	f32 x = X * r;
	f32 z = Z * r;
	Vector3F vtxs[12] =
	{
	   Vector3F(  - x, 0.0f,    z ), Vector3F(    x, 0.0f,    z ), Vector3F(  - x, 0.0f,  - z ), Vector3F(    x, 0.0f,  - z ),
	   Vector3F( 0.0f,    z,    x ), Vector3F( 0.0f,    z,  - x ), Vector3F( 0.0f,  - z,    x ), Vector3F( 0.0f,  - z,  - x ),
	   Vector3F(    z,    x, 0.0f ), Vector3F(  - z,    x, 0.0f ), Vector3F(    z,  - x, 0.0f ), Vector3F(  - z,  - x, 0.0f )
	};
	const GLushort indices[20][3] =
	{ 
		{  0,  4,  1 }, {  0,  9,  4 }, {  9,  5,  4}, {  4,  5,  8 }, {  4,  8,  1 },
		{  8, 10,  1 }, {  8,  3, 10 }, {  5,  3,  8}, {  5,  2,  3 }, {  2,  7,  3 },
		{  7, 10,  3 }, {  7,  6, 10 }, {  7, 11,  6}, { 11,  0,  6 }, {  0,  1,  6 },
		{  6,  1, 10 }, {  9,  0, 11 }, {  9, 11,  2}, {  9,  2,  5 }, {  7,  2, 11 }
	};

	rdr->bindTexture(0L);
	if(subdiv == 0)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, vtxs);
		for(int i = 0; i < 20; i++)
		{
			::glDrawElements(GL_LINE_STRIP, 3, GL_UNSIGNED_SHORT, indices[i]);
		}
		::glDisableClientState(GL_VERTEX_ARRAY);
	}
	else
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		for(int i = 0; i < 20; i++)
		{
			render3dLineSphere_diveSubDiv(&vtxs[indices[i][0]], &vtxs[indices[i][1]], &vtxs[indices[i][2]], r, subdiv);
		}
		::glDisableClientState(GL_VERTEX_ARRAY);
	}
}

/*---------------------------------------------------------------------*//**
	２０面体球の詳細分割描画再帰メソッド
**//*---------------------------------------------------------------------*/
void RendererUtils::render3dLineSphere_diveSubDiv(const Vector3F* v1, const Vector3F* v2, const Vector3F* v3, f32 r, s32 subdiv)
{
	if(subdiv == 0)
	{
		GLfloat vtxarr[3][3] =
		{
		   { v1->_v[0], v1->_v[1], v1->_v[2] },
		   { v2->_v[0], v2->_v[1], v2->_v[2] },
		   { v3->_v[0], v3->_v[1], v3->_v[2] },
		};
		::glVertexPointer(3, GL_FLOAT, 0, vtxarr);
		::glDrawArrays(GL_LINE_STRIP, 0, 3);
	}
	else
	{
		Vector3F v12, v23, v31;
		for(int i = 0; i < 3; i++)
		{
			v12._v[i] = v1->_v[i] + v2->_v[i];
			v23._v[i] = v2->_v[i] + v3->_v[i];
			v31._v[i] = v3->_v[i] + v1->_v[i];
		}
		v12.normalize();
		v23.normalize();
		v31.normalize();
		v12 *= r;
		v23 *= r;
		v31 *= r;
		render3dLineSphere_diveSubDiv(  v1, &v12, &v31, r, subdiv - 1);
		render3dLineSphere_diveSubDiv(  v2, &v23, &v12, r, subdiv - 1);
		render3dLineSphere_diveSubDiv(  v3, &v31, &v23, r, subdiv - 1);
		render3dLineSphere_diveSubDiv(&v12, &v23, &v31, r, subdiv - 1);
	}
}

/*---------------------------------------------------------------------*//**
	色の変更
**//*---------------------------------------------------------------------*/
void RendererUtils::boostColor(ColorU8* colOut, const ColorU8* colIn, f32 rate)
{
	for(int i = 0; i < 3; i++)
	{
		s32 n = (s32)(colIn->_v[i] * rate);
		if(n < 0)			{	n = 0;		}
		else if(n > 255)	{	n = 255;	}
		colOut->_v[i] = (u8)n;
	}
	colOut->_v[3] = colIn->_v[3];	// アルファ値はそのまま
}

/*---------------------------------------------------------------------*//**
	長辺をスクリーンにフィットさせてテクスチャを描画する
	（アスペクト比は維持する）
**//*---------------------------------------------------------------------*/
void RendererUtils::draw2dTextureToScreenFitLongSide(Renderer* rdr, const View* view, const RectF32* uvTex, const ColorU8* colSide)
{
	RectF32 rect;
	f32 aspectTex = (uvTex->width()) / uvTex->height();
	if(aspectTex == view->getLogicalAspect())
	{
		rect.set(0.0f, 0.0f, view->getLogicalWidth(), view->getLogicalHeight());
	}
	else if(aspectTex < view->getLogicalAspect())
	{
		rect._v[3] = view->getLogicalHeight();
		rect._v[2] = rect._v[3] * aspectTex;
		rect._v[0] = (view->getLogicalWidth() - rect._v[2]) * 0.5f;
		rect._v[1] = 0.0f;
	}
	else
	{
		rect._v[2] = view->getLogicalWidth();
		rect._v[3] = rect._v[2] / aspectTex;
		rect._v[0] = 0.0f;
		rect._v[1] = (view->getLogicalHeight() - rect._v[3]) * 0.5f;
	}
	draw2dTextureRect(rdr, &rect, uvTex);

	// 余った領域を塗りつぶす
	if((colSide != 0) && ((rect._v[0] != 0.0f) || (rect._v[1] != 0.0f)))
	{
		rdr->setSolidColor(colSide);
		if(rect._v[0] != 0.0f)
		{
			RendererUtils::draw2dRect(rdr, 0.0f, 0.0f, rect._v[0], view->getLogicalHeight());
			RendererUtils::draw2dRect(rdr, rect._v[0] + rect._v[2], 0.0f, rect._v[0], view->getLogicalHeight());
		}
		else if(rect._v[1] != 0.0f)
		{
			RendererUtils::draw2dRect(rdr, 0.0f, 0.0f, view->getLogicalWidth(), rect._v[1]);
			RendererUtils::draw2dRect(rdr, 0.0f, rect._v[1] + rect._v[3], view->getLogicalWidth(), rect._v[1]);
		}
	}
}

/*---------------------------------------------------------------------*//**
	短辺をスクリーンにフィットさせてテクスチャを描画する
	（アスペクト比は維持する）
**//*---------------------------------------------------------------------*/
void RendererUtils::draw2dTextureToScreenFitShortSide(Renderer* rdr, const View* view, const RectF32* uvTex)
{
	RectF32 rect;
	f32 aspectTex = (uvTex->width()) / uvTex->height();
	if(aspectTex == view->getLogicalAspect())
	{
		rect.set(0.0f, 0.0f, view->getLogicalWidth(), view->getLogicalHeight());
	}
	else if(aspectTex > view->getLogicalAspect())
	{
		rect._v[3] = view->getLogicalHeight();
		rect._v[2] = rect._v[3] * aspectTex;
		rect._v[0] = (view->getLogicalWidth() - rect._v[2]) * 0.5f;
		rect._v[1] = 0.0f;
	}
	else
	{
		rect._v[2] = view->getLogicalWidth();
		rect._v[3] = rect._v[2] / aspectTex;
		rect._v[0] = 0.0f;
		rect._v[1] = (view->getLogicalHeight() - rect._v[3]) * 0.5f;
	}
	draw2dTextureRect(rdr, &rect, uvTex);
}


////////////////////////////////////////////////////////////////////////////

TFW_END_NS
