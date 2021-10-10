#pragma once
/// <summary>
/// 组（其中牌关系只有相同）
/// </summary>
class group
{
public:
	explicit group(const int loc) :loc{ loc } {}
	/// <summary>
	/// 组内首张牌的序号
	/// </summary>
	const int loc{ 0 };
	/// <summary>
	/// 组内牌数
	/// </summary>
	int len{ 1 };
	/// <summary>
	/// 已确认（顺/刻）的牌数量
	/// </summary>
	int confirmed{ 0 };
};