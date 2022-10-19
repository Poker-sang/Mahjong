#pragma once
#include <stdexcept>

#include "meld.h"

class opponent;

class block
{
public:
	int loc;
	int len;
	bool type;
	explicit block(int loc);
	bool block_complete(opponent& op, int eyes_loc = -1);
	bool block_ignore_pair(opponent& op);
	bool block_traversal(opponent& op, bool mode) const;
};
class opponent
{
public:
	/// <summary>
	/// 手牌区
	/// </summary>
	std::vector<tile*> hands;
	/// <summary>
	/// 副露区
	/// </summary>
	std::vector<meld> melds;
	/// <summary>
	/// 牌河（舍张）
	/// </summary>
	std::vector<tile*> discards;
	explicit opponent() { hands.reserve(14); ready_hands.reserve(13); }
	
	tile*& operator[](const int num) { return hands[num]; }		// 直接访问牌元素
	int operator<<(tile* card)		//(进张)摸牌
	{
		int ru{ 0 };
		while (ru < static_cast<int>(hands.size()) && static_cast<int>(*card) >static_cast<int>(*hands[ru]))		// 找到进张插入的位置
			++ru;
		hands.insert(hands.begin() + ru, card);		// 插入牌
		return ru;		// 返回该牌的位置
	}
	int link(const int num)		//(前张牌序号)两张手牌间关系
	{
		try { return static_cast<int>(*hands.at(num + 1)) - static_cast<int>(*hands.at(num)); }
		catch (std::out_of_range) { return 100; }		//（尽量大的数）
	}
	/// <summary>
	/// 是否听牌
	/// </summary>
	bool is_ready_hand{ false };
	/// <summary>
	/// 可以和的牌
	/// </summary>
	std::vector<tile> ready_hands;
	/// <summary>
	/// 听牌判断（在摸牌前判断）
	/// </summary>
	void ting_judge()
	{
		{
			is_ready_hand = false;
			ready_hands.clear();		// 清空铳牌

			if (melds.empty())		// 如果没有副露（特殊牌型判断）
			{		// 国士牌型判断
				auto que{ false };		// 是否缺了某张幺九牌
				auto duo{ false };		// 是否多了某张幺九牌
				auto que_tile{ 0 };		// 缺的幺九牌
				for (auto i{ 0 }; i < 13; ++i)		// 判断十三张幺九牌的拥有情况
					if (static_cast<int>(*hands[i]) == (i + que - duo) << 3);		// 如果刚好位置映射
					else if (static_cast<int>(*hands[i]) == (i + que - duo - 1) << 3)		// 如果和上张映射幺九牌一样
					{
						if (duo)		// 如果已经有一个多的牌
							goto thirteen_orphans;
						else duo = true;		// 记录有多牌
					}
					else if (static_cast<int>(*hands[i]) == (i + que - duo + 1) << 3)		// 如果和下张映射幺九牌一样
					{
						if (que)		// 如果已经有一个缺牌
							goto thirteen_orphans;		// 不是国士
						else que = true;		// 记录有缺牌
						que_tile = i << 3;		// 记录缺的牌
					}
					else goto thirteen_orphans;		// 有不是幺九即无听
				if (duo)		// 若有多张
				{
					if (que)
						ready_hands.emplace_back(que_tile);		// 记听一面
					else ready_hands.emplace_back(96);		// 记听一面（中）（中在最后不会被que记录）
				}
				else for (auto i{ 0 }; i < 13; ++i)		// 若不缺张
					ready_hands.emplace_back(i << 3);		// 记听十三面
				is_ready_hand = true;
				return;
			thirteen_orphans:
				// 七对牌型判断
				auto dan{ false };		// 多出来的单张
				auto dan_tile{ 0 };		// 单张牌位置
				for (auto i{ 0 }; i < 12; ++i)		// 判断关系
					if ((i + dan) % 2 ^ static_cast<int>(static_cast<bool>(link(i))))		// 如果偶数位关系对应不是相同，或奇数位不是其他
					{		// 直接异或运算无法排除龙七对
						if (link(i) == 0)		// 如果这个错误关系是相同
							goto seven_pairs;		// 龙七对
						else if (dan == true)		// 如果已经有单牌了
							goto seven_pairs;		// 不是七对子
						else
						{
							dan = true;		// 记录单张
							dan_tile = static_cast<int>(*hands[i]);		// 记录序号
						}
					}
				if (!dan)		// 如果没查到单张
					dan_tile = static_cast<int>(*hands[12]);		// 那单张就是最后一个
				ready_hands.emplace_back(dan_tile);		// 记听一面
				is_ready_hand = true;
				// 有可能复合二杯口，故听牌后不退出
			}
		seven_pairs:
			auto block_err{ 0 };		// 不完整的块数（最多3个）
			std::vector<block> blocks;		// 块（最多6个）
			blocks.reserve(6);
			blocks.emplace_back(0);		// 第一块
			for (auto i{ 0 }; i < static_cast<int>(hands.size() - 1); ++i)
				if (link(i) > 1)		// 当关系不是相同或连续
				{
					blocks.back().len = i - blocks.back().loc + 1;		// 记录上一块的长度
					blocks.back().type = !(blocks.back().len % 3);		// 筛选完整型Lv.1
					block_err += !blocks.back().type;		// 记录
					if (blocks.size() == 6 - melds.size() || block_err == 4)		// 若块序号达到(6 - 副露数)或有4个不完整型
						return;		// 无听
					blocks.emplace_back(i + 1);		// 括号里是块内首张牌的序号
				}
			blocks.back().len = static_cast<int>(hands.size()) - blocks.back().loc;		// 记录上一块的长度
			blocks.back().type = !(blocks.back().len % 3);		// 筛选完整型Lv.1
			block_err += !blocks.back().type;		// 记录
			if (block_err == 4)		// 若有4个不完整型
				return;		// 无听
			const auto block_num{ static_cast<int>(blocks.size()) };		// 块数
			for (auto num{ 0 }; num < block_num; ++num)
				if (blocks[num].type == true)		// 只判断通过完整型Lv.1的
					if (blocks[num].block_complete(*this) == false)		// 筛选完整型Lv.2，若不完整
						return;		// 无听
			switch (block_err)
			{
			case 1:		// 有一块不完整型（一块全不完整型（3n+4））
			{
				for (auto err_block{ 0 }; err_block < block_num; ++err_block)
					if (blocks[err_block].type == false)		// 是不完整型
					{
						if (blocks[err_block].block_traversal(*this, true))
							break;
						else return;		// 无听
					}
				break;
			}
			case 2:		// 有两块不完整型（一块面子不完整型（3n+2），一块雀头完整型（3n+2））
			{
				int err_block_1{ 0 };
				for (; err_block_1 < block_num; ++err_block_1)
					if (blocks[err_block_1].type == false)		// 是不完整型
						break;		// 记录
				int err_block_2{ err_block_1 + 1 };		// 第二个不完整型
				for (; err_block_2 < block_num; ++err_block_2)
					if (blocks[err_block_2].type == false)		// 是不完整型
						break;		// 记录
				if (err_block_2 >= block_num)		// 如果后者的数字大于等于组数
					return;		// 无听
				bool temp = false;
				// 去对和遍历顺序不能互换，因为如果遍历成功会直接写入铳牌，而去对成功不会写入铳牌，因为&&的短路所以去对写在前面
				if (blocks[err_block_2].block_ignore_pair(*this) && blocks[err_block_1].block_traversal(*this, false)		// 满足任意一个，但是两条语句都要执行
					| blocks[err_block_1].block_ignore_pair(*this) && blocks[err_block_2].block_traversal(*this, false))		// 按位或不会出现短路
					break;		// 听牌
				else return;		// 无听
			}
			case 3:		// 有三块不完整型（两块半不完整型（3n+1），一块雀头完整型（3n+2））
			{
				int err_block{ 0 };
				for (; err_block < block_num; ++err_block)
					if (blocks[err_block].type == false)		// 是不完整型
						if (blocks[err_block].len % 3 == 1)		// 如果牌数是3的倍数+1
							break;		// 记录err_loc
				tile temp_ready_hands{ static_cast<int>(*hands[blocks[err_block].loc + blocks[err_block].len - 1]) + 1 };		// 中间隔的牌（临时记录铳牌）
				if (static_cast<int>(temp_ready_hands) != static_cast<int>(*hands[blocks[err_block + 1].loc]) - 1)		// 如果它和下一张牌不连续
					return;		// 无听
				for (auto eyes_block{ 0 }; eyes_block < block_num; ++eyes_block)
					if (blocks[eyes_block].type == false)		// 是不完整型
						if (blocks[eyes_block].len % 3 == 2)		// 如果牌数是3的倍数+2（雀头）
							if (blocks[eyes_block].block_ignore_pair(*this) == false)		// 如果不是雀头块
								return;		// 无听
				opponent temp_op;		// 临时用来判断的牌组
				block temp_block{ 0 };
				temp_block.len = blocks[err_block].len + 1 + blocks[err_block + 1].len;		// 这两个不完整型总长度+一张中间隔的牌
				int i{ 0 };
				for (; i < blocks[err_block].len; ++i)
					temp_op[i] = hands[blocks[err_block].loc + i];		// 复制该不完整型
				temp_op[i] = &temp_ready_hands;		// 记录中间的牌
				for (; i < temp_block.len; ++i)
					temp_op[i] = hands[blocks[err_block].loc + i];		// 复制该不完整型
				if (temp_block.block_complete(temp_op) == true)		// 如果该牌组完整
					ready_hands.emplace_back(temp_ready_hands);		// 记听一面
				break;
			}
			default:;
			}
			is_ready_hand = true;		// 听牌
		}
	}
};