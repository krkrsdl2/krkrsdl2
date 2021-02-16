# 吉里吉里SDL2

吉里吉里SDL2は、macOSやLinuxなど、[SDL2](https://www.libsdl.org/)をサポートするプラットフォームで実行できる[吉里吉里Z](https://krkrz.github.io/)の移植版です。  

このプログラムは、KAG（吉里吉里アドベンチャーゲーム）3の変更バージョンと共に使用できます。  
詳細については、次の場所を参照してください：https://github.com/krkrsdl2/kag3  

吉里吉里SDL2は[Emscripten](https://emscripten.org/)を使用して[WebAssembly](https://webassembly.org/)にコンパイルすることも可能です。  
Webブラウザーでプレイできるデモはここにあります：https://krkrsdl2.github.io/krkrsdl2/  

## ダウンロード

以下のビルドは、Github Actionsによって最新のソースコードから自動的に構築されます。

* [Webビルド](https://github.com/krkrsdl2/krkrsdl2/releases/download/latest/krkrsdl2-web.zip)
* [macOSビルド](https://github.com/krkrsdl2/krkrsdl2/releases/download/latest/krkrsdl2-macos.zip)
* [Ubuntuビルド](https://github.com/krkrsdl2/krkrsdl2/releases/download/latest/krkrsdl2-ubuntu.zip)

## 使用法

プログラムを使用するには、`startup.tjs`と同じディレクトリのコマンドラインで以下を実行します：
```bash
/path/to/krkrsdl2
```

起動ディレクトリまたはアーカイブは、コマンドラインで指定できます：
```bash
/path/to/krkrsdl2 /path/to/startup/directory
```

コマンドライン引数は、コマンドラインで指定できます：
```bash
/path/to/krkrsdl2 -drawthread=4
```

Webバージョンの場合は、[Releaser](https://krkrz.github.io/krkr2doc/kr2doc/contents/Releaser.html)によって作成した`data.xp3`をWebビルドと同じフォルダに配置し、Webサーバー上にアップロードします。 設置でき次第、プロジェクトはWebブラウザから動作します。

## クローニング

リポジトリのクローンを作成するには、ターミナルで次のコマンドを使用してください：

```bash
git clone --recursive https://github.com/krkrsdl2/krkrsdl2
```
プロジェクトがGitサブモジュールを使用するため、上記のコマンドを正しく使用しない場合、ソースファイルが欠落します。

## ビルディング

このプロジェクトは、Mesonビルドシステムを使用してビルドできます。  
Mesonビルドシステムの詳細については、次の場所をご覧ください： https://mesonbuild.com/
 
Mesonツールチェーンファイルは、[Emscripten](https://emscripten.org/)を使用する場合など、異なるプラットフォームへのクロスコンパイルに使用できます。    
便宜上、Mesonツールチェーンファイルは次の場所に置いてあります：https://github.com/krkrsdl2/meson_toolchains  

## クイックスタート

このプロジェクトをビルドしたら、`startup.tjs`を含むディレクトリに設置してください。  
その後、次のコマンドを実行します： `/path/to/krkrsdl2`  

## オリジナルプロジェクト

このプロジェクトのコードは、次のプロジェクトに基づいています：
* [吉里吉里2](https://github.com/krkrz/krkr2)
* [吉里吉里Z](https://github.com/krkrz/krkrz) dev_multi_platform ブランチ
* [KAGParser](https://github.com/krkrz/KAGParser)
* [SamplePlugin](https://github.com/krkrz/SamplePlugin)
* [wuvorbis](https://github.com/krkrz/wuvorbis)
* [ncbind](https://github.com/wtnbgo/ncbind)
* [fstat](https://github.com/wtnbgo/fstat)
* [json](https://github.com/wtnbgo/json)
* [varfile](https://github.com/wtnbgo/varfile)
* [simde](https://github.com/simd-everywhere/simde)
* [SDL](https://github.com/libsdl-org/SDL)

## IRCチャンネル

吉里吉里SDL2プロジェクトのメンバーは、[freenodeの#krkrsdl2チャンネル](https://webchat.freenode.net/?channel=#krkrsdl2)で見つけることができます。

## ライセンス

このコードは、修正された3条項のBSDライセンスに基づいています。 詳細については、`LICENSE`をお読みください。  
このプロジェクトには、サードパーティのコンポーネントが含まれています。 詳細については、各コンポーネントのライセンスファイルを参照してください。  
