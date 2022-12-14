#pragma once
#include "Block.h"

namespace MahjongHelper {
inline int BoolToInt(const bool _bool) { return _bool ? 1 : 0; }

/// <summary>
/// 听牌判断（在摸牌前判断）
/// </summary>
/// <returns>听的牌</returns>
inline List<Tile ^> ^ Opponent::ReadyHandJudge()
{
    auto readyHands = new List<Tile ^>();
    auto sevenPairsFlag = false;

    // 如果没有副露（特殊牌型判断）
    if (Melds == 0) {
        auto readyHandsList = new List<Tile ^>(ThirteenOrphansJudge());
        if (readyHandsList->Count != 0)
            return readyHandsList;

        auto tile = SevenPairsJudge();
        if (SevenPairsJudge() != nullptr) {
            readyHands->Add(tile);
            sevenPairsFlag = true;
        }
        // 有可能复合二杯口，故听牌后不退出（会进入case 1或2）
    }

    List<Block ^> ^ blocks = nullptr;
    auto errBlocks = GetBlocks(blocks);

    // 不完整型块数
    switch (errBlocks->Count) {
        // 有一块不完整型（一块雀面不完整型（3n+1））
        // 两杯口会在这里出现
    case 1: {
        // 将此不完整型遍历
        readyHands->AddRange(errBlocks[0]->Traversal(Hands, true));
        const auto index = blocks->IndexOf(errBlocks[0]);
        // 与前块连接
        if (index != 0) {
            auto joint = JointBlocks(blocks[index - 1], blocks[index]);
            // 如果该牌组完整，则记听一面
            if (joint != nullptr && joint->Item2->IgnoreEyesJudge(joint->Item1))
                readyHands->Add(joint->Item3);
        }
        // 与后块连接
        if (index != blocks->Count - 1) {
            auto joint = JointBlocks(blocks[index], blocks[index + 1]);
            // 如果该牌组是雀头完整型，则记听一面
            if (joint != nullptr && joint->Item2->IgnoreEyesJudge(joint->Item1))
                readyHands->Add(joint->Item3);
        }

        break;
    }
    // 有两块不完整型（一块面子不完整型（3n+2），一块雀头完整型（3n+2））
    case 2: {
        if (errBlocks[1]->IgnoreEyesJudge(Hands))
            readyHands->AddRange(errBlocks[0]->Traversal(Hands, false));
        if (errBlocks[0]->IgnoreEyesJudge(Hands))
            readyHands->AddRange(errBlocks[1]->Traversal(Hands, false));
        break;
    }
    // 有三块不完整型（两块半不完整型（3n+1），一块雀头完整型（3n+2））
    case 3: {
        // 如果3n+2的不完整型夹在中间或不是雀头完整型，则无
        auto eyesIndex = 0;
        for each (auto eyesBlock in errBlocks)
            if (eyesBlock->Integrity == Block::EIntegrityType::Type2)
                eyesIndex = errBlocks->IndexOf(eyesBlock);
        if (eyesIndex == 1 || !errBlocks[eyesIndex]->IgnoreEyesJudge(Hands))
            break;

        auto joint = eyesIndex == 0
            ? JointBlocks(errBlocks[1], errBlocks[2])
            : JointBlocks(errBlocks[0], errBlocks[1]);

        if (joint == nullptr)
            break;
        // 如果该牌组完整，则记听一面
        if (joint->Item2->IntegrityJudge(joint->Item1, -1))
            readyHands->Add(joint->Item3);
        break;
    }
    // 有两块不完整型（一块雀半完整型（3n），一块半不完整型（3n+1））
    case 4: {
        auto joint = errBlocks[0]->FirstLoc < errBlocks[1]->FirstLoc ? JointBlocks(errBlocks[0], errBlocks[1]) : JointBlocks(errBlocks[1], errBlocks[0]);

        if (joint == nullptr)
            break;
        // 如果该牌组是雀头完整型，则记听一面
        if (joint->Item2->IgnoreEyesJudge(joint->Item1))
            readyHands->Add(joint->Item3);

        break;
    }
    }

    // 如果有听（七对子），则为二杯口，删除七对子的听牌，否则会重复
    if (sevenPairsFlag && readyHands->Count > 1)
        readyHands->RemoveAt(0);
    return readyHands;
}
/// <summary>
/// 国士牌型判断
/// </summary>
/// <returns>听牌</returns>
inline List<Tile ^> ^ Opponent::ThirteenOrphansJudge()
{
    auto tempReturn = new List<Tile ^>();
    // 是否缺了某张幺九牌（0或1）
    auto shortage = false;
    // 是否多了某张幺九牌（0或1）
    auto redundancy = false;
    auto shortTile = 0; // 缺的幺九牌
    // 判断十三张幺九牌的拥有情况
    for (auto i = 0; i < 13; ++i) {
        const auto temp = (shortage ? 1 : 0) - (redundancy ? 1 : 0);
        // 如果和上张映射幺九牌一样
        if (Hands[i]->Val == (i + temp - 1) / 8) {
            // 如果之前已经有一个多的牌
            if (redundancy)
                return tempReturn;
            redundancy = true; // 记录有多牌
        } // 如果和下张映射幺九牌一样
        else if (Hands[i]->Val == (i + temp + 1) / 8) {
            // 如果之前已经有一个缺牌则不是国士，否则记录缺牌
            if (shortage)
                return tempReturn;
            shortage = true;
            shortTile = i / 8;
        } // 有不是幺九牌即不符合国士
        else if (Hands[i]->Val != (i + temp) / 8)
            return tempReturn;
    }
    // 若有多张，记听一面或记听一面（红中）（因为红中在最后不会被redundancy记录）
    if (redundancy)
        tempReturn->Add(new Tile(shortage ? shortTile : 96));
    // 若不缺张则记听十三面
    else
        for (auto i = 0; i < 13; ++i)
            tempReturn->Add(new Tile(i / 8));
    return tempReturn;
}

/// <summary>
/// 七对牌型判断
/// </summary>
/// <returns>听牌</returns>
inline Tile ^ Opponent::SevenPairsJudge()
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
            singleTile = Hands[i]->Val;
        }

    // 如果没查到单张
    if (!single)
        // 那单张就是最后一个
        singleTile = Hands[12]->Val;
    // 记听一面
    return new Tile(singleTile);
}

/// <summary>
/// 获取分块
/// </summary>
/// <returns>不完整的块数（最多3个）</returns>
inline List<Block ^> ^ Opponent::GetBlocks(List<Block ^> ^ &blocks)
{
    auto errBlocks = new List<Block ^>();
    blocks = new List<Block ^>();
    blocks->Add(new Block(0));
    for (auto i = 0; i < Hands->Count - 1; ++i)
        // 当关系不是相同或连续
        if (GetRelation(Hands, i) > 1) {
            // 记录上一块的长度
            Last(blocks)->Len = i - Last(blocks)->FirstLoc + 1;
            // 筛选完整型Lv->1
            switch (Last(blocks)->Len % 3) {
            case 0:
                Last(blocks)->Integrity = Block::EIntegrityType::Type0;
                break;
            case 1:
                Last(blocks)->Integrity = Block::EIntegrityType::Type1;
                break;
            case 2:
                Last(blocks)->Integrity = Block::EIntegrityType::Type2;
                break;
            default:
                throw new System::ArgumentOutOfRangeException();
            }
            // 如果类型是不完整则记录
            if (Last(blocks)->Integrity != Block::EIntegrityType::Type0)
                errBlocks->Add(Last(blocks));
            // 若块序号达到(6 - 副露数)或有4个不完整型则无听
            if (blocks->Count + Melds == 6 || errBlocks->Count == 4)
                return new List<Block ^>();
            // 下一块，括号里是块内首张牌的序号
            blocks->Add(new Block(i + 1));
        }
    // 最后一块的记录无法写进循环
    {
        Last(blocks)->Len = Hands->Count - Last(blocks)->FirstLoc;
        switch (Last(blocks)->Len % 3) {
        case 0:
            Last(blocks)->Integrity = Block::EIntegrityType::Type0;
            break;
        case 1:
            Last(blocks)->Integrity = Block::EIntegrityType::Type1;
            break;
        case 2:
            Last(blocks)->Integrity = Block::EIntegrityType::Type2;
            break;
        default:
            throw new System::ArgumentOutOfRangeException();
        }
        if (Last(blocks)->Integrity != Block::EIntegrityType::Type0)
            errBlocks->Add(Last(blocks));
        if (errBlocks->Count == 4)
            return new List<Block ^>();
    }
    // 通过完整型Lv.1的块，筛选完整型Lv.2发现有一块不完整，则为不完整型加半不完整型，多于一块则无听
    for each (auto block in blocks)
        if (block->Integrity == Block::EIntegrityType::Type0 && !block->IntegrityJudge(Hands, -1)) {
            if (errBlocks->Count != 4) {
                block->Integrity = Block::EIntegrityType::TypeEx;
                errBlocks->Add(block);
                // 特殊标记
                errBlocks->Add(new Block(0));
                errBlocks->Add(new Block(0));
                errBlocks->Add(new Block(0));
            } else
                return new List<Block ^>();
        }
    return errBlocks;
}

/// <summary>
/// 连接两块
/// </summary>
/// <param name="frontBlock">前块</param>
/// <param name="followBlock">后块</param>
/// <returns>连接后的牌、连接后的块、用来连接的牌</returns>
inline System::Tuple<List<Tile ^> ^, Block ^, Tile ^> ^ Opponent::JointBlocks(Block ^ frontBlock, Block ^ followBlock)
{
    // 判断连接的两块是否连续
    if (followBlock->FirstLoc - frontBlock->LastLoc() != 1)
        return nullptr;
    // 如果原来这两张牌中间不是隔一张，则无听
    if (GetRelation(Hands, frontBlock->LastLoc()) != 2)
        return nullptr;
    // 临时记录中间隔的牌（可能是铳牌）
    auto tempReadyHands = new Tile(Hands[frontBlock->LastLoc()]->Val + 1);
    // 临时用来判断的牌组
    auto jointedHands = new List<Tile ^>();
    // 这两块不完整型总张数
    auto jointedBlock = new Block(0);
    jointedBlock->Len = frontBlock->Len + 1 + followBlock->Len;
    // 复制该不完整型所有牌
    jointedHands->AddRange(Hands->GetRange(frontBlock->FirstLoc, jointedBlock->Len - 1));
    // 插入一张中间隔的牌
    jointedHands->Insert(frontBlock->Len, tempReadyHands);
    return new System::Tuple<List<Tile ^> ^, Block ^, Tile ^>(jointedHands, jointedBlock, tempReadyHands);
}
}
