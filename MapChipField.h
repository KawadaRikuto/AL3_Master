#pragma once

#include <cstdint>
#include <string>
#include <vector>

/// <summary>
/// マップチップの種類
/// </summary>
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

/// <summary>
/// マップチップデータ
/// </summary>
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField {

public:
	/// <summary>
	/// マップチップデータをリセットする
	/// </summary>
	void ResetMapChipData();

	/// <summary>
	/// CSVファイルからマップチップデータを読み込む
	/// </summary>
	void LoadMapChipCsv(const std::string& filePath);

	/// <summary>
	/// 指定したマスのマップチップを取得
	/// </summary>
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVertical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	// マップチップデータ
	MapChipData mapChipData_;
};