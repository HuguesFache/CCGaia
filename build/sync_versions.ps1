# =====================================================================
#  sync_versions.ps1  --  OUTIL VIVANT (a relancer a volonte)
#
#  CCGaia est la SOURCE UNIQUE. Ce script ne touche JAMAIS aux sources :
#  il regenere seulement, EN PLACE, les fichiers projet Windows :
#    - le bloc <ProjectConfigurations> de chaque .vcxproj (Client/Server x versions)
#    - la solution Gaia\Gaia.sln
#
#  Pour ajouter/retirer une version d'InDesign : editer $versions ci-dessous
#  (la SDK C:\SDKs\adobe_indesign_ccXXXX doit exister), puis relancer.
#  Le common props (Gaia.Common.props) est generique (ID_VER deduit du nom de
#  config) : il n'a PAS besoin d'etre modifie pour une nouvelle version.
# =====================================================================
$ErrorActionPreference = 'Stop'

# ----- Versions gerees : SEULE LIGNE a editer pour ajouter une version -----
$versions = @('2021','2022','2023','2024','2025','2026')
# ----- Projets non buildes en config Server (UI + plugin Pagemakeup) -------
$serverSkip = @('Pagemakeup','XPageUI','XRailUI')
# ---------------------------------------------------------------------------

$root     = Split-Path $PSScriptRoot -Parent      # ...\CCGaia
$gaiaDir  = Join-Path $root 'Gaia'
$latin1   = [Text.Encoding]::GetEncoding(28591)
$utf8     = New-Object Text.UTF8Encoding $false
$reSL     = [Text.RegularExpressions.RegexOptions]::Singleline
$typeGuid = '{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}'

$cfgNames = foreach ($v in $versions) { "Client-$v"; "Server-$v" }

# Bloc <ProjectConfigurations> canonique
$pcLines = ($cfgNames | ForEach-Object {
    "    <ProjectConfiguration Include=`"$_|x64`"><Configuration>$_</Configuration><Platform>x64</Platform></ProjectConfiguration>"
}) -join "`r`n"
$projConfigs = "<ItemGroup Label=`"ProjectConfigurations`">`r`n$pcLines`r`n  </ItemGroup>"

# Tous les vcxproj de CCGaia (hors Gaia\, build\, .vs)
$vcx = Get-ChildItem -Recurse $root -Filter *.vcxproj |
       Where-Object { $_.FullName -notmatch '\\(Gaia|build|\.vs)\\' }

$projs = foreach ($f in $vcx) {
    $t = [IO.File]::ReadAllText($f.FullName, $latin1)
    # remplace le bloc de configs par le set courant
    $t2 = [regex]::Replace($t, '<ItemGroup Label="ProjectConfigurations">.*?</ItemGroup>', { $projConfigs }, $reSL)
    if ($t2 -ne $t) { [IO.File]::WriteAllText($f.FullName, $t2, $latin1) }
    $name = ([regex]::Match($t2, '<ProjectName>([^<]+)</ProjectName>')).Groups[1].Value
    if (-not $name) { $name = $f.BaseName }
    $guid = ([regex]::Match($t2, '<ProjectGuid>(\{[^}]+\})</ProjectGuid>')).Groups[1].Value
    $rel  = '..\' + $f.FullName.Substring($root.Length + 1)   # relatif a Gaia\
    Write-Host "== $name ($($cfgNames.Count) configs)"
    [pscustomobject]@{ Name=$name; Guid=$guid; Rel=$rel }
}
$projs = $projs | Sort-Object Name

# ----- Gaia\Gaia.sln --------------------------------------------------------
New-Item -ItemType Directory -Force -Path $gaiaDir | Out-Null
$sb = New-Object Text.StringBuilder
[void]$sb.AppendLine('')
[void]$sb.AppendLine('Microsoft Visual Studio Solution File, Format Version 12.00')
[void]$sb.AppendLine('# Visual Studio 14')
foreach ($p in $projs) {
    [void]$sb.AppendLine("Project(`"$typeGuid`") = `"$($p.Name)`", `"$($p.Rel)`", `"$($p.Guid)`"")
    [void]$sb.AppendLine('EndProject')
}
[void]$sb.AppendLine('Global')
[void]$sb.AppendLine("`tGlobalSection(SolutionConfigurationPlatforms) = preSolution")
foreach ($c in $cfgNames) { [void]$sb.AppendLine("`t`t$c|x64 = $c|x64") }
[void]$sb.AppendLine("`tEndGlobalSection")
[void]$sb.AppendLine("`tGlobalSection(ProjectConfigurationPlatforms) = postSolution")
foreach ($p in $projs) {
    foreach ($c in $cfgNames) {
        [void]$sb.AppendLine("`t`t$($p.Guid).$c|x64.ActiveCfg = $c|x64")
        if (-not ($c.StartsWith('Server') -and $serverSkip -contains $p.Name)) {
            [void]$sb.AppendLine("`t`t$($p.Guid).$c|x64.Build.0 = $c|x64")
        }
    }
}
[void]$sb.AppendLine("`tEndGlobalSection")
[void]$sb.AppendLine("`tGlobalSection(SolutionProperties) = preSolution")
[void]$sb.AppendLine("`t`tHideSolutionNode = FALSE")
[void]$sb.AppendLine("`tEndGlobalSection")
[void]$sb.AppendLine('EndGlobal')
[IO.File]::WriteAllText((Join-Path $gaiaDir 'Gaia.sln'), $sb.ToString(), $utf8)
Write-Host "== Gaia\Gaia.sln : $($projs.Count) projets, $($cfgNames.Count) configs. Sources NON touchees."
