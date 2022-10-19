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
	/// ������
	/// </summary>
	std::vector<tile*> hands;
	/// <summary>
	/// ��¶��
	/// </summary>
	std::vector<meld> melds;
	/// <summary>
	/// �ƺӣ����ţ�
	/// </summary>
	std::vector<tile*> discards;
	explicit opponent() { hands.reserve(14); ready_hands.reserve(13); }
	
	tile*& operator[](const int num) { return hands[num]; }		// ֱ�ӷ�����Ԫ��
	int operator<<(tile* card)		//(����)����
	{
		int ru{ 0 };
		while (ru < static_cast<int>(hands.size()) && static_cast<int>(*card) >static_cast<int>(*hands[ru]))		// �ҵ����Ų����λ��
			++ru;
		hands.insert(hands.begin() + ru, card);		// ������
		return ru;		// ���ظ��Ƶ�λ��
	}
	int link(const int num)		//(ǰ�������)�������Ƽ��ϵ
	{
		try { return static_cast<int>(*hands.at(num + 1)) - static_cast<int>(*hands.at(num)); }
		catch (std::out_of_range) { return 100; }		//�������������
	}
	/// <summary>
	/// �Ƿ�����
	/// </summary>
	bool is_ready_hand{ false };
	/// <summary>
	/// ���Ժ͵���
	/// </summary>
	std::vector<tile> ready_hands;
	/// <summary>
	/// �����жϣ�������ǰ�жϣ�
	/// </summary>
	void ting_judge()
	{
		{
			is_ready_hand = false;
			ready_hands.clear();		// ������

			if (melds.empty())		// ���û�и�¶�����������жϣ�
			{		// ��ʿ�����ж�
				auto que{ false };		// �Ƿ�ȱ��ĳ���۾���
				auto duo{ false };		// �Ƿ����ĳ���۾���
				auto que_tile{ 0 };		// ȱ���۾���
				for (auto i{ 0 }; i < 13; ++i)		// �ж�ʮ�����۾��Ƶ�ӵ�����
					if (static_cast<int>(*hands[i]) == (i + que - duo) << 3);		// ����պ�λ��ӳ��
					else if (static_cast<int>(*hands[i]) == (i + que - duo - 1) << 3)		// ���������ӳ���۾���һ��
					{
						if (duo)		// ����Ѿ���һ�������
							goto thirteen_orphans;
						else duo = true;		// ��¼�ж���
					}
					else if (static_cast<int>(*hands[i]) == (i + que - duo + 1) << 3)		// ���������ӳ���۾���һ��
					{
						if (que)		// ����Ѿ���һ��ȱ��
							goto thirteen_orphans;		// ���ǹ�ʿ
						else que = true;		// ��¼��ȱ��
						que_tile = i << 3;		// ��¼ȱ����
					}
					else goto thirteen_orphans;		// �в����۾ż�����
				if (duo)		// ���ж���
				{
					if (que)
						ready_hands.emplace_back(que_tile);		// ����һ��
					else ready_hands.emplace_back(96);		// ����һ�棨�У���������󲻻ᱻque��¼��
				}
				else for (auto i{ 0 }; i < 13; ++i)		// ����ȱ��
					ready_hands.emplace_back(i << 3);		// ����ʮ����
				is_ready_hand = true;
				return;
			thirteen_orphans:
				// �߶������ж�
				auto dan{ false };		// ������ĵ���
				auto dan_tile{ 0 };		// ������λ��
				for (auto i{ 0 }; i < 12; ++i)		// �жϹ�ϵ
					if ((i + dan) % 2 ^ static_cast<int>(static_cast<bool>(link(i))))		// ���ż��λ��ϵ��Ӧ������ͬ��������λ��������
					{		// ֱ����������޷��ų����߶�
						if (link(i) == 0)		// �����������ϵ����ͬ
							goto seven_pairs;		// ���߶�
						else if (dan == true)		// ����Ѿ��е�����
							goto seven_pairs;		// �����߶���
						else
						{
							dan = true;		// ��¼����
							dan_tile = static_cast<int>(*hands[i]);		// ��¼���
						}
					}
				if (!dan)		// ���û�鵽����
					dan_tile = static_cast<int>(*hands[12]);		// �ǵ��ž������һ��
				ready_hands.emplace_back(dan_tile);		// ����һ��
				is_ready_hand = true;
				// �п��ܸ��϶����ڣ������ƺ��˳�
			}
		seven_pairs:
			auto block_err{ 0 };		// �������Ŀ��������3����
			std::vector<block> blocks;		// �飨���6����
			blocks.reserve(6);
			blocks.emplace_back(0);		// ��һ��
			for (auto i{ 0 }; i < static_cast<int>(hands.size() - 1); ++i)
				if (link(i) > 1)		// ����ϵ������ͬ������
				{
					blocks.back().len = i - blocks.back().loc + 1;		// ��¼��һ��ĳ���
					blocks.back().type = !(blocks.back().len % 3);		// ɸѡ������Lv.1
					block_err += !blocks.back().type;		// ��¼
					if (blocks.size() == 6 - melds.size() || block_err == 4)		// ������Ŵﵽ(6 - ��¶��)����4����������
						return;		// ����
					blocks.emplace_back(i + 1);		// �������ǿ��������Ƶ����
				}
			blocks.back().len = static_cast<int>(hands.size()) - blocks.back().loc;		// ��¼��һ��ĳ���
			blocks.back().type = !(blocks.back().len % 3);		// ɸѡ������Lv.1
			block_err += !blocks.back().type;		// ��¼
			if (block_err == 4)		// ����4����������
				return;		// ����
			const auto block_num{ static_cast<int>(blocks.size()) };		// ����
			for (auto num{ 0 }; num < block_num; ++num)
				if (blocks[num].type == true)		// ֻ�ж�ͨ��������Lv.1��
					if (blocks[num].block_complete(*this) == false)		// ɸѡ������Lv.2����������
						return;		// ����
			switch (block_err)
			{
			case 1:		// ��һ�鲻�����ͣ�һ��ȫ�������ͣ�3n+4����
			{
				for (auto err_block{ 0 }; err_block < block_num; ++err_block)
					if (blocks[err_block].type == false)		// �ǲ�������
					{
						if (blocks[err_block].block_traversal(*this, true))
							break;
						else return;		// ����
					}
				break;
			}
			case 2:		// �����鲻�����ͣ�һ�����Ӳ������ͣ�3n+2����һ��ȸͷ�����ͣ�3n+2����
			{
				int err_block_1{ 0 };
				for (; err_block_1 < block_num; ++err_block_1)
					if (blocks[err_block_1].type == false)		// �ǲ�������
						break;		// ��¼
				int err_block_2{ err_block_1 + 1 };		// �ڶ�����������
				for (; err_block_2 < block_num; ++err_block_2)
					if (blocks[err_block_2].type == false)		// �ǲ�������
						break;		// ��¼
				if (err_block_2 >= block_num)		// ������ߵ����ִ��ڵ�������
					return;		// ����
				bool temp = false;
				// ȥ�Ժͱ���˳���ܻ�������Ϊ��������ɹ���ֱ��д����ƣ���ȥ�Գɹ�����д����ƣ���Ϊ&&�Ķ�·����ȥ��д��ǰ��
				if (blocks[err_block_2].block_ignore_pair(*this) && blocks[err_block_1].block_traversal(*this, false)		// ��������һ��������������䶼Ҫִ��
					| blocks[err_block_1].block_ignore_pair(*this) && blocks[err_block_2].block_traversal(*this, false))		// ��λ�򲻻���ֶ�·
					break;		// ����
				else return;		// ����
			}
			case 3:		// �����鲻�����ͣ�����벻�����ͣ�3n+1����һ��ȸͷ�����ͣ�3n+2����
			{
				int err_block{ 0 };
				for (; err_block < block_num; ++err_block)
					if (blocks[err_block].type == false)		// �ǲ�������
						if (blocks[err_block].len % 3 == 1)		// ���������3�ı���+1
							break;		// ��¼err_loc
				tile temp_ready_hands{ static_cast<int>(*hands[blocks[err_block].loc + blocks[err_block].len - 1]) + 1 };		// �м�����ƣ���ʱ��¼��ƣ�
				if (static_cast<int>(temp_ready_hands) != static_cast<int>(*hands[blocks[err_block + 1].loc]) - 1)		// ���������һ���Ʋ�����
					return;		// ����
				for (auto eyes_block{ 0 }; eyes_block < block_num; ++eyes_block)
					if (blocks[eyes_block].type == false)		// �ǲ�������
						if (blocks[eyes_block].len % 3 == 2)		// ���������3�ı���+2��ȸͷ��
							if (blocks[eyes_block].block_ignore_pair(*this) == false)		// �������ȸͷ��
								return;		// ����
				opponent temp_op;		// ��ʱ�����жϵ�����
				block temp_block{ 0 };
				temp_block.len = blocks[err_block].len + 1 + blocks[err_block + 1].len;		// ���������������ܳ���+һ���м������
				int i{ 0 };
				for (; i < blocks[err_block].len; ++i)
					temp_op[i] = hands[blocks[err_block].loc + i];		// ���Ƹò�������
				temp_op[i] = &temp_ready_hands;		// ��¼�м����
				for (; i < temp_block.len; ++i)
					temp_op[i] = hands[blocks[err_block].loc + i];		// ���Ƹò�������
				if (temp_block.block_complete(temp_op) == true)		// �������������
					ready_hands.emplace_back(temp_ready_hands);		// ����һ��
				break;
			}
			default:;
			}
			is_ready_hand = true;		// ����
		}
	}
};