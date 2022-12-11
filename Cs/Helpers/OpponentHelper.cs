using System.Collections.Generic;
using Cs.Models;

namespace Cs.Helpers;

public static class OpponentHelper
{
    /// <summary>
    /// 两张手牌间关系
    /// </summary>
    /// <param name="hands">手牌</param>
    /// <param name="num">前张牌序号</param>
    public static int GetRelation(this IList<Tile> hands, int num)
    {
        if (num > hands.Count - 2)
            //（尽量大的数）
            return int.MaxValue;
        return hands[num + 1].Val - hands[num].Val;
    }

    /// <summary>
    /// 摸牌
    /// </summary>
    /// <param name="hands">手牌</param>
    /// <param name="tile">进张</param>
    /// <returns>插入牌的位置</returns>
    public static int TileIn(this IList<Tile> hands, Tile tile)
    {
        var index = 0;
        // 找到进张插入的位置
        while (index < hands.Count && tile.Val > hands[index].Val)
            ++index;
        hands.Insert(index, tile);
        return index;
    }
}
