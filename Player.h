#pragma once

#include "KamataEngine.h"

// 前方宣言
class MapChipField;

/// <summary>
/// 自キャラ
/// </summary>
class Player {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 速度を取得
	/// </summary>
	const KamataEngine::Vector3 GetVelocity() const { return velocity_; }

	/// <summary>
	/// ワールドトランスフォームを取得
	/// </summary>
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	/// <summary>
	/// マップチップフィールドを設定
	/// </summary>
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

private:
	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		KamataEngine::Vector3 move = {};
	};

	/// <summary>
	/// 移動入力
	/// </summary>
	void InputMove();

	/// <summary>
	/// マップ衝突判定
	/// </summary>
	void MapCollision(CollisionMapInfo& info);

	/// <summary>
	/// 上方向のマップ衝突判定
	/// </summary>
	void MapCollisionUp(CollisionMapInfo& info);

	/// <summary>
	/// 下方向のマップ衝突判定
	/// </summary>
	void MapCollisionDown(CollisionMapInfo& info);

	/// <summary>
	/// 右方向のマップ衝突判定
	/// </summary>
	void MapCollisionRight(CollisionMapInfo& info);

	/// <summary>
	/// 左方向のマップ衝突判定
	/// </summary>
	void MapCollisionLeft(CollisionMapInfo& info);

	/// <summary>
	/// 判定結果を反映して移動させる
	/// </summary>
	void Move(const CollisionMapInfo& info);

	/// <summary>
	/// 天井に接触している場合の処理
	/// </summary>
	void CeilingCollision(const CollisionMapInfo& info);

	/// <summary>
	/// 指定した角の座標を取得
	/// </summary>
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 当たり判定用の隙間
	static inline const float kBlank = 0.01f;

	// 加速度
	static inline const float kAcceleration = 0.01f;

	// 速度減衰率
	static inline const float kAttenuation = 0.1f;

	// 最大速度
	static inline const float kLimitRunSpeed = 0.2f;

	// 左右の向き
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;

	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 旋回時間
	static inline const float kTimeTurn = 0.3f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 重力加速度
	static inline const float kGravityAcceleration = 0.05f;

	// 最大落下速度
	static inline const float kLimitFallSpeed = 0.5f;

	// ジャンプ初速
	static inline const float kJumpAcceleration = 1.0f;
};