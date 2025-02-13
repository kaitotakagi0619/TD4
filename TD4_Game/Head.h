#pragma once
#include <RVector.h>
#include<ParticleManager.h>

#include "ProtoPlayer.h"
#include<NY_Object3DMgr.h>
#include<FbxLoader.h>

using namespace Rv3Ease;

class Head
{
public:

	Head() {};
	virtual ~Head() {};

	virtual void Init() {};

	virtual void ResetFrontEase() {};

	virtual void Update() {};

	virtual void Draw() {};

	virtual void DrawParticle() {};

	virtual void Finalize() {};

	virtual void KramerMove() {};

	virtual void SlappingMove() {};

	virtual void FailMove() {};

	virtual void GoHome() {};

	static void setPlayerPtr(std::shared_ptr<ProtoPlayer> ptr);

	static void loadHeadModel();

	static void setStaticData();

	void SetPlayer(ProtoPlayer* ptr) {
		this->playerPtr = ptr;
	}

	void Activate() {
		isactive = true;
	}

public:
	//モデルデータ
	static std::shared_ptr<fbxModel> headModelStatic;
	static std::shared_ptr<fbxModel> hairModelStatic;
	static std::shared_ptr<fbxModel> afroModelStatic;

	//ビンタされた時のパーティクル
	std::unique_ptr<ParticleManager> SlapParticle;

	static UINT slapTex;
	static UINT pullTex;

	//アフィン変換
	RVector3 pos, rot, scale;

	//吹っ飛びながら回転
	RVector3 blustVec;
	RVector3 blustRot;

	//吹っ飛ぶ時間
	int blustTime = 0;
	int maxBustTime = 15;

	//客のタイプ
	CheraType HeadType = CheraType::None;

	//先頭かどうか
	bool isMostFront = false;

	//有効化フラグ
	bool isactive = false;

	//待機時間
	int waitTime = 0;
	const int MaxWaitTime = 1200;

	//帰るかどうか
	bool isGoHome = false;

	//髪を切られた・抜かれたかどうか
	bool isHairDestroy = false;

	//間違えたかどうか
	bool isFail = false;
	int FailCount = 0;
	RVector3 ShakeBacePos = {};
	float ShakeOffset = 1.0f;

	//怒っているかどうか
	bool isAngree = false;

	//怒っている時間
	int AngreeTime = 0;
	const int MaxAngreeTime = 300;
	int positionUpDown = -1;

	//反撃するかどうか
	bool isCounter = false;

	//処理が終わったかどうか
	bool isAllMoveFinish = false;

	//プレイヤーポインタ
	ProtoPlayer* playerPtr;

	//クレーマーかどうか
	bool isKramer = false;

	//ビンタフラグ
	bool isSlap = false;

	//ビンタされた回数
	int SlapCount = 0;

	//ビンタの最大回数
	const int ManSlapCount = 5;

	//イージング用変数
	//前に進む時
	bool isFrontEase = false;		//前に進むか
	RVector3 FrontStart = {};
	RVector3 FrontEnd = {};
	int FrontEaseT = 0;		//時間
	const float FrontLength = 4.5f;	//変化量
	//ビンタされたとき
	float SlapEaseT = 0.0f;			//時間
	const float SlapLength = 1.0f;	//変化量
};

