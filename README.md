# 吉里吉里SDL2

これは、macOSやLinuxなど、[SDL2](https://www.libsdl.org/)をサポートするプラットフォームで実行できる[吉里吉里Z](https://krkrz.github.io/)の移植版です。  
このプログラムは、KAG（吉里吉里アドベンチャーゲーム）3の変更バージョンと一緒に使用できます。詳細については、次の場所を参照してください：https://github.com/uyjulian/kag3  
[Emscripten](https://emscripten.org/)を使用して[WebAssembly](https://webassembly.org/)にコンパイルされている吉里吉里SDL2のおかげで、Webブラウザーで再生可能なデモプロジェクトがここにあります：https://ghpages.uyjulian.pw/krkrsdl2/  

## クローニング

リポジトリのクローンを作成するには、ターミナルで次のコマンドを使用してください：

```bash
git clone --recursive https://github.com/uyjulian/krkrsdl2
```
上記の正確なコマンドを使用しない場合、プロジェクトがGitサブモジュールを使用するため、ファイルが失われます。

## ビルディング

このプロジェクトは、Mesonビルドシステムを使用してビルドできます。 システムの詳細については、次の場所をご覧ください： https://mesonbuild.com/  
Mesonツールチェーンファイルは、[Emscripten](https://emscripten.org/)を使用する場合など、異なるプラットフォームへのクロスコンパイルに使用できます。  
便宜上、Mesonツールチェーンファイルは次の場所にあります：https://github.com/uyjulian/meson_toolchains  

## クイックスタート

このプロジェクトをビルドしたら、ディレクトリを`startup.tjs`を含むディレクトリに変更できます。
その後、次のプログラムを実行します： `/path/to/krkrsdl2`  

## オリジナルプロジェクト

このプロジェクトのコードは、次のプロジェクトに基づいています：
* [吉里吉里2](https://github.com/krkrz/krkr2)
* [吉里吉里Z](https://github.com/krkrz/krkrz) dev_multi_platform ブランチ
* [KAGParser](https://github.com/krkrz/KAGParser)
* [SamplePlugin](https://github.com/krkrz/SamplePlugin)
* [binaryStream](https://github.com/wtnbgo/binaryStream)
* [csvParser](https://github.com/wtnbgo/csvParser)
* [fftgraph](https://github.com/krkrz/fftgraph)
* [fstat](https://github.com/wtnbgo/fstat)
* [getSample](https://github.com/wtnbgo/getSample)
* [json](https://github.com/wtnbgo/json)
* [krglhwebp](https://github.com/uyjulian/krglhwebp)
* [kwidgets](https://github.com/krkrz/kwidgets)
* [layerExAreaAverage](https://github.com/wtnbgo/layerExAreaAverage)
* [layerExBTOA](https://github.com/wtnbgo/layerExBTOA)
* [layerExDraw](https://github.com/wtnbgo/layerExDraw)
* [layerExImage](https://github.com/wtnbgo/layerExImage)
* [layerExLongExposure](https://github.com/wtnbgo/layerExLongExposure)
* [layerExRaster](https://github.com/wtnbgo/layerExRaster)
* [layerEx](https://github.com/wtnbgo/layerEx)
* [lineParser](https://github.com/wtnbgo/lineParser)
* [ncbind](https://github.com/wtnbgo/ncbind)
* [saveStruct](https://github.com/wtnbgo/saveStruct)
* [scriptsEx](https://github.com/wtnbgo/scriptsEx)
* [shrinkCopy](https://github.com/wtnbgo/shrinkCopy)
* [varFile](https://github.com/wtnbgo/varFile)
* [wuvorbis](https://github.com/krkrz/wuvorbis)

## IRCチャンネル

吉里吉里SDL2プロジェクトのメンバーは、[freenodeの#krkrsdl2チャンネル](https://webchat.freenode.net/?channel=#krkrsdl2)で見つけることができます。

## ライセンス

このコードは、修正された3条項のBSDライセンスに基づいています。 詳細については、`LICENSE`をお読みください。  
このプロジェクトには、サードパーティのコンポーネントが含まれています。 詳細については、各コンポーネントのライセンスファイルを参照してください。  
