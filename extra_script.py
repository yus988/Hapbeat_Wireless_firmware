import os
import shutil

print("===== extra_script.py 実行開始 =====")

# コピー元とコピー先のパス
src_file = os.path.join(os.getcwd(), 'src', 'adjustParams.cpp')
template_file = os.path.join(os.getcwd(), 'src', 'adjustParams.cpp.tpl')

# adjustParams.cpp が存在しない場合、テンプレートからコピー
if not os.path.exists(src_file):
    print("adjustParams.cpp が見つかりません。テンプレートからコピーします。")
    shutil.copy(template_file, src_file)
    print("adjustParams.cpp を作成しました。")
else:
    print("adjustParams.cpp は既に存在します。")
