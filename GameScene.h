#pragma once

#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "GuardEffect.h"
#include "HitEffect.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "ShieldEnemy.h"
#include "Skydome.h"
#include "WorldTransformUpdate.h"

#include <3d/DebugCamera.h>

#include <list>
#include <vector>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,
		kPlay,
		kDeath,
		kFadeOut,
	};

public:
	~GameScene();

	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }

	/// <summary>
	/// ヒットエフェクトを生成
	/// </summary>
	void CreateHitEffect(const KamataEngine::Vector3& position);

	/// <summary>
	/// ガードエフェクトを生成
	/// </summary>
	void CreateGuardEffect(const KamataEngine::Vector3& position);

private:
	void GenerateBlocks();
	void CheckAllCollisions();
	void UpdatePlayPhase();
	void UpdateDeathPhase();
	void ChangePhase();

private:
	Phase phase_ = Phase::kPlay;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelEnemy_ = nullptr;
	KamataEngine::Model* modelShieldEnemy_ = nullptr;
	KamataEngine::Model* modelAttack_ = nullptr;
	KamataEngine::Model* modelHitEffect_ = nullptr;

	// ガードエフェクト用3Dモデルデータ
	KamataEngine::Model* modelGuardEffect_ = nullptr;

	KamataEngine::Model* modelDeathParticle_ = nullptr;

	KamataEngine::Camera camera_;
	CameraController* cameraController_ = nullptr;
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	Player* player_ = nullptr;

	std::list<Enemy*> enemies_;
	std::list<ShieldEnemy*> shieldEnemies_;
	std::list<HitEffect*> hitEffects_;

	// ガードエフェクト
	std::list<GuardEffect*> guardEffects_;

	DeathParticles* deathParticles_ = nullptr;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	Fade* fade_ = nullptr;
	bool finished_ = false;

	static inline const float kFadeDuration = 1.0f;
};
