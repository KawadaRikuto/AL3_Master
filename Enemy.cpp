#include "Enemy.h"

#include "WorldTransformUpdate.h"

#include <cassert>
#include <cmath>
#include <numbers>

namespace {

/// <summary>
/// 線形補間
/// </summary>
float Lerp(float start, float end, float t) { return start + (end - start) * t; }

} // namespace

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	// NULLポインタチェック
	assert(model);
	assert(camera);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 初期座標を設定
	worldTransform_.translation_ = position;

	// 左方向を向くように初期回転角を設定
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	// 速度を設定する
	velocity_ = {-kWalkSpeed, 0.0f, 0.0f};

	// 経過時間を初期化
	walkTimer_ = 0.0f;
}

void Enemy::Update() {

	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	// サインカーブで繰り返す値を計算
	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);

	// -1.0f～1.0fを0.0f～1.0fへ変換
	float t = (param + 1.0f) / 2.0f;

	// 最初の角度と最後の角度を補間
	float walkMotionAngle = Lerp(kWalkMotionAngleStart, kWalkMotionAngleEnd, t);

	// 度をラジアンへ変換してX軸回転を設定
	worldTransform_.rotation_.x = walkMotionAngle * std::numbers::pi_v<float> / 180.0f;

	// ワールド行列を更新する
	UpdateWorldTransform(worldTransform_);
}

void Enemy::Draw() {

	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}