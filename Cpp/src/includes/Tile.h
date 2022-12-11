#pragma once

struct Tile {
    /// <summary>
    /// 存放牌信息
    /// </summary>
    int Value;

    explicit Tile(const int value)
        : Value { value }
    {
    }
};

typedef std::vector<Tile> TileList;
