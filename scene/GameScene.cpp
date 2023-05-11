#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "MathUtilityForText.h"
#include <time.h>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete spriteBG_; 
	delete modelStage_;
	delete modelPlayer_;
	delete modelBeam_;
	delete modelEnemy_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0,0});

	viewprojection_.Initialize();

	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();

	viewprojection_.translation_.y = 1;
	viewprojection_.translation_.z = -6;
	viewprojection_.Initialize();

	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f,1,40};

	worldTransformStage_.matWorld_ = MakeAffineMatrix(
		worldTransformStage_.scale_, 
		worldTransformStage_.rotation_,
		worldTransformStage_.translation_);

	worldTransformStage_.TransferMatrix();

	//プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f,0.5f,0.5f};
	worldTransformPlayer_.Initialize();
	// ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	worldTransformBeam_.scale_ = {0.3f, 0.3f, 0.3f};
	worldTransformBeam_.Initialize();
	// エネミー
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	worldTransformEnemy_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformEnemy_.Initialize();

	srand((unsigned int)time(NULL));

	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();
}
void GameScene::PlayerUpdate() {
	/// プレイヤー更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
		worldTransformPlayer_.scale_,
		worldTransformPlayer_.rotation_,
		worldTransformPlayer_.translation_);
	worldTransformPlayer_.TransferMatrix();

	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}
	worldTransformPlayer_.translation_.x = max(worldTransformPlayer_.translation_.x,-4);
	worldTransformPlayer_.translation_.x = min(worldTransformPlayer_.translation_.x, 4);
}
void GameScene::BeamUpdate() 
{
	BeamMove();
	BeamBorn();
	/// ビーム更新
	worldTransformBeam_.matWorld_ = MakeAffineMatrix(
	    worldTransformBeam_.scale_, 
		worldTransformBeam_.rotation_,
	    worldTransformBeam_.translation_);
	worldTransformBeam_.TransferMatrix();
}

void GameScene::BeamMove() 
{
	if (beamFlag_ == true) 
	{
		worldTransformBeam_.rotation_.x += 0.1f;
		worldTransformBeam_.translation_.z += 0.3f;

		if (worldTransformBeam_.translation_.z >= 40.0f) {
			beamFlag_ = false;
		}
	}
}

void GameScene::BeamBorn() 
{
	if (input_->TriggerKey(DIK_SPACE)&& beamFlag_ == false) 
	{
		worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
		worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;
		 
		beamFlag_ = true;
	}
}

void GameScene::EnemyUpdate() 
{
	EnemyMove();
	EnemyBorn();
	/// エネミー更新
	worldTransformEnemy_.matWorld_ = MakeAffineMatrix(
	    worldTransformEnemy_.scale_, 
		worldTransformEnemy_.rotation_,
	    worldTransformEnemy_.translation_);
	worldTransformEnemy_.TransferMatrix();
}

void GameScene::EnemyMove() 
{ 
	
	if (enemyFlag_ == true) {
		worldTransformEnemy_.rotation_.x -= 0.1f;
		worldTransformEnemy_.translation_.z -= 0.5f;
		if (worldTransformEnemy_.translation_.z <= -5.0f) {
			enemyFlag_ = false;
		}
	}
}

void GameScene::EnemyBorn() 
{
	
	if (enemyFlag_ == false) {
		enemyFlag_ = true;

		int x = rand() % 80;
		float x2 = (float)x / 10 - 4;
		worldTransformEnemy_.translation_.x = x2;

		worldTransformEnemy_.translation_.z = 40.0f;
	}
}

void GameScene::Collision() 
{ 
	CollisionPlayerEnemy();
	CollisionBeamEnemy();
}

void GameScene::CollisionPlayerEnemy() 
{ 
	if (enemyFlag_ == true) 
	{
		float dx = abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_.translation_.x);
		float dz = abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_.translation_.z);

		if (dx < 1 && dz < 1) 
		{
			enemyFlag_ = false;
			playerLife_ -= 1;
		}
	}
}

void GameScene::CollisionBeamEnemy() 
{
	if (enemyFlag_ == true) {
		float dx = abs(worldTransformBeam_.translation_.x - worldTransformEnemy_.translation_.x);
		float dz = abs(worldTransformBeam_.translation_.z - worldTransformEnemy_.translation_.z);

		if (dx < 1 && dz < 1) {
			enemyFlag_ = false;
			beamFlag_ = false;
			worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
			worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;
			gameScore_ += 1;
		}
	}
}

void GameScene::Update() 
{
	PlayerUpdate();
	BeamUpdate();
	EnemyUpdate();
	Collision();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	spriteBG_->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	modelStage_->Draw(worldTransformStage_,viewprojection_,textureHandleStage_);
	///プレイヤー
	modelPlayer_->Draw(worldTransformPlayer_,viewprojection_,textureHandlePlayer_);
	/// ビーム
	if (beamFlag_ == true) 
	{
		modelBeam_->Draw(worldTransformBeam_, viewprojection_, textureHandleBeam_);
	}
	/// エネミー
	if (enemyFlag_ == true) {
		modelEnemy_->Draw(worldTransformEnemy_, viewprojection_, textureHandleEnemy_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	/*debugText_->Print("AAA", 10, 10, 2);*/
	debugText_->DrawAll();
	//ゲームスコア
	char str[100];
	sprintf_s(str, "SCORE %d", gameScore_);
	debugText_->Print(str,200,10,2);

	char str1[100];
	sprintf_s(str1, "LIFE %d", playerLife_);
	debugText_->Print(str1, 880, 10, 2);
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
