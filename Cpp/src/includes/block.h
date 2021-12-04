#pragma once

#include "group.h"
#include "opponent.h"
/// <summary>
/// �飨�����ƹ�ϵֻ����ͬ��������
/// </summary>
class block
{
public:
	/// <summary>
	/// ���������Ƶ����
	/// </summary>
	int loc{ 0 };
	/// <summary>
	/// ��������������һ�ţ�
	/// </summary>
	int len{ 1 };
	/// <summary>
	/// ���ͣ��棨3n��Ϊ�����ͣ���������������ɣ�����Ϊ�������ͣ�����ȸͷ�������������ӣ���
	/// </summary>
	bool type{ true };
	explicit block(const int loc) : loc{ loc } {}
	/// <summary>
	/// ɸѡ������Lv.2
	/// </summary>
	/// <param name="op">�жϵ�����</param>
	/// <param name="eyes_loc">ȸͷ����ţ�-1Ϊû��ȸͷ��</param>
	bool block_complete(opponent& op, const int eyes_loc = -1)
	{
		std::vector<group> group;
		group.emplace_back(loc);
		{
			auto i{ loc };
			for (; i < loc + len - 1; ++i)		//�жϿ���ÿ����ϵ
				if (op.link(i) == 1)		//����ϵ������
					{
					group.back().len = i + 1 - group.back().loc;
					group.emplace_back(i + 1);		//��¼��һ����
					}
			group.back().len = i + 1 - group.back().loc;
		}
		const int group_num{ group.size() };		//����������

		std::vector<bool> block_card(len, true);		//�жϿ���ÿ���ƣ�����������˳�Ӽ���Ϊ˳��true���������Ϊ�̣�flase��
		if (eyes_loc != -1)		//��ȸͷ
		{
			++++group[eyes_loc].confirmed;		//ȸͷ����������
			block_card[group[eyes_loc].loc - loc] = false;
			block_card[group[eyes_loc].loc - loc + 1] = false;
		}
		for (auto i{ 0 }; i < group_num; ++i)		//ÿ��ѭ����¼һ����
		{
			switch (group[i].len - group[i].confirmed)		//��������
			{
			case 0:
				continue;		//��ʼ��һѭ��
			case 1:		//����˳
				if (group_num > i + 2)
				{
					++group[i + 1].confirmed;		//ȷ�����漸����2����˳
					++group[i + 2].confirmed;
					continue;
				}
				break;
			case 2:		//����˳
				if (group_num > i + 2)
				{
					++++group[i + 1].confirmed;		//ȷ�����漸����2����˳
					++++group[i + 2].confirmed;
					continue;
				}
				break;
			case 4:		//3��1˳
				if (group_num > i + 2)
				{
					++group[i + 1].confirmed;		//ȷ�����漸����2����˳
					++group[i + 2].confirmed;
					block_card[group[i].loc - loc] = false;		//�����ǿ�
					block_card[group[i].loc - loc + 1] = false;
					block_card[group[i].loc - loc + 2] = false;
					continue;
				}
				break;
			case 3:		//���ǿ�
				block_card[group[i].loc - loc] = false;		//�����ǿ�
				block_card[group[i].loc - loc + 1] = false;
				block_card[group[i].loc - loc + 2] = false;
				continue;
			default:		//�����Ǹ���
				break;
			}
			type = false;		//��������
			return false;
		}
		return true;
	}
	/// <summary>
	/// ȥ�ԣ�ȸͷ�����ͣ�
	/// </summary>
	/// <param name="op">�жϵ�����</param>
	/// <returns>�Ƿ�����</returns>
	bool block_ignore_pair(opponent& op)
	{
		for (auto i{ loc }, temp_group_num{ 0 }; i < loc + len - 1; ++i)
		{
			if (op.link(i) == 1)		//����ϵ������
				++temp_group_num;		//������һ
			else if (block_complete(op, temp_group_num) == true)		//����ϵ����ͬ���ж��Ƿ���ȸͷ������
				return true;		//������ȸͷ������
		}
		return false;		//������
	}
	/// <summary>
	/// ����
	/// </summary>
	/// <param name="op">�жϵ�����</param>
	/// <param name="mode">��Ϊ���Ӳ������ͣ���Ϊȫ��������</param>
	/// <returns>�Ƿ�����</returns>
	bool block_traversal(opponent& op, const bool mode) const
	{
		auto first{ static_cast<int>(*op[loc]) - 1 };		//���ܵ�������
		if ((static_cast<int>(*op[loc]) & 15) == 0 || static_cast<int>(*op[loc]) >> 3 > 5)		//���������һ��Ͳ����������
			++first;		//firstû��ǰһ�ţ��ӻ�op[loc]
		auto last = static_cast<int>(*op[loc + len - 1]) + 1;		//���ܵ�ĩ����
		if ((static_cast<int>(*op[loc + len - 1]) & 15) == 8 || static_cast<int>(*op[loc + len - 1]) >> 3 > 5)		//���ĩ���Ǿ���Ͳ����������
			--last;		//lastû�к�һ�ţ�����op[loc]
		block temp_block{ 0 };		//�������Ӧ�Ŀ�
		temp_block.len = this->len + 1;
		auto temp_tile{ first };
		auto temp{ 0 };
		opponent temp_op;
		for (int i{ 0 }; i < last - first + 1; ++i, ++temp_tile)		//ÿ���ƶ����볢��һ�Σ�������
		{
			for (int j{ loc }; j < loc + len; ++j)		//���¸���������
				temp_op.hands.emplace_back(op[j]);
			temp = temp_op << new tile(temp_tile);		//���볢�Ե���
			if (mode && temp_block.block_ignore_pair(temp_op) 		//���棩ȫ���������ұ�����ȥ�Ժ�����
				|| !mode && temp_block.block_complete(temp_op))		//���٣����Ӳ��������ұ���������
				op.ready_hands.emplace_back(temp_tile);
			delete temp_op[temp];
			temp_op.hands.clear();
		}
		return !op.ready_hands.empty();		//���ܱ������Ͳ�Ϊ�գ����������ﲻӰ�죨�������ʱ���ܸĶ���
	}
};
