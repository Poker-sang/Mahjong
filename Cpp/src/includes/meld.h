#pragma once
#include <vector>

#include "e_meld.h"
#include "e_wind.h"
#include "tile.h"

class meld
{
public:
	/// <summary>
	/// 记录副露
	/// </summary>
	std::vector<tile*> tiles;
	/// <summary>
	/// 记录副露类型
	/// </summary>
	e_meld mode{ e_meld::null_meld };
	/// <summary>
	/// 存放牌来源方
	/// </summary>
	e_wind income;
	/// <summary>
	/// 分配空间
	/// </summary>
	explicit meld(const e_wind income_wind) :income{ income_wind } { tiles.reserve(4); }
	/// <summary>
	/// 直接访问牌元素
	/// </summary>
	tile*& operator[](const int num) { return tiles[num]; }
};
