using System;
using Cs.Models;

var op = new Opponent();
op.TileIn(Tile.TileValue.M1);
op.TileIn(Tile.TileValue.M9);
op.TileIn(Tile.TileValue.P1);
op.TileIn(Tile.TileValue.P9);
op.TileIn(Tile.TileValue.S1);
op.TileIn(Tile.TileValue.S9);
op.TileIn(Tile.TileValue.CEast);
op.TileIn(Tile.TileValue.CSouth);
op.TileIn(Tile.TileValue.CWest);
op.TileIn(Tile.TileValue.CNorth);
op.TileIn(Tile.TileValue.CWhite);
op.TileIn(Tile.TileValue.CGreen);
op.TileIn(Tile.TileValue.CRed);
op.DebugAddMelds();
foreach (var tile in op.ReadyHandJudge())
    Console.WriteLine(tile);
var op2 = new Opponent();
op2.TileIn(Tile.TileValue.M1);
op2.TileIn(Tile.TileValue.M1);
op2.TileIn(Tile.TileValue.M1);
op2.TileIn(Tile.TileValue.M2);
op2.TileIn(Tile.TileValue.M3);
op2.TileIn(Tile.TileValue.M4);
op2.TileIn(Tile.TileValue.M5);
op2.TileIn(Tile.TileValue.M6);
op2.TileIn(Tile.TileValue.M7);
op2.TileIn(Tile.TileValue.M8);
op2.TileIn(Tile.TileValue.M9);
op2.TileIn(Tile.TileValue.M9);
op2.TileIn(Tile.TileValue.M9);
op2.DebugAddMelds();
foreach (var tile in op2.ReadyHandJudge())
    Console.WriteLine(tile);
