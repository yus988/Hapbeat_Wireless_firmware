param(
  [string]$SourceBranch = "hapbeat_dev",
  [string]$BaseRef = "origin/main",
  [string]$PublishBranch = "",
  [switch]$Push,
  [switch]$OpenPR
)

set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($PublishBranch)) {
  $ts = Get-Date -Format "yyyyMMdd-HHmm"
  $PublishBranch = "publish/$ts"
}

Write-Host "==> Fetch origin" -ForegroundColor Cyan
git fetch origin | Out-Host

Write-Host "==> Create branch $PublishBranch from $BaseRef" -ForegroundColor Cyan
git switch -c $PublishBranch $BaseRef | Out-Host

Write-Host "==> Restore allowlisted paths from $SourceBranch" -ForegroundColor Cyan
git restore -s $SourceBranch -- `
  include/ `
  lib/ `
  licenses/ `
  LICENSE `
  platformio.ini `
  readme.md `
  src/adjustParams_defaults.cpp `
  src/adjustParams.cpp `
  src/globals.cpp `
  src/main.cpp `
  src/sample_tasks/ | Out-Host

$status = git status --porcelain
if ([string]::IsNullOrWhiteSpace($status)) {
  Write-Host "No public changes to publish." -ForegroundColor Yellow
  exit 0
}

Write-Host "==> Commit public snapshot" -ForegroundColor Cyan
git add -A | Out-Host
git commit -m "publish: extract public changes from $SourceBranch (ignore private_tasks)" | Out-Host

if ($Push) {
  Write-Host "==> Push branch $PublishBranch" -ForegroundColor Cyan
  git push -u origin $PublishBranch | Out-Host

  if ($OpenPR) {
    $remote = git config --get remote.origin.url
    if ($remote -match "^git@github.com:(.*)\.git$") {
      $slug = $Matches[1]
      $url = "https://github.com/$slug/compare/main...$PublishBranch?expand=1"
      Start-Process $url | Out-Null
    } elseif ($remote -match "^https://github.com/(.*)\.git$") {
      $slug = $Matches[1]
      $url = "https://github.com/$slug/compare/main...$PublishBranch?expand=1"
      Start-Process $url | Out-Null
    }
  }
}

Write-Host "Done." -ForegroundColor Green


