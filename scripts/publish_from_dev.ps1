param(
  [string]$SourceBranch = "hapbeat_dev",
  [string]$BaseRef = "origin/main",
  [string]$PublishBranch = "",
  [switch]$IncludeMyPrivateTask,
  [string]$Title = "Publish from hapbeat_dev",
  [string]$Body = "Allowlist: include sample_tasks (and MyPrivateTask if opted)"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($PublishBranch)) {
  $PublishBranch = "publish/" + (Get-Date -Format "yyyyMMdd-HHmm")
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

if ($IncludeMyPrivateTask) {
  git restore -s $SourceBranch -- src/private_tasks/MyPrivateTask/ 2>$null | Out-Host
}

if (git diff --quiet) {
  Write-Host "No public changes to publish." -ForegroundColor Yellow
  exit 0
}

Write-Host "==> Commit snapshot" -ForegroundColor Cyan
git add -A | Out-Host
git commit -m ("publish: extract public from {0} (sample_tasks{1})" -f $SourceBranch, $(if($IncludeMyPrivateTask){'+ MyPrivateTask'}else{''})) | Out-Host

Write-Host "==> Push $PublishBranch" -ForegroundColor Cyan
git push -u origin $PublishBranch | Out-Host

if (-not (Get-Command gh -EA SilentlyContinue)) {
  Write-Host "gh CLI not found. Please open PR manually." -ForegroundColor Yellow
  $remote = git config --get remote.origin.url
  if ($remote -match '^git@github.com:(.*)\.git$') { $slug = $Matches[1] }
  elseif ($remote -match '^https://github.com/(.*)\.git$') { $slug = $Matches[1] }
  if ($slug) { Write-Host ("https://github.com/{0}/compare/main...{1}?expand=1" -f $slug,$PublishBranch) }
  exit 0
}

Write-Host "==> Create PR via gh" -ForegroundColor Cyan
gh pr create --base main --head $PublishBranch --title $Title --body $Body | Out-Host

Write-Host "Done." -ForegroundColor Green


