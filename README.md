# **Aviutl2** 入力プラグインのたたき台（フレームワークにちょっと肉付けしたもの）を作ってみた
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/title.png)
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/title2.png)
# この記事の対象読者
- プラグインというものは知っているけど、なんか、難しそうで、今まで触ってこなかったけど、さわりの部分はどんなものか、興味がある方
- Aviutl2にこんな機能があったらいいなと思っている方
- 何かのプラグインを作る練習をしたい方
- 自由研究の課題を探している方

そんなことを考えている皆様のインスピレーションのきっかけになれば幸いです。

　と書いている私は自由研究として７月中に出せるだろうと思っていたのですが、慣れてないところもあり、思った以上に時間がかかってしまいました。
# 注意事項
## この記事について  
記事中の図は記事を書いている途中のものです。プログラムの変更などにより実際とは違うものになる場合があります。
## SDK最終更新日について  
この記事はSDKの最終更新日**2025/10/12**を元に作成されています。  
しかし、.\aviutl2_sdk\ディレクトリにはこの記事より後で更新されたものが入っている場合もあります。
# プロトタイプ／デモプログラムの構造と動作概要
## デモプログラムの構築と実行
### ビルドまでの概要
　まず、このソリューションはAviutl2のプラグインとなっています。Aviutl2を事前にインストールされ、動作確認、おおまかな操作の仕方を把握されることを推奨します。この記事では標準的なディレクトリにAviutl2がインストールされていることを前提として話を進めていきます。

ソースをダウンロード後、**ビルドできるよう**に設定をします。このプログラム群はパワーシェルスクリプト（以下.ps1）も使っていることもあり、Windowsが.ps1の実行が許可されていないと、いきなりビルドしても途中でビルドエラーになってしまいます。まずはパワーシェル（以下PS）がビルド中に実行できるようにシステムを設定していきます。また、VisualStudio2022（以下VS）のプロジェクトファイルは、プロパティのデバッグ項目の設定がGitHubにアップロードされません。その為、手動での設定が必要になってきます。最終成果物はDLLであり、デバッグはAviutl2がプラグインと認識されるディレクトリに配置し、実行はAviutl2を起動しなくてはなりません。  
### ソースコードダウンロード  
この記事で使うソースコードはGitHubから入手できます。  
[https://github.com](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin.git)  
SDKを別途ダウンロードしたい場合は次のリンクからダウンロードできます。  
[https://spring-fragrance.mints.ne.jp/aviutl](https://spring-fragrance.mints.ne.jp/aviutl)    

この後の操作はZIPでダウンロードした場合を想定しています。各自、それぞれのやり方に合った方法で設定して下さればと思います。  
ZIPを展開すると、いくつかのディレクトリとファイルが作成されます。aviutil2_test.slnxを開き、ソリューションエクスプローラーを開くと次のように表示されます。  
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/SolutionEx1.png)  
プロジェクト数が結構ありますが、他で使っていたライブラリをこのソリューションでも使いまわししています。また個々のコンポーネントの単体テストをする為のテストダイアログ、ライブラリ化したコンポーネント、RBF（リバーシボードフォーマット）のファイルを作るCUIのプログラム等のプロジェクトも含んでいます。

### パワーシェルスクリプトの実行を有効にする
#### なぜパワーシェルスクリプトを有効にするのか
RBFReaderフォルダーとTestDlgフォルダーにupdate_build.ps1というファイルがあります。これはbuild_info.hとbuild_info.h.bakファイルにビルドした回数が記録されているのですが、この回数をインクリメントするのにパワーシェルスクリプトを使います。これをビルド前イベントとして設定すると、ビルドした回数が判るという手筈になっています。
なお、このスクリプトはAIにより作成されています。

build_info.hファイルの内容は
``` cpp
#define BUILD_NUMBER 252
```
となっています。build_info.h.bakは、万が一スクリプトのエラーでbuild_info.hの内容が壊れてしまった場合、復旧できるようにする為のバックアップファイルです。
#### 現在のupdate_build.ps1の状況  
このスクリプトを、プロジェクト**RBFRader**と**TestDlg**の**ビルド前イベント**として実行しています。しかし、このスクリプト実行時「ファイルが開けない」エラーが、VSの操作状況により頻繁に発生する場合があります。その場合、build_info.hの中身が空になります。対処方法は、build_info.h.bakの中身をコピーして、build_info.hに張り付け、保存することです。原因は判っていません。
#### この機能を無効とする場合
この機能を使用しなくともビルドでき、機能的に問題はないので、その場合は**プロジェクトのプロパティ＞ビルドイベント＞ビルド前のイベント**の設定を無効にすればビルドは可能です。

#### パワーシェルスクリプトの実行権限を有効にする手順

パワーシェルプロンプトから
```powershell
Get-ExecutionPolicy -List
```
と、入力すると
``` powershell
PS C:\Users\usera> Get-ExecutionPolicy -List

        Scope ExecutionPolicy
        ----- ---------------
MachinePolicy       Undefined
   UserPolicy       Undefined
      Process       Undefined
  CurrentUser       Undefined
 LocalMachine       Undefined
```
と、このような画面が表示されます。
この場合、スクリプトの実行権限がないので変更します。
``` powershell
Set-ExecutionPolicy RemoteSigned -Scope CurrentUser -Force
```
と、入力しますと
``` powershell
PS C:\Users\usera> Get-ExecutionPolicy -List

        Scope ExecutionPolicy
        ----- ---------------
MachinePolicy       Undefined
   UserPolicy       Undefined
      Process       Undefined
  CurrentUser    RemoteSigned
 LocalMachine       Undefined
```
と、権限が変わりました。この辺りは、スクリプトが実行できればいいので、それぞれの事情にあった設定をして下さればと思います。

次に、.ps1はダウンロードしたファイルなので、そのまま実行しようとすると、許可を求めるダイアログボックスが現れます。これが、ビルド時のエラーとなってしまいますので、あらかじめ許可をしておく設定をします。
RBFReaderとTestDlgというサブディレクトリに**update_build.ps1**というファイルがあります。このファイルをエクスプローラーで選択して**右クリック＞プロパティ＞詳細設定ボタン＞**で、ダイアログボックスに下図のような文言が現れます。

![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/accept.png)

初めての場合はこのような表示が出てきます。「セキュリティ：このファイルは・・・」の許可するのチェックボックスにチェックを入れ適用します。一度設定すればもうこの表示は出なくなります。

### プロジェクトのデバッグプロパティの設定
Visual Studio の各プロジェクトにおける 「構成プロパティ ＞ デバッグ」 の設定は、通常別の.vcxproj.userファイルに保存されます。
このファイルはユーザーごとの環境依存設定であるため、GitHub などのソース管理には含まれないのが一般的です。
そのため、DLL をデバッグする際には、ホストアプリケーション（例：AviUtl2）を手動で指定する必要があります。
#### **RBFReader**プロジェクトのプロパティ＞構成構成プロパティ＞デバッグを設定する
このインプットプラグインを使用するには、**RBFReader**プロジェクトの「構成プロパティ＞デバッグ」の「コマンド」をAviUtl2の実行ファイルのパスに設定し、「作業ディレクトリ」はそのAviUtl2のディレクトリに設定します。
ホストプログラムの指定が正しく行われていれば、プラグインは正常に動作するはずです。  

![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/07CfgDbg.png)  

### インプットプラグインをビルドし実行する
**RBFReader**プロジェクトをビルドし、実行してみてください。  
初めての場合、次のような表示が出ます。

![構成図](images/08AddPluginAccept.png)

この表示が出ればAviUtl2に認識されたと判断できます。  
しかし、プラグインが正しく動作するのかは、まだ、分かりませんので、次に、このプラグインが読み込めるファイルを作るプログラムをビルドしていきます。

### RBFファイルを作るプログラムをビルドする
   このインプットプラグインは.RBFファイルを読み込み、デコードしてDIB形式にし、AviUtl2に渡すプログラムです。そのRBFファイルを作るプログラム**RBFMaker**をビルドしていきます。
#### RBFファイルとは
リバーシ・ボード・フォーマット（Reversi Board Format）の頭文字をとったもので、この、デモプログラムの為に作った、でっちあげのファイル形式です。ヘッダーもなく、メモリ上のデータ形式`bit_manip::m256`をそのままファイルにしたものです。`bit_manip::m256`は、**.\BitManip256\m256.h**で定義され、**__m256i**との共用体です。

### **RBFMaker**をビルドし実行する
#### 実行し「test.rbf」を生成する
**RBFMaker**プロジェクトを「スタートアッププロジェクト」に指定し、
実行すると、コマンドプロンプトが立ち上がり、数字の羅列が続いて  

``` cmd
It was a success.
```
と表示されれば成功です。プロジェクトフォルダに「test.rbf」が生成されています。後に、AviUtl2からこのディレクトリにアクセスすればいいでしょう。

### ビルド完了後
ではいよいよ、インプットプラグインの動作確認に入りたいと思います。

### AviUtl2をVSから起動する
スタートアッププロジェクトを再び**RBFReader**に切り替え「ローカルWindowsデバッガー」で起動してみましょう。
test.rbfファイルをタイムラインに張り付けると  
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/09paste.png)  
の様に表示されます。  
#### **注意**
ファイルビューの**test.rbf**をダブルクリックしても関連付けられてないので、何も起動しません。**必ず**ドラッグアンドドロップまたは、同等の操作をして下さい。  
タイムラインカーソル（赤の縦線を勝手に命名）を動かすと画像が変わっていきますでしょうか？  
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/10TimeLineCursor.png)  


ここで更に、エクスプローラービューでtesut.rbfをコピーして貼り付け、「**test - コピー.rbf**」ファイルを作って、これも張り付けてみましょう。  
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/10-2.png)  
このような感じになりましたでしょうか。なれば**デモプログラムの構築と実行**は成功です。

## デモプログラムの挙動を観察する
### プラグインの設定を変える
動画を表示できるようにはなりましたが、「**FPSをもう少し下げたい**」とか「**画像サイズを大きめに**」とか設定したい場合があるかもしれません。もちろんAviUtl2でもある程度カバーはできます。しかしジャギーが出たり何かと不便が出る場合があります。その為に元動画の設定を変えれるようにしています。
#### 入力プラグインの設定ダイアログを出す
AviUtl2のメニュー＞設定＞入力プラグインの設定を選ぶ  
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/11ChangeCfgPlgin.png)  

すると入力プラグインの設定ダイアログが表示されます。
この中に「Reversi Board Format Reader version 0.01」があります。  
これを選択しながら右クリックすると、ポップアップメニュー出ますので「プラグインの設定」を選択すると  
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/12InputPlginListDlg.png)  

このように.rbfファイルの設定ダイアログが表示されます。  
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/13-2PluginSettingDlg.png)  
#### 設定項目の概要
- **新規ファイルのデフォルト設定** 新しいファイルを読み込ませるときの、動画ファイルの設定を行います。
- **個別のファイルの設定** 今まで読み込んだことのあるファイルで、現在もその存在が確認できたファイルの設定の変更をします。
- **一辺のドット数** 正方形で描かれますので、その一辺の数を設定します。
- **Rate** AVIファイルの単位時間辺りの刻み数を設定します。
- **Scale** AVIファイルの刻みの単位時間を設定します。
- **FPS** RateとScaleから求められる1秒辺りのフレーム数を表示しています。
- **リストの項目** 現在設定変更できるファイルを表示しています。ギアのアイコンをクリックするか、項目をダブルクリックすることにより、設定変更のダイアログが現れます。
#### 個別のファイル設定をする
リストの項目をダブルクリックするか、ギアのアイコンをクリックしてみてください。
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/14IndividualSettings.png)  
このようにプレビューウィンドウと個別に設定するダイアログが表示されます。  
「▶」や「◀」をクリックすると、盤面が変わります。  
また、一辺のドット数を変えるとプレビューウィンドウの大きさが変わります。  
プレビューウィンドウを選択して、テンキーの「+」や「-」ボタンを押したり、Ctrlを押してマウスホイールを動かすと、表示倍率が変わります。  
では、試しに「一辺のドット数」を「256」に変えて「OK」ボタンを押してダイアログを閉じてみましょう。  
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/15AfterChange.png)  
設定が反映されています。  

このダイアログも「OK」を押して閉じて、入力プラグインの設定ダイアログも閉じてみてください。  
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/16Reflected.png)  
設定変更が反映されています。  
### 入力プラグインの動作まとめ
- ファイルごとの個別の設定も可能
- 設定変更はすぐに反映され、プロジェクトファイルの再読み込みは不要

## ソリューションの各プロジェクトの概要  
先にも載せていたソリューションの構成ですが、各プロジェクトの概要です。  
![構成図](https://github.com/NewGoldSmith/AviUtl2_Input_Plugin/blob/main/images/SolutionEx1.png)  
- **BitManip** ビット操作のスタティックライブラリ。
- **BitManip256** __m256iというAVXの型のビット操作をするスタティックライブラリ。
- **CommonConfigDlg** 設定ダイアログを提供するDLLプロジェクト。本DLLはCOMインターフェースを介してアクセスされる。COMインターフェースを利用することにより、インクルードファイルの依存関係を最小化し、呼び出し元コードに対して実装の詳細を隠蔽する構成としている。
呼び出し元は、`PublicCfgDlgInterface.h`を実装ファイル（.cpp）のみにインクルードすることで、当該ファイルのみがインターフェースの存在を認識する設計としている。インターフェースの利用側と実装側の明確な分離が実現されている。
本DLLの利用例は、後述する「TestDlg」プロジェクトに実装されている。インターフェースの取得方法としては、DLLに対して直接`CoCreateInstance`を呼び出す方法に加え、レジストリに登録した上でOle32.dllの`CoCreateInstance`を呼び出す方法の双方が利用可能。
- **CommonLib** 汎用的に利用可能な関数群を収録したスタティックライブラリ。
例えば、`WtoA`はかつてMFCに含まれていたマクロに類似した機能を提供。また、`GdiSelectAndRestorer`はGDIにおいて頻繁に発生する`SelectObject`の利用後に元のオブジェクトへ復元する処理を、自動化したRAII機構として実装したクラス。今後の開発において再利用性の高い機能を順次収録していく予定。
- **DebugFnc** デバッグ用のスタティックライブラリ。トレース等で多用している。
- **DebugFncM256** __m256i用のデバッグスタティックライブラリ。__m256iの内容を、8x8x4のバイナリーで表示が可能。
- **DummyDlg** ダミーダイアログクラスのDLL。インポートライブラリでのダイアログ起動は可能か確認している。プロジェクト**TestDlg**でランチしている。
- **PreviewWindow** ビットマップをプレビュー表示するためのウィンドウを表示するクラスのスタティックライブラリ。
- **RBFMaker** RBFファイルを作成するためのCLIのプログラム。
- **RBFReader** 最終成果物入力プラグインを作るプロジェクト。
- **SliderEditControl** スライダーエディットコントロールというダイアログカスタムコントロールをサブクラス化するためのクラスのスタティックライブラリ。ダイアログエディタのプロパティのクラス名の設定が肝。
- **TestDlg** GUIのコンポーネントの単体テストを実施する実行ファイルを作るプロジェクト。**CommonConfigDlg**のcomインターフェースのテストも行っている。テストを実施する前に、例えば`regsvr32.exe .\CommonConfigDlg-Debug.dll`の様にレジストリへの登録が必要。
- **UtilDefAviUtl2** AviUtl2でしか使われないであろう関数を収録したスタティックライブラリ。  
### SDKディレクトリについて
プロジェクトとしては作成してないが、.\aviutl2_sdk\というディレクトリを作成し、そこに解凍したSDKファイルを配置した。AviUtl2の定義はここから参照するようにした。
## オフィシャルSDKの仕様の解釈と実装
SDKを解釈し、実装を行った。

### INPUT_HANDLE
　デコーダーのインスタンスを保持するポインタ型。

### INPUT_PLUGIN_TABLE
AviUtl2がプラグインにアクセスするためのインターフェーステーブル。

### INPUT_HANDLE (*func_open)(LPCWSTR file)
ファイルとデコーダーインスタンスとを一対一で対応させる関数ポインタ。ファイルごとにデコーダーインスタンスを作成するため、ファイルごとの独自の設定が可能。 
#### コード解説  
- STRUCTURE_FOR_INSTANCE（デコーダーオブジェクト）作成。
- ファイル名を保存、フラグをセット。
- INIファイルが無ければ、作成し、初期設定を書き込む。
- 入力ファイルの名前セクションが既にあるか確認。INIファイル操作が誤動作しないように、セクション名はBase64でエンコードされている。
- 無ければセクションを作成、キーをデフォルト値で設定。
- セクションがある場合でも特定のキーが無ければ（アップデート等でキーが追加されることが考えられる）キーを追加。
- 指定されたファイルセクションを読み込み。
- ビットマップインフォヘッダー、ビデオストリームインフォなどをINIから読み取りセットし、変数として保持しておく。
- 最後までエラーが無ければ、デコーダーオブジェクトのアドレスを返す。  
```cpp
   ,[](LPCWSTR lpcwstrzInFilePathName)->INPUT_HANDLE{
   // 入力ファイルをオープンする関数へのポインタ
   // file		: ファイル名
   // 戻り値	: TRUEなら入力ファイルハンドル
   /// @brief func_open
   DPTD("func_open");

   /// @detail pInstをINPUT_HANDLEにキャストして返す。
   /// 返す前に初期設定をする。途中でエラーがあった場合、
   /// デフォルトデリーターでデリートされるトラップを設定。
   unique_ptr<STRUCTURE_FOR_INSTANCE> 
      pInst(new(nothrow) STRUCTURE_FOR_INSTANCE((HMODULE)&__ImageBase));
   if(!pInst){
      DPT("Allocation failed.");
      return FALSE;
   };

   // ファイルパス名をコピー
   pInst->FilePathNameW = lpcwstrzInFilePathName;

   pInst->flag = INPUT_PLUGIN_TABLE::FLAG_VIDEO;

   /// @brief INIファイルが無ければ、作成し、初期設定を書き込む
   if(!(::PathFileExistsW(MakeIniPathNameW(pInst->hModule).c_str()))){
      MakeIniFile(pInst->hModule, BUILD_NUMBER);
   }

   /// @brief METAセクションLAST_UPDATE_INIの書き込み
   if(!WriteMapToIniW(
      L"META"
      , { {L"LAST_BUILD_DATE",MakeDateStringW()},{L"BUILD_NUMBER",to_wstring(BUILD_NUMBER)} }
      , MakeIniPathNameW(pInst->hModule))){
      DPT("META section write error.");
      return 0;
   }

   /// @brief コモンセクション読み込み
   map cfg_common = ReadIniToMapW(
      L"COMMON"
      , MakeIniPathNameW(pInst->hModule));

   /// @brief 入力ファイルのセクションがあるか確認し、
   /// 無ければセクションを作成し、デフォルト設定を書き込む。

   if(!IniSectionExistsW(
      BinToB64W(WtoBin(pInst->FilePathNameW))
      , MakeIniPathNameW(pInst->hModule))){

      /// @brief ファイルごとのデフォルト初期設定値読み込み
      map<wstring, wstring> deault_file_cfg = ReadIniToMapW(
         STR_DEFAULT_INITIAL_FILE_SETTINGS
         , MakeIniPathNameW(pInst->hModule));

      /// @brief ファイルパスとファイル名をbase64にエンコード
      deault_file_cfg[L"FILE_PATH_NAME"] = BinToB64W(
         WtoBin(pInst->FilePathNameW));

      /// @brief ファイルパスネームをbase64にエンコードしたものをセクション名に設定
      WriteMapToIniW(
         BinToB64W(WtoBin(pInst->FilePathNameW))
         , deault_file_cfg
         , MakeIniPathNameW(pInst->hModule));
   } else{
      /// 既にセクションがある場合も必要なキーが揃っていなければ、そのキーを初期値で更新。
      map<wstring, wstring> deault_file_cfg = ReadIniToMapW(
         STR_DEFAULT_INITIAL_FILE_SETTINGS
         , MakeIniPathNameW(pInst->hModule));
      map a_file_cfg = ReadIniToMapW(
         BinToB64W(WtoBin(pInst->FilePathNameW))
         , MakeIniPathNameW(pInst->hModule));
      /// @brief キーが揃っているか確認し、揃ってなければそのキーをInsert。
      if(InsertOnlyNewKeysW(a_file_cfg, deault_file_cfg)){
         WriteMapToIniW(
            BinToB64W(WtoBin(pInst->FilePathNameW))
            , a_file_cfg
            , MakeIniPathNameW(pInst->hModule));
      };
   }

   /// @brief 個別ファイル設定読み込み
   map a_file_cfg = ReadIniToMapW(
      BinToB64W(WtoBin(pInst->FilePathNameW))
      , MakeIniPathNameW(pInst->hModule));
   pInst->bmih = {
      /*biSize;         */ sizeof(BITMAPINFOHEADER)
      /*biWidth;        */ ,stoi(a_file_cfg[L"N_DOTS"])
      /*biHeight;       */ ,stoi(a_file_cfg[L"N_DOTS"])
      /*biPlanes;       */ ,1
      /*biBitCount;     */ ,24
      /*biCompression;  */ ,BI_RGB
      /*biSizeImage;    */ ,((stoi(a_file_cfg[L"N_DOTS"]) * 3 + 3) & ~3) * (DWORD)stoi(a_file_cfg[L"N_DOTS"])
      /*biYPelsPerMeter;*/
      /*biXPelsPerMeter;*/
      /*biClrUsed;      */
      /*biClrImportant; */
   };

   pInst->vrbf.clear();
   LoadM256ArrayW(pInst->FilePathNameW, pInst->vrbf);

   pInst->video_stream_info = {
   /*fccType              */streamtypeVIDEO
   /*fccHandler           */,0
   /*dwFlags              */,0
   /*dwCaps               */,0
   /*wPriority            */,0
   /*wLanguage            */,0
   /*dwScale              */,(DWORD)stoi(a_file_cfg[L"SCALE"]) // 1秒あたりの分母
   /*dwRate               */,(DWORD)stoi(a_file_cfg[L"RATE"]) // 1秒あたりの分子（1/1=1fps）
   /*dwStart              */,0
   /*dwLength             */,(DWORD)pInst->vrbf.size()// フレーム数（1フレーム）
   /*dwInitialFrames      */,0
   /*dwSuggestedBufferSize*/,pInst->bmih.biSizeImage
   /*dwQuality            */
   /*dwSampleSize         */
   /*rcFrame              */
   /*dwEditCount          */
   /*dwFormatChangeCount  */
   /*szName[64]           */
   };

   SetRect(&pInst->video_stream_info.rcFrame, 0, 0, stoi(a_file_cfg[L"N_DOTS"]), stoi(a_file_cfg[L"N_DOTS"]));
   INPUT_PLUGIN_TABLE* pipt = GetInputPluginTable();
   wcscpy_s(pInst->video_stream_info.szName, pipt->information);

   /// @brief releaseでトラップ解除。
   return pInst.release();
   }
```  
#### 私感  
多分どこかで、入力プラグインの配置されているディレクトリ情報が欲しくなる可能性が高いです。それが`func_open`なのか、`func_info_get`なのかは判りませんが。と、言うのは保存しておいた設定データを読み出すのに、ディレクトリ情報がほしいからです。モジュールハンドルが引数にあれば楽なのですが無いので、  
`extern "C" IMAGE_DOS_HEADER __ImageBase;`  
と宣言し、    
`GetModuleFileNameW`  
を呼び出してディレクトリ情報を得ています。  

### bool (\*func_close)(INPUT_HANDLE ih)
デコーダーインスタンスをデリートする関数ポインタ。
#### コード解説
- STRUCTURE_FOR_INSTANCEにキャストしてデリートする。  
```cpp
   , [](INPUT_HANDLE ih)->bool{
	// 入力ファイルをクローズする関数へのポインタ
	// ih		: 入力ファイルハンドル
	// 戻り値	: TRUEなら成功
   // @brief func_close
      DPTD("func_close");
      STRUCTURE_FOR_INSTANCE* pInst = (STRUCTURE_FOR_INSTANCE*)ih;
      delete pInst;
      return true;
      }
```
### bool (\*func_info_get)(INPUT_HANDLE ih, INPUT_INFO* iip)
デコーダーが出力する動画情報をAviUtl2が取得する関数ポインタ。  
#### コード解説
- iip->flagにINPUT_INFO::FLAG_VIDEOとINPUT_INFO::FLAG_TIME_TO_FRAMEをセット。
- 後は保存しておいた、設定値をセット。  
```cpp
      , [](INPUT_HANDLE ih, INPUT_INFO* iip)->bool{
         // @brief 入力ファイルの情報を取得する関数へのポインタ
         // ih		: 入力ファイルハンドル
         // iip		: 入力ファイル情報構造体へのポインタ
         // 戻り値	: TRUEなら成功
         //func_info_get
      DPTD("func_info_get");
      STRUCTURE_FOR_INSTANCE* pInst = (STRUCTURE_FOR_INSTANCE*)ih;
      iip->flag = 0;
      if(pInst->flag & INPUT_PLUGIN_TABLE::FLAG_VIDEO){
         iip->flag |= INPUT_INFO::FLAG_VIDEO | INPUT_INFO::FLAG_TIME_TO_FRAME;
         iip->rate = pInst->video_stream_info.dwRate;
         iip->scale = pInst->video_stream_info.dwScale;
         iip->n = pInst->video_stream_info.dwLength;
         iip->format = (BITMAPINFOHEADER*)&pInst->bmih;
         iip->format_size = pInst->bmih.biSize;
      }

      if(pInst->flag & INPUT_PLUGIN_TABLE::FLAG_AUDIO){
         iip->flag |= INPUT_INFO::FLAG_AUDIO;
         iip->audio_n = pInst->audio_stream_info.dwLength;
         iip->audio_format = (WAVEFORMATEX*)pInst->audioformat;
         iip->audio_format_size = pInst->audioformatsize;
      }

      return true;
      }
```  
事前に保存しておいた`pInst->video_stream_info`、`pInst->bmih`等を用いて`INPUT_INFO* iip`にセット。
 `pInst->video_stream_info`、`pInst->bmih`を変数に持っておくのは冗長な様に思えるが、この後の操作が高速に行える。
### int (\*func_read_video)(INPUT_HANDLE ih, int frame, void\* buf)
AviUtl2が指定したフレーム番号のビットマップビッツをbufポインタに書き込みを要求する関数ポインタ。  
#### コード解説
- INPUT_HANDLEをSTRUCTURE_FOR_INSTANCE\*にキャスト
- DIB作成
- DIBに対象の盤面を描画
- ::GetDIBitsによりbufにコピー
- 事前に計算しておいたコピーサイズをリターン　　
```cpp
      , [](INPUT_HANDLE ih, int frame, void* buf)->int{
      //func_read_video
      // 画像データを読み込む関数へのポインタ
      // ih		: 入力ファイルハンドル
      // frame	: 読み込むフレーム番号
      // buf		: データを読み込むバッファへのポインタ
      // 戻り値	: 読み込んだデータサイズ

      DPTD("func_read_video");
      STRUCTURE_FOR_INSTANCE* pInst = (STRUCTURE_FOR_INSTANCE*)ih;

      unique_ptr<remove_pointer_t<HBITMAP>, decltype(DeleteObject)*> hBitmap=
      { CreateDIB24(pInst->bmih.biWidth, pInst->bmih.biHeight),DeleteObject };

      DrawBoardDIB(pInst->vrbf[frame], hBitmap.get());

      unique_ptr<remove_pointer_t<HDC>, void(*)(HDC hDC)> hDC
      { GetDC(NULL),[](HDC hDC){ReleaseDC(NULL,hDC); } };

      ::GetDIBits(hDC.get(), hBitmap.get(), 0, pInst->bmih.biHeight, buf, (BITMAPINFO*)&pInst->bmih, DIB_RGB_COLORS);
      return pInst->bmih.biSizeImage;
      }
```  
コピーした画像データはAviUtl2でキャッシュするらしく、キャッシュに収まっていれば、フレームは何度も要求されることは無い様。 
#### 毎回DIB作成について  
呼ばれる度にDIBを作成しているが、まずは動作確認を優先しているので最適化、効率化は考慮していない。  
### int (\*func_read_audio)(INPUT_HANDLE ih, int start, int length, void\* buf)  
この関数は設定により、呼ばれる事はない。  
```cpp
      , [](INPUT_HANDLE ih, int start, int length, void* buf)->int{
      ///func_read_audio
      DPTD("func_read_audio");
      STRUCTURE_FOR_INSTANCE* pInst = (STRUCTURE_FOR_INSTANCE*)ih;
      LONG size{};
      int samplesize{};

      samplesize = ((WAVEFORMATEX*)pInst->audioformat)->nBlockAlign;
      if(AVIStreamRead(pInst->paudio, start, length, buf, samplesize * length, NULL, &size) != S_OK) return 0;
      return size; }
```  
### bool (*func_config)(HWND hwnd, HINSTANCE dll_hinst)
入力プラグインの設定をする時に呼ばれる関数ポインタ。設定は各プラグインが保存しなければならない。このプラグインはINIファイルを使用する仕様にしたが、別にjsonでもXMLでも、または、SQLでも構わないだろうと考えられる。
#### コード解説
- このプラグインをプラグインホルダーに配置して、AviUtl2を起動して、いきなりプラグインの設定をする場合も考えられる。その場合、INIファイルはまだ作成されていないので、この関数が呼ばれたら、まず、INIファイルが存在しているか確認し、無ければ作成する。  
- L"CommonConfigDlg*.dll"をプラグインディレクトリで探す。このファイルが設定ダイアログの本体DLLである。  
- あれば`LoadLibraryW`でロードする。
- `GetProcAddress`で"CoCreateInstance"という関数のアドレスを得る。
- 取得した関数アドレスで関数を実行し「ICfgDlg」インタフェース取得を試みる。関数の機能はole2.dllの`CoCreateInstance`と同等であるが、「PublicCfgDlgInterface.h」で定義されているもの以外は取得できないように設定している。
- `DialogBoxParamW`を実行。設定ダイアログをランチする。
- `DialogBoxParamW(hwnd, dll_hinst, result)`の第二引数は、INIファイルにたどり着くためのモジュールハンドル。  
```cpp
      , [](HWND hwnd, HINSTANCE dll_hinst)->bool{
      // 入力設定のダイアログを要求された時に呼ばれる関数へのポインタ (nullptrなら呼ばれません)
      // hwnd			: ウィンドウハンドル
      // dll_hinst	: インスタンスハンドル
      // 戻り値		: TRUEなら成功
       //func_config
      DPTD("func_config");

      /// @brief INIファイルが無ければ、作成し、初期設定を書き込む
      if(!(::PathFileExistsW(MakeIniPathNameW(dll_hinst).c_str()))){
         MakeIniFile(dll_hinst, BUILD_NUMBER);
      }

      /// @brief METAセクションLAST_BUILD_DATE、BUILD_NUMBERの書き込み
      if(!WriteMapToIniW(
         L"META"
         , { {L"LAST_BUILD_DATE",MakeDateStringW()},{L"BUILD_NUMBER",to_wstring(BUILD_NUMBER)} }
         , MakeIniPathNameW(dll_hinst))){
         DPT("META section write error.");
         return 0;
      }

         /// モジュールファイルを探しロードし、関数ポインタからCoCreateInstanceを実行。
      WCHAR path[MAX_PATH + 1];
      if(!::GetModuleFileNameW(dll_hinst, path, MAX_PATH))
         return FALSE;
      if(::PathCchRemoveFileSpec(path, MAX_PATH) != S_OK){
         return FALSE;
      }

      wstring strfind = path;
      strfind += L"\\";
      /// ワイルドカードも可能な文字列
      strfind += FIND_MODULE_STRING;
      wstring strfound;
      if(strfound = FindFirstMatchingFile(strfind);
         strfound.empty()){
         DPT("DLL not found.");
         ::MessageBoxA(hwnd
                     ,"CommonConfigDlg*.dllが見つかりません。プラグインと同じディレクトリに配置してください。"
                     ,"Error"
                     ,MB_OK | MB_ICONEXCLAMATION);
         return FALSE;
      }

      unique_ptr<remove_pointer_t<HMODULE>, decltype(FreeLibrary)*>hDll
      { ::LoadLibraryW(strfound.c_str()),FreeLibrary };
      if(!hDll){
         DPLE;
         return FALSE;
      }

      using PFN_CO_CREATE_INSTANCE =
         HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);

      PFN_CO_CREATE_INSTANCE pfnCoCreateInstance =
         (PFN_CO_CREATE_INSTANCE)::GetProcAddress(hDll.get(), CO_CREATE_INSTANCE_STRING);
      if(!pfnCoCreateInstance){
         DPLE;
         return FALSE;
      }

      ComPtr<ICfgDlg> pDlg;
      HRESULT hr = pfnCoCreateInstance(
         CLSID_CFG_DLG
         , NULL
         , CLSCTX_INPROC_SERVER
         , IID_ICfgDlg
         , (void**)pDlg.GetAddressOf());

      if(SUCCEEDED(hr) && pDlg){
         LONGLONG result{};
         pDlg->DialogBoxParamW(hwnd, dll_hinst, result);
      }

      // DLLを開放されても設定が残るように保存しておいてください
      return true; }
```  
### int (*func_set_track)(INPUT_HANDLE ih, int type, int index)  
この関数は使用しないので、コールされないように設定されている。
```cpp
      , [](INPUT_HANDLE ih, int type, int index)->int{
	// 入力ファイルの読み込み対象トラックを設定する関数へのポインタ (FLAG_MULTI_TRACKが有効の時のみ呼ばれます)
	// func_open()の直後にトラック数取得、トラック番号設定が呼ばれます。※オープン直後の設定以降は呼ばれません
	// ih		: 入力ファイルハンドル
	// type		: メディア種別 ( 0 = 映像 / 1 = 音声 )
	// index	: トラック番号 ( -1 が指定された場合はトラック数の取得 )
	// 戻り値	: 設定したトラック番号 (失敗した場合は -1 を返却)
	//			  トラック数の取得の場合は設定可能なトラックの数 (メディアが無い場合は 0 を返却)
        // func_set_track
      DPTD("func_set_track");
      return -1;
      }
```  
### int (*func_time_to_frame)(INPUT_HANDLE ih, double time)
この関数は可変FPSに対応する為に用意されていると思われる。指定timeでの適切なフレームをデコーダーが指示するものであろう。使わなくても動作確認はとれていたが、敢えて使用している。  
```cpp
      , [](INPUT_HANDLE ih, double time)->int{
      // 映像の時間から該当フレーム番号を算出する時に呼ばれる関数へのポインタ (FLAG_TIME_TO_FRAMEが有効の時のみ呼ばれます)
      // 画像データを読み込む前に呼び出され、結果のフレーム番号で読み込むようになります。
      // ※FLAG_TIME_TO_FRAMEを利用する場合のINPUT_INFOのrate,scale情報は平均フレームレートを表す値を設定してください
      // ih		: 入力ファイルハンドル
      // time		: 映像の時間(秒)
      // 戻り値	: 映像の時間に対応するフレーム番号
            // func_time_to_frame
         DPTD("func_time_to_frame");
         STRUCTURE_FOR_INSTANCE* pInst = (STRUCTURE_FOR_INSTANCE*)ih;

         double fps = (double)pInst->video_stream_info.dwRate
            / (double)pInst->video_stream_info.dwScale;
         return (int)(time * fps); // 切り捨て
         }
```  
### AviUtl2の仕様についてのまとめ  
映像についてのみの私感ですが、フレームをDIB形式でドロー出来る仕様はありがたいです。SDKの**AviReader.cpp**も大変参考になりました。  
これで、「オフィシャルSDKの仕様の解釈と実装」の解説は終わります。  
# どのように発展させるのか？
今回作ったRBFReaderですが、リバーシの盤面を表示させるという、需要がそう、ありそうでないものを選択しています。まあ、  
「自由研究だから、流れを追うために簡単でいいだろう。」  
と、思っていたのですが結構時間がかかりました。  
## 3Dモデル
作っているときにふと  
「3Dモデルとかも表示できるよな。」  
とか考えたりしました。しかし、それを動かすとなると個別モデルは何とかアニメーションみたいなことはできるが、編集画面いっぱいに動かすとなると、AviUtil側に何かを求めることになるし、それを求めるのは違うよなという考えに至りました。  
## Blenderスクリプト  
で、次に  
「Blenderにスクリプト機能があるので、それを読み込ませれば。」  
と考えました。しかし、それならBlenderで動画ファイル作って、AviUtl2で合成すればいいのでは、という考えに至りました。
## リモートでレンダーframeを指定する  
しかし、レンダーするフレームを指定できるならプラグインからフレームを指定して、レンダー出来るのではと考えました。これは、Blenderで動画ファイルを作るのとどこが違うのかというと、ほかのオブジェクトと同期出来ることです。ボイスと同期でき、またクロマキーを設定すると他のビジュアルオブジェクトとはめ込み合成もできます。  
AIにBlenderをリモートで動かすことはできないか尋ねたところ、できないとのこと。
## Unreal Engine  
では、AIにリモートで操作できる3Dソフトはないのかと尋ねたところ、**Unreal Engine**がリモート操作ができるとのこと。Unreal Engineは使ったことはありません。「**ブループリント**」という言葉は聞いたことがあるくらいで、実際どういう操作なのかわかりません。リモートで何まで出来るのか判りませんが、AIに尋ねると画像転送は出来るのではとのこと。
TCP/IP通信できるとの事なので、インターリーブで複数のframeを分散してレンダリングする事も出来そうです。共有機能もあるようなので、一台は編集用、後はその追随用としての使用もできる可能性があります。  
# まとめ  
- AviUtil2の入力プラグインの作成を試みた。
- DIB形式対応で、実装するのに助かった。
- 発展系としてUnreal Engineとの同期し画像生成し、転送して動画オブジェクトとして編集可能になるのではと考えられた。
# 終わりに
　AviUtl、AviUtl2という素晴らしいソフトを公開して下さいましたKENくん様に感謝申し上げます。  
また、読者様におかれましては、この記事が閃きのきっかけになれば幸いです。  









