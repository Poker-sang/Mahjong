namespace Cs.Classes
{
    /// <summary>
    /// 组（其中牌关系只有相同）
    /// </summary>
    public class Group
    {
        /// <summary>
        /// 组内首张牌的序号
        /// </summary>
        public int Loc { get; }
        /// <summary>
        /// 组内牌数
        /// </summary>
        public int Len { get; set; } = 1;
        /// <summary>
        /// 已确认（顺/刻）的牌数量
        /// </summary>
        public int Confirmed { get; set; }

        public Group(int loc) => Loc = loc;
    }
}