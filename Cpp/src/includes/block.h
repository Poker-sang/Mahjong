#pragma once

#include "group.h"
#include "opponent.h"
/// <summary>
/// 块（其中牌关系只有相同或连续）
/// </summary>
class block
{
public:
	/// <summary>
	/// 块内首张牌的序号
	/// </summary>
	int loc{ 0 };
	/// <summary>
	/// 块内牌数（至少一张）
	/// </summary>
	int len{ 1 };
	/// <summary>
	/// 类型（真（3n）为完整型（由整数个面子组成），假为不完整型（含有雀头、不完整的面子））
	/// </summary>
	bool type{ true };
	explicit block(const int loc) : loc{ loc } {}
	/// <summary>
	/// 筛选完整型Lv.2
	/// </summary>
	/// <param name="op">判断的牌组</param>
	/// <param name="eyes_loc">雀头的序号（-1为没有雀头）</param>
	bool block_complete(opponent& op, const int eyes_loc = -1)
	{
		std::vector<group> group;
		group.emplace_back(loc);
		{
			auto i{ loc };
			for (; i < loc + len - 1; ++i)		//判断块内每个关系
				if (op.link(i) == 1)		//当关系是连续
					{
					group.back().len = i + 1 - group.back().loc;
					group.emplace_back(i + 1);		//记录多一个组
					}
			group.back().len = i + 1 - group.back().loc;
		}
		const int group_num{ group.size() };		//包含的组数

		std::vector<bool> block_card(len, true);		//判断块内每张牌，若该牌属于顺子即记为顺（true），否则记为刻（flase）
		if (eyes_loc != -1)		//有雀头
		{
			++++group[eyes_loc].confirmed;		//雀头处有两个刻
			block_card[group[eyes_loc].loc - loc] = false;
			block_card[group[eyes_loc].loc - loc + 1] = false;
		}
		for (auto i{ 0 }; i < group_num; ++i)		//每次循环记录一个组
		{
			switch (group[i].len - group[i].confirmed)		//该组牌数
			{
			case 0:
				continue;		//开始下一循环
			case 1:		//都是顺
				if (group_num > i + 2)
				{
					++group[i + 1].confirmed;		//确定后面几组有2张是顺
					++group[i + 2].confirmed;
					continue;
				}
				break;
			case 2:		//都是顺
				if (group_num > i + 2)
				{
					++++group[i + 1].confirmed;		//确定后面几组有2张是顺
					++++group[i + 2].confirmed;
					continue;
				}
				break;
			case 4:		//3刻1顺
				if (group_num > i + 2)
				{
					++group[i + 1].confirmed;		//确定后面几组有2张是顺
					++group[i + 2].confirmed;
					block_card[group[i].loc - loc] = false;		//三张是刻
					block_card[group[i].loc - loc + 1] = false;
					block_card[group[i].loc - loc + 2] = false;
					continue;
				}
				break;
			case 3:		//都是刻
				block_card[group[i].loc - loc] = false;		//三张是刻
				block_card[group[i].loc - loc + 1] = false;
				block_card[group[i].loc - loc + 2] = false;
				continue;
			default:		//可能是负数
				break;
			}
			type = false;		//不完整型
			return false;
		}
		return true;
	}
	/// <summary>
	/// 去对（雀头完整型）
	/// </summary>
	/// <param name="op">判断的牌组</param>
	/// <returns>是否听牌</returns>
	bool block_ignore_pair(opponent& op)
	{
		for (auto i{ loc }, temp_group_num{ 0 }; i < loc + len - 1; ++i)
		{
			if (op.link(i) == 1)		//当关系是连续
				++temp_group_num;		//组数加一
			else if (block_complete(op, temp_group_num) == true)		//当关系是相同，判断是否是雀头完整型
				return true;		//该组是雀头完整型
		}
		return false;		//不完整
	}
	/// <summary>
	/// 遍历
	/// </summary>
	/// <param name="op">判断的牌组</param>
	/// <param name="mode">假为面子不完整型，真为全不完整型</param>
	/// <returns>是否听牌</returns>
	bool block_traversal(opponent& op, const bool mode) const
	{
		auto first{ static_cast<int>(*op[loc]) - 1 };		//可能的首张牌
		if ((static_cast<int>(*op[loc]) & 15) == 0 || static_cast<int>(*op[loc]) >> 3 > 5)		//如果首张是一万、筒、索或字牌
			++first;		//first没有前一张，加回op[loc]
		auto last = static_cast<int>(*op[loc + len - 1]) + 1;		//可能的末张牌
		if ((static_cast<int>(*op[loc + len - 1]) & 15) == 8 || static_cast<int>(*op[loc + len - 1]) >> 3 > 5)		//如果末张是九万、筒、索或字牌
			--last;		//last没有后一张，减回op[loc]
		block temp_block{ 0 };		//新牌组对应的块
		temp_block.len = this->len + 1;
		auto temp_tile{ first };
		auto temp{ 0 };
		opponent temp_op;
		for (int i{ 0 }; i < last - first + 1; ++i, ++temp_tile)		//每张牌都插入尝试一次（遍历）
		{
			for (int j{ loc }; j < loc + len; ++j)		//重新复制所有牌
				temp_op.hands.emplace_back(op[j]);
			temp = temp_op << new tile(temp_tile);		//插入尝试的牌
			if (mode && temp_block.block_ignore_pair(temp_op) 		//（真）全不完整型且遍历、去对后完整
				|| !mode && temp_block.block_complete(temp_op))		//（假）面子不完整型且遍历后完整
				op.ready_hands.emplace_back(temp_tile);
			delete temp_op[temp];
			temp_op.hands.clear();
		}
		return !op.ready_hands.empty();		//可能本来它就不为空，不过在这里不影响（将来算符时可能改动）
	}
};
