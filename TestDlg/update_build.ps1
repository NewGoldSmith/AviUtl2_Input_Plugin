$scriptDir = if ($PSScriptRoot) { $PSScriptRoot } else { Split-Path -Parent $MyInvocation.MyCommand.Path }
$path      = Join-Path $scriptDir "build_info.h"
$backup    = "$path.bak"
$pattern   = '#define\s+BUILD_NUMBER\s+(\d+)'

function Get-BuildNumberFromFile($filePath) {
    try {
        $content = Get-Content $filePath -Raw
    } catch {
        return $null
    }

    if ($content -match $pattern) {
        return [int]$matches[1]
    } else {
        return $null
    }
}

function Update-BuildNumberInFile($filePath, $newValue) {
    try {
        $updated = $null
        if (Test-Path $filePath) {
            try {
                $content = Get-Content $filePath -Raw
            } catch {
                $content = ""
            }

            if ($null -eq $content) { $content = "" }

            if ($content -match $pattern) {
                $updated = [regex]::Replace($content, $pattern, "#define BUILD_NUMBER $newValue")
            } else {
                if ([string]::IsNullOrWhiteSpace($content)) {
                    $updated = "#define BUILD_NUMBER $newValue"
                } else {
                    $updated = $content.TrimEnd("`r", "`n") + "`r`n#define BUILD_NUMBER $newValue"
                }
            }
        } else {
            $updated = "#define BUILD_NUMBER $newValue"
        }

        # 改行を CRLF に統一し、CRLF が2回以上続く箇所を単一の CRLF にまとめる
        if ($null -eq $updated) { $updated = "" }
        # まずすべての改行を CRLF に統一
        $updated = [regex]::Replace($updated, '\r?\n', "`r`n")
        # 続く CRLF を 1 つにまとめる（空行を削除）
        $updated = [regex]::Replace($updated, '(?:\r\n){2,}', "`r`n")
        # 末尾は必ず 1 個の CRLF にする
        if (-not $updated.EndsWith("`r`n")) { $updated += "`r`n" }
        $updated = [string]$updated

        $folder = Split-Path $filePath
        if ($folder -and -not (Test-Path $folder)) {
            Write-Warning "フォルダが存在しません: $folder"
            return $false
        }

        if ([string]::IsNullOrWhiteSpace($updated)) {
            Write-Warning "更新内容が空です。ファイルに書き込めません。"
            return $false
        }

        $updated | Set-Content -Path $filePath -Encoding utf8
        return $true
    } catch {
        Write-Warning "Set-Content で例外が発生しました: $_"
        return $false
    }
}

# Read both files
$mainNumber = Get-BuildNumberFromFile $path
$bakNumber  = Get-BuildNumberFromFile $backup

# If both fail, abort
if ($mainNumber -eq $null -and $bakNumber -eq $null) {
    Write-Warning "BUILD_NUMBER not found in either file. Aborting."
    return
}

# Choose the source number (prefer .bak if available)
if ($bakNumber -ne $null) {
    $base = $bakNumber
} elseif ($mainNumber -ne $null) {
    $base = $mainNumber
} else {
    Write-Warning "Unable to determine base build number."
    return
}

$next = $base + 1

# Update both files
$mainUpdated = Update-BuildNumberInFile $path   $next
$bakUpdated  = Update-BuildNumberInFile $backup $next

# Report result
if ($mainUpdated -and $bakUpdated) {
    Write-Host "BUILD_NUMBER updated to $next in both files."
} elseif ($mainUpdated) {
    Write-Host "BUILD_NUMBER updated to $next in build_info.h only."
} elseif ($bakUpdated) {
    Write-Host "BUILD_NUMBER updated to $next in backup only."
} else {
    Write-Warning "Failed to update either file."
}
