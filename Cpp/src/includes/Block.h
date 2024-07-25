#pragma once
#include <vector>

#include "Group.h"
#include "OpponentHelper.h"
#include "Tile.h"

/// <summary>
/// 块（其中牌关系只有相同或连续）
/// </summary>
class Block {
public:
    /// <summary>
    /// 块内首张牌的序号
    /// </summary>
    int Loc { 0 };

    /// <summary>
    /// 块内牌数（至少一张）
    /// </summary>
    int Len { 1 };

    /// <summary>
    /// 完整类型
    /// </summary>
    enum class IntegrityType {
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
    };

    /// <summary>
    /// 完整性
    /// </summary>
    IntegrityType Integrity;

    /// <summary>
    /// 块内末张牌的序号
    /// </summary>
    [[nodiscard]] int LastLoc() const { return Loc + Len - 1; }

    explicit Block(const int loc, const int len = -1)
        : Loc { loc }
        , Len { len }
    {
        Integrity = len == -1 ? IntegrityType::TypeEx : static_cast<IntegrityType>(len % 3);
    }

    /// <summary>
    /// 筛选完整型Lv.2
    /// </summary>
    /// <param name="hands">判断的牌组</param>
    /// <param name="eyesLoc">雀头的序号（-1为没有雀头）</param>
    bool IntegrityJudge(const TileList& hands, const int eyesLoc = -1)
    {
        auto groups = GetGroups(hands);

        // 在此处没用，但在和牌算符时会用到
        std::vector blockTiles(Len, TileType::Sequence);
        // 若有雀头，则将雀头认为是刻
        if (eyesLoc != -1) {
            ++groups[eyesLoc].Confirmed;
            ++groups[eyesLoc].Confirmed;
            blockTiles[groups[eyesLoc].Loc - Loc] = TileType::Triplet;
            blockTiles[groups[eyesLoc].Loc - Loc + 1] = TileType::Triplet;
        }
        // 每次循环记录一个组
        for (auto i = 0; i < static_cast<int>(groups.size()); ++i) {
            // 该组牌数
            switch (groups[i].Len - groups[i].Confirmed) {
            // 刚好全部确定
            case 0:
                continue;
            // 都是顺，确定后面2组分别有1张是顺
            case 1:
                if (static_cast<int>(groups.size()) > i + 2) {
                    ++groups[i + 1].Confirmed;
                    ++groups[i + 2].Confirmed;
                    continue;
                }
                break;
            // 都是顺，确定后面2组分别有2张是顺
            case 2:
                if (static_cast<int>(groups.size()) > i + 2) {
                    ++groups[i + 1].Confirmed;
                    ++groups[i + 1].Confirmed;
                    ++groups[i + 2].Confirmed;
                    ++groups[i + 2].Confirmed;
                    continue;
                }
                break;
            // 3刻1顺，确定后面2组分别有1张是顺
            case 4:
                if (static_cast<int>(groups.size()) > i + 2) {
                    ++groups[i + 1].Confirmed;
                    ++groups[i + 2].Confirmed;
                    blockTiles[groups[i].Loc - Loc] = TileType::Triplet;
                    blockTiles[groups[i].Loc - Loc + 1] = TileType::Triplet;
                    blockTiles[groups[i].Loc - Loc + 2] = TileType::Triplet;
                    continue;
                }
                break;
            // 3张是刻
            case 3:
                blockTiles[groups[i].Loc - Loc] = TileType::Triplet;
                blockTiles[groups[i].Loc - Loc + 1] = TileType::Triplet;
                blockTiles[groups[i].Loc - Loc + 2] = TileType::Triplet;
                continue;
            // 可能是负数
            default:
                break;
            }
            Integrity = eyesLoc == -1 ? IntegrityType::TypeEx : IntegrityType::Type2;
            return false;
        }
        return true;
    }

    /// <summary>
    /// 去对（雀头完整型）
    /// </summary>
    /// <param name="hands">判断的牌组</param>
    /// <returns>是否听牌</returns>
    bool IgnoreEyesJudge(const TileList& hands)
    {
        for (auto i = Loc, tempGroupNum = 0; i < Loc + Len - 1; ++i) {
            // 当关系是连续，则组数加一
            if (GetRelation(hands, i) == 1)
                ++tempGroupNum;
            // 当关系是相同，若是雀头完整型，则听牌
            else if (IntegrityJudge(hands, tempGroupNum) == true)
                return true;
        }
        return false;
    }

    /// <summary>
    /// 遍历
    /// </summary>
    /// <param name="hands">判断的牌组</param>
    /// <param name="mode">假为面子不完整型，真为全不完整型</param>
    /// <returns>是否听牌</returns>
    [[nodiscard]] TileList Traversal(const TileList& hands, const bool mode) const
    {
        // 可能的首张牌
        auto first = hands[Loc].Value - 1;
        // 如果首张是一万、筒、索或字牌，则first没有前一张
        if (hands[Loc].Value % 16 == 0 || hands[Loc].Value / 8 > 5)
            ++first;
        // 可能的末张牌
        auto last = hands[Loc + Len - 1].Value + 1;
        // 如果末张是九万、筒、索或字牌，则得last没有后一张
        if (hands[Loc + Len - 1].Value % 16 == 8 || hands[Loc + Len - 1].Value / 8 > 5)
            --last;
        auto tempBlock = Block(0, Len + 1);
        auto tempTile = first;
        auto readyHands = TileList();
        // 每张牌都插入尝试一次（遍历）
        for (auto i = 0; i < last - first + 1; ++i, ++tempTile) {
            // 重新复制所有牌
            auto tempHands = std::vector(hands.begin() + Loc, hands.begin() + Loc + Len);
            // 插入尝试的牌
            TileIn(tempHands, Tile(tempTile));
            if (mode // 雀面不完整型且遍历、去对后完整，则听牌
                    ? tempBlock.IgnoreEyesJudge(tempHands)
                    // 面子不完整型且遍历后完整，则听牌
                    : tempBlock.IntegrityJudge(tempHands))
                readyHands.emplace_back(tempTile);
        }
        return readyHands;
    }

private:
    /// <summary>
    /// 获得分组
    /// </summary>
    /// <param name="hands"></param>
    /// <returns></returns>
    [[nodiscard]] std::vector<Group> GetGroups(const TileList& hands) const
    {
        auto tempLoc = Loc;
        auto group = std::vector<Group>();
        // 判断块内每个关系
        for (auto i = Loc; i < Loc + Len - 1; ++i)
            // 当关系是连续，则记录多一个组
            if (GetRelation(hands, i) == 1) {
                group.emplace_back(tempLoc, i + 1 - tempLoc);
                tempLoc = i + 1;
            }
        group.emplace_back(tempLoc, Loc + Len - tempLoc);
        return group;
    }

    /// <summary>
    /// 每张牌的类型
    /// </summary>
    enum class TileType {
        /// <summary>
        /// 顺
        /// </summary>
        Sequence,

        /// <summary>
        /// 刻或雀头
        /// </summary>
        Triplet
    };
};
