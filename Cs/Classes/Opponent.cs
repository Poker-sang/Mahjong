using System.Collections.Generic;
using System.Linq;

namespace Cs.Classes
{
    public class Opponent
    {
        /// <summary>
        /// 手牌区
        /// </summary>
        public List<Tile> Hands { get; } = new(14);

        /// <summary>
        /// 副露区
        /// </summary>
        public List<Meld> Melds { get; } = new(4);

        /// <summary>
        /// 牌河（舍张）
        /// </summary>
        public List<Tile> Discards { get; } = new(30);

        /// <summary>
        /// 可以和的牌
        /// </summary>
        public List<Tile> ReadyHands { get; } = new(13);

        /// <summary>
        /// 是否听牌
        /// </summary>
        public bool IsReadyHand { get; private set; }

        /// <summary>
        /// 打牌
        /// </summary>
        /// <param name="index">打的牌的序号</param>
        /// <param name="now">now</param>
        public void TileOut(int index, ref Tile now)
        {
            now = Hands[index];
            Discards.Add(Hands[index]); //放入牌河
            Hands.RemoveAt(index); //删除元素
        }

        /// <summary>
        /// 摸牌
        /// </summary>
        /// <param name="card">进张</param>
        /// <returns>插入牌的位置</returns>
        public int TileIn(Tile card)
        {
            var ru = 0;
            while (ru < Hands.Count && card.Val > Hands[ru].Val) //找到进张插入的位置
                ++ru;
            Hands.Insert(ru, card); //插入牌
            return ru;
        }

        /// <summary>
        /// 直接访问牌元素
        /// </summary>
        public Tile this[int index]
        {
            get => Hands[index];
            set => Hands[index] = value;
        }

        /// <summary>
        /// 两张手牌间关系
        /// </summary>
        /// <param name="num">前张牌序号</param>
        public int Link(int num)
        {
            try
            {
                return Hands[num + 1].Val - Hands[num].Val;
            }
            catch (System.Exception)
            {
                return int.MaxValue;
            } //（尽量大的数）
        }
        
        /// <summary>
        /// 听牌判断（在摸牌前判断）
        /// </summary>
        public void ReadyHandJudge()
        {
            IsReadyHand = false;
            ReadyHands.Clear(); //清空铳牌

            if (Melds is { Count: > 0 }) //如果没有副露（特殊牌型判断）
            {
                //国士牌型判断
                var que = false; //是否缺了某张幺九牌（0或1）
                var duo = false; //是否多了某张幺九牌（0或1）
                var queTile = 0; //缺的幺九牌
                for (var i = 0; i < 13; ++i) //判断十三张幺九牌的拥有情况
                    if (Hands[i].Val == (i + (que ? 1 : 0) - (duo ? 1 : 0)) << 3) ; //如果刚好位置映射
                    else if (Hands[i].Val == (i + (que ? 1 : 0) - (duo ? 1 : 0) - 1) << 3) //如果和上张映射幺九牌一样
                    {
                        if (duo) //如果已经有一个多的牌
                            goto ThirteenOrphans;
                        else duo = true; //记录有多牌
                    }
                    else if (Hands[i].Val == (i + (que ? 1 : 0) - (duo ? 1 : 0) + 1) << 3) //如果和下张映射幺九牌一样
                    {
                        if (que) //如果已经有一个缺牌
                            goto ThirteenOrphans; //不是国士
                        else que = true; //记录有缺牌
                        queTile = i << 3; //记录缺的牌
                    }
                    else goto ThirteenOrphans; //有不是幺九即无听

                if (duo) //若有多张，记听一面或记听一面（中）（中在最后不会被que记录）
                    ReadyHands.Add(que ? new Tile(queTile) : new Tile(96));
                else
                    for (var i = 0; i < 13; ++i) //若不缺张
                        ReadyHands.Add(new Tile(i << 3)); //记听十三面
                IsReadyHand = true;
                return; 
                ThirteenOrphans: //七对牌型判断
                var dan = false; //多出来的单张
                var danTile = 0; //单张牌位置
                for (var i = 0; i < 12; ++i) //判断关系
                    if (((i + (duo ? 1 : 0)) % 2 ^ (Link(i) > 0 ? 1 : 0)) > 0) //如果偶数位关系对应不是相同，或奇数位不是其他
                    {
                        //直接异或运算无法排除龙七对
                        if (Link(i) == 0) //如果这个错误关系是相同
                            goto SevenPairs; //龙七对
                        else if (dan) //如果已经有单牌了
                            goto SevenPairs; //不是七对子
                        else
                        {
                            dan = true; //记录单张
                            danTile = Hands[i].Val; //记录序号
                        }
                    }

                if (!dan) //如果没查到单张
                    danTile = Hands[12].Val; //那单张就是最后一个
                ReadyHands.Add(new Tile(danTile)); //记听一面
                IsReadyHand = true;
                //有可能复合二杯口，故听牌后不退出
            }

            SevenPairs:
            var blockErr = 0; //不完整的块数（最多3个）
            var block = new List<Block>(6) { new Block(0) }; //第一块（最多6块）
            for (var i = 0; i < Hands.Count - 1; ++i)
                if (Link(i) > 1) //当关系不是相同或连续
                {
                    block.Last().Len = i - block.Last().Loc + 1; //记录上一块的长度
                    block.Last().Type = block.Last().Len % 3 <= 0; //筛选完整型Lv.1
                    if (!block.Last().Type) //记录
                        ++blockErr;
                    if (block.Count == 6 - Melds.Count || blockErr == 4) //若块序号达到(6 - 副露数)或有4个不完整型
                        return; //无听
                    block.Add(new Block(i + 1)); //括号里是块内首张牌的序号
                }

            block.Last().Len = Hands.Count - block.Last().Loc; //记录上一块的长度
            block.Last().Type = block.Last().Len % 3 <= 0; //筛选完整型Lv.1
            if (!block.Last().Type) //记录
                ++blockErr;
            if (blockErr == 4) //若有4个不完整型
                return; //无听
            if (block.Where(each => each.Type).Any(each => !each.BlockComplete(this)))
                return; //无听
            switch (blockErr)
            {
                case 1: //有一块不完整型（一块全不完整型（3n+4））
                    {
                        foreach (var errBlock in block.Where(errBlock => !errBlock.Type))
                            if (errBlock.BlockTraversal(this, true))
                                break;
                            else return; //无听
                        break;
                    }
                case 2: //有两块不完整型（一块面子不完整型（3n+2），一块雀头完整型（3n+2））
                    {
                        var errBlock1 = 0;
                        for (; errBlock1 < block.Count; ++errBlock1)
                            if (!block[errBlock1].Type) //是不完整型
                                break; //记录
                        var errBlock2 = errBlock1 + 1; //第二个不完整型
                        for (; errBlock2 < block.Count; ++errBlock2)
                            if (!block[errBlock2].Type) //是不完整型
                                break; //记录
                        if (errBlock2 >= block.Count) //如果后者的数字大于等于组数
                            return; //无听
                        if ((block[errBlock1].BlockTraversal(this, false) && block[errBlock2].BlockIgnorePair(this)) //满足任意一个，但是两条语句都要执行
                            | (block[errBlock2].BlockTraversal(this, false) && block[errBlock1].BlockIgnorePair(this))) //按位或不会出现短路
                            break; //听牌
                        else return; //无听
                    }
                case 3: //有三块不完整型（两块半不完整型（3n+1），一块雀头完整型（3n+2））
                    {
                        var errBlock = 0;
                        for (; errBlock < block.Count; ++errBlock)
                            if (!block[errBlock].Type) //是不完整型
                                if (block[errBlock].Len % 3 == 1) //如果牌数是3的倍数+1
                                    break; //记录errLoc
                        var tempReadyHands = new Tile(Hands[block[errBlock].Loc + block[errBlock].Len - 1].Val + 1); //中间隔的牌（临时记录铳牌）
                        if (tempReadyHands.Val != Hands[block[errBlock + 1].Loc].Val - 1) //如果它和下一张牌不连续
                            return; //无听
                        if (block.Where(eyesBlock => !eyesBlock.Type).Where(eyesBlock => eyesBlock.Len % 3 == 2).Any(eyesBlock => !eyesBlock.BlockIgnorePair(this)))
                            return; //无听
                        var tempOp = new Opponent(); //临时用来判断的牌组
                        var tempBlock = new Block(0) { Len = block[errBlock].Len + 1 + block[errBlock + 1].Len }; //这两个不完整型总长度+一张中间隔的牌
                        var i = 0;
                        for (; i < block[errBlock].Len; ++i)
                            tempOp[i] = Hands[block[errBlock].Loc + i]; //复制该不完整型
                        tempOp[i] = tempReadyHands; //记录中间的牌
                        for (; i < tempBlock.Len; ++i)
                            tempOp[i] = Hands[block[errBlock].Loc + i]; //复制该不完整型
                        if (tempBlock.BlockComplete(tempOp)) //如果该牌组完整
                            ReadyHands.Add(tempReadyHands); //记听一面
                        break;
                    }
            }
            IsReadyHand = true; //听牌
        }
    }
}
