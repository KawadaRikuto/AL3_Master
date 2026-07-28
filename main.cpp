#include <Windows.h>

#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"

#ifdef _DEBUG
#include "2d/ImGuiManager.h"
#endif

// ゲームシーン
GameScene* gameScene = nullptr;

// タイトルシーン
TitleScene* titleScene = nullptr;

// シーン
enum class Scene {

	kUnknown = 0,

	kTitle,
	kGame,
};

// 現在シーン
Scene scene = Scene::kUnknown;

/// <summary>
/// シーン切り替え
/// </summary>
void ChangeScene() {

	switch (scene) {

	case Scene::kTitle:

		// タイトルシーンが終了した
		if (titleScene->IsFinished()) {

			// シーンをゲームに変更
			scene = Scene::kGame;

			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;

			// 新シーンの生成と初期化
			gameScene = new GameScene();
			gameScene->Initialize();
		}

		break;

	case Scene::kGame:

		// ゲームシーンが終了した
		if (gameScene->IsFinished()) {

			// シーンをタイトルに変更
			scene = Scene::kTitle;

			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;

			// 新シーンの生成と初期化
			titleScene = new TitleScene();
			titleScene->Initialize();
		}

		break;

	case Scene::kUnknown:
		break;
	}
}

/// <summary>
/// 現在シーンの更新
/// </summary>
void UpdateScene() {

	switch (scene) {

	case Scene::kTitle:

		titleScene->Update();

		break;

	case Scene::kGame:

		gameScene->Update();

		break;

	case Scene::kUnknown:
		break;
	}
}

/// <summary>
/// 現在シーンの描画
/// </summary>
void DrawScene() {

	switch (scene) {

	case Scene::kTitle:

		titleScene->Draw();

		break;

	case Scene::kGame:

		gameScene->Draw();

		break;

	case Scene::kUnknown:
		break;
	}
}

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LE2B_07_カワダ_リクト");

	// 名前空間の使用
	using namespace KamataEngine;

	// 画面描画
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

#ifdef _DEBUG
	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
#endif

	// 最初のシーンの初期化
	scene = Scene::kTitle;

	titleScene = new TitleScene();
	titleScene->Initialize();

	// メインループ
	while (true) {

		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

#ifdef _DEBUG
		// ImGui受付開始
		imguiManager->Begin();
#endif

		// シーン切り替え
		ChangeScene();

		// 現在シーンの更新
		UpdateScene();

#ifdef _DEBUG
		// ImGui受付終了
		imguiManager->End();
#endif

		// 描画前処理
		dxCommon->PreDraw();

		// 3Dモデル描画前処理
		Model::PreDraw();

		// 現在シーンの描画
		DrawScene();

		// 3Dモデル描画後処理
		Model::PostDraw();

#ifdef _DEBUG
		// ImGui描画
		imguiManager->Draw();
#endif

		// 描画後処理
		dxCommon->PostDraw();
	}

	// シーンの解放
	delete titleScene;
	delete gameScene;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}