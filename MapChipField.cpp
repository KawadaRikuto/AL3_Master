#include "MapChipField.h"

#include <cassert>
#include <fstream>
#include <map>
#include <sstream>

namespace {

/// <summary>
/// マップチップ種別テーブル
/// </summary>
std::map<char, MapChipType> mapChipTypeTable = {
    {'B', MapChipType::kBlock },
    {'P', MapChipType::kPlayer},
    {'E', MapChipType::kEnemy },
};

} // namespace

void MapChipField::ResetMapChipData() {

	// マップチップデータをリセット
	mapChipData_.data.clear();

	// 縦方向の要素数を設定
	mapChipData_.data.resize(kNumBlockVertical);

	// 横方向の要素数を設定
	for (std::vector<MapChipDataUnit>& mapChipDataLine : mapChipData_.data) {

		mapChipDataLine.resize(kNumBlockHorizontal);

		// 全てのマスを空白で初期化
		for (MapChipDataUnit& mapChipDataUnit : mapChipDataLine) {

			mapChipDataUnit.type = MapChipType::kBlank;
			mapChipDataUnit.subID = 0;
		}
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {

	// マップチップデータをリセット
	ResetMapChipData();

	// ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	// マップチップCSV
	std::stringstream mapChipCsv;

	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();

	// ファイルを閉じる
	file.close();

	// CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {

		std::string line;
		std::getline(mapChipCsv, line);

		// 1行分の文字列をストリームに変換
		std::istringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {

			std::string word;
			std::getline(lineStream, word, ',');

			// 空白の単語はスキップ
			if (word.empty()) {
				continue;
			}

			// 先頭文字がマップチップ種別に該当するか確認
			if (!mapChipTypeTable.contains(word[kChipType])) {
				continue;
			}

			// 先頭文字でマップチップ種別を判別
			mapChipData_.data[i][j].type = mapChipTypeTable[word[kChipType]];

			// サブIDを表す文字が無い場合は0番で固定
			if (word.size() <= kChipSubID) {
				continue;
			}

			// マップチップのサブIDを設定
			mapChipData_.data[i][j].subID = static_cast<uint8_t>(word[kChipSubID] - '0');
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) { return mapChipData_.data[yIndex][xIndex].type; }

uint8_t MapChipField::GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex) { return mapChipData_.data[yIndex][xIndex].subID; }

KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {

	KamataEngine::Vector3 position = {};

	position.x = kBlockWidth * xIndex;

	position.y = kBlockHeight * (kNumBlockVertical - 1 - yIndex);

	position.z = 0.0f;

	return position;
}

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position) {

	IndexSet indexSet = {};

	// X番号の計算
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2.0f) / kBlockWidth);

	// 反転前のY番号
	uint32_t reversedYIndex = static_cast<uint32_t>((position.y + kBlockHeight / 2.0f) / kBlockHeight);

	// Y番号を反転
	indexSet.yIndex = kNumBlockVertical - 1 - reversedYIndex;

	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {

	// 指定ブロックの中心座標を取得する
	KamataEngine::Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect;

	rect.left = center.x - kBlockWidth / 2.0f;

	rect.right = center.x + kBlockWidth / 2.0f;

	rect.bottom = center.y - kBlockHeight / 2.0f;

	rect.top = center.y + kBlockHeight / 2.0f;

	return rect;
}
