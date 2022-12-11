#include "includes/Opponent.h"
#include "includes/Block.h"
int main(int argc, char* argv[])
{
    auto a = Opponent();
    a.TileIn(Tile(0));
    a.TileIn(Tile(0));
    a.TileIn(Tile(0));
    a.TileIn(Tile(1));
    a.TileIn(Tile(2));
    a.TileIn(Tile(3));
    a.TileIn(Tile(4));
    a.TileIn(Tile(5));
    a.TileIn(Tile(6));
    a.TileIn(Tile(7));
    a.TileIn(Tile(8));
    a.TileIn(Tile(8));
    a.TileIn(Tile(8));
    auto b = a.ReadyHandJudge();

	return 0;
}
