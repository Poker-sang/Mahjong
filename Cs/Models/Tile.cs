using System;

namespace Cs.Models;

/// <summary>
/// </summary>
/// <param name="Val">存放牌信息</param>
public readonly record struct Tile(Tile.TileValue Val)
{
    public int IntVal => (int)Val;

    public override string ToString() => Enum.GetName(Val) ?? "错误";

    public static implicit operator TileValue(Tile tile) => tile.Val;

    public static implicit operator Tile(TileValue val) => new(val);

    public enum TileValue
    {
        M1 = 0,
        M2 = 1,
        M3 = 2,
        M4 = 3,
        M5 = 4,
        M6 = 5,
        M7 = 6,
        M8 = 7,
        M9 = 8,
        P1 = 16,
        P2 = 17,
        P3 = 18,
        P4 = 19,
        P5 = 20,
        P6 = 21,
        P7 = 22,
        P8 = 23,
        P9 = 24,
        S1 = 32,
        S2 = 33,
        S3 = 34,
        S4 = 35,
        S5 = 36,
        S6 = 37,
        S7 = 38,
        S8 = 39,
        S9 = 40,
        CEast = 48,
        CSouth = 56,
        CWest = 64,
        CNorth = 72,
        CWhite = 80,
        CGreen = 88,
        CRed = 96
    }
}
