#include "GameScene.h"

void GameScene::Initialize() {

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = KamataEngine::TextureManager::Load("uvChecker.png");

	// 3Dモデルデータの生成
	model_ = KamataEngine::Model::Create();

	// カメラの初期化
	camera_.Initialize();

	// 自キャラの生成
	player_ = new Player();

	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &camera_);
}

void GameScene::Update() {

	// 自キャラの更新
	player_->Update();
}

void GameScene::Draw() {

	// 自キャラの描画
	player_->Draw();
}

GameScene::~GameScene() {

	// 自キャラの解放
	delete player_;

	// 3Dモデルデータの解放
	delete model_;
}