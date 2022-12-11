using Cs.Interfaces;

namespace Cs.Models;

/// <summary>
/// 组（其中牌关系只有相同）
/// </summary>
/// <param name="Loc">组内首张牌的序号</param>
/// <param name="Len">组内牌数</param>
public record Group(int Loc, int Len) : ISequential
{
    /// <summary>
    /// 已确认（顺/刻）的牌数量
    /// </summary>
    public int Confirmed { get; set; }
}
