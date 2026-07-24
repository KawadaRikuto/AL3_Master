#include "Player.h"
#include "WorldTransformUpdate.h"

#include <cassert>

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {

	// NULLポインタチェック
	assert(model);
	assert(camera);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
}

void Player::Update() {

	// アフィン変換行列を計算してメンバ変数に代入し、
	// 定数バッファに転送
	UpdateWorldTransform(worldTransform_);
}

void Player::Draw() {

	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}