#pragma once
namespace MahjongHelper {
/// <summary>
/// �飨�����ƹ�ϵֻ����ͬ��
/// </summary>
public
ref class Group sealed {
public:
    /// <summary>
    /// ���������Ƶ����
    /// </summary>
    const int Loc;
    /// <summary>
    /// ��������
    /// </summary>
    int Len = 1;
    /// <summary>
    /// ��ȷ�ϣ�˳/�̣���������
    /// </summary>
    int Confirmed;
    Group(const int loc)
        : Loc { loc }
    {
    }
};
}