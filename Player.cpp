#define NOMINMAX

#include "Player.h"

#include "MapChipField.h"
#include "WorldTransformUpdate.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <numbers>

namespace {

/// <summary>
/// イーズイン・イーズアウト補間
/// </summary>
float EaseInOut(float start, float end, float t) {

	t = t * t * (3.0f - 2.0f * t);

	return start + (end - start) * t;
}

} // namespace

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	// NULLポインタチェック
	assert(model);
	assert(camera);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// 初期座標を設定
	worldTransform_.translation_ = position;

	// 初期回転角を指定
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::InputMove() {

	// 接地状態
	if (onGround_) {

		// 左右移動操作
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT) || KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			KamataEngine::Vector3 acceleration = {};

			if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {

				// 左移動中の右入力
				if (velocity_.x < 0.0f) {

					// 速度と逆方向に入力中は減速
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x += kAcceleration;

				// 右向きではなかったら右向きに変更
				if (lrDirection_ != LRDirection::kRight) {

					lrDirection_ = LRDirection::kRight;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;

					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}

			} else if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

				// 右移動中の左入力
				if (velocity_.x > 0.0f) {

					// 速度と逆方向に入力中は減速
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;

				// 左向きではなかったら左向きに変更
				if (lrDirection_ != LRDirection::kLeft) {

					lrDirection_ = LRDirection::kLeft;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;

					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			}

			// 加速
			velocity_.x += acceleration.x;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {

			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		// ジャンプ入力
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_UP)) {

			// ジャンプ初速
			velocity_.y += kJumpAcceleration;
		}

	} else {

		// 落下速度
		velocity_.y -= kGravityAcceleration;

		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

KamataEngine::Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {

	KamataEngine::Vector3 offsetTable[kNumCorner] = {
	    {
         +kWidth / 2.0f,
         -kHeight / 2.0f,
         0.0f, },
	    {
         -kWidth / 2.0f,
         -kHeight / 2.0f,
         0.0f, },
	    {
         +kWidth / 2.0f,
         +kHeight / 2.0f,
         0.0f, },
	    {
         -kWidth / 2.0f,
         +kHeight / 2.0f,
         0.0f, },
	};

	KamataEngine::Vector3 result = {};

	result.x = center.x + offsetTable[static_cast<uint32_t>(corner)].x;

	result.y = center.y + offsetTable[static_cast<uint32_t>(corner)].y;

	result.z = center.z + offsetTable[static_cast<uint32_t>(corner)].z;

	return result;
}

void Player::MapCollision(CollisionMapInfo& info) {

	MapCollisionUp(info);
	MapCollisionDown(info);
	MapCollisionRight(info);
	MapCollisionLeft(info);
}

void Player::MapCollisionUp(CollisionMapInfo& info) {

	// 上昇あり？
	if (info.move.y <= 0.0f) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<KamataEngine::Vector3, kNumCorner> positionsNew;

	KamataEngine::Vector3 centerNew = {};

	centerNew.x = worldTransform_.translation_.x + info.move.x;

	centerNew.y = worldTransform_.translation_.y + info.move.y;

	centerNew.z = worldTransform_.translation_.z + info.move.z;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {

		positionsNew[i] = CornerPosition(centerNew, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {

		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);

		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		// ブロックの下面より下に収まるように移動量を修正
		info.move.y = std::max(0.0f, info.move.y + rect.bottom - positionsNew[kLeftTop].y - kBlank);

		// 天井に当たったことを記録する
		info.ceiling = true;
	}
}

void Player::MapCollisionDown(CollisionMapInfo& info) { (void)info; }

void Player::MapCollisionRight(CollisionMapInfo& info) { (void)info; }

void Player::MapCollisionLeft(CollisionMapInfo& info) { (void)info; }

void Player::Move(const CollisionMapInfo& info) {

	// 移動
	worldTransform_.translation_.x += info.move.x;

	worldTransform_.translation_.y += info.move.y;

	worldTransform_.translation_.z += info.move.z;
}

void Player::CeilingCollision(const CollisionMapInfo& info) {

	// 天井に当たった？
	if (info.ceiling) {

		KamataEngine::DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");

		velocity_.y = 0.0f;
	}
}

void Player::Update() {

	// ①移動入力
	InputMove();

	// ②移動量を加味して衝突判定する

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;

	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;

	// マップ衝突判定
	MapCollision(collisionMapInfo);

	// ③判定結果を反映して移動させる
	Move(collisionMapInfo);

	// ④天井に接触している場合の処理
	CeilingCollision(collisionMapInfo);

	// ⑤壁に接触している場合の処理
	// 現時点では未実装

	// ⑥接地状態の切り替え

	// 着地フラグ
	bool landing = false;

	// 地面との当たり判定
	// 下降中？
	if (velocity_.y < 0.0f) {

		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f) {

			landing = true;
		}
	}

	// 接地判定
	if (onGround_) {

		// ジャンプ開始
		if (velocity_.y > 0.0f) {

			// 空中状態に移行
			onGround_ = false;
		}

	} else {

		// 着地
		if (landing) {

			// めり込みを排除
			worldTransform_.translation_.y = 1.0f;

			// 摩擦で横方向速度が減速する
			velocity_.x *= (1.0f - kAttenuation);

			// 下方向速度をリセット
			velocity_.y = 0.0f;

			// 接地状態に移行
			onGround_ = true;
		}
	}

	// ⑦旋回制御
	if (turnTimer_ > 0.0f) {

		// 旋回タイマーを1/60秒だけカウントダウンする
		turnTimer_ -= 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,
		    std::numbers::pi_v<float> * 3.0f / 2.0f,
		};

		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// 補間割合
		float turnProgress = 1.0f - turnTimer_ / kTimeTurn;

		turnProgress = std::clamp(turnProgress, 0.0f, 1.0f);

		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = EaseInOut(turnFirstRotationY_, destinationRotationY, turnProgress);
	}

	// ⑧行列計算
	UpdateWorldTransform(worldTransform_);
}

void Player::Draw() {

	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}