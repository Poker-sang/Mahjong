#pragma once
#include "ReadyHandJudge.h"
namespace MahjongHelper {
using namespace System;
using namespace System::ComponentModel;
using namespace Collections;
using namespace Windows;
using namespace Controls;
using namespace System::Data;
using namespace Text::RegularExpressions;
using namespace Drawing;

/// <summary>
/// MainWindow 摘要
/// </summary>
public
ref class MainWindow sealed : public Window {
public:
    MainWindow()
    {
        InitializeComponent();
    }

    ~MainWindow() { }

private:
    /// <summary>
    /// 听牌查询
    /// </summary>
    void ApplyClicked(Object ^ sender, RoutedEventArgs ^ routedEventArgs)
    {
        auto matches = (new Regex(L"([1-9]+[MmPpSsZz])"))->Matches(_inputTextBox->Text);
        auto tempOp = new Opponent();
        for each (auto object in matches) {
            auto match = object->ToString();
            switch (match[match->Length - 1]) {
            case 'M':
            case 'm':
                for each (auto c in match)
                    if ('9' >= c && c >= '1')
                        TileIn(tempOp->Hands, new Tile(c - '1'));
                break;
            case 'P':
            case 'p':
                for each (auto c in match)
                    if ('9' >= c && c >= '1')
                        TileIn(tempOp->Hands, new Tile(c - '1' + 16));
                break;
            case 'S':
            case 's':
                for each (auto c in match)
                    if ('9' >= c && c >= '1')
                        TileIn(tempOp->Hands, new Tile(c - '1' + 32));
                break;
            case 'Z':
            case 'z':
                for each (auto c in match)
                    if ('7' >= c && c >= '1')
                        TileIn(tempOp->Hands, new Tile((c - '1') * 8 + 48));
                break;
            default:
                break;
            }
        }
        if ((tempOp->Hands->Count - 1) % 3 != 0 || tempOp->Hands->Count > 13) {
            MessageBox::Show(L"输入有误，请检查牌数和语法", L"错误", MessageBoxButton::OK, MessageBoxImage::Error);
            return;
        }
        _yourTiles->Text = tempOp->GetHands();
        _readyHands->Text = tempOp->GetReadyHands();
    }

#pragma region 控件
    DockPanel ^ _root = new DockPanel();
    DockPanel ^ _dockPanel = new DockPanel();
    DockPanel ^ _yourTilesPanel = new DockPanel();
    DockPanel ^ _readyHandsPanel = new DockPanel();

    Button ^ _applyButton = new Button();
    TextBox ^ _inputTextBox = new TextBox();

    TextBlock ^ _yourTilesText = new TextBlock();
    TextBlock ^ _yourTiles = new TextBlock();

    TextBlock ^ _readyHandsText = new TextBlock();
    TextBlock ^ _readyHands = new TextBlock();
#pragma endregion

    /// <summary>
    /// 加载窗口
    /// </summary>
    void InitializeComponent()
    {
        Width = 450;
        Height = 240;
        Title = L"MahjongHelper";
        ResizeMode = Windows::ResizeMode::CanMinimize;
        Content = _root;
        InitializeControls();
    }
    /// <summary>
    /// 加载控件
    /// </summary>
    void InitializeControls()
    {
        _root->Margin = Thickness(10);
        _root->Children->Add(_dockPanel);
        _root->Children->Add(_yourTilesPanel);
        _root->Children->Add(_readyHandsPanel);
        {
            DockPanel::SetDock(_dockPanel, Dock::Top);
            _dockPanel->Margin = Thickness(0, 0, 0, 10);
            _dockPanel->Height = 30;
            _dockPanel->Children->Add(_applyButton);
            _dockPanel->Children->Add(_inputTextBox);
            {
                DockPanel::SetDock(_applyButton, Dock::Right);
                _applyButton->Margin = Thickness(10, 0, 0, 0);
                _applyButton->Width = 70;
                _applyButton->Content = L"查询听牌";
                _applyButton->Click += new RoutedEventHandler(this, &MainWindow::ApplyClicked);

                _inputTextBox->FontSize = 23;
                _inputTextBox->VerticalContentAlignment = Windows::VerticalAlignment::Center;
            }
        }
        {
            DockPanel::SetDock(_yourTilesPanel, Dock::Top);
            _yourTilesPanel->Margin = Thickness(0, 0, 0, 10);
            _yourTilesPanel->Children->Add(_yourTilesText);
            _yourTilesPanel->Children->Add(_yourTiles);
            {
                DockPanel::SetDock(_yourTilesText, Dock::Top);
                _yourTilesText->FontSize = 15;
                _yourTilesText->Text = L"你的手牌：";

                _yourTiles->FontSize = 28;
            }
        }
        {
            DockPanel::SetDock(_readyHandsPanel, Dock::Top);
            _readyHandsPanel->Margin = Thickness(0, 0, 0, 10);
            _readyHandsPanel->Children->Add(_readyHandsText);
            _readyHandsPanel->Children->Add(_readyHands);
            {
                DockPanel::SetDock(_readyHandsText, Dock::Top);
                _readyHandsText->FontSize = 15;
                _readyHandsText->Text = L"你听的牌：";

                _readyHands->FontSize = 28;
            }
        }
    }
};
}
