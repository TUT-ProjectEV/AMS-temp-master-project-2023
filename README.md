# AMS_temp_master_project

## Overview
- AMSの温度監視プログラム
- マスター側
- 2023年度10月時点でテスト中

## Requirement
### Development
- VScode + PlatformIO
### Boards
- [AMS_Temp_Master](https://github.com/TUT-ProjectEV/AMS_Temp_Master)
### Libraries
- MsTimer2 \@ ^1.1
- CAN_BUS_Shield \@ ^2.3.3

## Usage
### CAN communication
1. CAN_Tempクラスのオブジェクト生成、コンストラクタの引数にIDを設定、init関数で初期化
2. setTemp関数で最高、最低、平均温度をセット (Typeでどれをセットするのか指定、Typeはenum型)
3. sendTempMsg関数でCAN Busにメッセージを流す
### I2C communication
1. Thermistorクラスのオブジェクト生成
2. 通常通りI2Cで来たメッセージをそのまま配列に格納
3. setData関数で配列を渡す
4. getMaxTemp, getMinTemp, getAvrTemp関数で最高、最低、平均温度を取得

## Features
### I2C communication
- スレーブから送信されるメッセージのビットアサインに一癖あり
- Thermistorライブラリで処理実装済み

## Reference

## Author
- Takamura
  - [GitHub](https://github.com/ST04-tkmr)

## License
- Copyright (c) 2023 東京工科大学 ProjectEV [Released under the MIT license](https://github.com/TUT-ProjectEV/AMS-temp-master-project-2023/blob/main/LICENSE)
