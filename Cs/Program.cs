﻿using Cs.Classes;

namespace Cs
{
    internal static class Program
    {
        private static void Main(string[] args)
        {
            var a = new Opponent();
            a.TileIn(new(5));
            a.TileIn(new(7));
            a.TileIn(new(7));
            a.TileIn(new(7));
            a.TileIn(new(32));
            a.TileIn(new(39));
            a.TileIn(new(39));
            // a.TileIn(new(22));
            // a.TileIn(new(40));
           // a.TileIn(new(40));
            var b = a.ReadyHandJudge();
            // a.TileIn(new(32));
            // a.TileIn(new(16));
            // a.TileIn(new(16));
        }
    }
}
