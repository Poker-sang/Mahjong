#pragma once
#include "Group.h"
#include "OpponentHelper.h"

namespace MahjongHelper {
using namespace System::Collections::Generic;
/// <summary>
/// �飨�����ƹ�ϵֻ����ͬ��������
/// </summary>
public
ref class Block sealed {
public:
    /// <summary>
    /// ���ͣ��棨3n��Ϊ�����ͣ���������������ɣ�����Ϊ�������ͣ�����ȸͷ�������������ӣ���
    /// </summary>
    enum class EIntegrityType {
        /// <summary>
        /// �����ͣ�3n��
        /// </summary>
        Type0,
        /// <summary>
        /// ȸ�治�����ͻ�벻�����ͣ�3n+1��
        /// </summary>
        Type1,
        /// <summary>
        /// ȸͷ�������ͻ����Ӳ������ͣ�3n+2��
        /// </summary>
        Type2,
        /// <summary>
        /// ȸ�벻�����ͣ�3n��
        /// </summary>
        TypeEx
    } Integrity
        = EIntegrityType::Type0;

    /// <summary>
    /// ��������������һ�ţ�
    /// </summary>
    int Len = 1;

    /// <summary>
    /// ���������Ƶ����
    /// </summary>
    int const FirstLoc;

    int LastLoc() { return FirstLoc + Len - 1; }

    /// <summary>
    /// ���ͣ��棨3n��Ϊ�����ͣ���������������ɣ�����Ϊ�������ͣ�����ȸͷ�������������ӣ���
    /// </summary>
    bool Type = true;

    Block(const int loc)
        : FirstLoc { loc }
    {
    }

    /// <summary>
    /// ��÷���
    /// </summary>
    /// <param name="hands"></param>
    /// <returns></returns>
    List<Group ^> ^ GetGroups(List<Tile ^> ^ hands) {
        auto groups = new List<Group ^>();
        groups->Add(new Group(FirstLoc));
        //�жϿ���ÿ����ϵ
        for (auto i = FirstLoc; i < FirstLoc + Len - 1; ++i)
            //����ϵ�����������¼��һ����
            if (GetRelation(hands, i) == 1) {
                Last(groups)->Len = i + 1 - Last(groups)->Loc;
                groups->Add(new Group(i + 1));
            }
        Last(groups)->Len = FirstLoc + Len - Last(groups)->Loc;
        return groups;
    }

        /// <summary>
        /// ÿ���Ƶ����ͣ�˳��̣�ȸͷ���ڿ̣���
        /// </summary>
        enum class ETileType { Sequence,
            Triplet };
    /// <summary>
    /// ɸѡ������Lv.2
    /// </summary>
    /// <param name="hands">�жϵ�����</param>
    /// <param name="eyesLoc">ȸͷ����ţ�-1Ϊû��ȸͷ��</param>
    bool IntegrityJudge(List<Tile ^> ^ hands, int eyesLoc)
    {
        auto groups = GetGroups(hands);

        //�ڴ�ʱû�ã����ں������ʱ���õ�
        auto blockTiles = new array<ETileType>(Len);
        for (auto i = 0; i < blockTiles->Length; ++i)
            blockTiles[i] = ETileType::Sequence;
        //����ȸͷ����ȸͷ��Ϊ�ǿ�
        if (eyesLoc != -1) {
            ++groups[eyesLoc]->Confirmed;
            ++groups[eyesLoc]->Confirmed;
            blockTiles[groups[eyesLoc]->Loc - FirstLoc] = ETileType::Triplet;
            blockTiles[groups[eyesLoc]->Loc - FirstLoc + 1] = ETileType::Triplet;
        }
        //ÿ��ѭ����¼һ����
        for (auto i = 0; i < groups->Count; ++i) {
            //��������
            switch (groups[i]->Len - groups[i]->Confirmed) {
                //�պ�ȫ��ȷ��
            case 0:
                continue;
                //����˳��ȷ������2��ֱ���1����˳
            case 1:
                if (groups->Count > i + 2) {
                    ++groups[i + 1]->Confirmed;
                    ++groups[i + 2]->Confirmed;
                    continue;
                }
                break;
                //����˳��ȷ������2��ֱ���2����˳
            case 2:
                if (groups->Count > i + 2) {
                    ++groups[i + 1]->Confirmed;
                    ++groups[i + 1]->Confirmed;
                    ++groups[i + 2]->Confirmed;
                    ++groups[i + 2]->Confirmed;
                    continue;
                }
                break;
                // 3��1˳��ȷ������2��ֱ���1����˳
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
                // 3���ǿ�
            case 3:
                blockTiles[groups[i]->Loc - FirstLoc] = ETileType::Triplet;
                blockTiles[groups[i]->Loc - FirstLoc + 1] = ETileType::Triplet;
                blockTiles[groups[i]->Loc - FirstLoc + 2] = ETileType::Triplet;
                continue;
                //�����Ǹ���
            default:
                break;
            }
            Integrity = eyesLoc == -1 ? EIntegrityType::TypeEx : EIntegrityType::Type2;
            return false;
        }
        return true;
    }

    /// <summary>
    /// ȥ�Ժ�������ȸͷ�����ͣ�
    /// </summary>
    /// <param name="hands">�жϵ�����</param>
    /// <returns>�Ƿ�����</returns>
    bool IgnoreEyesJudge(List<Tile ^> ^ hands)
    {
        for (int i = FirstLoc, tempGroupNum = 0; i < FirstLoc + Len - 1; ++i) {
            //����ϵ����������������һ
            if (GetRelation(hands, i) == 1)
                ++tempGroupNum;
            //����ϵ����ͬ������ȸͷ�����ͣ�������
            else if (IntegrityJudge(hands, tempGroupNum))
                return true;
        }
        return false;
    }
    /// <summary>
    /// ����
    /// </summary>
    /// <param name="hands">�жϵ�����</param>
    /// <param name="mode">�Ƿ�Ҫȥ�ԣ���Ϊȸ�治�����ͣ���Ϊ���Ӳ������ͣ�</param>
    /// <returns>�����ƣ����ܱ������Ͳ�Ϊ�գ����������ﲻӰ�죨�������ʱ���ܸĶ���</returns>
    List<Tile ^> ^ Traversal(List<Tile ^> ^ hands, const bool mode) {
        //���ܵ�������
        auto first = hands[FirstLoc]->Val - 1;
        //���������һ��Ͳ���������ƣ���firstû��ǰһ�ţ��ӻ�hands[loc]
        if ((hands[FirstLoc]->Val & 15) == 0 || hands[FirstLoc]->Val / 8 > 5)
            ++first;
        //���ܵ�ĩ����
        auto last = hands[FirstLoc + Len - 1]->Val + 1;
        //���ĩ���Ǿ���Ͳ���������ƣ����lastû�к�һ�ţ�����hands[loc]
        if ((hands[FirstLoc + Len - 1]->Val & 15) == 8 || hands[FirstLoc + Len - 1]->Val / 8 > 5)
            --last;
        auto tempBlock = new Block(0);
        tempBlock->Len = Len + 1;
        auto tempTile = first;
        auto tempReturn = new List<Tile ^>();
        //ÿ���ƶ����볢��һ�Σ�������
        for (auto i = 0; i < last - first + 1; ++i, ++tempTile) {
            auto tempHands = new List<Tile ^>();
            //���¸���������
            for (auto j = FirstLoc; j < FirstLoc + Len; ++j)
                tempHands->Add(new Tile(hands[j]->Val));
            //���볢�Ե���
            TileIn(tempHands, new Tile(tempTile));
            //ȸ�治�������ұ�����ȥ�Ժ�������������
            //���Ӳ��������ұ�����������������
            if (mode && tempBlock->IgnoreEyesJudge(tempHands) || !mode && tempBlock->IntegrityJudge(tempHands, -1))
                tempReturn->Add(new Tile(tempTile));
        }
        return tempReturn;
    }
};
}
