#pragma once

#include "KamataEngine.h"

#include <array>
#include <numbers>

/// <summary>
/// デスパーティクル
/// </summary>
class DeathParticles {

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
	/// 終了フラグのgetter
	/// </summary>
	bool IsFinished() const { return isFinished_; }

private:
	// パーティクルの数
	static inline const uint32_t kNumParticles = 8;

	// パーティクルの表示時間
	static inline const float kDuration = 1.0f;

	// パーティクルの移動速度
	static inline const float kSpeed = 0.1f;

	// パーティクルごとの角度
	static inline const float kAngleUnit = 2.0f * std::numbers::pi_v<float> / static_cast<float>(kNumParticles);

	// ワールドトランスフォーム
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// オブジェクトカラー
	KamataEngine::ObjectColor objectColor_;

	// 色
	KamataEngine::Vector4 color_;

	// 終了フラグ
	bool isFinished_ = false;

	// 経過時間
	float counter_ = 0.0f;
};