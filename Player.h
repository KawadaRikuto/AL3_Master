#pragma once

#include "AABB.h"
#include "KamataEngine.h"

// 前方宣言
class MapChipField;
class Enemy;

/// <summary>
/// 自キャラ
/// </summary>
class Player {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

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

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	KamataEngine::Vector3 GetWorldPosition() const;

	/// <summary>
	/// AABBを取得
	/// </summary>
	AABB GetAABB();

	/// <summary>
	/// 衝突応答
	/// </summary>
	void OnCollision(const Enemy* enemy);

	/// <summary>
	/// ノックバックをリクエスト
	/// </summary>
	void RequestKnockback() { knockbackRequest_ = true; }

	/// <summary>
	/// デスフラグのgetter
	/// </summary>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// 攻撃中かどうか
	/// </summary>
	bool IsAttack() const { return behavior_ == Behavior::kAttack; }

	/// <summary>
	/// 右を向いているか取得
	/// </summary>
	bool IsFacingRight() const;

private:
	// 振るまい
	enum class Behavior {
		kUnknown,
		kRoot,
		kAttack,
		kKnockback,
	};

	// 攻撃フェーズ
	enum class AttackPhase {
		kCharge,
		kDash,
		kRecovery,
	};

	// ノックバックフェーズ
	enum class KnockbackPhase {
		kMove,
		kRecovery,
	};

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,
		kNumCorner,
	};

	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		KamataEngine::Vector3 move = {};
	};

	void InputMove();
	void MapCollision(CollisionMapInfo& info);
	void MapCollisionUp(CollisionMapInfo& info);
	void MapCollisionDown(CollisionMapInfo& info);
	void MapCollisionRight(CollisionMapInfo& info);
	void MapCollisionLeft(CollisionMapInfo& info);
	void Move(const CollisionMapInfo& info);
	void CeilingCollision(const CollisionMapInfo& info);
	void WallCollision(const CollisionMapInfo& info);
	void SwitchGroundState(const CollisionMapInfo& info);

	void BehaviorRootInitialize();
	void BehaviorAttackInitialize();
	void BehaviorKnockbackInitialize();

	void BehaviorRootUpdate();
	void BehaviorAttackUpdate();
	void BehaviorKnockbackUpdate();

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransformAttack_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelAttack_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	MapChipField* mapChipField_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.01f;
	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kAttenuationLanding = 0.1f;
	static inline const float kAttenuationWall = 0.1f;
	static inline const float kLimitRunSpeed = 0.2f;

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;

	static inline const float kGravityAcceleration = 0.05f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kJumpAcceleration = 1.0f;

	bool isDead_ = false;

	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	AttackPhase attackPhase_ = AttackPhase::kCharge;
	uint32_t attackParameter_ = 0;

	static inline const uint32_t kChargeDuration = 20;
	static inline const uint32_t kDashDuration = 10;
	static inline const uint32_t kRecoveryDuration = 15;

	// ノックバックリクエスト
	bool knockbackRequest_ = false;

	// 現在のノックバックフェーズ
	KnockbackPhase knockbackPhase_ = KnockbackPhase::kMove;

	// ノックバック経過時間
	uint32_t knockbackParameter_ = 0;

	// ノックバック移動時間
	static inline const uint32_t kKnockbackMoveDuration = 10;

	// ノックバック復帰時間
	static inline const uint32_t kKnockbackRecoveryDuration = 18;

	// ノックバック横速度
	static inline const float kKnockbackSpeed = 0.35f;

	// ノックバック上方向速度
	static inline const float kKnockbackJumpSpeed = 0.25f;
};
