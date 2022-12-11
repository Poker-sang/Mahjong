namespace Cs.Interfaces;

public interface ISequential
{
    /// <summary>
    /// 牌数（至少一张）
    /// </summary>
    public int Len { get; }

    /// <summary>
    /// 首张牌的序号
    /// </summary>
    public int Loc { get; }
}
