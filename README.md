ReactiveEffector
===

## ビルド手順

[STM32CubeIDE(1.8.0)](https://www.st.com/ja/development-tools/stm32cubeide.html)もしくは `MacOS + CMake + ARMコンパイラ` でビルドできます。

## MacOS + CMake + ARMコンパイラ

`cmake` インストール

```sh
$ brew install cmake
$ cmake --version
cmake version 3.20.1

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

`arm-none-eabi-gcc` インストール

```sh
$ brew tap ArmMbed/homebrew-formulae
$ brew install arm-none-eabi-gcc
$ arm-none-eabi-gcc --version
arm-none-eabi-gcc (GNU Tools for Arm Embedded Processors 9-2019-q4-major) 9.2.1 20191025 (release) [ARM/arm-9-branch revision 277599]
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

`stlink` インストール

```sh
$ brew install stlink
$ st-flash --version
v1.7.0
```

ビルドコマンド

```sh
$ bash build.sh
```

書き込みコマンド

```sh
$ st-flash --format ihex write build/ReactiveEffector.hex
```

## ディレクトリ構成

```
（ディレクトリ）
├── cmake（CMake関連）
├── Core（自動生成・アプリケーションコード）
├── Drivers（自動生成・ドライバなど）
├── Middlewares（自動生成・RTOS, USBなど）
├── USB_DEVICE（自動生成・USBなど）
├── User（編集用・アプリケーションコード）

（ファイル）
├── README.md（このファイル）
├── Doxyfile（doxygen設定）
├── ReactiveEffector.ioc（CubeMXファイル）
├── STM32F745VGTX_FLASH.ld（自動生成・FLASH設定）
├── build.sh（CMakeビルド実行スクリプト）
├── .clang-format（clang-format設定）
└── format.sh（clang-format実行スクリプト）
```

CubeMXで自動生成したファイルのうち、以下は一部手動で編集した。
* Core/Src/main.c
* Core/Src/stm32f7xx_it.c
* USB_DEVICE/App/usbd_cdc_if.c

## 仕様（一部）

### 割り込み

#### DMA割り込み

|DMA|チャネル|アサイン|
|:---|:---|:---|
|1|0|I2C受信完了/エラー|
|1|1|-|
|1|2|-|
|1|3|SPI2（SRAM）受信完了/エラー|
|1|4|SPI2（SRAM）送信完了/エラー|
|1|5|SPI3（NeoPixel）送信完了/エラー|
|1|6|UART2（IR）送信完了/エラー|
|1|7|I2C送信完了/エラー|
|2|0|SPI4（RPI）受信完了/エラー|
|2|1|SPI4（RPI）送信完了/エラー|
|2|2|-|
|2|3|SAI1 A|
|2|4|ADC変換完了/エラー|
|2|5|SAI1 B|
|2|6|-|
|2|7|UART1（RPI）送信完了/エラー|

#### TIM割り込み

|TIM|周期|アサイン|
|:---|:---|:---|
|1|-|-|
|2|-|-|
|3|-|-|
|4|-|-|
|5|-|-|
|6|500ms|ACT LED点滅|
|7|1ms|HAL/FreeRTOS|
|8|-|-|
|9|-|-|
|10|100ms|ADC/APP|
|11|-|-|
|12|-|-|
|13|-|-|
|14|-|-|

#### EXTI割り込み

|EXTI|ピン|アサイン|
|:---|:---|:---|
|0|-|-|
|1|-|-|
|2|-|-|
|3|-|-|
|4|-|-|
|5|PA5|タッチセンサ入力通知|
|6|PA6|ジャイロセンサ更新通知|
|7|PA7|ロータリーエンコーダイベント通知|
|8|-|-|
|9|-|-|
|10|-|-|
|11|PE11|SPI4（RPI）NSS変化通知|
|12|-|-|
|13|-|-|
|14|-|-|
|15|-|-|

#### その他割り込み

* I2C1イベント
* I2C1エラー
* UART1（RPI）受信
* UART2（IR）受信
* USB FS
