#pragma once

/// <summary>
/// �飨�����ƹ�ϵֻ����ͬ��
/// </summary>
struct Group {
    explicit Group(const int loc, const int len)
        : Loc { loc }
        , Len { len }
    {
    }

    /// <summary>
    /// ���������Ƶ����
    /// </summary>
    int Loc;

    /// <summary>
    /// ��������
    /// </summary>
    int Len;

    /// <summary>
    /// ��ȷ�ϣ�˳/�̣���������
    /// </summary>
    int Confirmed { 0 };
};