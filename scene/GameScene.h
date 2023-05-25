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
	WorldTransform worldTransformStage_[20];

	void StageUpdate();

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
	Model* modelBeam_ = {};
	WorldTransform worldTransformBeam_[10];

	void BeamUpdate();

	void BeamMove();

	void BeamBorn();

	bool beamFlag_[10] = {};
	int beamTimer_ = 0;
	///
	/// エネミー
	///
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = 0;
	WorldTransform worldTransformEnemy_[10];

	void EnemyUpdate();

	void EnemyMove();

	void EnemyBorn();

	int enemyFlag_[10] = {};

	float enemySpeed_[10] = {};

	float enemyJumpSpeed_[10] = {};

	void EnemyJump();



	///
	/// 衝突判定
	///
	
	void Collision();
	void CollisionPlayerEnemy();
	void CollisionBeamEnemy();

	/// <summary>
	/// スコア＆ライフ
	/// </summary>

	DebugText* debugText_ = nullptr;

	uint32_t textureHandleNumber_ = 0;
	Sprite* spriteNumber_[5] = {};

	uint32_t textureHandleScore_ = 0;
	Sprite* spriteScore_ = nullptr;

	uint32_t textureHandlePlayerLife_ = 0;
	Sprite* spritePlayerLife_[3] = {};

	int playerTimer_ = 0;

	void DrawScore();

	int gameScore_ = 0;
	int playerLife_ = 3;


	/// <summary>
	/// タイトル＆エンド描画
	/// </summary>

	int sceneMode_ = 1;

	void TitleUpdate();
	void TitleDraw2DNear();

	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;

	uint32_t textureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;

	void EndUpdate();
	void EndDraw2DNear();

	uint32_t textureHandleEnd_ = 0;
	Sprite* spriteEnd_ = nullptr;

	void GamePlayerStart();

	int gameTimer_ = 0;

	uint32_t soundataHandleTitleBGM_ = 0;
	uint32_t soundataHandleGamePlayBGM_ = 0;
	uint32_t soundataHandleGameOverBGM_ = 0;
	uint32_t soundataHandleEnemyHitSE_ = 0;
	uint32_t soundataHandlePlayerHitSE_ = 0;
	uint32_t voiceHandleBGM_ = 0;


	void GamePlayerUpdate();
	void GamePlayerDraw3d();
	void GamePlayerDraw2DBack();
	void GamePlayerDraw2DNear();

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
