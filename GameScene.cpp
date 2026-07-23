#include "GameScene.h"

#ifdef _DEBUG
#include "2d/ImGuiManager.h"
#endif

using namespace KamataEngine;

void GameScene::Initialize() {

	// テクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	// スプライトを生成
	sprite_ = Sprite::Create(textureHandle_, {100.0f, 50.0f});

	// モデル生成
	model_ = Model::Create();

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();

	// カメラ初期化
	camera_.Initialize();

	// ライン描画が参照するカメラを指定する
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);

	// 軸方向表示が参照するビュープロジェクションを指定する
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	// 音声ファイルを読み込む
	soundDataHandle_ = Audio::GetInstance()->LoadWave("fanfare.wav");

	// 音声をループ再生する
	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true);
}

void GameScene::Update() {

	// スプライトの現在座標を取得
	Vector2 position = sprite_->GetPosition();

	// 移動
	position.x += 2.0f;
	position.y += 1.0f;

	// 座標を反映
	sprite_->SetPosition(position);

	// ワールド行列更新
	worldTransform_.TransferMatrix();

	// デバッグカメラの更新
	debugCamera_->Update();

	// スペースキーを押した瞬間
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {

		// 音声停止
		Audio::GetInstance()->StopWave(voiceHandle_);
	}

#ifdef _DEBUG

	ImGui::Begin("Debug1");

	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);

	ImGui::InputFloat3("InputFloat3", inputFloat3_);

	ImGui::SliderFloat3("SliderFloat3", inputFloat3_, 0.0f, 1.0f);

	ImGui::End();

	ImGui::ShowDemoWindow();

#endif
}

void GameScene::Draw() {

	// 3Dモデル描画前処理
	Model::PreDraw();

	// モデルを描画
	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);

	// 3Dモデル描画後処理
	Model::PostDraw();

	// ラインを描画する
	PrimitiveDrawer::GetInstance()->DrawLine3d({0.0f, 0.0f, 0.0f}, {0.0f, 10.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f});
}

GameScene::~GameScene() {

	delete sprite_;
	sprite_ = nullptr;

	delete model_;
	model_ = nullptr;

	delete debugCamera_;
	debugCamera_ = nullptr;
}