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
            a.TileIn(new(20));
            a.TileIn(new(20));
            //a.TileIn(new Tile(5));
            a.TileIn(new(20));
            a.TileIn(new(21));
            a.TileIn(new(36));
            a.TileIn(new(37));
            a.TileIn(new(38));
            a.TileIn(new(40));
            var b = a.ReadyHandJudge();
            //a.TileIn(new Tile(32));
            //a.TileIn(new Tile(16));
            //a.TileIn(new Tile(16));
        }
    }
}
