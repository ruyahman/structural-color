# Diff

Direct3D 11 による構造色のデモです。


## 必要な環境

- Windows 10 / 11
- Visual Studio 2022 または Build Tools 2022 の「C++ によるデスクトップ開発」
  （MSVC v143 と Windows SDK）
- VS Code でデバッグする場合は Microsoft の C/C++ 拡張機能

旧 DirectX SDK (June 2010)、Visual Studio 2012 の DLL、FBX SDK は不要です。
数学処理には DirectXMath、シェーダーのコンパイルには D3DCompileFromFile を使います。
DDS ローダーは同梱の DirectXTK から必要なファイルだけを
`Diff/third_party/DirectXTK` に収録しています（同ディレクトリのライセンス参照）。
古い形式の `font.dds` は読み込み時にメモリ上で変換します。


## 操作

- 矢印キー: モデルを回転
- Space: 表面の光の模様を変更
- W / A / S / D: カメラを移動
- マウス移動: 視線方向を変更
- R: モデル・模様・カメラをリセット
- Esc: アプリを終了

デバッグ停止は **Shift+F5** です。
