#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

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
	
		//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲（座標用）
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	for (int i = 0; i < XYZ; i++) 
	{
		x[i] = posDist(engine);
		y[i] = posDist(engine);
		z[i] = posDist(engine);
	}

	oldInput = input_->PushKey(DIK_SPACE);

	viewProjection_.eye = {x[0], y[0], z[0]};

	viewProjection_.target = {0, 0, 0};
	viewProjection_.up = {0, 1.0f, 0};

	viewProjection_.Initialize();
	
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0, 0, 0};
	worldTransform_.translation_ = {0,0,0};

	worldTransform_.Initialize();
}

void GameScene::Update() 
{
	if ((input_->PushKey(DIK_SPACE)) && oldInput == false) 
	{
		cameraNum++;
	}
	
	if (input_->PushKey(DIK_SPACE)) 
	{
		oldInput = true;
	} 
	else 
	{
		oldInput = false;
	}

	if (cameraNum >=XYZ) 
	{
		cameraNum = 0;
	}

	for (int i = 0;i < XYZ;i++) 
	{
		if (cameraNum == i) 
		{
			viewProjection_.eye = {x[i], y[i], z[i]};
		}
	}

	viewProjection_.UpdateMatrix();

	debugText_->SetPos(50, 30);
	debugText_->Printf("Camera1");
	debugText_->SetPos(50, 130);
	debugText_->Printf("Camera2");
	debugText_->SetPos(50, 230);
	debugText_->Printf("Camera3");

	for (int i = 0; i < XYZ; i++) 
	{
		debugText_->SetPos(50, 50 + i*100);
		debugText_->Printf("eye:(%f,%f,%f)", x[i], y[i], z[i]);
		debugText_->SetPos(50, 70 + i * 100);
		debugText_->Printf("target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,viewProjection_.target.z);
		debugText_->SetPos(50, 90 + i * 100);
		debugText_->Printf("up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
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
