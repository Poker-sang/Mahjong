#pragma once

/// <summary>
/// 副露（标志）
/// </summary>
enum class MeldType
{
	/// <summary>
	/// 无
	/// </summary>
	NullMeld = 0b0,

	/// <summary>
	/// 吃
	/// </summary>
	Chow = 0b1,

	/// <summary>
	/// 碰
	/// </summary>
	Pong = 0b10,

	/// <summary>
	/// 加杠
	/// </summary>
	ExposedKongFromExposedPong = 0b100,

	/// <summary>
	/// 大明杠
	/// </summary>
	ExposedKong = 0b1000,

	/// <summary>
	/// 暗杠
	/// </summary>
	ConcealedKong = 0b10000,

	/// <summary>
	/// 立直
	/// </summary>
	Riichi = 0b100000,

	/// <summary>
	/// 流局
	/// </summary>
	Draw = 0b1000000,

	/// <summary>
	/// 荣和
	/// </summary>
	CallMahjong = 0b10000000,

	/// <summary>
	/// 自摸
	/// </summary>
	SelfDraw = 0b100000000,

	/// <summary>
	/// 打牌
	/// </summary>
	Discard = 0b1000000000,

	/// <summary>
	/// 抓牌
	/// </summary>
	DrawTile = 0b10000000000
};