![r0nna-aqshy](https://user-images.githubusercontent.com/61189782/132125471-a12e120f-0ef6-4d49-9e70-e03e6d01b375.gif)

<p>初代x86エミュレータです。ここはもう閉じました。今はこちらで作業しています。->  https://github.com/NiwakaDev/X86_EMULATOR_2</p>

<p>ここは閉じてます。</p>

# NIWAKA_X86
x86_emulator written in C++11

<h2>Build</h2>
make

<h2>Run</h2>
./outputs/x86

<h2>Library</h2>
SDL2.0

<h2>Reference</h2>
<h3><a href="https://www.amazon.co.jp/%E8%87%AA%E4%BD%9C%E3%82%A8%E3%83%9F%E3%83%A5%E3%83%AC%E3%83%BC%E3%82%BF%E3%81%A7%E5%AD%A6%E3%81%B6x86%E3%82%A2%E3%83%BC%E3%82%AD%E3%83%86%E3%82%AF%E3%83%81%E3%83%A3-%E3%82%B3%E3%83%B3%E3%83%94%E3%83%A5%E3%83%BC%E3%82%BF%E3%81%8C%E5%8B%95%E3%81%8F%E4%BB%95%E7%B5%84%E3%81%BF%E3%82%92%E5%BE%B9%E5%BA%95%E7%90%86%E8%A7%A3-%E5%86%85%E7%94%B0%E5%85%AC%E5%A4%AA/dp/4839954747">自作エミュレータで学ぶx86アーキテクチャ-コンピュータが動く仕組みを徹底理解! </a></h3>
<p>エミュレータの第一歩として読みました。リアルモードやプロテクトモードや仮想記憶やディスプレイやマウスやキーボードは実装されておらず、あくまでもエミュレータの基礎が学べる入門書。無の状態から何かに取り組むのはハードルが高いですが、その第一歩が技術書として存在しているのはありがたいことです。</p>
<h3>https://github.com/shift-crops/x86emu</h3>
<p>キーボードコントローラを参照しました。その際にビットフィールドなるものがあることを参照先で知り、この後に開発したファミコンエミュレータではビットフィールドを多用することになりました。ある程度自力で完成させた後に、参照先を覗いてみたのですが、完成度が高くてショックを受けました。自分の力量がまだまだであることを思い知りました。参照先と異なる点は、機械語命令をクラスとして実装している点が大きく異なります。未実装の機械語命令にも対応しやすいように工夫してあります。</p>
<h3><a href="https://www.intel.co.jp/content/dam/www/public/ijkk/jp/ja/documents/developer/IA32_Arh_Dev_Man_Vol1_Online_i.pdf">IA-32 インテル® アーキテクチャソフトウェア・デベロッパーズ・マニュアル　上巻</a></h3>
<p>IA-32の仕様が網羅されているので、参考になりました。</p>
<h3><a href="https://www.intel.co.jp/content/dam/www/public/ijkk/jp/ja/documents/developer/IA32_Arh_Dev_Man_Vol2A_i.pdf">IA-32 インテル® アーキテクチャソフトウェア・デベロッパーズ・マニュアル　中巻 A： 命令セット・リファレンス A-M</a></h3>
<p>頭文字A-Mの機械語命令の仕様が公開されており、参考になりました。</p>
<h3><a href="https://www.intel.co.jp/content/dam/www/public/ijkk/jp/ja/documents/developer/IA32_Arh_Dev_Man_Vol2B_i.pdf">IA-32 インテル® アーキテクチャソフトウェア・デベロッパーズ・マニュアル　中巻 B： 命令セット・リファレンス N-Z</a></h3>
<p>頭文字N-Zの機械語命令の仕様が公開されており、参考になりました。</p>
<h3><a href="https://www.amazon.co.jp/%E3%81%AF%E3%81%98%E3%82%81%E3%81%A6%E8%AA%AD%E3%82%80486%E2%80%9532%E3%83%93%E3%83%83%E3%83%88%E3%82%B3%E3%83%B3%E3%83%94%E3%83%A5%E3%83%BC%E3%82%BF%E3%82%92%E3%82%84%E3%81%95%E3%81%97%E3%81%8F%E8%AA%9E%E3%82%8B-%E8%92%B2%E5%9C%B0-%E8%BC%9D%E5%B0%9A/dp/4756102131">はじめて読む486―32ビットコンピュータをやさしく語る</a></h3>
<p>IA-32の仕様書をいきなり読もうとしても挫折したので、まずはこちらから読みました。特権レベルや仮想記憶(セグメンテーションやページング)やリアルモードやプロテクトモードやアドレッシングモードといった気になるところがほぼ全て解説されており、この本がなければIA-32のエミュレータを作ることはなかったと思います。皆さんに読んで欲しいのですが、絶版なのが残念です。</p>
<h3><a href="https://www.amazon.co.jp/30%E6%97%A5%E3%81%A7%E3%81%A7%E3%81%8D%E3%82%8B-OS%E8%87%AA%E4%BD%9C%E5%85%A5%E9%96%80-%E5%B7%9D%E5%90%88-%E7%A7%80%E5%AE%9F/dp/4839919844/ref=pd_vtp_5/356-0188609-1667566?pd_rd_w=qrIxz&pf_rd_p=949e26f5-c2ef-4c96-bfde-49d7614d0317&pf_rd_r=FQQA4209JZRMHNABD541&pd_rd_r=2bdba9dc-9d7d-401a-b66c-053b098be917&pd_rd_wg=dpohq&pd_rd_i=4839919844&psc=1">30日でできる! OS自作入門</a></h3>
<p>このエミュレータ上で動かしているOSです。低レイヤな分野に初めて触れることになった技術書です。この本がきっかけで低レイヤな分野を素人の私でも少しは詳しくなれましたし、この著者の方には感謝しかありません。</p>
