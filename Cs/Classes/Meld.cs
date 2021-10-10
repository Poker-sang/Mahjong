using Cs.Enum;

namespace Cs.Classes
{
    /// <summary>
    /// 副露
    /// </summary>
    public class Meld
    {
        /// <summary>
        /// 记录副露
        /// </summary>
        public System.Collections.Generic.List<Tile> Tile { get; } = new(4);

        /// <summary>
        /// 记录副露类型
        /// </summary>
        public EMeld Mode { get; set; } = EMeld.NullMeld;
        /// <summary>
        /// 存放牌来源方
        /// </summary>
        public EWind? Income { get; }
        /// <summary>
        /// 直接访问牌元素
        /// </summary>
        public Tile this[int index] { get => Tile[index]; set => Tile[index] = value; }

        public Meld(EWind? income) => Income = income;
    }
}
