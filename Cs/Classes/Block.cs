using System.Collections.Generic;
using System.Linq;

namespace Cs.Classes
{
    /// <summary>
    /// 块（其中牌关系只有相同或连续）
    /// </summary>
    public class Block
    {
        /// <summary>
        /// 块内首张牌的序号
        /// </summary>
        public int Loc { get; }
        /// <summary>
        /// 块内牌数（至少一张）
        /// </summary>
        public int Len { get; set; } = 1;
        /// <summary>
        /// 类型（真（3n）为完整型（由整数个面子组成），假为不完整型（含有雀头、不完整的面子））
        /// </summary>
        public bool Type { get; set; } = true;


        public Block(int loc) => Loc = loc;
        /// <summary>
        /// 筛选完整型Lv.2
        /// </summary>
        /// <param name="op">判断的牌组</param>
        /// <param name="eyesLoc">雀头的序号（-1为没有雀头）</param>
        public bool BlockComplete(Opponent op, int eyesLoc = -1)
        {
            var group = new List<Group> { new Group(Loc) };
            for (var i = Loc; i < Loc + Len - 1; ++i)       //判断块内每个关系
                if (op.Link(i) == 1)        //当关系是连续
                {
                    group.Last().Len = i + 1 - group.Last().Loc;
                    group.Add(new Group(i + 1));       //记录多一个组
                }
            group.Last().Len = Loc + Len - group.Last().Loc;
            var groupNum = group.Count;        //包含的组数
            var blockCard = new bool[Len];     //判断块内每张牌，若该牌属于顺子即记为顺（true），否则记为刻（flase）
            for (var i = 0; i < Len; ++i)
                blockCard[i] = true;
            if (eyesLoc != -1)       //有雀头
            {
                ++group[eyesLoc].Confirmed;      //雀头处有两个刻
                ++group[eyesLoc].Confirmed;
                blockCard[group[eyesLoc].Loc - Loc] = false;
                blockCard[group[eyesLoc].Loc - Loc + 1] = false;
            }
            for (var i = 0; i < groupNum; ++i)     //每次循环记录一个组
            {
                switch (group[i].Len - group[i].Confirmed)      //该组牌数
                {
                    case 0:
                        continue;       //开始下一循环
                    case 1:     //都是顺
                        if (groupNum > i + 2)
                        {
                            ++group[i + 1].Confirmed;       //确定后面几组有2张是顺
                            ++group[i + 2].Confirmed;
                            continue;
                        }
                        break;
                    case 2:     //都是顺
                        if (groupNum > i + 2)
                        {
                            ++group[i + 1].Confirmed;       //确定后面几组有2张是顺
                            ++group[i + 1].Confirmed;
                            ++group[i + 2].Confirmed;
                            ++group[i + 2].Confirmed;
                            continue;
                        }
                        break;
                    case 4:     //3刻1顺
                        if (groupNum > i + 2)
                        {
                            ++group[i + 1].Confirmed;       //确定后面几组有2张是顺
                            ++group[i + 2].Confirmed;
                            blockCard[group[i].Loc - Loc] = false;     //三张是刻
                            blockCard[group[i].Loc - Loc + 1] = false;
                            blockCard[group[i].Loc - Loc + 2] = false;
                            continue;
                        }
                        break;
                    case 3:     //都是刻
                        blockCard[group[i].Loc - Loc] = false;     //三张是刻
                        blockCard[group[i].Loc - Loc + 1] = false;
                        blockCard[group[i].Loc - Loc + 2] = false;
                        continue;
                    default:        //可能是负数
                        break;
                }
                Type = false;       //不完整型
                return false;
            }
            return true;
        }
        /// <summary>
        /// 去对（雀头完整型）
        /// </summary>
        /// <param name="op">判断的牌组</param>
        /// <returns>是否听牌</returns>
        public bool BlockIgnorePair(Opponent op)
        {
            for (int i = Loc, tempGroupNum = 0; i < Loc + Len - 1; ++i)
            {
                if (op.Link(i) == 1)        //当关系是连续
                    ++tempGroupNum;       //组数加一
                else if (BlockComplete(op, tempGroupNum))        //当关系是相同，判断是否是雀头完整型
                    return true;        //该组是雀头完整型
            }
            return false;       //不完整
        }
        /// <summary>
        /// 遍历
        /// </summary>
        /// <param name="op">判断的牌组</param>
        /// <param name="mode">假为面子不完整型，真为全不完整型</param>
        /// <returns>是否听牌</returns>
        public bool BlockTraversal(Opponent op, bool mode)
        {
            var first = op[Loc].Val - 1;        //可能的首张牌
            if ((op[Loc].Val & 15) == 0 || op[Loc].Val >> 3 > 5)      //如果首张是一万、筒、索或字牌
                ++first;        //first没有前一张，加回op[loc]
            var last = op[Loc + Len - 1].Val + 1;        //可能的末张牌
            if ((op[Loc + Len - 1].Val & 15) == 8 || op[Loc + Len - 1].Val >> 3 > 5)      //如果末张是九万、筒、索或字牌
                --last;     //last没有后一张，减回op[loc]
            var tempBlock = new Block(0) { Len = Len + 1 };      //新牌组对应的块
            var tempTile = first;
            var tempOp = new Opponent();
            for (var i = 0; i < last - first + 1; ++i, ++tempTile)     //每张牌都插入尝试一次（遍历）
            {
                for (var j = Loc; j < Loc + Len; ++j)       //重新复制所有牌
                    tempOp.Hands.Add(new Tile(op[j].Val));
                tempOp.TileIn(new Tile(tempTile));      //插入尝试的牌
                if (mode && tempBlock.BlockIgnorePair(tempOp)         //（真）全不完整型且遍历、去对后完整
                    || !mode && tempBlock.BlockComplete(tempOp))     //（假）面子不完整型且遍历后完整
                    op.ReadyHands.Add(new Tile(tempTile));
                tempOp.Hands.Clear();
            }
            return op.ReadyHands is null || op.ReadyHands.Count <= 0;       //可能本来它就不为空，不过在这里不影响（将来算符时可能改动）
        }
    }
}