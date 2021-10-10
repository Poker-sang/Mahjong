#pragma once
/// <summary>
/// 副露（离散）
/// </summary>
enum class e_meld
{
	/// <summary>
	/// 无
	/// </summary>
	null_meld = 0b1,
	/// <summary>
	/// 
	/// </summary>
	chow = 0b10,
	/// <summary>
	/// 碰
	/// </summary>
	pong = 0b100,
	/// <summary>
	/// 加杠
	/// </summary>
	exposed_kong_from_exposed_pong = 0b1000,
	/// <summary>
	/// 大明杠
	/// </summary>
	exposed_kong = 0b10000,
	/// <summary>
	/// 暗杠
	/// </summary>
	concealed_kong = 0b100000,
	/// <summary>
	/// 立直
	/// </summary>
	riichi = 0b1000000,
	/// <summary>
	/// 流局
	/// </summary>
	draw = 0b10000000,
	/// <summary>
	/// 荣和
	/// </summary>
	calling_mahjong = 0b100000000,
	/// <summary>
	/// 自摸
	/// </summary>
	self_draw = 0b1000000000,
	/// <summary>
	/// 打牌
	/// </summary>
	discard = 0b10000000000,
	/// <summary>
	/// 抓牌
	/// </summary>
	draw_tile = 0b100000000000
};