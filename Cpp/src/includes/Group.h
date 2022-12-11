#pragma once

/// <summary>
/// 组（其中牌关系只有相同）
/// </summary>
struct Group {
    explicit Group(const int loc, const int len)
        : Loc { loc }
        , Len { len }
    {
    }

    /// <summary>
    /// 组内首张牌的序号
    /// </summary>
    int Loc;

    /// <summary>
    /// 组内牌数
    /// </summary>
    int Len;

    /// <summary>
    /// 已确认（顺/刻）的牌数量
    /// </summary>
    int Confirmed { 0 };
};