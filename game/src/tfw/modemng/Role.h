/***********************************************************************//**
 *	Role.h
 *  Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_MODEMNG_ROLE_H_
#define _TFW_MODEMNG_ROLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class Mode;

class Role : public FrameExecDraw
{
	//=========================================================================
	// クラス
protected:
	class CmdParam
	{
	public:
		s32					_cid;
		void*				_param;
		s32					_step;
		s32					_ecntStep;		// ステップの Exec（フレーム制御）カウンタ
		s32					_ecntCmd;		// コマンドの Exec（フレーム制御）カウンタ
		s32					_ecntBegun;		// ロール開始からの Exec（フレーム制御）カウンタ
		f32					_frmcntStep;	// ステップのフレームカウンタ
		f32					_frmcntCmd;		// コマンドのフレームカウンタ
		f32					_frmcntBegun;	// ロール開始からのフレームカウンタ
		CmdParam(s32 cid, void* param, s32 step, s32 ecntStep, s32 ecntCmd, s32 ecntBegun, f32 frmcntStep, f32 frmcntCmd, f32 frmcntBegun) : _cid(cid), _param(param), _step(step), _ecntBegun(ecntBegun), _ecntStep(ecntStep), _ecntCmd(ecntCmd), _frmcntStep(frmcntStep), _frmcntCmd(frmcntCmd), _frmcntBegun(frmcntBegun) {}
	};
	
private:
	class Command
	{
	public:
		s32					_cid;		// コマンド ID
		void*				_param;		// パラメータ
		Command(s32 cid, void* param) : _cid(cid), _param(param) {}
	};

	//=========================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在ロールコマンドを実行しているかを得る
	inline bool isRunCommand() const	{	return _cmdCur != 0L;	}
	// 現在のコマンド ID を得る
	inline s32 getCurCommandId() const	{	return (_cmdCur != 0L) ? _cmdCur->_cid : 0;	}
	// オーナーモードを得る（設定されていない場合は NULL が返る）
	inline Mode* getOwnerMode() const	{	return _modeOwnerRef;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void stackCommand(s32 cid, void* param);
	void setForceEndStack(bool isForceEndStack);
	void setForceNextCommand(bool isForceNextCommand);
	bool setForceJumpCommand(s32 cid);
	void begin();
	void end();
	void reset();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Role();
	virtual ~Role();
	virtual bool create(Mode* modeOwnerRef);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);
	virtual void render(const RenderCtx* rc);
	virtual void draw(const RenderCtx* rc);

protected:
	virtual void notifyBegin() {}
	virtual void notifyEnd() {}
	virtual s32 execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param) = 0;
	virtual void renderCommand(const RenderCtx* rc, const CmdParam* param) {}
	virtual void drawCommand(const RenderCtx* rc, const CmdParam* param) {}

private:
	bool nextCommand(bool isFirst);
	bool goCommand(u32 idxCmd);
	
	//=========================================================================
	// 変数
private:
	List<Command*>*	_listCmdStck;

	u32				_idxCmd;
	u32				_idxForceJumpCmdReserved;
	Command*		_cmdCur;
	bool			_isForceEndStack;
	bool			_isForceNextCommand;
	
	s32				_stepCmd;
	s32				_ecntStep;
	s32				_ecntCmd;
	s32				_ecntBegun;
	f32				_frmcntStep;
	f32				_frmcntCmd;
	f32				_frmcntBegun;

	Mode*			_modeOwnerRef;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_MODEMNG_ROLE_H_
