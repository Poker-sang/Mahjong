using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

namespace Cs.Classes
{
    /// <summary>
    /// 块（其中牌关系只有相同或连续）
    /// </summary>
    public class Block
    {
        /// <summary>
        /// 块内牌数（至少一张）
        /// </summary>
        public int Len { get; set; } = 1;
        /// <summary>
        /// 类型（真（3n）为完整型（由整数个面子组成），假为不完整型（含有雀头、不完整的面子））
        /// </summary>
        public IntegrityType Integrity { get; set; } = IntegrityType.Type0;

        /// <summary>
        /// 块内首张牌的序号
        /// </summary>
        public int FirstLoc { get; }

        public int LastLoc => FirstLoc + Len - 1;
        /// <summary>
        /// 完整类型
        /// </summary>
        public enum IntegrityType
        {
            /// <summary>
            /// 完整型（3n）
            /// </summary>
            Type0,
            /// <summary>
            /// 雀面不完整型或半不完整型（3n+1）
            /// </summary>
            Type1,
            /// <summary>
            /// 雀头不完整型或面子不完整型（3n+2）
            /// </summary>
            Type2,
            /// <summary>
            /// 雀半不完整型（3n）
            /// </summary>
            TypeEx
        }

        public Block(int loc) => FirstLoc = loc;

        /// <summary>
        /// 每张牌的类型（顺或刻（雀头属于刻））
        /// </summary>
        private enum TileType { Sequence, Triplet };
        /// <summary>
        /// 筛选完整型Lv.2
        /// </summary>
        /// <param name="hands">判断的牌组</param>
        /// <param name="eyesLoc">雀头的序号（-1为没有雀头）</param>
        public bool IntegrityJudge(List<Tile> hands, int eyesLoc = -1)
        {
            var groups = GetGroups(hands);
            
            //在此时没用，但在和牌算符时会用到
            var blockTiles = new TileType[Len];
            for (var i = 0; i < blockTiles.Length; ++i)
                blockTiles[i] = TileType.Sequence;
            //若有雀头，则将雀头认为是刻
            if (eyesLoc is not -1)
            {
                ++groups[eyesLoc].Confirmed;
                ++groups[eyesLoc].Confirmed;
                blockTiles[groups[eyesLoc].Loc - FirstLoc] = TileType.Triplet;
                blockTiles[groups[eyesLoc].Loc - FirstLoc + 1] = TileType.Triplet;
            }
            //每次循环记录一个组
            for (var i = 0; i < groups.Count; ++i)
            {
                //该组牌数
                switch (groups[i].Len - groups[i].Confirmed)
                {
                    //刚好全部确定
                    case 0:
                        continue;
                    //都是顺，确定后面2组分别有1张是顺
                    case 1:
                        if (groups.Count > i + 2)
                        {
                            ++groups[i + 1].Confirmed;
                            ++groups[i + 2].Confirmed;
                            continue;
                        }
                        break;
                    //都是顺，确定后面2组分别有2张是顺
                    case 2:
                        if (groups.Count > i + 2)
                        {
                            ++groups[i + 1].Confirmed;
                            ++groups[i + 1].Confirmed;
                            ++groups[i + 2].Confirmed;
                            ++groups[i + 2].Confirmed;
                            continue;
                        }
                        break;
                    //3刻1顺，确定后面2组分别有1张是顺
                    case 4:
                        if (groups.Count > i + 2)
                        {
                            ++groups[i + 1].Confirmed;
                            ++groups[i + 2].Confirmed;
                            blockTiles[groups[i].Loc - FirstLoc] = TileType.Triplet;
                            blockTiles[groups[i].Loc - FirstLoc + 1] = TileType.Triplet;
                            blockTiles[groups[i].Loc - FirstLoc + 2] = TileType.Triplet;
                            continue;
                        }
                        break;
                    //3张是刻
                    case 3:
                        blockTiles[groups[i].Loc - FirstLoc] = TileType.Triplet;
                        blockTiles[groups[i].Loc - FirstLoc + 1] = TileType.Triplet;
                        blockTiles[groups[i].Loc - FirstLoc + 2] = TileType.Triplet;
                        continue;
                    //可能是负数
                    default:
                        break;
                }
                Integrity = eyesLoc is -1 ? IntegrityType.TypeEx : IntegrityType.Type2;
                return false;
            }
            return true;
        }

        private List<Group> GetGroups(List<Tile> hands)
        {
            var groups = new List<Group> { new(FirstLoc) };
            //判断块内每个关系
            for (var i = FirstLoc; i < FirstLoc + Len - 1; ++i)
                //当关系是连续，则记录多一个组
                if (hands.GetRelation(i) is 1)
                {
                    groups.Last().Len = i + 1 - groups.Last().Loc;
                    groups.Add(new(i + 1));
                }
            groups.Last().Len = FirstLoc + Len - groups.Last().Loc;
            return groups;
        }

        /// <summary>
        /// 遍历
        /// </summary>
        /// <param name="hands">判断的牌组</param>
        /// <param name="mode">是否要去对（真为雀面不完整型，假为面子不完整型）</param>
        /// <returns>听的牌，可能本来它就不为空，不过在这里不影响（将来算符时可能改动）</returns>
        public IEnumerable<Tile> Traversal(List<Tile> hands, bool mode)
        {
            //可能的首张牌
            var first = hands[FirstLoc].Val - 1;
            //如果首张是一万、筒、索或字牌，则first没有前一张，加回hands[loc]
            if ((hands[FirstLoc].Val & 15) is 0 || hands[FirstLoc].Val / 8 > 5)
                ++first;
            //可能的末张牌
            var last = hands[FirstLoc + Len - 1].Val + 1;
            //如果末张是九万、筒、索或字牌，则得last没有后一张，减回hands[loc]
            if ((hands[FirstLoc + Len - 1].Val & 15) is 8 || hands[FirstLoc + Len - 1].Val / 8 > 5)
                --last;
            var tempBlock = new Block(0) { Len = Len + 1 };
            var tempTile = first;
            //每张牌都插入尝试一次（遍历）
            for (var i = 0; i < last - first + 1; ++i, ++tempTile)
            {
                var tempHands = new List<Tile>();
                //重新复制所有牌
                for (var j = FirstLoc; j < FirstLoc + Len; ++j)
                    tempHands.Add(new Tile(hands[j].Val));
                //插入尝试的牌
                tempHands.TileIn(new Tile(tempTile));
                if (mode switch
                    {
                        //雀面不完整型且遍历、去对后完整，则听牌
                        true => tempBlock.IgnoreEyesJudge(tempHands),
                        //面子不完整型且遍历后完整，则听牌
                        false => tempBlock.IntegrityJudge(tempHands)
                    })
                    yield return new Tile(tempTile);
            }
        }

        /// <summary>
        /// 去对后完整（雀头完整型）
        /// </summary>
        /// <param name="hands">判断的牌组</param>
        /// <returns>是否完整</returns>
        public bool IgnoreEyesJudge(List<Tile> hands)
        {
            for (int i = FirstLoc, tempGroupNum = 0; i < FirstLoc + Len - 1; ++i)
            {
                //当关系是连续，则组数加一
                if (hands.GetRelation(i) is 1)
                    ++tempGroupNum;
                //当关系是相同，若是雀头完整型，则听牌
                else if (IntegrityJudge(hands, tempGroupNum))
                    return true;
            }
            return false;
        }
    }
}