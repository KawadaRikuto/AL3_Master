#include "CameraController.h"

#include "Player.h"

#include <algorithm>
#include <cassert>

void CameraController::Initialize() {

	// カメラの初期化
	camera_.Initialize();
}

void CameraController::Reset() {

	assert(target_);

	// 追従対象のワールドトランスフォームを参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;

	camera_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;

	camera_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;

	// 行列を更新する
	camera_.UpdateMatrix();
}

void CameraController::Update() {

	assert(target_);

	// 追従対象のワールドトランスフォームを参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;

	camera_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;

	camera_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;

	// カメラ移動範囲
	camera_.translation_.x = std::clamp(camera_.translation_.x, movableArea_.left, movableArea_.right);

	camera_.translation_.y = std::clamp(camera_.translation_.y, movableArea_.bottom, movableArea_.top);

	// 行列を更新する
	camera_.UpdateMatrix();
}