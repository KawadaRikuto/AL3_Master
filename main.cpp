#include <Windows.h>

#include "GameScene.h"
#include "KamataEngine.h"

#ifdef _DEBUG
#include "2d/ImGuiManager.h"
#endif

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

	// ゲームシーンの生成
	GameScene* gameScene = new GameScene();

	// ゲームシーンの初期化
	gameScene->Initialize();

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

		// ゲームシーンの更新
		gameScene->Update();

#ifdef _DEBUG
		// ImGui受付終了
		imguiManager->End();
#endif

		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		gameScene->Draw();

		// 軸表示の描画
		AxisIndicator::GetInstance()->Draw();

#ifdef _DEBUG
		// ImGuiの描画
		imguiManager->Draw();
#endif

		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;
	gameScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}