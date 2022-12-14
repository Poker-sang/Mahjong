#pragma once
#include "Opponent.h"
namespace MahjongHelper {
using namespace System::Collections::Generic;
generic<typename T> T Last(List<T> ^ list) { return list[list->Count - 1]; }
/// <summary>
/// �������Ƽ��ϵ
/// </summary>
/// <param name="hands">����</param>
/// <param name="num">ǰ�������</param>
static int GetRelation(List<Tile ^> ^ hands, const int num)
{
    try {
        return hands[num + 1]->Val - hands[num]->Val;
    } catch (System::Exception ^) {
        //�������������
        return System::Int32::MaxValue;
    }
}

/// <summary>
/// ����
/// </summary>
/// <param name="hands">����</param>
/// <param name="tile">����</param>
/// <returns>�����Ƶ�λ��</returns>
static int TileIn(List<Tile ^> ^ hands, Tile ^ tile)
{
    auto ru = 0;
    //�ҵ����Ų����λ��
    while (ru < hands->Count && tile->Val > hands[ru]->Val)
        ++ru;
    hands->Insert(ru, tile);
    return ru;
}
}