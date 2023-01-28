# Mahjong

**不使用**查表法和遍历法的高效日麻判听牌算法。

长期维护的算法介绍文档：《[日麻听牌判断归类算法](https://www.bilibili.com/read/cv4601162)》，大致原理是对牌组进行分类剪枝。

<table>
    <tr>
        <th colspan="2">
            完整型判断Lv.1
        </th>
        <th colspan="2">
            完整型判断Lv.2
        </th>
        <th>
            ~指“不完整型”
        </th>
    </tr>
    <tr>
        <th>牌数</th>
        <th>IntegrityType</th>
        <th>类型名</th>
        <th>判断听牌方法</th>
        <th>备注</th>
    </tr>
    <tr>
        <td rowspan="2">3n</td>
        <td>Type0</td>
        <td>完整型</td>
        <td>直接判断（IntegrityJudge()）</td>
        <td></td>
    </tr>
    <tr>
        <td>TypeEx</td>
        <td>雀半不完整型</td>
        <td>去对+取坎张</td>
        <td>半~与雀头~合并而成</td>
    </tr>
    <tr>
        <td rowspan="2">3n+1</td>
        <td rowspan="2">Type1</td>
        <td>半不完整型</td>
        <td>取坎张（会成对出现）</td>
        <td></td>
    </tr>
    <tr>
        <td>雀面不完整型</td>
        <td>遍历+去对</td>
        <td>雀头~与面子~合并而成</td>
    </tr>
    <tr>
        <td rowspan="2">3n+2</td>
        <td rowspan="2">Type2</td>
        <td>雀头不完整型</td>
        <td>去对（去掉一个对子）</td>
        <td></td>
    </tr>
    <tr>
        <td>面子不完整型</td>
        <td>遍历（3-9次）+与前后块连接</td>
        <td></td>
    </tr>
</table>

仓库提供C#、C++ (Standard: 20)、C++/CLI三种语言实现，也可以按照文档用其他语言实现。

若有BUG欢迎提[ISSUE](https://github.com/Poker-sang/Mahjong/issues)

## LICENSE

[MIT LICENSE](https://github.com/Poker-sang/Mahjong/blob/master/LICENSE)
