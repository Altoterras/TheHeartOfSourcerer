/***********************************************************************//**
 *	Mode.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_MODEMNG_MODE_H_
#define _TFW_MODEMNG_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class VcString;
class Role;

/*---------------------------------------------------------------------*//**
 *	モード管理
 *		ツリー構造を持ったモード管理クラス
 *
**//*---------------------------------------------------------------------*/
class Mode : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	static const short	MODE_NULL	= 0;

	// モードの関係性を示す値
	enum Relation
	{
		REL_NULL,		// 無関係である
		REL_PARENT,		// 親である
		REL_CHILD,		// 子である
		REL_SIBLING,	// 兄弟である
		REL_DESCENDANT,	// 子孫である
		REL_ANCESTOR,	// 先祖である
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 作成済みかどうかを得る
	inline bool isCreated() const							{	return _isCreated;			}
	// 開始しているかどうかを得る
	inline bool isBegan() const								{	return _cntFrame != 0;		}
	// ロールを取得
	inline Role* getSelfRole() const						{	return _roleSelf;			}
	// 現在の（子）モード ID を取得
	inline short getCurrentModeId() const					{	return _modeStack[0];		}
	// 現在の（子）モードを取得
	inline Mode* getCurrentMode() const						{	return _modeCurChild;		}
	// 親のモードを取得
	inline Mode* getParentMode() const						{	return _modeParent;			}
	// 自身の モード ID を取得
	inline short getSelfModeId() const						{	return _modeidSelf;			}
	// 最終予約されたモードを取得
	inline short getLastReservedMode() const				{	return _modeReserved[0];	}
	// 予約されたモード数を取得
	int getReservedModeNum() const;
	// 予約されたモードを取得
	short getReservedMode(int index) const					{	return _modeReserved[index]; }
	// スタックされたモード数を取得
	int getStackedModeNum() const;
	// スタックされたモードを取得
	short getStackedMode(int index) const					{ return _modeStack[index]; }
	// 一つ前のモードを取得
	short getPrevMode() const;
	// 汎用フレームカウンタ取得
	inline unsigned int getFrameCounter() const				{	return _cntFrame;			}
	// 汎用パラメータ取得
	inline void* getParameter() const						{	return _objParamRef;		}
	// exec で setFromReservedMode を自動呼出しするかのフラグを取得
	inline bool isResolveReseredModeOnExec() const			{	return _isExecRslvRsvd;		}
	// 汎用パラメータセット
	void setParameter(void* objParamRef);
	// exec で setFromReservedMode を自動呼出しするかの設定
	inline void setResolveReseredModeOnExec(bool isEnable)	{	_isExecRslvRsvd = isEnable;	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void changeMode(short modeid, bool isReset);
	virtual void changePrevMode();
	virtual void reserveMode(short modeid);
	virtual void reserveModeCancel(short modeidCancel);
	virtual void changeFromReservedMode();
	virtual short getNextMode();
	virtual bool begin(void* objParamRef);
	virtual void end(void* objParamRef);

	Relation checkRelation(const Mode* modeTrg) const;
	Relation checkRelation(short modeidTrg) const;
	Mode* findChildModeDive(short modeid) const;
	bool isReservedMode(bool isDiveChild) const;
	void clearReservedMode(bool isDiveChild) const;

	virtual void callCustomEvent(void* objRes, const VcString* strParam, const void* objParam) {}

protected:
	virtual bool notifyBegin(void* objParamRef)				{	return true;				}
	virtual void notifyEnd(void* objParamRef)				{								}
	s32	getChildModeNum() const;
	Mode* getChildMode(s32 index);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Mode();
	Mode(int numStack, Role* roleSelf);
	virtual ~Mode();
	virtual bool create();
	bool create(int numStack, Role* roleSelf, bool isRoleDlg);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);
	virtual void render(const RenderCtx* rc);
	virtual void draw(const RenderCtx* rc);

	virtual bool registerMode(short modeid, Mode* mode);

private:
	void changeModeActually(short modeid);

public:
	virtual void notifyChildModeBegin(short modeid, void* objParamRef) {}
	virtual void notifyChildModeEnd(short modeid, void* objParamRef) {}
	virtual void notifyChildModeChanged(short modeid, void* objParamRef) {}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// ユーティリティ
private:
	static void shiftArrayAsc(short* arr, int len, short valueNull);
	static void shiftArrayAsc(int* arr, int len, int valueNull);
	static void shiftArrayDesc(short* arr, int len, short valueNull);
	static void shiftArrayDesc(int* arr, int len, int valueNull);

	//======================================================================
	// クラス
private:

	//======================================================================
	// 変数
private:
	short*			_modeStack;				// モードスタック
	short*			_modeReserved;			// 予約されたモード
	unsigned int	_cntFrame;				// フレームカウンタ
	int				_lenStack;				// スタック長さ
	bool			_isCreated;				// 作成済みかどうか
	bool			_isExecRslvRsvd;		// exec で予約を解決するかどうか
	short			_modeidSelf;			// 自身のモード ID

	void*			_objParamRef;			// 汎用パラメータポインタ

	List<Mode*>*	_listChildMode;			// 子モードリスト（子がない場合は使わなくても良い）
	Mode*			_modeCurChild;			// 現在の子モード
	Mode*			_modeParent;			// 親のモード

	Role*			_roleSelf;				// 自身のロールへのポインタ
	bool			_isRoleEntr;			// ロールの管理を委譲されているかどうか（委譲されている場合は、作成・削除を担う）
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_MODEMNG_MODE_H_
