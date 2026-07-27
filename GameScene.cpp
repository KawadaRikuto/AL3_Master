#include "GameScene.h"

#include <cmath>

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

	// マップチップフィールドの生成
	mapChipField_ = new MapChipField();

	// CSVファイルからマップチップデータを読み込む
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// 自キャラ用3Dモデルデータの生成
	model_ = KamataEngine::Model::CreateFromOBJ("player", true);

	// ブロック用3Dモデルデータの生成
	modelBlock_ = KamataEngine::Model::CreateFromOBJ("block", true);

	// 天球用3Dモデルデータの生成
	modelSkydome_ = KamataEngine::Model::CreateFromOBJ("skydome", true);

	// カメラのfarZを変更
	camera_.farZ = 1000.0f;

	// カメラの初期化
	camera_.Initialize();

	// デバッグカメラの生成
	debugCamera_ = new KamataEngine::DebugCamera(KamataEngine::WinApp::kWindowWidth, KamataEngine::WinApp::kWindowHeight);

	debugCamera_->SetFarZ(1000.0f);

	// 自キャラの生成
	player_ = new Player();

	// 座標をマップチップ番号で指定
	KamataEngine::Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);

	// 自キャラの初期化
	player_->Initialize(model_, &camera_, playerPosition);

	// マップチップデータをセット
	player_->SetMapChipField(mapChipField_);

	// カメラコントローラの生成
	cameraController_ = new CameraController();

	// カメラコントローラの初期化
	cameraController_->Initialize();

	// 移動範囲の指定
	CameraController::Rect cameraArea = {
	    0.0f,
	    100.0f,
	    0.0f,
	    20.0f,
	};

	cameraController_->SetMovableArea(cameraArea);

	// 追従対象をセット
	cameraController_->SetTarget(player_);

	// リセット
	cameraController_->Reset();

	camera_.matView = cameraController_->GetCamera().matView;

	camera_.matProjection = cameraController_->GetCamera().matProjection;

	camera_.TransferMatrix();

	// 天球の生成
	skydome_ = new Skydome();

	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// ブロックの生成
	GenerateBlocks();
}

void GameScene::GenerateBlocks() {

	// 縦方向の要素数を設定
	worldTransformBlocks_.resize(MapChipField::kNumBlockVertical);

	for (uint32_t i = 0; i < MapChipField::kNumBlockVertical; ++i) {

		// 横方向の要素数を設定
		worldTransformBlocks_[i].resize(MapChipField::kNumBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < MapChipField::kNumBlockVertical; ++i) {

		for (uint32_t j = 0; j < MapChipField::kNumBlockHorizontal; ++j) {

			// 空白なら生成しない
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlank) {

				continue;
			}

			worldTransformBlocks_[i][j] = new KamataEngine::WorldTransform();

			worldTransformBlocks_[i][j]->Initialize();

			// マップチップ番号から座標を取得
			KamataEngine::Vector3 blockPosition = mapChipField_->GetMapChipPositionByIndex(j, i);

			// ブロック座標を設定
			worldTransformBlocks_[i][j]->translation_ = blockPosition;
		}
	}
}

void GameScene::Update() {

#ifdef _DEBUG

	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_TAB)) {

		isDebugCameraActive_ = !isDebugCameraActive_;
	}

#endif

	// 自キャラの更新
	player_->Update();

	// 天球の更新
	skydome_->Update();

	// ブロックの更新
	for (std::vector<KamataEngine::WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}

			UpdateWorldTransform(*worldTransformBlock);
		}
	}

	// カメラの処理
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
}

void GameScene::Draw() {

	// 天球の描画
	skydome_->Draw();

	// 自キャラの描画
	player_->Draw();

	// ブロックの描画
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

	delete mapChipField_;
	delete skydome_;
	delete modelSkydome_;
	delete cameraController_;
	delete debugCamera_;
	delete player_;
	delete model_;
	delete modelBlock_;
}