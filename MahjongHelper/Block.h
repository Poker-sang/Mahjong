#pragma once
#include "Group.h"
#include "OpponentHelper.h"

namespace MahjongHelper {
using namespace System::Collections::Generic;
/// <summary>
/// 块（其中牌关系只有相同或连续）
/// </summary>
public
ref class Block sealed {
public:
    /// <summary>
    /// 类型（真（3n）为完整型（由整数个面子组成），假为不完整型（含有雀头、不完整的面子））
    /// </summary>
    enum class EIntegrityType {
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
    } Integrity
        = EIntegrityType::Type0;

    /// <summary>
    /// 块内牌数（至少一张）
    /// </summary>
    int Len = 1;

    /// <summary>
    /// 块内首张牌的序号
    /// </summary>
    int const FirstLoc;

    int LastLoc() { return FirstLoc + Len - 1; }

    /// <summary>
    /// 类型（真（3n）为完整型（由整数个面子组成），假为不完整型（含有雀头、不完整的面子））
    /// </summary>
    bool Type = true;

    Block(const int loc)
        : FirstLoc { loc }
    {
    }

    /// <summary>
    /// 获得分组
    /// </summary>
    /// <param name="hands"></param>
    /// <returns></returns>
    List<Group ^> ^ GetGroups(List<Tile ^> ^ hands) {
        auto groups = new List<Group ^>();
        groups->Add(new Group(FirstLoc));
        //判断块内每个关系
        for (auto i = FirstLoc; i < FirstLoc + Len - 1; ++i)
            //当关系是连续，则记录多一个组
            if (GetRelation(hands, i) == 1) {
                Last(groups)->Len = i + 1 - Last(groups)->Loc;
                groups->Add(new Group(i + 1));
            }
        Last(groups)->Len = FirstLoc + Len - Last(groups)->Loc;
        return groups;
    }

        /// <summary>
        /// 每张牌的类型（顺或刻（雀头属于刻））
        /// </summary>
        enum class ETileType { Sequence,
            Triplet };
    /// <summary>
    /// 筛选完整型Lv.2
    /// </summary>
    /// <param name="hands">判断的牌组</param>
    /// <param name="eyesLoc">雀头的序号（-1为没有雀头）</param>
    bool IntegrityJudge(List<Tile ^> ^ hands, int eyesLoc)
    {
        auto groups = GetGroups(hands);

        //在此时没用，但在和牌算符时会用到
        auto blockTiles = new array<ETileType>(Len);
        for (auto i = 0; i < blockTiles->Length; ++i)
            blockTiles[i] = ETileType::Sequence;
        //若有雀头，则将雀头认为是刻
        if (eyesLoc != -1) {
            ++groups[eyesLoc]->Confirmed;
            ++groups[eyesLoc]->Confirmed;
            blockTiles[groups[eyesLoc]->Loc - FirstLoc] = ETileType::Triplet;
            blockTiles[groups[eyesLoc]->Loc - FirstLoc + 1] = ETileType::Triplet;
        }
        //每次循环记录一个组
        for (auto i = 0; i < groups->Count; ++i) {
            //该组牌数
            switch (groups[i]->Len - groups[i]->Confirmed) {
                //刚好全部确定
            case 0:
                continue;
                //都是顺，确定后面2组分别有1张是顺
            case 1:
                if (groups->Count > i + 2) {
                    ++groups[i + 1]->Confirmed;
                    ++groups[i + 2]->Confirmed;
                    continue;
                }
                break;
                //都是顺，确定后面2组分别有2张是顺
            case 2:
                if (groups->Count > i + 2) {
                    ++groups[i + 1]->Confirmed;
                    ++groups[i + 1]->Confirmed;
                    ++groups[i + 2]->Confirmed;
                    ++groups[i + 2]->Confirmed;
                    continue;
                }
                break;
                // 3刻1顺，确定后面2组分别有1张是顺
            case 4:
                if (groups->Count > i + 2) {
                    ++groups[i + 1]->Confirmed;
                    ++groups[i + 2]->Confirmed;
                    blockTiles[groups[i]->Loc - FirstLoc] = ETileType::Triplet;
                    blockTiles[groups[i]->Loc - FirstLoc + 1] = ETileType::Triplet;
                    blockTiles[groups[i]->Loc - FirstLoc + 2] = ETileType::Triplet;
                    continue;
                }
                break;
                // 3张是刻
            case 3:
                blockTiles[groups[i]->Loc - FirstLoc] = ETileType::Triplet;
                blockTiles[groups[i]->Loc - FirstLoc + 1] = ETileType::Triplet;
                blockTiles[groups[i]->Loc - FirstLoc + 2] = ETileType::Triplet;
                continue;
                //可能是负数
            default:
                break;
            }
            Integrity = eyesLoc == -1 ? EIntegrityType::TypeEx : EIntegrityType::Type2;
            return false;
        }
        return true;
    }

    /// <summary>
    /// 去对后完整（雀头完整型）
    /// </summary>
    /// <param name="hands">判断的牌组</param>
    /// <returns>是否完整</returns>
    bool IgnoreEyesJudge(List<Tile ^> ^ hands)
    {
        for (int i = FirstLoc, tempGroupNum = 0; i < FirstLoc + Len - 1; ++i) {
            //当关系是连续，则组数加一
            if (GetRelation(hands, i) == 1)
                ++tempGroupNum;
            //当关系是相同，若是雀头完整型，则听牌
            else if (IntegrityJudge(hands, tempGroupNum))
                return true;
        }
        return false;
    }
    /// <summary>
    /// 遍历
    /// </summary>
    /// <param name="hands">判断的牌组</param>
    /// <param name="mode">是否要去对（真为雀面不完整型，假为面子不完整型）</param>
    /// <returns>听的牌，可能本来它就不为空，不过在这里不影响（将来算符时可能改动）</returns>
    List<Tile ^> ^ Traversal(List<Tile ^> ^ hands, const bool mode) {
        //可能的首张牌
        auto first = hands[FirstLoc]->Val - 1;
        //如果首张是一万、筒、索或字牌，则first没有前一张，加回hands[loc]
        if (hands[FirstLoc]->Val % 16 == 0 || hands[FirstLoc]->Val / 8 > 5)
            ++first;
        //可能的末张牌
        auto last = hands[FirstLoc + Len - 1]->Val + 1;
        //如果末张是九万、筒、索或字牌，则得last没有后一张，减回hands[loc]
        if (hands[FirstLoc + Len - 1]->Val % 16 == 8 || hands[FirstLoc + Len - 1]->Val / 8 > 5)
            --last;
        auto tempBlock = new Block(0);
        tempBlock->Len = Len + 1;
        auto tempTile = first;
        auto tempReturn = new List<Tile ^>();
        //每张牌都插入尝试一次（遍历）
        for (auto i = 0; i < last - first + 1; ++i, ++tempTile) {
            auto tempHands = new List<Tile ^>();
            //重新复制所有牌
            for (auto j = FirstLoc; j < FirstLoc + Len; ++j)
                tempHands->Add(new Tile(hands[j]->Val));
            //插入尝试的牌
            TileIn(tempHands, new Tile(tempTile));
            //雀面不完整型且遍历、去对后完整，则听牌
            //面子不完整型且遍历后完整，则听牌
            if (mode && tempBlock->IgnoreEyesJudge(tempHands) || !mode && tempBlock->IntegrityJudge(tempHands, -1))
                tempReturn->Add(new Tile(tempTile));
        }
        return tempReturn;
    }
};
}
