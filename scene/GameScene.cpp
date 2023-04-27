#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "MathUtilityForText.h"

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete spriteBG_; 
	delete modelStage_;
	delete modelPlayer_;
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
void GameScene::Update() 
{
	PlayerUpdate();
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
	

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
