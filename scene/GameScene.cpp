#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include<random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; }

void GameScene::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	textureHandle_ = TextureManager::Load("mario.jpg");
	model_ = Model::Create();

	//カメラ注意してん座標の設定

	//viewProjection_.target = {10, 0, 0};
	////カメラ上方向ベクトルを設定（右上45度指定）
	//viewProjection_.up = {cosf(XM_PI / 4.0f), sinf(XM_PI / 4.0f), 0.0f};
	
	//カメラ垂直方向視野角を設定
	//viewProjection_.fovAngleY = XMConvertToRadians(10.0f);
	//アスペクト比を設定
	//viewProjection_.aspectRatio = 1.0f;
	//
	////カメラに近い側と遠い側の表示限界
	////ニアクリップ距離を設定
	//viewProjection_.nearZ = 52.0f;
	////ファークリップ距離を設定
	//viewProjection_.farZ = 53.0f;

	viewProjection_.Initialize();
	
	//キャラクターの大元
	worldTransform_[PartId::Root].Initialize();
	
	//脊椎
	worldTransform_[PartId::Spine].translation_ = {0, 3.5f, 0};
	worldTransform_[PartId::Spine].parent_ = &worldTransform_[PartId::Root];
	worldTransform_[PartId::Spine].Initialize();

	//上半身
	//胸
	worldTransform_[PartId::Chest].Initialize();
	worldTransform_[PartId::Chest].parent_ = &worldTransform_[PartId::Spine];
	worldTransform_[PartId::Chest].translation_ = {0, 0, 0};
	//頭
	worldTransform_[PartId::Head].Initialize();
	worldTransform_[PartId::Head].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::Head].translation_ = {0, 3.5f, 0};
	//左腕
	worldTransform_[PartId::ArmL].Initialize();
	worldTransform_[PartId::ArmL].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmL].translation_ = {3.5f, 0, 0};
	//右腕
	worldTransform_[PartId::ArmR].Initialize();
	worldTransform_[PartId::ArmR].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmR].translation_ = {-3.5f, 0, 0};
	//下半身
	//尻
	worldTransform_[PartId::Hip].Initialize();
	worldTransform_[PartId::Hip].parent_ = &worldTransform_[PartId::Spine];
	worldTransform_[PartId::Hip].translation_ = {0, -3.5f, 0};
	//左足
	worldTransform_[PartId::LegL].Initialize();
	worldTransform_[PartId::LegL].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegL].translation_ = {3.5, -3.5f, 0};
	//右足
	worldTransform_[PartId::LegR].Initialize();
	worldTransform_[PartId::LegR].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegR].translation_ = {-3.5, -3.5f, 0};


}

void GameScene::Update() {
	
	{
		//キャラクターの移動処理
		XMFLOAT3 move = {0, 0, 0};

		//キャラクターの移動の速さ
		const float kCharacterSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_LEFT)) {
			move = {-kCharacterSpeed, 0, 0};
		} else if (input_->PushKey(DIK_RIGHT)) {
			move = {kCharacterSpeed, 0, 0};
		}

		//注視点移動（ベクトルの加算）
		worldTransform_[PartId::Root].translation_.x += move.x;
		worldTransform_[PartId::Root].translation_.y += move.y;
		worldTransform_[PartId::Root].translation_.z += move.z;

		//デバッグ用表示
		debugText_->SetPos(50, 150);
		debugText_->Printf(
		  "Root:(%f,%f,%f)", worldTransform_[PartId::Root].translation_.x += move.x,
		worldTransform_[PartId::Root].translation_.y += move.y,
		worldTransform_[PartId::Root].translation_.z += move.z);
		
		worldTransform_[PartId::Root].UpdateMatrix();
		worldTransform_[PartId::Spine].UpdateMatrix();
		worldTransform_[PartId::Chest].UpdateMatrix();
		worldTransform_[PartId::Head].UpdateMatrix();
		worldTransform_[PartId::ArmL].UpdateMatrix();
		worldTransform_[PartId::ArmR].UpdateMatrix();
		worldTransform_[PartId::Hip].UpdateMatrix();
		worldTransform_[PartId::LegL].UpdateMatrix();
		worldTransform_[PartId::LegR].UpdateMatrix();
	}

	//上半身の回転処理
	{ 
		//上半身の回転の速さ[ラジアン/flame]
		const float kChectSpeed = 0.05f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_U)) 
		{
			worldTransform_[PartId::Chest].rotation_.y -= kChectSpeed;
		} 
		else if (input_->PushKey(DIK_I))
		{
			worldTransform_[PartId::Chest].rotation_.y += kChectSpeed;
		}
	}

	//下半身の回転処理
	{
		//下半身の回転の速さ[ラジアン/flame]
		const float kHipSpeed = 0.05f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_J)) 
		{
			worldTransform_[PartId::Hip].rotation_.y -= kHipSpeed;
		} 
		else if (input_->PushKey(DIK_K)) 
		{
			worldTransform_[PartId::Hip].rotation_.y += kHipSpeed;
		}
	}

}

void GameScene::Draw() 
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

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
	//model_->Draw(worldTransform_[PartId::Root], viewProjection_, textureHandle_);
	//model_->Draw(worldTransform_[PartId::Spine], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Chest], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Head], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::ArmL], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::ArmR], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Hip], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::LegL], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::LegR], viewProjection_, textureHandle_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	
	//デバッグ用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
	  viewProjection_.target.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);

	debugText_->SetPos(50, 110);
	debugText_->Printf("forAngleY(Degree):%f", XMConvertToDegrees(viewProjection_.fovAngleY));

	debugText_->SetPos(50, 130);
	debugText_->Printf("nearZ:%f", viewProjection_.nearZ);

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
