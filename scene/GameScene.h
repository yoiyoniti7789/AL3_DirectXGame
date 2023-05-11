#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugText.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// BG（スプライト）
	/// </summary>
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;

	///
	/// ビュー＆ステージ
	/// 
	ViewProjection viewprojection_;

	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_;

	///
	///プレイヤー
	///
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;

	void PlayerUpdate();

	///
	/// ビーム
	///
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = nullptr;
	WorldTransform worldTransformBeam_;

	void BeamUpdate();

	void BeamMove();

	void BeamBorn();

	bool beamFlag_ = false;

	///
	/// エネミー
	///
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_;

	void EnemyUpdate();

	void EnemyMove();

	void EnemyBorn();

	bool enemyFlag_ = false;

	///
	/// 衝突判定
	///
	
	void Collision();
	void CollisionPlayerEnemy();
	void CollisionBeamEnemy();

	DebugText* debugText_ = nullptr;

	int gameScore_ = 0;
	int playerLife_ = 3;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
