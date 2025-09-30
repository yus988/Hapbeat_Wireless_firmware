param(
  [string]$SampleTaskName = "",
  [string]$NewTaskName = "",
  [string]$EnvName = "",
  [ValidateSet('ESPNOW','MQTT','WIRED')]
  [string]$Protocol = "ESPNOW",
  [ValidateSet('BAND_V3','NECKLACE_V3')]
  [string]$Device = "NECKLACE_V3"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Prompt-Choice($title, $choices) {
  Write-Host $title -ForegroundColor Cyan
  for ($i=0; $i -lt $choices.Count; $i++) { Write-Host ("  [{0}] {1}" -f $i, $choices[$i]) }
  while ($true) {
    $idx = Read-Host "番号を入力"
    if ($idx -match '^[0-9]+$' -and [int]$idx -ge 0 -and [int]$idx -lt $choices.Count) { return $choices[[int]$idx] }
    Write-Host "無効な入力です" -ForegroundColor Yellow
  }
}

$repoRoot = Split-Path -Path $PSCommandPath -Parent
Set-Location $repoRoot

$sampleRoot = Join-Path $repoRoot "src/sample_tasks"
$privateRoot = Join-Path $repoRoot "src/private_tasks"
if (-not (Test-Path $sampleRoot)) { throw "not found: $sampleRoot" }
if (-not (Test-Path $privateRoot)) { New-Item -ItemType Directory -Force $privateRoot | Out-Null }

$samples = Get-ChildItem -Path $sampleRoot -Directory | Select-Object -ExpandProperty Name
if ([string]::IsNullOrWhiteSpace($SampleTaskName)) {
  $SampleTaskName = Prompt-Choice "コピー元 (sample_tasks) を選択" $samples
}
if (-not ($samples -contains $SampleTaskName)) { throw "未知のサンプル: $SampleTaskName" }

if ([string]::IsNullOrWhiteSpace($NewTaskName)) {
  $suffix = Read-Host "新しい private タスク名の末尾のみ (例: NeckNewESPNOW) — 先頭 'task' は自動付与"
  if ([string]::IsNullOrWhiteSpace($suffix)) { throw "タスク名が空です" }
  $NewTaskName = "task" + $suffix.Trim()
}
if ($NewTaskName -notmatch '^task') {
  # 先頭に 'task' が無ければ自動付与
  $NewTaskName = "task" + $NewTaskName
}
if ($NewTaskName -notmatch '^task[A-Za-z0-9]') { throw "タスク名は 'task' に続けて英数字で指定してください" }

# サンプル名から Device / Protocol を自動推定（対話時の手戻り軽減）
if ($SampleTaskName -match '^taskBand') { $Device = 'BAND_V3' }
elseif ($SampleTaskName -match '^taskNeck') { $Device = 'NECKLACE_V3' }

if ($SampleTaskName -match 'MQTT') { $Protocol = 'MQTT' }
elseif ($SampleTaskName -match 'WIRED') { $Protocol = 'WIRED' }
else { $Protocol = 'ESPNOW' }

# 既存チェック
$destDir = Join-Path $privateRoot $NewTaskName
if (Test-Path $destDir) { throw "既に存在します: $destDir" }

# コピー
Write-Host "==> Copy $SampleTaskName -> $NewTaskName" -ForegroundColor Cyan
Copy-Item -Recurse -Force (Join-Path $sampleRoot $SampleTaskName) $destDir

# 既定値の補助入力（自動命名）
if ([string]::IsNullOrWhiteSpace($EnvName)) {
  $prefix = ($Device -eq 'BAND_V3') ? 'BandWL_V3' : 'DuoWL_V3'
  $taskPart = ($NewTaskName -replace '^task','').ToUpper()
  $EnvName = ("{0}_{1}" -f $prefix, $taskPart)
}

# 1) platformio.private.ini へ env を追記
$piPriv = Join-Path $privateRoot "platformio.private.ini"
if (-not (Test-Path $piPriv)) { New-Item -ItemType File -Force $piPriv | Out-Null }
$piBody = Get-Content -Raw $piPriv
$envPattern = "(?m)^\[env:" + [regex]::Escape($EnvName) + "\]"
if ($piBody -notmatch $envPattern) {
  # platformio.ini から元 env の build_flags を抽出
  $pioPath = Join-Path $repoRoot "platformio.ini"
  $pioText = Get-Content -Raw $pioPath
  $sectionMatches = [regex]::Matches($pioText, "(?ms)^\[env:(?<name>[^\]]+)\]\s*(?<body>.*?)(?=^\[env:|\Z)")
  $srcFlags = @()
  foreach ($m in $sectionMatches) {
    $body = $m.Groups['body'].Value
    if ($body -match [regex]::Escape("sample_tasks/$SampleTaskName/")) {
      # build_flags ブロック内に限定せず、セクション全体から -D/-I 行のみ抽出
      $flagLines = [regex]::Matches($body, "(?m)^[ \t]+(-D|-I)\b.*$")
      foreach ($fl in $flagLines) { $srcFlags += $fl.Value.Trim() }
      break
    }
  }
  # 変換: sample の include を private に、TASK_* を新マクロに、EN_MCP4018 追加
  $macro = ("TASK_{0}" -f ($NewTaskName -replace '^task','' ) ).ToUpper()
  $outFlags = New-Object System.Collections.Generic.List[string]
  $hasInclude = $false
  $hasEN = $false
  foreach ($f in ($srcFlags | Select-Object -Unique)) {
    if ($f -match '^-D\s+TASK_') { continue }
    if ($f -match '^-I\s+src/sample_tasks/') {
      $f = "-I src/private_tasks/$NewTaskName"
      $hasInclude = $true
    }
    if ($f -match '^-D\s+EN_MCP4018') { $hasEN = $true }
    $outFlags.Add($f)
  }
  if (-not $hasInclude) { $outFlags.Add("-I src/private_tasks/$NewTaskName") }
  if (-not $hasEN) { $outFlags.Add("-D EN_MCP4018") }
  $outFlags.Add("-D $macro")

  $flagsText = ($outFlags | ForEach-Object { "    $_" }) -join "`r`n"
  $section = "[env:$EnvName]`r`nbuild_flags =`r`n$flagsText`r`n" +
             "lib_ignore = MQTT_manager`r`n" +
             "lib_deps =`r`n" +
             "    `${env.lib_deps}`r`n" +
             "build_src_filter =`r`n" +
             "    +<*> -<sample_tasks/*> -<private_tasks/*> +<private_tasks/$NewTaskName/>`r`n"
  Add-Content -Path $piPriv -Value ("`r`n" + $section)
  Write-Host "==> platformio.private.ini を更新: [env:$EnvName]" -ForegroundColor Green
} else {
  Write-Host "platformio.private.ini: 既に [env:$EnvName] が存在します（スキップ）" -ForegroundColor Yellow
}

# 2) dispatcher に 1 行追加
$dispatcher = Join-Path $privateRoot "adjustParams.hpp"
if (-not (Test-Path $dispatcher)) {
  Set-Content -Path $dispatcher -Value "#pragma once`r`n`r`n#if 0`r`n#endif`r`n" -Encoding utf8
}
$dispContent = Get-Content -Raw $dispatcher
$taskMacro = ("TASK_{0}" -f ($NewTaskName -replace '^task','' ) ).ToUpper()
$nl = "`r`n"
$includeLine = ("#elif defined({0}){1}  #include ""{2}/adjustParams.hpp""" -f $taskMacro, $nl, $NewTaskName)
if ($dispContent -notmatch [regex]::Escape($includeLine)) {
  # 挿入位置: 末尾の #endif の直前に差し込む。無ければ末尾に足す。
  if ($dispContent -match "#endif\s*$") {
    $dispContent = $dispContent -replace "#endif\s*$", ($includeLine + $nl + "#endif" + $nl)
  } else {
    $dispContent += ($nl + $includeLine + $nl + "#endif" + $nl)
  }
  Set-Content -Path $dispatcher -Value $dispContent -Encoding utf8
  Write-Host "==> adjustParams.hpp に分岐を追加: $taskMacro -> $NewTaskName" -ForegroundColor Green
} else {
  Write-Host "adjustParams.hpp: 既に分岐が存在します（スキップ）" -ForegroundColor Yellow
}

Write-Host "完了:" -ForegroundColor Cyan
Write-Host ("  コピー元: src/sample_tasks/{0}" -f $SampleTaskName)
Write-Host ("  作成先:   src/private_tasks/{0}" -f $NewTaskName)
Write-Host ("  追加env:  [env:{0}] ({1}, {2})" -f $EnvName, $Device, $Protocol)
Write-Host "PlatformIO の環境一覧を更新してビルドしてください。" -ForegroundColor Cyan
Write-Host "表示されない場合は VSCode のウィンドウ再読み込み (Ctrl+Shift+P → Reload Window) または 'PlatformIO: Rebuild IntelliSense Index' を実行してください。" -ForegroundColor Yellow


