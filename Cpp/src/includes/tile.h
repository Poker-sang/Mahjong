#pragma once

class tile
{
private:
	const int value_;
public:
	/// <summary>
	/// 存放牌信息
	/// </summary>
	explicit tile(const int value) : value_{ value } {}
	explicit operator int() const { return value_; }
	friend void swap(tile*& x, tile*& y) noexcept { tile* z{ x }; x = y; y = z; }		// 转移指针
};