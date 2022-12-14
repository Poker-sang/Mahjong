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
    /// ������
    /// </summary>
    TileList Hands = {};

    /// <summary>
    /// ��¶��
    /// </summary>
    std::vector<Meld> Melds = {};

    /// <summary>
    /// �ƺӣ����ţ�
    /// </summary>
    TileList Discards = {};

    /// <summary>
    /// ����
    /// </summary>
    /// <param name="index">����Ƶ����</param>
    Tile TileOut(const int index)
    {
        // �����ƺ�
        Discards.emplace_back(Hands[index]);
        // ɾ��Ԫ��
        Hands.erase(Hands.begin() + index);
        return Hands[index];
    }

    /// <summary>
    /// ����
    /// </summary>
    /// <param name="tile">����</param>
    /// <returns>�����Ƶ�λ��</returns>
    int TileIn(Tile tile)
    {
        auto index = 0;
        // �ҵ����Ų����λ��
        while (index < static_cast<int>(Hands.size()) && tile.Value > Hands[index].Value)
            ++index;
        // ������
        Hands.emplace(Hands.begin() + index, tile);
        return index;
    }

    /// <summary>
    /// �����жϣ�������ǰ�жϣ�
    /// </summary>
    /// <returns>������</returns>
    [[nodiscard]] TileList ReadyHandJudge() const
    {
        auto readyHands = TileList();
        auto sevenPairsFlag = false;

        // ���û�и�¶�����������жϣ�
        if (Melds.empty()) {
            if (auto readyHandsList = ThirteenOrphansJudge(); !readyHandsList.empty())
                return readyHandsList;

            if (const auto tile = SevenPairsJudge(); tile != nullptr) {
                readyHands.emplace_back(tile->Value);
                sevenPairsFlag = true;
            }
            // �п��ܸ��϶����ڣ������ƺ��˳��������case 1��2��
        }
        auto blocks = std::vector<Block>();
        auto errBlocks = GetBlocks(blocks);

        // �������Ϳ���
        switch (errBlocks.size()) {
        // ��һ�鲻�����ͣ�һ��ȸ�治�����ͣ�3n+1����
        // ������ȱȸͷ�����������
        case 1: {
            // ���˲������ͱ���
            readyHands.append_range(errBlocks[0].Traversal(Hands, true));
            const auto index = std::ranges::find_if(blocks, [errBlocks](const Block block) { return block.Loc == errBlocks[0].Loc; }) - blocks.begin();
            // ��ǰ������
            if (index != 0) {
                const auto joint = JointBlocks(blocks[index - 1], blocks[index]);
                // ��������������������һ��
                if (joint != nullptr && std::get<1>(*joint).IgnoreEyesJudge(std::get<0>(*joint)) == true)
                    readyHands.emplace_back(std::get<2>(*joint));
            }
            // ��������
            if (index != static_cast<long long>(blocks.size()) - 1) {
                const auto joint = JointBlocks(blocks[index], blocks[index + 1]);
                // �����������ȸͷ�����ͣ������һ��
                if (joint != nullptr && std::get<1>(*joint).IgnoreEyesJudge(std::get<0>(*joint)) == true)
                    readyHands.emplace_back(std::get<2>(*joint));
            }

            break;
        }
        // �����鲻�����ͣ�һ�����Ӳ������ͣ�3n+2����һ��ȸͷ�����ͣ�3n+2����
        // ������ȱ���ӻ����������
        case 2: {
            if (errBlocks[1].IgnoreEyesJudge(Hands))
                readyHands.append_range(errBlocks[0].Traversal(Hands, false));
            if (errBlocks[0].IgnoreEyesJudge(Hands))
                readyHands.append_range(errBlocks[1].Traversal(Hands, false));
            break;
        }
        // �����鲻�����ͣ�����벻�����ͣ�3n+1����һ��ȸͷ�����ͣ�3n+2����
        case 3: {
            // ���3n+2�Ĳ������ͼ����м����ȸͷ�����ͣ�������
            const auto eyesIndex = std::ranges::find_if(errBlocks, [](const Block eyesBlock) { return eyesBlock.Integrity == Block::IntegrityType::Type2; }) - errBlocks.begin();

            if (eyesIndex == 1 || !errBlocks[eyesIndex].IgnoreEyesJudge(Hands))
                break;

            const auto joint = eyesIndex == 0
                ? JointBlocks(errBlocks[1], errBlocks[2])
                : JointBlocks(errBlocks[0], errBlocks[1]);
            if (joint == nullptr)
                break;
            // ��������������������һ��
            if (std::get<1>(*joint).IntegrityJudge(std::get<0>(*joint)))
                readyHands.emplace_back(std::get<2>(*joint));
            break;
        }
        // �����鲻�����ͣ�һ��ȸ�������ͣ�3n����һ��벻�����ͣ�3n+1����
        case 4: {
            const auto joint = errBlocks[0].Loc < errBlocks[1].Loc
                ? JointBlocks(errBlocks[0], errBlocks[1])
                : JointBlocks(errBlocks[1], errBlocks[0]);

            if (joint == nullptr)
                break;
            // �����������ȸͷ�����ͣ������һ��
            if (std::get<1>(*joint).IgnoreEyesJudge(std::get<0>(*joint)))
                readyHands.emplace_back(std::get<2>(*joint));

            break;
        }
        default:
            break;
        }

        // ������߶��ӣ����������ƣ���Ϊ�����ڣ�ɾ���߶��ӵ����ƣ�������ظ�
        if (sevenPairsFlag && readyHands.size() > 1)
            readyHands.erase(readyHands.begin());
        return readyHands;
    }

    /// <summary>
    /// ��������
    /// </summary>
    /// <param name="frontBlock">ǰ��</param>
    /// <param name="followBlock">���</param>
    /// <returns>���Ӻ�����顢���Ӻ�Ŀ顢�������ӵ���</returns>
    [[nodiscard]] std::shared_ptr<std::tuple<TileList, Block, Tile>> JointBlocks(const Block frontBlock, const Block followBlock) const
    {
        // �ж����ӵ������Ƿ�����
        if (followBlock.Loc - frontBlock.LastLoc() != 1)
            return nullptr;
        // ���ԭ�����������м䲻�Ǹ�һ�ţ�������
        if (GetRelation(Hands, frontBlock.LastLoc()) != 2)
            return nullptr;
        // ��ʱ��¼�м�����ƣ���������ƣ�
        auto tempReadyHands = Tile(Hands[frontBlock.LastLoc()].Value + 1);
        // ��ʱ�����жϵ�����

        // �����鲻������������
        auto jointedBlock = Block(0, frontBlock.Len + 1 + followBlock.Len);
        // ���Ƹò�������������
        auto jointedHands = TileList(Hands.begin() + frontBlock.Loc, Hands.begin() + jointedBlock.Len - 1);
        // ����һ���м������
        jointedHands.insert(jointedHands.begin() + frontBlock.Len, tempReadyHands);
        return std::make_shared<std::tuple<TileList, Block, Tile>>(jointedHands, jointedBlock, tempReadyHands);
    }

    /// <summary>
    /// �߶������ж�
    /// </summary>
    /// <returns>������</returns>
    [[nodiscard]] std::shared_ptr<Tile> SevenPairsJudge() const
    {
        // ������ĵ���
        auto single = false;
        // �õ�����λ��
        auto singleTile = 0;
        // �ж���ͬ�������Ĺ�ϵ
        for (auto i = 0; i < 12; ++i)
            // ���ż��λ��ϵ��Ӧ������ͬ��������λ����������ϵ�����ֵ��ţ�
            if (((i + (single ? 1 : 0)) % 2 ^ (GetRelation(Hands, i) > 0 ? 1 : 0)) > 0) {
                // ֱ����������޷��ų����߶�
                // �����������ϵ����ͬ���������߶ԣ����֮ǰ�Ѿ��е����ˣ������߶���
                if (GetRelation(Hands, i) == 0 || single)
                    return nullptr;

                single = true;
                singleTile = Hands[i].Value;
            }

        // ���û�鵽����
        if (!single)
            // �ǵ��ž������һ��
            singleTile = Hands[12].Value;
        // ����һ��
        return std::make_shared<Tile>(singleTile);
    }

    /// <summary>
    /// ��ʿ�����ж�
    /// </summary>
    /// <returns>����</returns>
    [[nodiscard]] TileList ThirteenOrphansJudge() const
    {
        // �Ƿ�ȱ��ĳ���۾��ƣ�0��1��
        auto isMissing = false;
        // �Ƿ����ĳ���۾��ƣ�0��1��
        auto isRedundant = false;
        // ȱ���۾���
        auto missingTile = 0;
        auto readyHands = TileList();
        // �ж�ʮ�����۾��Ƶ�ӵ�����
        for (auto i = 0; i < 13; ++i) {
            const auto offset = (isMissing ? 1 : 0) - (isRedundant ? 1 : 0);
            // ���������ӳ���۾���һ��
            if (Hands[i].Value == (i + offset - 1) / 8) {
                // ���֮ǰ�Ѿ���һ�������
                if (isRedundant)
                    return readyHands;
                // ��¼�ж���
                isRedundant = true;
            }
            // ���������ӳ���۾���һ��
            else if (Hands[i].Value == (i + offset + 1) / 8) {
                // ���֮ǰ�Ѿ���һ��ȱ�����ǹ�ʿ�������¼ȱ��
                if (isMissing)
                    return readyHands;
                isMissing = true;
                missingTile = i / 8;
            }
            // �в����۾��Ƽ������Ϲ�ʿ
            else if (Hands[i].Value != (i + offset) / 8)
                return readyHands;
        }

        // ���ж��ţ�����һ������һ�棨���У�����Ϊ��������󲻻ᱻredundancy��¼��
        if (isRedundant)
            readyHands.emplace_back(isMissing ? missingTile : 96);
        // ����ȱ�������ʮ����
        else
            for (auto i = 0; i < 13; ++i)
                readyHands.emplace_back(i / 8);
        return readyHands;
    }

    /// <summary>
    /// ��ȡ�ֿ�
    /// </summary>
    /// <exception cref="ArgumentOutOfRangeException"></exception>
    /// <returns>�������Ŀ��������3����</returns>
    std::vector<Block> GetBlocks(std::vector<Block>& blocks) const
    {
        auto errBlocks = std::vector<Block>();
        blocks.reserve(6);
        auto tempLoc = 0;
        for (auto i = 0; i < static_cast<int>(Hands.size()) - 1; ++i)
            // ����ϵ������ͬ������
            if (GetRelation(Hands, i) > 1) {
                blocks.emplace_back(tempLoc, i - tempLoc + 1);
                // ��������ǲ��������¼
                if (blocks.back().Integrity != Block::IntegrityType::Type0)
                    errBlocks.emplace_back(blocks.back());
                // ������Ŵﵽ(6 - ��¶��)����4����������������
                if (blocks.size() + Melds.size() == 6 || errBlocks.size() == 4)
                    return {};
                // ��һ�飬�������ǿ��������Ƶ����
                tempLoc = i + 1;
            }
        // ���һ��ļ�¼����ʵ��
        {
            blocks.emplace_back(tempLoc, Hands.size() - tempLoc);
            if (blocks.back().Integrity != Block::IntegrityType::Type0)
                errBlocks.emplace_back(blocks.back());
            if (errBlocks.size() == 4)
                return {};
        }
        // ͨ��������Lv.1�Ŀ飬ɸѡ������Lv.2������һ�鲻��������Ϊ�������ͼӰ벻�����ͣ�����һ��������
        for (auto& block : blocks) {
            if (block.Integrity == Block::IntegrityType::Type0
                && !block.IntegrityJudge(Hands)) {
                if (errBlocks.size() != 4) {
                    errBlocks.emplace_back(block);
                    // ������
                    errBlocks.emplace_back(0);
                    errBlocks.emplace_back(0);
                } else
                    return {};
            }
        }
        return errBlocks;
    }
};
