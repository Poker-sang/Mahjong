using Cs.Classes;

namespace Cs
{
    internal static class Program
    {
        private static void Main(string[] args)
        {
            var a = new Opponent();
            a.TileIn(new Tile(0));
            a.TileIn(new Tile(1));
            a.TileIn(new Tile(2));
            a.TileIn(new Tile(4));
            //a.TileIn(new Tile(5));
            a.TileIn(new Tile(6));
            a.TileIn(new Tile(8));
            a.TileIn(new Tile(8));
            a.TileIn(new Tile(8));
            a.TileIn(new Tile(32));
            a.TileIn(new Tile(32));
            a.TileIn(new Tile(32));
            a.TileIn(new Tile(16));
            a.TileIn(new Tile(16));
        }
    }
}
