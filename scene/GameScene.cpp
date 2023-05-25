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
	delete spriteTitle_;
	delete spriteEnter_;
	delete spriteEnd_;
	for (int i = 0; i < 5; i++) {
		delete spriteNumber_[i];
	}
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0,0});

	viewprojection_.Initialize();

	textureHandleStage_ = TextureManager::Load("stage2.jpg");
	modelStage_ = Model::Create();
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].Initialize();
	}

	viewprojection_.translation_.y = 1;
	viewprojection_.translation_.z = -6;
	viewprojection_.Initialize();
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].translation_ = {0, -1.5f, 2.0f * i - 5};
		worldTransformStage_[i].scale_ = {4.5f, 1, 1};

		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);

		worldTransformStage_[i].TransferMatrix();
	}
	//プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f,0.5f,0.5f};
	worldTransformPlayer_.Initialize();
	// ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].scale_ = {0.3f, 0.3f, 0.3f};
		worldTransformBeam_[i].Initialize();
	}
	// エネミー
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	for (int i = 0; i < 10;i++) {
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].Initialize();
	}

	srand((unsigned int)time(NULL));

	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();

	//タイトル（2Dスプライト）
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0,0});
	//タイトル(Hit Eneter Key)
	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 500});
	// タイトル(game over)
	textureHandleEnd_ = TextureManager::Load("gameover.png");
	spriteEnd_ = Sprite::Create(textureHandleEnd_, {0, 200});
	//スコア数値(2Dスプライト)
	textureHandleScore_ = TextureManager::Load("score.png");
	spriteScore_ = Sprite::Create(textureHandleScore_, {170, 0});
	textureHandleNumber_ = TextureManager::Load("number.png");
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i] = Sprite::Create(textureHandleNumber_, {300.0f + i * 26, 0});
	}
	//ライフ(2Dスプライト)
	for (int i = 0; i < playerLife_; i++) {
		spritePlayerLife_[i] = Sprite::Create(textureHandlePlayer_, {800.0f + i * 60, 0});
		spritePlayerLife_[i]->SetSize({40,40});
	}
	//サウンド読み込み
	soundataHandleTitleBGM_ = audio_->LoadWave("Audio/Ring05.wav");
	soundataHandleGamePlayBGM_ = audio_->LoadWave("Audio/Ring08.wav");
	soundataHandleGameOverBGM_ = audio_->LoadWave("Audio/Ring09.wav");
	soundataHandleEnemyHitSE_ = audio_->LoadWave("Audio/chord.wav");
	soundataHandlePlayerHitSE_ = audio_->LoadWave("Audio/tada.wav");
	//タイトルBGMを再生
	voiceHandleBGM_ = audio_->PlayWave(soundataHandleTitleBGM_,true);
}

void GameScene::StageUpdate() {
	for (int i = 0; i < 20; i++) {

		worldTransformStage_[i].translation_.z -= 0.1f;

		if (worldTransformStage_[i].translation_.z < -5) {
			worldTransformStage_[i].translation_.z += 40;
		}

		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);

		worldTransformStage_[i].TransferMatrix();
	}
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
	if (playerTimer_ >= 0) {
		playerTimer_ -= 1;
	}
}

void GameScene::BeamUpdate() 
{
	BeamMove();
	BeamBorn();
	/// ビーム更新
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformBeam_[i].scale_, worldTransformBeam_[i].rotation_,
		    worldTransformBeam_[i].translation_);
		worldTransformBeam_[i].TransferMatrix();
	}
}

void GameScene::BeamMove() 
{
	for (int i = 0; i < 10; i++) {
		if (beamFlag_[i] == true) {
			worldTransformBeam_[i].rotation_.x += 0.1f;
			worldTransformBeam_[i].translation_.z += 0.3f;
		}
		if (worldTransformBeam_[i].translation_.z >= 40.0f) {
			beamFlag_[i] = false;
		}
	}
}

void GameScene::BeamBorn() 
{
	if (beamTimer_ == 0) {
		for (int i = 0; i < 10; i++) {
			if (input_->TriggerKey(DIK_SPACE) && beamFlag_[i] == false) {
				worldTransformBeam_[i].translation_.x = worldTransformPlayer_.translation_.x;
				worldTransformBeam_[i].translation_.z = worldTransformPlayer_.translation_.z;

				beamFlag_[i] = true;
				break;
			}
		}
	} 
	else {
		beamTimer_++;
		if (beamTimer_ > 10) {
			beamTimer_ = 0;
		}
	}
}

void GameScene::EnemyUpdate() 
{
	EnemyMove();
	EnemyBorn();
	EnemyJump();	
	/// エネミー更新
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformEnemy_[i].scale_, worldTransformEnemy_[i].rotation_,
		    worldTransformEnemy_[i].translation_);
		worldTransformEnemy_[i].TransferMatrix();

		worldTransformEnemy_[i].translation_.x = max(worldTransformEnemy_[i].translation_.x, -4);
		worldTransformEnemy_[i].translation_.x = min(worldTransformEnemy_[i].translation_.x, 4);
	}
}

void GameScene::EnemyMove() 
{
	
	for (int i = 0; i < 10; i++) {
		if (enemyFlag_[i]) {
			worldTransformEnemy_[i].rotation_.x -= 0.1f;
			worldTransformEnemy_[i].translation_.x -= enemySpeed_[i];
			worldTransformEnemy_[i].translation_.z -= 0.3f;
			if (worldTransformEnemy_[i].translation_.x <= -4.0) {
				enemySpeed_[i] *= -1;
			} else if (worldTransformEnemy_[i].translation_.x >= 4.0) {
				enemySpeed_[i] *= -1;
			}
			if (worldTransformEnemy_[i].translation_.z <= -5.0f) {
				enemyFlag_[i] = false;
			}
		}

		worldTransformEnemy_[i].translation_.z -= 0.5f;
		worldTransformEnemy_[i].translation_.z -= gameTimer_ / 1000.0f;
		
	}
}

void GameScene::EnemyBorn() 
{
	if (rand() % 10 == 0) {
		for (int i = 0; i < 10; i++) {
			if (!enemyFlag_[i]) {
				int x = rand() % 80;
				float x2 = (float)x / 10 - 4;
				worldTransformEnemy_[i].translation_.z = 40.0f;
				worldTransformEnemy_[i].translation_.y = 0;
				worldTransformEnemy_[i].translation_.x = x2;
				enemyFlag_[i] = 1;

				if (rand() % 2 == 0) {
					enemySpeed_[i] = 0.1f;
				} 
				else {
					enemySpeed_[i] = -0.1f;
				}

				break;
			}
		}
	}
}

void GameScene::EnemyJump() 
{
	for (int i = 0; i < 10; i++) {
		if (enemyFlag_[i] == 2) {

			worldTransformEnemy_[i].translation_.y += enemyJumpSpeed_[i];

			enemyJumpSpeed_[i] -= 0.1f;

			//worldTransformEnemy_[i].translation_.x += enemySpeed_[i] * 4;

			if (worldTransformEnemy_[i].translation_.y < -3) {
				worldTransformEnemy_[i].translation_.y = 0;
				enemyFlag_[i] = 0;
			}
		}
	}
}

void GameScene::Collision() 
{ 
	CollisionPlayerEnemy();
	CollisionBeamEnemy();
}

void GameScene::CollisionPlayerEnemy() 
{
	for (int i = 0; i < 10; i++) {
		if (enemyFlag_[i] == 1) {
			float dx =
			    abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
			float dz =
			    abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);

			if (dx < 1 && dz < 1) {
				enemyFlag_[i] = 0;
				playerLife_ -= 1;
				playerTimer_ = 60;
				audio_->PlayWave(soundataHandlePlayerHitSE_);
			}
			if (playerLife_ <= 0) {
				sceneMode_ = 2;
			}
		}
	}
}

void GameScene::CollisionBeamEnemy() {
	for (int e = 0; e < 10; e++) {
		for (int b = 0; b < 10; b++) {
			if (enemyFlag_[e] == 1 && beamFlag_[b]) {

				float dx = abs(
				    worldTransformBeam_[b].translation_.x - worldTransformEnemy_[e].translation_.x);
				float dz = abs(
				    worldTransformBeam_[b].translation_.z - worldTransformEnemy_[e].translation_.z);

				if (dx < 1 && dz < 1) {

					worldTransformBeam_[b].translation_.x = worldTransformPlayer_.translation_.x;
					worldTransformBeam_[b].translation_.z = worldTransformPlayer_.translation_.z;


					enemyFlag_[e] = 2;
					beamFlag_[b] = 0;
					enemyJumpSpeed_[e] = 1;

					gameScore_ += 1;

					audio_->PlayWave(soundataHandleEnemyHitSE_);
				}
			}
		}
	}
}

void GameScene::DrawScore() 
{
	int eachNumber[5] = {};
	int number = gameScore_;
	
	int keta = 10000;
	
	for (int i = 0; i < 5; i++) 
	{
		eachNumber[i] = number / keta;
		number = number % keta;
		keta = keta / 10; 
	}

	for (int  i = 0; i < 5; i++) 
	{
		spriteNumber_[i]->SetSize({32, 64}); 
		spriteNumber_[i]->SetTextureRect({32.0f * eachNumber[i], 0}, {32, 64});
		spriteNumber_[i]->Draw();
	}
	for (int i = 0; i < playerLife_; i++) {
		spritePlayerLife_[i]->Draw();
		
	}

}

void GameScene::GamePlayerUpdate() 
{
	PlayerUpdate();
	BeamUpdate();
	EnemyUpdate();
	Collision();
	StageUpdate();
}

void GameScene::GamePlayerDraw3d() 
{
	for (int i = 0; i < 20; i++) {
		modelStage_->Draw(worldTransformStage_[i], viewprojection_, textureHandleStage_);
	}
	/// プレイヤー
	if (playerTimer_ % 4 < 2) {
		modelPlayer_->Draw(worldTransformPlayer_, viewprojection_, textureHandlePlayer_);
	}
	/// ビーム
	for (int i = 0; i < 10; i++) {
		if (beamFlag_[i]) {
			modelBeam_->Draw(worldTransformBeam_[i], viewprojection_, textureHandleBeam_);
		}
	}
	/// エネミー
	for (int i = 0; i < 10; i++) {
		if (!(!enemyFlag_[i])) {
			modelEnemy_->Draw(worldTransformEnemy_[i], viewprojection_, textureHandleEnemy_);
		}
	}
}

void GameScene::GamePlayerDraw2DBack() 
{ 
	spriteBG_->Draw();
}

void GameScene::GamePlayerDraw2DNear() 
{
	DrawScore();

	spriteScore_->Draw();
	
	// ゲームスコア
	/*char str[100];
	sprintf_s(str, "SCORE %d", gameScore_);
	debugText_->Print(str, 200, 10, 2);*/

	/*sprintf_s(str, "LIFE %d", playerLife_);
	debugText_->Print(str, 900, 10, 2);*/

}

void GameScene::TitleUpdate() 
{
	if (input_->TriggerKey(DIK_RETURN)) 
	{
		GamePlayerStart();
		sceneMode_ = 0;

		audio_->StopWave(voiceHandleBGM_);
		voiceHandleBGM_ = audio_->PlayWave(soundataHandleGamePlayBGM_,true);
	}
}

void GameScene::TitleDraw2DNear() 
{ 

	spriteTitle_->Draw();
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

void GameScene::EndUpdate() 
{ 
	if (input_->TriggerKey(DIK_RETURN)) {
		GamePlayerStart();
		sceneMode_ = 1;

		audio_->StopWave(voiceHandleBGM_);
		voiceHandleBGM_ = audio_->PlayWave(soundataHandleGameOverBGM_, true);	
	}
}

void GameScene::EndDraw2DNear() 
{ 
	spriteEnd_->Draw(); 
}

void GameScene::GamePlayerStart() {
	gameScore_ = 0;
	playerLife_ = 3;
	for (int e = 0; e < 10; e++) {
		for (int b = 0; b < 10; b++) {
			enemyFlag_[e] = false;
			beamFlag_[b] = false;
		}
	}
	worldTransformPlayer_.translation_.x = 0;
	worldTransformPlayer_.translation_.y = 0;
	worldTransformPlayer_.translation_.z = 0;

	gameTimer_ = 0;
	playerTimer_ = 0;
}

void GameScene::Update() 
{
	switch (sceneMode_) 
	{ 
	case 0:
		GamePlayerUpdate();
		break;
	case 1:
		TitleUpdate();
		gameTimer_++;
		break;
	case 2:
		EndUpdate();
		break;
	}
	
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
	switch (sceneMode_) {
	case 0:
		GamePlayerDraw2DBack();
		break;
	case 2:
		GamePlayerDraw2DBack();
		EndUpdate();
		break;
	}
	
	
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
	
	switch (sceneMode_) {
	case 0:
		GamePlayerDraw3d();
		break;
	case 2:
		GamePlayerDraw3d();
		EndUpdate();
		break;
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
	switch (sceneMode_) {
	case 0:
		GamePlayerDraw2DNear();
		break;
	case 1:
		TitleDraw2DNear();
		break;
	case 2:
		GamePlayerDraw2DNear();
		EndDraw2DNear();
		break;
	}
	
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
