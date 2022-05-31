#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include<math.h>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	textureHandle_ = TextureManager::Load("mario.jpg");
	model_ = Model::Create();
	
	viewProjection_.eye = {10.0f, 0, 0};

	viewProjection_.Initialize();
	
	worldTransform_.scale_ = {1.0f, 1.0f,1.0f};
	worldTransform_.translation_ = {0,0,0};
	worldTransform_.Initialize();
}

void GameScene::Update() 
{
	viewProjection_.eye.x = Radius * cos(kakudo); 
	viewProjection_.eye.z = Radius * sin(kakudo);

	kakudo+= 0.03f;

	if (kakudo > 360) 
	{
		kakudo = 0;
	}

	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
	debugText_->SetPos(50, 70);
	debugText_->Printf("kakudo:(%f)", kakudo);

	//if (viewProjection_.eye.x < -10.0f || viewProjection_.eye.x > 10.0f) 
	//{
	//	XCameraSpeed *= -1.0f;
	//}
	//if (viewProjection_.eye.z < -10.0f || viewProjection_.eye.z > 10.0f) 
	//{
	//	ZCameraSpeed *= -1.0f;
	//}

	viewProjection_.UpdateMatrix();

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
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);
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
	

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
