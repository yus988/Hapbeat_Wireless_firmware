オリジナルのタスクはこちらに格納してください

優先読み込み（オーバーライド）
- `src/private_tasks/adjustParams.hpp` を置くと、`src/adjustParams.cpp` が最優先でこれを取り込みます（`__has_include`）。
- 既存の `TASK_*` に依存しない一括上書きが可能です。

使い方の例
1. `src/sample_tasks/taskBandGenESPNOW` を参考に、必要な配列や設定をまとめた `src/private_tasks/adjustParams.hpp` を作成
2. `platformio.ini` で対象 env をビルド
3. 公開したくない限り、このファイルは main の公開スナップショットに含まれません