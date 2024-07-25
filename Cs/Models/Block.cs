using System.Collections.Generic;
using System.Linq;
using Cs.Helpers;
using Cs.Interfaces;

namespace Cs.Models;

/// <summary>
/// 块（其中牌关系只有相同或连续）
/// </summary>
public record Block : ISequential
{
    /// <summary>
    /// 块内首张牌的序号
    /// </summary>
    public int Loc { get; }

    /// <summary>
    /// 块内牌数
    /// </summary>
    public int Len { get; }

    /// <summary>
    /// 完整性
    /// </summary>
    public IntegrityType Integrity { get; private set; }

    /// <summary>
    /// 块内末张牌的序号
    /// </summary>
    public int LastLoc => Loc + Len - 1;

    /// <summary>
    /// 完整类型
    /// </summary>
    public enum IntegrityType
    {
        /// <summary>
        /// 完整型
        /// </summary>
        /// <remarks>3n</remarks>
        Type0,

        /// <summary>
        /// 雀面不完整型或半不完整型
        /// </summary>
        /// <remarks>3n+1</remarks>
        Type1,

        /// <summary>
        /// 雀头不完整型或面子不完整型
        /// </summary>
        /// <remarks>3n+2</remarks>
        Type2,

        /// <summary>
        /// 雀半不完整型
        /// </summary>
        /// <remarks>3n</remarks>
        TypeEx
    }

    /// <summary>
    /// </summary>
    /// <param name="loc"></param>
    /// <param name="len">-1为<see cref="IntegrityType.TypeEx"/></param>
    public Block(int loc, int len = -1)
    {
        Loc = loc;
        Len = len;
        Integrity = len is -1 ? IntegrityType.TypeEx : (IntegrityType)(len % 3);
    }

    /// <summary>
    /// 每张牌的类型
    /// </summary>
    private enum TileType
    {
        /// <summary>
        /// 顺
        /// </summary>
        Sequence,

        /// <summary>
        /// 刻或雀头
        /// </summary>
        Triplet
    };

    /// <summary>
    /// 筛选完整型Lv.2
    /// </summary>
    /// <param name="hands">判断的牌组</param>
    /// <param name="eyesLoc">雀头的序号（-1为没有雀头）</param>
    /// <remarks><paramref name="eyesLoc"/>为-1时，<paramref name="hands"/>长度应是3n，否则是3n+2</remarks>
    public bool IntegrityJudge(List<Tile> hands, int eyesLoc = -1)
    {
        var groups = GetGroups(hands).ToArray();

        // 在此处没用，但在和牌算符时会用到
        var blockTiles = new TileType[Len];
        // 若有雀头，则将雀头认为是刻
        if (eyesLoc is not -1)
        {
            ++groups[eyesLoc].Confirmed;
            ++groups[eyesLoc].Confirmed;
            blockTiles[groups[eyesLoc].Loc - Loc] = TileType.Triplet;
            blockTiles[groups[eyesLoc].Loc - Loc + 1] = TileType.Triplet;
        }
        // 每次循环记录一个组
        for (var i = 0; i < groups.Length; ++i)
        {
            // 该组牌数
            switch (groups[i].Len - groups[i].Confirmed)
            {
                // 刚好全部确定
                case 0:
                    continue;
                // 都是顺，确定后面2组分别有1张是顺
                case 1:
                    if (groups.Length > i + 2)
                    {
                        ++groups[i + 1].Confirmed;
                        ++groups[i + 2].Confirmed;
                        continue;
                    }
                    break;
                // 都是顺，确定后面2组分别有2张是顺
                case 2:
                    if (groups.Length > i + 2)
                    {
                        ++groups[i + 1].Confirmed;
                        ++groups[i + 1].Confirmed;
                        ++groups[i + 2].Confirmed;
                        ++groups[i + 2].Confirmed;
                        continue;
                    }
                    break;
                // 3刻1顺，确定后面2组分别有1张是顺
                case 4:
                    if (groups.Length > i + 2)
                    {
                        ++groups[i + 1].Confirmed;
                        ++groups[i + 2].Confirmed;
                        blockTiles[groups[i].Loc - Loc] = TileType.Triplet;
                        blockTiles[groups[i].Loc - Loc + 1] = TileType.Triplet;
                        blockTiles[groups[i].Loc - Loc + 2] = TileType.Triplet;
                        continue;
                    }
                    break;
                // 3张是刻
                case 3:
                    blockTiles[groups[i].Loc - Loc] = TileType.Triplet;
                    blockTiles[groups[i].Loc - Loc + 1] = TileType.Triplet;
                    blockTiles[groups[i].Loc - Loc + 2] = TileType.Triplet;
                    continue;
                // 可能是负数
                default:
                    break;
            }
            Integrity = eyesLoc is -1 ? IntegrityType.TypeEx : IntegrityType.Type2;
            return false;
        }
        return true;
    }

    /// <summary>
    /// 获得分组
    /// </summary>
    /// <param name="hands"></param>
    /// <returns></returns>
    private IEnumerable<Group> GetGroups(IList<Tile> hands)
    {
        var tempLoc = Loc;
        // 判断块内每个关系
        for (var i = Loc; i < Loc + Len - 1; ++i)
            // 当关系是连续，则记录多一个组
            if (hands.GetRelation(i) is 1)
            {
                yield return new(tempLoc, i + 1 - tempLoc);
                tempLoc = i + 1;
            }
        yield return new(tempLoc, Loc + Len - tempLoc);
    }

    /// <summary>
    /// 遍历
    /// </summary>
    /// <param name="hands">判断的牌组（本块所在的牌组）</param>
    /// <param name="mode">是否要去对（真为雀面不完整型，假为面子不完整型）</param>
    /// <returns>听的牌，可能本来它就不为空，不过在这里不影响（将来算符时可能改动）</returns>
    public IEnumerable<Tile> Traversal(List<Tile> hands, bool mode)
    {
        // 可能的首张牌
        var first = hands[Loc].Val - 1;
        // 如果首张是一万、筒、索或字牌，则first没有前一张
        if (hands[Loc].IntVal % 16 is 0 || hands[Loc].IntVal / 8 > 5)
            ++first;
        // 可能的末张牌
        var last = hands[Loc + Len - 1].Val + 1;
        // 如果末张是九万、筒、索或字牌，则得last没有后一张
        if (hands[Loc + Len - 1].IntVal % 16 is 8 || hands[Loc + Len - 1].IntVal / 8 > 5)
            --last;
        var tempBlock = new Block(0, Len + 1);
        var tempTile = first;
        // 每张牌都插入尝试一次（遍历）
        for (var i = 0; i < last - first + 1; ++i, ++tempTile)
        {
            // 重新复制所有牌
            var tempHands = hands.GetRange(Loc, Len);
            // 插入尝试的牌
            _ = tempHands.TileIn(new(tempTile));
            if (mode ?
                // 雀面不完整型且遍历、去对后完整，则听牌
                tempBlock.IgnoreEyesJudge(tempHands)
                // 面子不完整型且遍历后完整，则听牌
                : tempBlock.IntegrityJudge(tempHands))
                yield return new(tempTile);
        }
    }

    /// <summary>
    /// 去对后完整（雀头完整型）
    /// </summary>
    /// <param name="hands">判断的牌组</param>
    /// <returns>是否完整</returns>
    public bool IgnoreEyesJudge(List<Tile> hands)
    {
        for (int i = Loc, tempGroupNum = 0; i < Loc + Len - 1; ++i)
        {
            // 当关系是连续，则组数加一
            if (hands.GetRelation(i) is 1)
                ++tempGroupNum;
            // 当关系是相同，若是雀头完整型，则听牌
            else if (IntegrityJudge(hands, tempGroupNum))
                return true;
        }
        return false;
    }
}
