using Cs.Classes;

namespace Cs
{
    internal static class Program
    {
        private static void Main(string[] args)
        {
            var a = new Opponent();
            a.TileIn(new(2));
            a.TileIn(new(2));
            a.TileIn(new(3));
            a.TileIn(new(3));
            a.TileIn(new(4));
            a.TileIn(new(4));
            a.TileIn(new(23));
            a.TileIn(new(21));
            a.TileIn(new(21));
            a.TileIn(new(22));
            a.TileIn(new(22));
            a.TileIn(new(40));
            a.TileIn(new(40));
            var b = a.ReadyHandJudge();
            //a.TileIn(new(32));
            //a.TileIn(new(16));
            //a.TileIn(new(16));
        }
    }
}
