NeonFrame
---

C++/DirectCompositionで光るウィンドウ
![NeonFramePreview](https://raw.githubusercontent.com/Pctg-x8/NeonFrame/master/screen.png)

## Summary

Windows 10で新設されたDirectCompositionの新機能を使って、比較的簡単に発光するウィンドウを作ってみました。  
ウィンドウの移動、リサイズは対応していますがクローズボタンはないのでタスクバーから終了することになります。

## Classes

- AppContext: ウィンドウ、メッセージ周りの処理
- Dx2: DirectX/DirectWrite周りのオブジェクト管理
- Comp: DirectCompositionのオブジェクト管理/UIの作成と管理

## License

Public Domain
