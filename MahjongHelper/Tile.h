#pragma once
namespace MahjongHelper {
public
ref class Tile sealed {
public:
    /// <summary>
    /// 存放牌信息
    /// </summary>
    const int Val;
    Tile(const int val)
        : Val { val }
    {
    }

    /// <summary>
    /// 获取Unicode
    /// </summary>
    System::String ^ GetUnicode() {
        switch (Val) {
        case 0:
            return L"🀇";
        case 1:
            return L"🀈";
        case 2:
            return L"🀉";
        case 3:
            return L"🀊";
        case 4:
            return L"🀋";
        case 5:
            return L"🀌";
        case 6:
            return L"🀍";
        case 7:
            return L"🀎";
        case 8:
            return L"🀏";

        case 16:
            return L"🀙";
        case 17:
            return L"🀚";
        case 18:
            return L"🀛";
        case 19:
            return L"🀜";
        case 20:
            return L"🀝";
        case 21:
            return L"🀞";
        case 22:
            return L"🀟";
        case 23:
            return L"🀠";
        case 24:
            return L"🀡";

        case 32:
            return L"🀐";
        case 33:
            return L"🀑";
        case 34:
            return L"🀒";
        case 35:
            return L"🀓";
        case 36:
            return L"🀔";
        case 37:
            return L"🀕";
        case 38:
            return L"🀖";
        case 39:
            return L"🀗";
        case 40:
            return L"🀘";

        case 48:
            return L"🀀";
        case 56:
            return L"🀁";
        case 64:
            return L"🀂";
        case 72:
            return L"🀃";
        case 80:
            return L"🀆";
        case 88:
            return L"🀅";
        case 96:
            return L"🀄";
        default:
            return L"!";
        }
    }
};
}