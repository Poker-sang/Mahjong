using System;

namespace Cs.Enums;

[Flags]
public enum MeldType
{
    NullMeld = 0b0,
    Chow = 0b1,
    Pong = 0b10,
    Jia = 0b100,
    DaMing = 0b1000,
    An = 0b10000,
    Riichi = 0b100000,
    Draw = 0b1000000,
    CallMahjong = 0b10000000,
    ZiMo = 0b100000000,
    Da = 0b1000000000,
    Mo = 0b10000000000
}
