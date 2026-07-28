#pragma once

#include "AABB.h"
#include "KamataEngine.h"

// 前方宣言
class Player;
class GameScene;

/// <summary>
/// 敵
/// </summary>
class Enemy {

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
	/// ワールド座標を取得
	/// </summary>
	KamataEngine::Vector3 GetWorldPosition();

	/// <summary>
	/// AABBを取得
	/// </summary>
	AABB GetAABB();

	/// <summary>
	/// 衝突応答
	/// </summary>
	void OnCollision(const Player* player);

	/// <summary>
	/// デスフラグのgetter
	/// </summary>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// コリジョン無効フラグのgetter
	/// </summary>
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

	/// <summary>
	/// ゲームシーンを設定
	/// </summary>
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

private:
	// 振るまい
	enum class Behavior {
		kUnknown,
		kRoot,
		kDeath,
	};

	/// <summary>
	/// 歩行ビヘイビアの初期化
	/// </summary>
	void BehaviorRootInitialize();

	/// <summary>
	/// デス演出ビヘイビアの初期化
	/// </summary>
	void BehaviorDeathInitialize();

	/// <summary>
	/// 歩行ビヘイビアの更新
	/// </summary>
	void BehaviorRootUpdate();

	/// <summary>
	/// デス演出ビヘイビアの更新
	/// </summary>
	void BehaviorDeathUpdate();

private:
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.05f;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = -10.0f;

	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 10.0f;

	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;

	// 経過時間
	float walkTimer_ = 0.0f;

	// 敵の当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 現在の振るまい
	Behavior behavior_ = Behavior::kRoot;

	// 次の振るまいリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// デス演出の経過時間
	float deathTimer_ = 0.0f;

	// デス演出時間
	static inline const float kDeathMotionTime = 1.0f;

	// デスフラグ
	bool isDead_ = false;

	// コリジョン無効フラグ
	bool isCollisionDisabled_ = false;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;
};