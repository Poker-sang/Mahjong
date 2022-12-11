#pragma once

#include <memory>
#include <ranges>
#include <vector>

#include "Block.h"
#include "Meld.h"
#include "OpponentHelper.h"

class Opponent {

public:
    /// <summary>
    /// 手牌区
    /// </summary>
    TileList Hands = {};

    /// <summary>
    /// 副露区
    /// </summary>
    std::vector<Meld> Melds = {};

    /// <summary>
    /// 牌河（舍张）
    /// </summary>
    TileList Discards = {};

    /// <summary>
    /// 打牌
    /// </summary>
    /// <param name="index">打的牌的序号</param>
    Tile TileOut(const int index)
    {
        // 放入牌河
        Discards.emplace_back(Hands[index]);
        // 删除元素
        Hands.erase(Hands.begin() + index);
        return Hands[index];
    }

    /// <summary>
    /// 摸牌
    /// </summary>
    /// <param name="tile">进张</param>
    /// <returns>插入牌的位置</returns>
    int TileIn(Tile tile)
    {
        auto index = 0;
        // 找到进张插入的位置
        while (index < static_cast<int>(Hands.size()) && tile.Value > Hands[index].Value)
            ++index;
        // 插入牌
        Hands.emplace(Hands.begin() + index, tile);
        return index;
    }

    /// <summary>
    /// 听牌判断（在摸牌前判断）
    /// </summary>
    /// <returns>听的牌</returns>
    [[nodiscard]] TileList ReadyHandJudge() const
    {
        auto readyHands = TileList();
        auto sevenPairsFlag = false;

        // 如果没有副露（特殊牌型判断）
        if (Melds.empty()) {
            if (auto readyHandsList = ThirteenOrphansJudge(); !readyHandsList.empty())
                return readyHandsList;

            if (const auto tile = SevenPairsJudge(); tile != nullptr) {
                readyHands.emplace_back(tile->Value);
                sevenPairsFlag = true;
            }
            // 有可能复合二杯口，故听牌后不退出（会进入case 1或2）
        }
        auto blocks = std::vector<Block>();
        auto errBlocks = GetBlocks(blocks);

        // 不完整型块数
        switch (errBlocks.size()) {
        // 有一块不完整型（一块雀面不完整型（3n+1））
        // 二杯口缺雀头会在这里出现
        case 1: {
            // 将此不完整型遍历
            readyHands.append_range(errBlocks[0].Traversal(Hands, true));
            const auto index = std::ranges::find_if(blocks, [errBlocks](const Block block) { return block.Loc == errBlocks[0].Loc; }) - blocks.begin();
            // 与前块连接
            if (index != 0) {
                const auto joint = JointBlocks(blocks[index - 1], blocks[index]);
                // 如果该牌组完整，则记听一面
                if (joint != nullptr && std::get<1>(*joint).IgnoreEyesJudge(std::get<0>(*joint)) == true)
                    readyHands.emplace_back(std::get<2>(*joint));
            }
            // 与后块连接
            if (index != static_cast<long long>(blocks.size()) - 1) {
                const auto joint = JointBlocks(blocks[index], blocks[index + 1]);
                // 如果该牌组是雀头完整型，则记听一面
                if (joint != nullptr && std::get<1>(*joint).IgnoreEyesJudge(std::get<0>(*joint)) == true)
                    readyHands.emplace_back(std::get<2>(*joint));
            }

            break;
        }
        // 有两块不完整型（一块面子不完整型（3n+2），一块雀头完整型（3n+2））
        // 二杯口缺面子会在这里出现
        case 2: {
            if (errBlocks[1].IgnoreEyesJudge(Hands))
                readyHands.append_range(errBlocks[0].Traversal(Hands, false));
            if (errBlocks[0].IgnoreEyesJudge(Hands))
                readyHands.append_range(errBlocks[1].Traversal(Hands, false));
            break;
        }
        // 有三块不完整型（两块半不完整型（3n+1），一块雀头完整型（3n+2））
        case 3: {
            // 如果3n+2的不完整型夹在中间或不是雀头完整型，则无听
            const auto eyesIndex = std::ranges::find_if(errBlocks, [](const Block eyesBlock) { return eyesBlock.Integrity == Block::IntegrityType::Type2; }) - errBlocks.begin();

            if (eyesIndex == 1 || !errBlocks[eyesIndex].IgnoreEyesJudge(Hands))
                break;

            const auto joint = eyesIndex == 0
                ? JointBlocks(errBlocks[1], errBlocks[2])
                : JointBlocks(errBlocks[0], errBlocks[1]);
            if (joint == nullptr)
                break;
            // 如果该牌组完整，则记听一面
            if (std::get<1>(*joint).IntegrityJudge(std::get<0>(*joint)))
                readyHands.emplace_back(std::get<2>(*joint));
            break;
        }
        // 有两块不完整型（一块雀半完整型（3n），一块半不完整型（3n+1））
        case 4: {
            const auto joint = errBlocks[0].Loc < errBlocks[1].Loc
                ? JointBlocks(errBlocks[0], errBlocks[1])
                : JointBlocks(errBlocks[1], errBlocks[0]);

            if (joint == nullptr)
                break;
            // 如果该牌组是雀头完整型，则记听一面
            if (std::get<1>(*joint).IgnoreEyesJudge(std::get<0>(*joint)))
                readyHands.emplace_back(std::get<2>(*joint));

            break;
        }
        default:
            break;
        }

        // 如果是七对子，且听多张牌，则为二杯口，删除七对子的听牌，否则会重复
        if (sevenPairsFlag && readyHands.size() > 1)
            readyHands.erase(readyHands.begin());
        return readyHands;
    }

    /// <summary>
    /// 连接两块
    /// </summary>
    /// <param name="frontBlock">前块</param>
    /// <param name="followBlock">后块</param>
    /// <returns>连接后的牌组、连接后的块、用来连接的牌</returns>
    [[nodiscard]] std::shared_ptr<std::tuple<TileList, Block, Tile>> JointBlocks(const Block frontBlock, const Block followBlock) const
    {
        // 判断连接的两块是否连续
        if (followBlock.Loc - frontBlock.LastLoc() != 1)
            return nullptr;
        // 如果原来这两张牌中间不是隔一张，则无听
        if (GetRelation(Hands, frontBlock.LastLoc()) != 2)
            return nullptr;
        // 临时记录中间隔的牌（可能是铳牌）
        auto tempReadyHands = Tile(Hands[frontBlock.LastLoc()].Value + 1);
        // 临时用来判断的牌组

        // 这两块不完整型总张数
        auto jointedBlock = Block(0, frontBlock.Len + 1 + followBlock.Len);
        // 复制该不完整型所有牌
        auto jointedHands = TileList(Hands.begin() + frontBlock.Loc, Hands.begin() + jointedBlock.Len - 1);
        // 插入一张中间隔的牌
        jointedHands.insert(jointedHands.begin() + frontBlock.Len, tempReadyHands);
        return std::make_shared<std::tuple<TileList, Block, Tile>>(jointedHands, jointedBlock, tempReadyHands);
    }

    /// <summary>
    /// 七对牌型判断
    /// </summary>
    /// <returns>听的牌</returns>
    [[nodiscard]] std::shared_ptr<Tile> SevenPairsJudge() const
    {
        // 多出来的单张
        auto single = false;
        // 该单张牌位置
        auto singleTile = 0;
        // 判断相同或连续的关系
        for (auto i = 0; i < 12; ++i)
            // 如果偶数位关系对应不是相同，或奇数位不是其他关系（出现单张）
            if (((i + (single ? 1 : 0)) % 2 ^ (GetRelation(Hands, i) > 0 ? 1 : 0)) > 0) {
                // 直接异或运算无法排除龙七对
                // 如果这个错误关系是相同，则是龙七对；如果之前已经有单牌了，则不是七对子
                if (GetRelation(Hands, i) == 0 || single)
                    return nullptr;

                single = true;
                singleTile = Hands[i].Value;
            }

        // 如果没查到单张
        if (!single)
            // 那单张就是最后一个
            singleTile = Hands[12].Value;
        // 记听一面
        return std::make_shared<Tile>(singleTile);
    }

    /// <summary>
    /// 国士牌型判断
    /// </summary>
    /// <returns>听牌</returns>
    [[nodiscard]] TileList ThirteenOrphansJudge() const
    {
        // 是否缺了某张幺九牌（0或1）
        auto isMissing = false;
        // 是否多了某张幺九牌（0或1）
        auto isRedundant = false;
        // 缺的幺九牌
        auto missingTile = 0;
        auto readyHands = TileList();
        // 判断十三张幺九牌的拥有情况
        for (auto i = 0; i < 13; ++i) {
            const auto offset = (isMissing ? 1 : 0) - (isRedundant ? 1 : 0);
            // 如果和上张映射幺九牌一样
            if (Hands[i].Value == (i + offset - 1) / 8) {
                // 如果之前已经有一个多的牌
                if (isRedundant)
                    return readyHands;
                // 记录有多牌
                isRedundant = true;
            }
            // 如果和下张映射幺九牌一样
            else if (Hands[i].Value == (i + offset + 1) / 8) {
                // 如果之前已经有一个缺牌则不是国士，否则记录缺牌
                if (isMissing)
                    return readyHands;
                isMissing = true;
                missingTile = i / 8;
            }
            // 有不是幺九牌即不符合国士
            else if (Hands[i].Value != (i + offset) / 8)
                return readyHands;
        }

        // 若有多张，记听一面或记听一面（红中）（因为红中在最后不会被redundancy记录）
        if (isRedundant)
            readyHands.emplace_back(isMissing ? missingTile : 96);
        // 若不缺张则记听十三面
        else
            for (auto i = 0; i < 13; ++i)
                readyHands.emplace_back(i / 8);
        return readyHands;
    }

    /// <summary>
    /// 获取分块
    /// </summary>
    /// <exception cref="ArgumentOutOfRangeException"></exception>
    /// <returns>不完整的块数（最多3个）</returns>
    std::vector<Block> GetBlocks(std::vector<Block>& blocks) const
    {
        auto errBlocks = std::vector<Block>();
        blocks.reserve(6);
        auto tempLoc = 0;
        for (auto i = 0; i < static_cast<int>(Hands.size()) - 1; ++i)
            // 当关系不是相同或连续
            if (GetRelation(Hands, i) > 1) {
                blocks.emplace_back(tempLoc, i - tempLoc + 1);
                // 如果类型是不完整则记录
                if (blocks.back().Integrity != Block::IntegrityType::Type0)
                    errBlocks.emplace_back(blocks.back());
                // 若块序号达到(6 - 副露数)或有4个不完整型则无听
                if (blocks.size() + Melds.size() == 6 || errBlocks.size() == 4)
                    return {};
                // 下一块，括号里是块内首张牌的序号
                tempLoc = i + 1;
            }
        // 最后一块的记录单独实现
        {
            blocks.emplace_back(tempLoc, Hands.size() - tempLoc);
            if (blocks.back().Integrity != Block::IntegrityType::Type0)
                errBlocks.emplace_back(blocks.back());
            if (errBlocks.size() == 4)
                return {};
        }
        // 通过完整型Lv.1的块，筛选完整型Lv.2发现有一块不完整，则为不完整型加半不完整型，多于一块则无听
        for (auto& block : blocks) {
            if (block.Integrity == Block::IntegrityType::Type0
                && !block.IntegrityJudge(Hands)) {
                if (errBlocks.size() != 4) {
                    errBlocks.emplace_back(block);
                    // 特殊标记
                    errBlocks.emplace_back(0);
                    errBlocks.emplace_back(0);
                } else
                    return {};
            }
        }
        return errBlocks;
    }
};
