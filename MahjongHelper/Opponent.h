#pragma once
#include "Tile.h"
namespace MahjongHelper {
using namespace System::Collections::Generic;
ref class Block;
public
ref class Opponent sealed {
public:
    /// <summary>
    /// 手牌区
    /// </summary>
    List<Tile ^> ^ Hands = new List<Tile ^>();

    /// <summary>
    /// 副露数
    /// </summary>
    int Melds = 0;

    System::String ^ GetHands() {
        System::String ^ temp = L"";
        for each (auto tile in Hands)
            temp += tile->GetUnicode();
        return temp;
    } System::String
        ^ GetReadyHands() {
              Melds = 4 - (Hands->Count - 1) / 3;
              auto readyHands = ReadyHandJudge();
              if (readyHands->Count == 0)
                  return L"无听";
              System::String ^ temp = L"";
              for each (auto tile in readyHands)
                  temp += tile->GetUnicode();
              return temp;
          }

        List<Tile ^> ^ ReadyHandJudge();
    System::Tuple<List<Tile ^> ^, Block ^, Tile ^> ^ JointBlocks(Block ^ frontBlock, Block ^ followBlock);
    List<Tile ^> ^ ThirteenOrphansJudge();
    Tile ^ SevenPairsJudge();
    List<Block ^> ^ GetBlocks(List<Block ^> ^ &blocks);
};
}
