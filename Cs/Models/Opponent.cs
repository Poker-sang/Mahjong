using System;
using System.Collections.Generic;
using System.Linq;
using Cs.Helpers;

namespace Cs.Models;

public class Opponent
{
    /// <summary>
    /// 手牌区
    /// </summary>
    private List<Tile> Hands { get; } = new(14);

    /// <summary>
    /// 副露区
    /// </summary>
    private List<Meld> Melds { get; } = new(4);

    /// <summary>
    /// 牌河（舍张）
    /// </summary>
    private List<Tile> Discards { get; } = new(30);

    /// <summary>
    /// 打牌
    /// </summary>
    /// <param name="index">打的牌的序号</param>
    public Tile TileOut(int index)
    {
        // 放入牌河
        Discards.Add(Hands[index]);
        // 删除元素
        Hands.RemoveAt(index);
        return Hands[index];
    }

    /// <summary>
    /// 摸牌
    /// </summary>
    /// <param name="tile">进张</param>
    /// <returns>插入牌的位置</returns>
    public int TileIn(Tile tile)
    {
        var index = 0;
        // 找到进张插入的位置
        while (index < Hands.Count && tile.Val > Hands[index].Val)
            ++index;
        // 插入牌
        Hands.Insert(index, tile);
        return index;
    }

    /// <summary>
    /// 听牌判断（在摸牌前判断）
    /// </summary>
    /// <returns>听的牌</returns>
    public IEnumerable<Tile> ReadyHandJudge()
    {
        var readyHands = new List<Tile>();
        var sevenPairsFlag = false;

        // 如果没有副露（特殊牌型判断）
        if (Melds.Count is 0)
        {
            if (ThirteenOrphansJudge().ToArray() is { Length: not 0 } readyHandsList)
                return readyHandsList;

            if (SevenPairsJudge() is { } tile)
            {
                readyHands.Add(tile);
                sevenPairsFlag = true;
            }
            // 有可能复合二杯口，故听牌后不退出（会进入case 1或2）
        }

        var errBlocks = GetBlocks(out var blocks);

        // 不完整型块数
        switch (errBlocks.Count)
        {
            // 有一块不完整型（一块雀面不完整型（3n+1））
            // 二杯口缺雀头会在这里出现
            case 1:
            {
                // 将此不完整型遍历
                readyHands.AddRange(errBlocks[0].Traversal(Hands, true));
                var index = blocks.IndexOf(errBlocks[0]);
                // 与前块连接
                if (index is not 0)
                {
                    var joint = JointBlocks(blocks[index - 1], blocks[index]);
                    // 如果该牌组完整，则记听一面
                    if (joint?.JointedBlock.IgnoreEyesJudge(joint.Value.JointedHands) is true)
                        readyHands.Add(joint.Value.MiddleTile);
                }
                // 与后块连接
                if (index != blocks.Count - 1)
                {
                    var joint = JointBlocks(blocks[index], blocks[index + 1]);
                    // 如果该牌组是雀头完整型，则记听一面
                    if (joint?.JointedBlock.IgnoreEyesJudge(joint.Value.JointedHands) is true)
                        readyHands.Add(joint.Value.MiddleTile);
                }

                break;
            }
            // 有两块不完整型（一块面子不完整型（3n+2），一块雀头完整型（3n+2））
            // 二杯口缺面子会在这里出现
            case 2:
            {
                if (errBlocks[1].IgnoreEyesJudge(Hands))
                    readyHands.AddRange(errBlocks[0].Traversal(Hands, false));
                if (errBlocks[0].IgnoreEyesJudge(Hands))
                    readyHands.AddRange(errBlocks[1].Traversal(Hands, false));
                break;
            }
            // 有三块不完整型（两块半不完整型（3n+1），一块雀头完整型（3n+2））
            case 3:
            {
                // 如果3n+2的不完整型夹在中间或不是雀头完整型，则无听
                var eyesIndex = errBlocks
                    .FindIndex(eyesBlock => eyesBlock.Integrity is Block.IntegrityType.Type2);
                if (eyesIndex is 1 || !errBlocks[eyesIndex].IgnoreEyesJudge(Hands))
                    break;

                var joint = eyesIndex is 0
                    ? JointBlocks(errBlocks[1], errBlocks[2])
                    : JointBlocks(errBlocks[0], errBlocks[1]);
                if (joint is null)
                    break;
                // 如果该牌组完整，则记听一面
                if (joint.Value.JointedBlock.IntegrityJudge(joint.Value.JointedHands))
                    readyHands.Add(joint.Value.MiddleTile);
                break;
            }
            // 有两块不完整型（一块雀半完整型（3n），一块半不完整型（3n+1））
            case 4:
            {
                var joint = errBlocks[0].Loc < errBlocks[1].Loc ?
                    JointBlocks(errBlocks[0], errBlocks[1]) :
                    JointBlocks(errBlocks[1], errBlocks[0]);

                if (joint is null)
                    break;
                // 如果该牌组是雀头完整型，则记听一面
                if (joint.Value.JointedBlock.IgnoreEyesJudge(joint.Value.JointedHands))
                    readyHands.Add(joint.Value.MiddleTile);

                break;
            }
        }

        // 如果是七对子，且听多张牌，则为二杯口，删除七对子的听牌，否则会重复
        if (sevenPairsFlag && readyHands.Count > 1)
            readyHands.RemoveAt(0);
        return readyHands;
    }

    /// <summary>
    /// 连接两块
    /// </summary>
    /// <param name="frontBlock">前块</param>
    /// <param name="followBlock">后块</param>
    /// <returns>连接后的牌组、连接后的块、用来连接的牌</returns>
    private (List<Tile> JointedHands, Block JointedBlock, Tile MiddleTile)? JointBlocks(Block frontBlock, Block followBlock)
    {
        // 判断连接的两块是否连续
        if (followBlock.Loc - frontBlock.LastLoc is not 1)
            return null;
        // 如果原来这两张牌中间不是隔一张，则无听
        if (Hands.GetRelation(frontBlock.LastLoc) is not 2)
            return null;
        // 临时记录中间隔的牌（可能是铳牌）
        var tempReadyHands = new Tile(Hands[frontBlock.LastLoc].Val + 1);
        // 临时用来判断的牌组
        var jointedHands = new List<Tile>();
        // 这两块不完整型总张数
        var jointedBlock = new Block(0, frontBlock.Len + 1 + followBlock.Len);
        // 复制该不完整型所有牌
        jointedHands.AddRange(Hands.GetRange(frontBlock.Loc, jointedBlock.Len - 1));
        // 插入一张中间隔的牌
        jointedHands.Insert(frontBlock.Len, tempReadyHands);
        return (jointedHands, jointedBlock, tempReadyHands);
    }

    /// <summary>
    /// 七对牌型判断
    /// </summary>
    /// <returns>听的牌</returns>
    private Tile? SevenPairsJudge()
    {
        // 多出来的单张
        var single = false;
        // 该单张牌位置
        var singleTile = 0;
        // 判断相同或连续的关系
        for (var i = 0; i < 12; ++i)
            // 如果偶数位关系对应不是相同，或奇数位不是其他关系（出现单张）
            if (((i + (single ? 1 : 0)) % 2 ^ (Hands.GetRelation(i) > 0 ? 1 : 0)) > 0)
            {
                // 直接异或运算无法排除龙七对
                // 如果这个错误关系是相同，则是龙七对；如果之前已经有单牌了，则不是七对子
                if (Hands.GetRelation(i) is 0 || single)
                    return null;

                single = true;
                singleTile = Hands[i].Val;
            }

        // 如果没查到单张
        if (!single)
            // 那单张就是最后一个
            singleTile = Hands[12].Val;
        // 记听一面
        return new(singleTile);
    }

    /// <summary>
    /// 国士牌型判断
    /// </summary>
    /// <returns>听牌</returns>
    private IEnumerable<Tile> ThirteenOrphansJudge()
    {
        // 是否缺了某张幺九牌（0或1）
        var isMissing = false;
        // 是否多了某张幺九牌（0或1）
        var isRedundant = false;
        // 缺的幺九牌
        var missingTile = 0;
        // 判断十三张幺九牌的拥有情况
        for (var i = 0; i < 13; ++i)
        {
            var offset = (isMissing ? 1 : 0) - (isRedundant ? 1 : 0);
            switch (0)
            {
                // 如果和上张映射幺九牌一样
                case 0 when Hands[i].Val == (i + offset - 1) / 8:
                    // 如果之前已经有一个多的牌
                    if (isRedundant)
                        yield break;
                    // 记录有多牌
                    isRedundant = true;
                    break;
                // 如果和下张映射幺九牌一样
                case 0 when Hands[i].Val == (i + offset + 1) / 8:
                {
                    // 如果之前已经有一个缺牌则不是国士，否则记录缺牌
                    if (isMissing)
                        yield break;
                    isMissing = true;
                    missingTile = i / 8;
                    break;
                }
                // 有不是幺九牌即不符合国士
                case 0 when Hands[i].Val != (i + offset) / 8:
                    yield break;

            }
        }
        // 若有多张，记听一面或记听一面（红中）（因为红中在最后不会被redundancy记录）
        if (isRedundant)
            yield return new(isMissing ? missingTile : 96);
        // 若不缺张则记听十三面
        else
            for (var i = 0; i < 13; ++i)
                yield return new(i / 8);
    }

    /// <summary>
    /// 获取分块
    /// </summary>
    /// <exception cref="ArgumentOutOfRangeException"></exception>
    /// <returns>不完整的块数（最多3个）</returns>
    private List<Block> GetBlocks(out List<Block> blocks)
    {
        var errBlocks = new List<Block>(4);
        blocks = new(6);
        var tempLoc = 0;
        for (var i = 0; i < Hands.Count - 1; ++i)
            // 当关系不是相同或连续
            if (Hands.GetRelation(i) > 1)
            {
                blocks.Add(new(tempLoc, i - tempLoc + 1));
                // 如果类型是不完整则记录
                if (blocks[^1].Integrity is not Block.IntegrityType.Type0)
                    errBlocks.Add(blocks[^1]);
                // 若块序号达到(6 - 副露数)或有4个不完整型则无听
                if (blocks.Count + Melds.Count is 6 || errBlocks.Count is 4)
                    return new();
                // 下一块，括号里是块内首张牌的序号
                tempLoc = i + 1;
            }
        // 最后一块的记录单独实现
        {
            blocks.Add(new(tempLoc, Hands.Count - tempLoc));
            if (blocks[^1].Integrity is not Block.IntegrityType.Type0)
                errBlocks.Add(blocks[^1]);
            if (errBlocks.Count is 4)
                return new();
        }
        // 通过完整型Lv.1的块，筛选完整型Lv.2发现有一块不完整，则为不完整型加半不完整型，多于一块则无听
        foreach (var block in blocks.Where(block => block.Integrity is Block.IntegrityType.Type0
                                                    && !block.IntegrityJudge(Hands)))
            if (errBlocks.Count is not 4)
            {
                errBlocks.Add(block);
                // 特殊标记
                errBlocks.Add(new(0));
                errBlocks.Add(new(0));
            }
            else
                return new();
        return errBlocks;
    }
}
