#define new gcnew
#include "MainWindow.h"

using namespace MahjongHelper;
using namespace Controls;
using namespace Windows::Media;

[STAThread] void Main(array<String ^> ^ args) {
    auto ap = new Application();
    auto w = new MainWindow();
    ap->Run(w);
}