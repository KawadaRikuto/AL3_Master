#pragma once

#include "KamataEngine.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene();

	/// <summary>
	/// 終了フラグのgetter
	/// </summary>
	bool IsFinished() const { return finished_; }

private:
	// タイトル文字の3Dモデル
	KamataEngine::Model* modelTitle_ = nullptr;

	// タイトル文字のワールドトランスフォーム
	KamataEngine::WorldTransform worldTransformTitle_;

	// カメラ
	KamataEngine::Camera camera_;

	// アニメーション用の時間
	float animationTimer_ = 0.0f;

	// 終了フラグ
	bool finished_ = false;
};