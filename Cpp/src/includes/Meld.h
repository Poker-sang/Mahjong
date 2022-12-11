#pragma once
#include <vector>

#include "MeldType.h"
#include "Tile.h"
#include "Wind.h"

class Meld {
public:
    /// <summary>
    /// 记录副露
    /// </summary>
    TileList Tiles;

    /// <summary>
    /// 记录副露类型
    /// </summary>
    MeldType Mode { MeldType::NullMeld };

    /// <summary>
    /// 存放牌来源方
    /// </summary>
    Wind Income;

    explicit Meld(const Wind income)
        : Income { income }
    {
        Tiles.reserve(4);
    }
};
