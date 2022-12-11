#pragma once
#include <vector>

#include "Tile.h"

/// <summary>
/// 两张手牌间关系
/// </summary>
/// <param name="hands">手牌</param>
/// <param name="num">前张牌序号</param>
static int GetRelation(const TileList& hands, const int num)
{
    if (num + 2 > static_cast<int>(hands.size()))
        // （尽量大的数）
        return INT32_MAX;
    return hands[num + 1].Value - hands[num].Value;
}

/// <summary>
/// 摸牌
/// </summary>
/// <param name="hands">手牌</param>
/// <param name="tile">进张</param>
/// <returns>插入牌的位置</returns>
static int TileIn(TileList& hands, const Tile& tile)
{
    auto ru = 0;
    // 找到进张插入的位置
    while (ru < static_cast<int>(hands.size()) && tile.Value > hands[ru].Value)
        ++ru;
    hands.emplace(hands.begin() + ru, tile);
    return ru;
}
