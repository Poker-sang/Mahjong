#pragma once
#include <vector>

#include "Group.h"
#include "OpponentHelper.h"
#include "Tile.h"

/// <summary>
/// �飨�����ƹ�ϵֻ����ͬ��������
/// </summary>
class Block {
public:
    /// <summary>
    /// ���������Ƶ����
    /// </summary>
    int Loc { 0 };

    /// <summary>
    /// ��������������һ�ţ�
    /// </summary>
    int Len { 1 };

    /// <summary>
    /// ��������
    /// </summary>
    enum class IntegrityType {
        /// <summary>
        /// ������
        /// </summary>
        /// <remarks>3n</remarks>
        Type0,

        /// <summary>
        /// ȸ�治�����ͻ�벻������
        /// </summary>
        /// <remarks>3n+1</remarks>
        Type1,

        /// <summary>
        /// ȸͷ�������ͻ����Ӳ�������
        /// </summary>
        /// <remarks>3n+2</remarks>
        Type2,

        /// <summary>
        /// ȸ�벻������
        /// </summary>
        /// <remarks>3n</remarks>
        TypeEx
    };

    /// <summary>
    /// ������
    /// </summary>
    IntegrityType Integrity;

    /// <summary>
    /// ����ĩ���Ƶ����
    /// </summary>
    [[nodiscard]] int LastLoc() const { return Loc + Len - 1; }

    explicit Block(const int loc, const int len = -1)
        : Loc { loc }
        , Len { len }
    {
        Integrity = len == -1 ? IntegrityType::TypeEx : static_cast<IntegrityType>(len % 3);
    }

    /// <summary>
    /// ɸѡ������Lv.2
    /// </summary>
    /// <param name="hands">�жϵ�����</param>
    /// <param name="eyesLoc">ȸͷ����ţ�-1Ϊû��ȸͷ��</param>
    bool IntegrityJudge(const TileList& hands, const int eyesLoc = -1)
    {
        auto groups = GetGroups(hands);

        // �ڴ˴�û�ã����ں������ʱ���õ�
        std::vector blockTiles(Len, TileType::Sequence);
        // ����ȸͷ����ȸͷ��Ϊ�ǿ�
        if (eyesLoc != -1) {
            ++groups[eyesLoc].Confirmed;
            ++groups[eyesLoc].Confirmed;
            blockTiles[groups[eyesLoc].Loc - Loc] = TileType::Triplet;
            blockTiles[groups[eyesLoc].Loc - Loc + 1] = TileType::Triplet;
        }
        // ÿ��ѭ����¼һ����
        for (auto i = 0; i < static_cast<int>(groups.size()); ++i) {
            // ��������
            switch (groups[i].Len - groups[i].Confirmed) {
            // �պ�ȫ��ȷ��
            case 0:
                continue;
            // ����˳��ȷ������2��ֱ���1����˳
            case 1:
                if (static_cast<int>(groups.size()) > i + 2) {
                    ++groups[i + 1].Confirmed;
                    ++groups[i + 2].Confirmed;
                    continue;
                }
                break;
            // ����˳��ȷ������2��ֱ���2����˳
            case 2:
                if (static_cast<int>(groups.size()) > i + 2) {
                    ++groups[i + 1].Confirmed;
                    ++groups[i + 1].Confirmed;
                    ++groups[i + 2].Confirmed;
                    ++groups[i + 2].Confirmed;
                    continue;
                }
                break;
            // 3��1˳��ȷ������2��ֱ���1����˳
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
            // 3���ǿ�
            case 3:
                blockTiles[groups[i].Loc - Loc] = TileType::Triplet;
                blockTiles[groups[i].Loc - Loc + 1] = TileType::Triplet;
                blockTiles[groups[i].Loc - Loc + 2] = TileType::Triplet;
                continue;
            // �����Ǹ���
            default:
                break;
            }
            Integrity = eyesLoc == -1 ? IntegrityType::TypeEx : IntegrityType::Type2;
            return false;
        }
        return true;
    }

    /// <summary>
    /// ȥ�ԣ�ȸͷ�����ͣ�
    /// </summary>
    /// <param name="hands">�жϵ�����</param>
    /// <returns>�Ƿ�����</returns>
    bool IgnoreEyesJudge(const TileList& hands)
    {
        for (auto i = Loc, tempGroupNum = 0; i < Loc + Len - 1; ++i) {
            // ����ϵ����������������һ
            if (GetRelation(hands, i) == 1)
                ++tempGroupNum;
            // ����ϵ����ͬ������ȸͷ�����ͣ�������
            else if (IntegrityJudge(hands, tempGroupNum) == true)
                return true;
        }
        return false;
    }

    /// <summary>
    /// ����
    /// </summary>
    /// <param name="hands">�жϵ�����</param>
    /// <param name="mode">��Ϊ���Ӳ������ͣ���Ϊȫ��������</param>
    /// <returns>�Ƿ�����</returns>
    [[nodiscard]] TileList Traversal(const TileList& hands, const bool mode) const
    {
        // ���ܵ�������
        auto first = hands[Loc].Value - 1;
        // ���������һ��Ͳ���������ƣ���firstû��ǰһ��
        if ((hands[Loc].Value & 15) == 0 || hands[Loc].Value / 8 > 5)
            ++first;
        // ���ܵ�ĩ����
        auto last = hands[Loc + Len - 1].Value + 1;
        // ���ĩ���Ǿ���Ͳ���������ƣ����lastû�к�һ��
        if ((hands[Loc + Len - 1].Value & 15) == 8 || hands[Loc + Len - 1].Value / 8 > 5)
            --last;
        auto tempBlock = Block(0, Len + 1);
        auto tempTile = first;
        auto readyHands = TileList();
        // ÿ���ƶ����볢��һ�Σ�������
        for (auto i = 0; i < last - first + 1; ++i, ++tempTile) {
            // ���¸���������
            auto tempHands = std::vector(hands.begin() + Loc, hands.begin() + Loc + Len);
            // ���볢�Ե���
            TileIn(tempHands, Tile(tempTile));
            if (mode // ȸ�治�������ұ�����ȥ�Ժ�������������
                    ? tempBlock.IgnoreEyesJudge(tempHands)
                    // ���Ӳ��������ұ�����������������
                    : tempBlock.IntegrityJudge(tempHands))
                readyHands.emplace_back(tempTile);
        }
        return readyHands;
    }

private:
    /// <summary>
    /// ��÷���
    /// </summary>
    /// <param name="hands"></param>
    /// <returns></returns>
    [[nodiscard]] std::vector<Group> GetGroups(const TileList& hands) const
    {
        auto tempLoc = Loc;
        auto group = std::vector<Group>();
        // �жϿ���ÿ����ϵ
        for (auto i = Loc; i < Loc + Len - 1; ++i)
            // ����ϵ�����������¼��һ����
            if (GetRelation(hands, i) == 1) {
                group.emplace_back(tempLoc, i + 1 - tempLoc);
                tempLoc = i + 1;
            }
        group.emplace_back(tempLoc, Loc + Len - tempLoc);
        return group;
    }

    /// <summary>
    /// ÿ���Ƶ�����
    /// </summary>
    enum class TileType {
        /// <summary>
        /// ˳
        /// </summary>
        Sequence,

        /// <summary>
        /// �̻�ȸͷ
        /// </summary>
        Triplet
    };
};