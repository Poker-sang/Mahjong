#pragma once
#include "Opponent.h"
namespace MahjongHelper {
using namespace System::Collections::Generic;
generic<typename T> T Last(List<T> ^ list) { return list[list->Count - 1]; }
/// <summary>
/// 两张手牌间关系
/// </summary>
/// <param name="hands">手牌</param>
/// <param name="num">前张牌序号</param>
static int GetRelation(List<Tile ^> ^ hands, const int num)
{
    try {
        return hands[num + 1]->Val - hands[num]->Val;
    } catch (System::Exception ^) {
        //（尽量大的数）
        return System::Int32::MaxValue;
    }
}

/// <summary>
/// 摸牌
/// </summary>
/// <param name="hands">手牌</param>
/// <param name="tile">进张</param>
/// <returns>插入牌的位置</returns>
static int TileIn(List<Tile ^> ^ hands, Tile ^ tile)
{
    auto ru = 0;
    //找到进张插入的位置
    while (ru < hands->Count && tile->Val > hands[ru]->Val)
        ++ru;
    hands->Insert(ru, tile);
    return ru;
}
}