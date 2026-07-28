#include "GameScene.h"

#include <cmath>

namespace {

/// <summary>
/// AABB同士の交差判定
/// </summary>
bool IsCollision(const AABB& aabb1, const AABB& aabb2) {

	if (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x && aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y && aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z) {

		return true;
	}

	return false;
}

} // namespace

KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2) {

	KamataEngine::Matrix4x4 result{};

	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			for (int i = 0; i < 4; ++i) {

				result.m[row][column] += m1.m[row][i] * m2.m[i][column];
			}
		}
	}

	return result;
}

KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale) {

	KamataEngine::Matrix4x4 result{};

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	result.m[3][3] = 1.0f;

	return result;
}

KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian) {

	KamataEngine::Matrix4x4 result{};

	result.m[0][0] = 1.0f;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[3][3] = 1.0f;

	return result;
}

KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian) {

	KamataEngine::Matrix4x4 result{};

	result.m[0][0] = std::cos(radian);
	result.m[0][2] = -std::sin(radian);
	result.m[1][1] = 1.0f;
	result.m[2][0] = std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[3][3] = 1.0f;

	return result;
}

KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian) {

	KamataEngine::Matrix4x4 result{};

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	return result;
}

KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate) {

	KamataEngine::Matrix4x4 result{};

	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;

	return result;
}

KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotation, const KamataEngine::Vector3& translation) {

	KamataEngine::Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	KamataEngine::Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotation.x);
	KamataEngine::Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotation.y);
	KamataEngine::Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotation.z);
	KamataEngine::Matrix4x4 translateMatrix = MakeTranslateMatrix(translation);
	KamataEngine::Matrix4x4 rotateMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	return Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
}

void UpdateWorldTransform(KamataEngine::WorldTransform& worldTransform) {

	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	worldTransform.TransferMatrix();
}

void GameScene::Initialize() {

	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	model_ = KamataEngine::Model::CreateFromOBJ("player", true);
	modelBlock_ = KamataEngine::Model::CreateFromOBJ("block", true);
	modelSkydome_ = KamataEngine::Model::CreateFromOBJ("skydome", true);
	modelEnemy_ = KamataEngine::Model::CreateFromOBJ("enemy", true);

	// デスパーティクル用3Dモデルデータの生成
	modelDeathParticle_ = KamataEngine::Model::CreateFromOBJ("deathParticle", true);

	camera_.farZ = 1000.0f;
	camera_.Initialize();

	debugCamera_ = new KamataEngine::DebugCamera(KamataEngine::WinApp::kWindowWidth, KamataEngine::WinApp::kWindowHeight);
	debugCamera_->SetFarZ(1000.0f);

	player_ = new Player();
	KamataEngine::Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(10, 16);
	player_->Initialize(model_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	const int32_t kNumEnemies = 3;
	for (int32_t i = 0; i < kNumEnemies; ++i) {

		Enemy* newEnemy = new Enemy();
		KamataEngine::Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(10 + i * 5, 18);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	cameraController_ = new CameraController();
	cameraController_->Initialize();

	CameraController::Rect cameraArea = {
	    0.0f,
	    100.0f,
	    0.0f,
	    20.0f,
	};

	cameraController_->SetMovableArea(cameraArea);
	cameraController_->SetTarget(player_);
	cameraController_->Reset();

	camera_.matView = cameraController_->GetCamera().matView;
	camera_.matProjection = cameraController_->GetCamera().matProjection;
	camera_.TransferMatrix();

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	GenerateBlocks();

	// ゲームプレイフェーズから開始
	phase_ = Phase::kPlay;

	// 終了フラグを初期化
	finished_ = false;

}

void GameScene::GenerateBlocks() {

	worldTransformBlocks_.resize(MapChipField::kNumBlockVertical);

	for (uint32_t i = 0; i < MapChipField::kNumBlockVertical; ++i) {
		worldTransformBlocks_[i].resize(MapChipField::kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < MapChipField::kNumBlockVertical; ++i) {
		for (uint32_t j = 0; j < MapChipField::kNumBlockHorizontal; ++j) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlank) {
				continue;
			}

			worldTransformBlocks_[i][j] = new KamataEngine::WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();

			KamataEngine::Vector3 blockPosition = mapChipField_->GetMapChipPositionByIndex(j, i);
			worldTransformBlocks_[i][j]->translation_ = blockPosition;
		}
	}
}

void GameScene::Update() {

	switch (phase_) {

	case Phase::kPlay:

		// ゲームプレイフェーズの処理
		UpdatePlayPhase();
		break;

	case Phase::kDeath:

		// デス演出フェーズの処理
		UpdateDeathPhase();
		break;
	}

	// フェーズの切り替え
	ChangePhase();
}

void GameScene::UpdatePlayPhase() {

#ifdef _DEBUG
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_TAB)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	// 天球の更新
	if (skydome_) {
		skydome_->Update();
	}

	// 自キャラの更新
	if (player_) {
		player_->Update();
	}

	// 敵の更新
	for (Enemy* enemy : enemies_) {
		if (enemy) {
			enemy->Update();
		}
	}

	// カメラの更新
	if (isDebugCameraActive_) {

		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();

	} else {

		cameraController_->Update();
		camera_.matView = cameraController_->GetCamera().matView;
		camera_.matProjection = cameraController_->GetCamera().matProjection;
		camera_.TransferMatrix();
	}

	// ブロックの更新
	for (std::vector<KamataEngine::WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}

			UpdateWorldTransform(*worldTransformBlock);
		}
	}

	// 全ての当たり判定
	CheckAllCollisions();
}

void GameScene::UpdateDeathPhase() {

	// 天球の更新
	if (skydome_) {
		skydome_->Update();
	}

	// 敵の更新
	for (Enemy* enemy : enemies_) {

		if (enemy) {
			enemy->Update();
		}
	}

	// デスパーティクルの更新
	if (deathParticles_) {
		deathParticles_->Update();
	}

	// デスパーティクルが有効で、
	// パーティクルの演出が終了した
	if (deathParticles_ && deathParticles_->IsFinished()) {

		// ゲームシーンの終了フラグを立てる
		finished_ = true;
	}

	// ブロックの更新
	for (std::vector<KamataEngine::WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}

			UpdateWorldTransform(*worldTransformBlock);
		}
	}
}

void GameScene::ChangePhase() {

	switch (phase_) {

	case Phase::kPlay:

		// 自キャラがデス状態
		if (player_->IsDead()) {

			// デス演出フェーズに切り替え
			phase_ = Phase::kDeath;

			// 自キャラの座標を取得
			const KamataEngine::Vector3 deathParticlesPosition = player_->GetWorldPosition();

			// デスパーティクルを生成
			deathParticles_ = new DeathParticles();

			// 自キャラの座標にデスパーティクルを発生、初期化
			deathParticles_->Initialize(modelDeathParticle_, &camera_, deathParticlesPosition);
		}

		break;

	case Phase::kDeath:

		break;
	}
}

void GameScene::CheckAllCollisions() {

#pragma region 自キャラと敵キャラの当たり判定

	AABB aabb1;
	AABB aabb2;

	aabb1 = player_->GetAABB();

	for (Enemy* enemy : enemies_) {

		if (!enemy) {
			continue;
		}

		aabb2 = enemy->GetAABB();

		if (IsCollision(aabb1, aabb2)) {

			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}

#pragma endregion
}

void GameScene::Draw() {

	if (skydome_) {
		skydome_->Draw();
	}

	// ゲームプレイフェーズのときだけ自キャラを描画
	if (phase_ == Phase::kPlay) {

		if (player_) {
			player_->Draw();
		}
	}

	for (Enemy* enemy : enemies_) {

		if (enemy) {
			enemy->Draw();
		}
	}

	// デスパーティクルが存在するなら描画
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	for (std::vector<KamataEngine::WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}

			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}
}

GameScene::~GameScene() {

	for (std::vector<KamataEngine::WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	enemies_.clear();

	// デスパーティクルの解放
	delete deathParticles_;

	delete mapChipField_;
	delete skydome_;
	delete modelSkydome_;
	delete cameraController_;
	delete debugCamera_;
	delete modelEnemy_;

	// デスパーティクル用3Dモデルデータの解放
	delete modelDeathParticle_;

	delete player_;
	delete model_;
	delete modelBlock_;
}
