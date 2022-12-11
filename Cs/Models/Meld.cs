using System.Collections;
using System.Collections.Generic;
using Cs.Enums;

namespace Cs.Models;

/// <summary>
/// 副露
/// </summary>
public class Meld : IEnumerable<Tile>
{
    /// <summary>
    /// 记录副露
    /// </summary>
    public List<Tile> Tiles { get; } = new(4);

    /// <summary>
    /// 记录副露类型
    /// </summary>
    public MeldType Mode { get; set; } = MeldType.NullMeld;

    /// <summary>
    /// 存放牌来源方
    /// </summary>
    public Wind? Income { get; }

    /// <summary>
    /// 直接访问牌元素
    /// </summary>
    public Tile this[int index] { get => Tiles[index]; set => Tiles[index] = value; }

    public Meld(Wind? income) => Income = income;

    public IEnumerator<Tile> GetEnumerator() => Tiles.GetEnumerator();

    IEnumerator IEnumerable.GetEnumerator() => GetEnumerator();
}
