#include "GameScene.h"

#include <cmath>

#ifdef _DEBUG
#include <imgui.h>
#endif

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

void GameScene::Initialize(StageManager* stageDataManager) {

	// 引数をメンバ変数に記録する
	stageManager_ = stageDataManager;

	// マップチップフィールドの生成
	mapChipField_ = new MapChipField();

	// 現在のステージデータを取得する
	const StageManager::StageData& stageData = stageManager_->GetCurrentStageData();

	// ステージファイルパスの生成
	const std::string stageFileName = "Resources/fields/" + stageData.name + ".csv";

	// ステージファイルの読み込み
	mapChipField_->LoadMapChipCsv(stageFileName);

	model_ = KamataEngine::Model::CreateFromOBJ("player", true);

	modelBlock_ = KamataEngine::Model::CreateFromOBJ("block", true);

	modelSkydome_ = KamataEngine::Model::CreateFromOBJ("skydome", true);

	modelEnemy_ = KamataEngine::Model::CreateFromOBJ("enemy", true);

	// 盾持ち敵用3Dモデルデータの生成
	modelShieldEnemy_ = KamataEngine::Model::CreateFromOBJ("shieldEnemy", true);

	// 攻撃エフェクト用3Dモデルデータの生成
	modelAttack_ = KamataEngine::Model::CreateFromOBJ("hit_effect", true);

	// ヒットエフェクト用3Dモデルデータの生成
	modelHitEffect_ = KamataEngine::Model::CreateFromOBJ("HitEffect", true);

	// ガードエフェクト用3Dモデルデータの生成
	modelGuardEffect_ = KamataEngine::Model::CreateFromOBJ("ring", true);

	modelDeathParticle_ = KamataEngine::Model::CreateFromOBJ("deathParticle", true);

	camera_.farZ = 1000.0f;
	camera_.Initialize();

	// ヒットエフェクト用3Dモデルデータの生成
	modelHitEffect_ = KamataEngine::Model::CreateFromOBJ("HitEffect", true);

	// ヒットエフェクトで使用するモデルとカメラを設定
	HitEffect::SetModel(modelHitEffect_);
	HitEffect::SetCamera(&camera_);

	// ガードエフェクトで使用するモデルとカメラを設定
	GuardEffect::SetModel(modelGuardEffect_);
	GuardEffect::SetCamera(&camera_);

	debugCamera_ = new KamataEngine::DebugCamera(KamataEngine::WinApp::kWindowWidth, KamataEngine::WinApp::kWindowHeight);

	debugCamera_->SetFarZ(1000.0f);

	// フィールドオブジェクトを生成
	GenerateFieldObjects();

	// CSVに自キャラが配置されているか確認
	assert(player_);

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

	// フェードの生成
	fade_ = new Fade();

	// フェードの初期化
	fade_->Initialize();

	// フェードイン開始
	fade_->Start(Fade::Status::FadeIn, kFadeDuration);

	// フェードインフェーズから開始
	phase_ = Phase::kFadeIn;

	// 終了フラグを初期化
	finished_ = false;
}

void GameScene::CreateHitEffect(const KamataEngine::Vector3& position) {

	// ヒットエフェクトを生成
	HitEffect* newHitEffect = HitEffect::Create(position);

	// リストへ追加
	hitEffects_.push_back(newHitEffect);
}

void GameScene::CreateGuardEffect(const KamataEngine::Vector3& position) {

	// ガードエフェクトを生成
	GuardEffect* newGuardEffect = GuardEffect::Create(position);

	// リストへ追加
	guardEffects_.push_back(newGuardEffect);
}

void GameScene::GenerateFieldObjects() {

	worldTransformBlocks_.resize(MapChipField::kNumBlockVertical);

	for (uint32_t i = 0; i < MapChipField::kNumBlockVertical; ++i) {

		worldTransformBlocks_[i].resize(MapChipField::kNumBlockHorizontal);
	}

	// フィールドオブジェクトの生成
	for (uint32_t i = 0; i < MapChipField::kNumBlockVertical; ++i) {

		for (uint32_t j = 0; j < MapChipField::kNumBlockHorizontal; ++j) {

			// マップチップ種別を取得
			MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(j, i);

			switch (mapChipType) {

			case MapChipType::kBlock: {

				// ブロックの生成
				worldTransformBlocks_[i][j] = new KamataEngine::WorldTransform();

				worldTransformBlocks_[i][j]->Initialize();

				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);

				break;
			}

			case MapChipType::kPlayer: {

				assert(player_ == nullptr && "自キャラを二重に配置しようとしています");

				// 自キャラの生成
				player_ = new Player();

				// 座標を指定して自キャラの初期化
				KamataEngine::Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(j, i);

				player_->Initialize(model_, modelAttack_, &camera_, playerPosition);

				// 自キャラにマップチップフィールドをセット
				player_->SetMapChipField(mapChipField_);

				break;
			}

			case MapChipType::kEnemy: {

				// 敵のサブIDを取得
				uint8_t subID = mapChipField_->GetMapChipSubIDByIndex(j, i);

				switch (subID) {

				case 0: {

					// 通常敵の生成
					Enemy* newEnemy = new Enemy();

					// 座標を指定して通常敵を初期化
					KamataEngine::Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(j, i);

					newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);

					// 敵にゲームシーンのポインタをセット
					newEnemy->SetGameScene(this);

					// 通常敵のリストに追加
					enemies_.push_back(newEnemy);

					break;
				}

				case 1: {

					// 盾持ち敵の生成
					ShieldEnemy* newShieldEnemy = new ShieldEnemy();

					// 座標を指定して盾持ち敵を初期化
					KamataEngine::Vector3 shieldEnemyPosition = mapChipField_->GetMapChipPositionByIndex(j, i);

					newShieldEnemy->Initialize(modelShieldEnemy_, &camera_, shieldEnemyPosition);

					// 盾持ち敵にゲームシーンのポインタをセット
					newShieldEnemy->SetGameScene(this);

					// 盾持ち敵のリストに追加
					shieldEnemies_.push_back(newShieldEnemy);

					break;
				}

				default:
					break;
				}

				break;
			}

			case MapChipType::kBlank:
			default:
				break;
			}
		}
	}
}

void GameScene::Update() {

#ifdef _DEBUG
	// リロードボタン
	if (ImGui::Button("Reload")) {

		// リロード要求フラグを立てる
		reloadRequested_ = true;
	}
#endif

	switch (phase_) {

	case Phase::kFadeIn:

		// フェードの更新
		fade_->Update();

		// フェードイン終了
		if (fade_->IsFinished()) {

			// フェードを停止
			fade_->Stop();

			// ゲームプレイフェーズへ移行
			phase_ = Phase::kPlay;
		}

		break;

	case Phase::kPlay:

		// ゲームプレイフェーズの処理
		UpdatePlayPhase();
		break;

	case Phase::kDeath:

		// デス演出フェーズの処理
		UpdateDeathPhase();
		break;

	case Phase::kFadeOut:

		// フェードの更新
		fade_->Update();

		// フェードアウト終了
		if (fade_->IsFinished()) {

			// ゲームシーンを終了
			finished_ = true;
		}

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

	if (skydome_) {
		skydome_->Update();
	}

	if (player_) {
		player_->Update();
	}

	for (Enemy* enemy : enemies_) {

		if (enemy) {
			enemy->Update();
		}
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {

		if (shieldEnemy) {
			shieldEnemy->Update();
		}
	}

	// ヒットエフェクトの更新
	for (HitEffect* hitEffect : hitEffects_) {

		if (hitEffect) {
			hitEffect->Update();
		}
	}

	// ガードエフェクトの更新
	for (GuardEffect* guardEffect : guardEffects_) {

		if (guardEffect) {
			guardEffect->Update();
		}
	}

	// デス状態になったガードエフェクトを削除
	guardEffects_.remove_if([](GuardEffect* guardEffect) {
		if (guardEffect == nullptr) {
			return true;
		}

		if (guardEffect->IsDead()) {

			delete guardEffect;
			return true;
		}

		return false;
	});

	for (std::vector<KamataEngine::WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}

			UpdateWorldTransform(*worldTransformBlock);
		}
	}

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

	// 全ての当たり判定
	CheckAllCollisions();

	// デスフラグの立った敵を削除
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {

			delete enemy;
			return true;
		}

		return false;
	});

	// デスフラグの立った盾持ち敵を削除
	shieldEnemies_.remove_if([](ShieldEnemy* shieldEnemy) {
		if (shieldEnemy->IsDead()) {

			delete shieldEnemy;
			return true;
		}

		return false;
	});
}

void GameScene::UpdateDeathPhase() {

	if (skydome_) {
		skydome_->Update();
	}

	for (Enemy* enemy : enemies_) {

		if (enemy) {
			enemy->Update();
		}
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {

		if (shieldEnemy) {
			shieldEnemy->Update();
		}
	}

	if (deathParticles_) {
		deathParticles_->Update();
	}

	if (deathParticles_ && deathParticles_->IsFinished()) {

		// フェードアウト開始
		fade_->Start(Fade::Status::FadeOut, kFadeDuration);

		// フェードアウトフェーズへ移行
		phase_ = Phase::kFadeOut;
	}

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

	case Phase::kFadeIn:
		break;

	case Phase::kPlay:

		if (player_ && player_->IsDead()) {

			// デス演出フェーズへ切り替え
			phase_ = Phase::kDeath;

			// 自キャラの座標を取得
			const KamataEngine::Vector3 deathParticlesPosition = player_->GetWorldPosition();

			// デスパーティクルを生成
			deathParticles_ = new DeathParticles();

			// 自キャラの位置へ発生
			deathParticles_->Initialize(modelDeathParticle_, &camera_, deathParticlesPosition);
		}

		break;

	case Phase::kDeath:
		break;

	case Phase::kFadeOut:
		break;
	}
}

void GameScene::CheckAllCollisions() {

	AABB aabb1;
	AABB aabb2;

	aabb1 = player_->GetAABB();

	for (Enemy* enemy : enemies_) {

		if (!enemy) {
			continue;
		}

		// コリジョン無効の敵はスキップ
		if (enemy->IsCollisionDisabled()) {
			continue;
		}

		aabb2 = enemy->GetAABB();

		if (IsCollision(aabb1, aabb2)) {

			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {

		if (!shieldEnemy) {
			continue;
		}

		// コリジョン無効の盾持ち敵はスキップ
		if (shieldEnemy->IsCollisionDisabled()) {
			continue;
		}

		aabb2 = shieldEnemy->GetAABB();

		if (IsCollision(aabb1, aabb2)) {

			player_->OnCollision(static_cast<const Enemy*>(nullptr));
			shieldEnemy->OnCollision(player_);
		}
	}
}

void GameScene::Draw() {

	if (skydome_) {
		skydome_->Draw();
	}

	// ゲームプレイ中だけ自キャラを描画
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

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {

		if (shieldEnemy) {
			shieldEnemy->Draw();
		}
	}

	// ヒットエフェクトの描画
	for (HitEffect* hitEffect : hitEffects_) {

		if (hitEffect) {
			hitEffect->Draw();
		}
	}

	// ガードエフェクトの描画
	for (GuardEffect* guardEffect : guardEffects_) {

		if (guardEffect) {
			guardEffect->Draw();
		}
	}

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

	// 最前面にフェードを描画
	fade_->Draw();
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

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		delete shieldEnemy;
	}

	shieldEnemies_.clear();

	// ヒットエフェクトの解放
	for (HitEffect* hitEffect : hitEffects_) {
		delete hitEffect;
	}

	hitEffects_.clear();

	// ガードエフェクトの解放
	for (GuardEffect* guardEffect : guardEffects_) {
		delete guardEffect;
	}

	guardEffects_.clear();

	delete deathParticles_;

	delete mapChipField_;
	delete skydome_;
	delete modelSkydome_;
	delete cameraController_;
	delete debugCamera_;
	delete modelEnemy_;
	delete modelShieldEnemy_;
	delete modelAttack_;
	delete modelHitEffect_;
	delete modelGuardEffect_;
	delete modelDeathParticle_;
	delete player_;
	delete model_;
	delete modelBlock_;

	// フェードを解放
	delete fade_;
}