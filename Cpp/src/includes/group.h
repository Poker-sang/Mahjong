#pragma once
/// <summary>
/// �飨�����ƹ�ϵֻ����ͬ��
/// </summary>
class group
{
public:
	explicit group(const int loc) :loc{ loc } {}
	/// <summary>
	/// ���������Ƶ����
	/// </summary>
	const int loc{ 0 };
	/// <summary>
	/// ��������
	/// </summary>
	int len{ 1 };
	/// <summary>
	/// ��ȷ�ϣ�˳/�̣���������
	/// </summary>
	int confirmed{ 0 };
};