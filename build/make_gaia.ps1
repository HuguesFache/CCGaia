# =====================================================================
#  !!! BOOTSTRAP UNIQUEMENT - NE PLUS RELANCER !!!
#  Ce script a servi UNE FOIS a creer CCGaia depuis CC2026 (copie des
#  sources + transformation des projets + patches sources). CCGaia est
#  desormais la SOURCE UNIQUE ; CC2026/CC2024 sont archives.
#  Le relancer ECRASERAIT les sources de CCGaia depuis CC2026 (perte des
#  edits). Pour la maintenance courante (ajout de version, regen .sln),
#  utiliser sync_versions.ps1 (qui ne touche JAMAIS aux sources).
#  Conserve ici uniquement comme documentation de la transformation.
# =====================================================================
#  make_gaia.ps1 - Genere/maj l'arbre unifie CCGaia pour un plugin.
#  Tout est calque sur Pagemakeup : on garde UNE config de reference
#  (InDesign|x64 si presente, sinon Release|x64), rendue inconditionnelle
#  -> appliquee aux 4 configs Client-2024/2026 et Server-2024/2026 (x64).
#  Debug et InCopy sont jetes. Reference = CC2026 (lecture seule).
#  Les projets traites = membres reels du .sln d'origine du plugin.
#  Usage : make_gaia.ps1 [NomPlugin]   (defaut: XRailClient)
# =====================================================================
$ErrorActionPreference = 'Stop'

$srcRoot = 'C:\Users\Kukug\Documents\InDesign\Sources\CC2026'
$dstRoot = 'C:\Users\Kukug\Documents\InDesign\Sources\CCGaia'
$plugin  = if ($args.Count -gt 0) { $args[0] } else { 'XRailClient' }

$src = Join-Path $srcRoot $plugin
$dst = Join-Path $dstRoot $plugin
$buildDir = Join-Path $dstRoot 'build'

$SDKLIT = 'C:\SDKs\adobe_indesign_CC2026'
$OUTLIT = 'C:\Program Files\Adobe\Adobe InDesign 2026\Plug-Ins\Trias\'

$reSL = [Text.RegularExpressions.RegexOptions]::Singleline
$reIC = [Text.RegularExpressions.RegexOptions]::IgnoreCase
$latin1 = [Text.Encoding]::GetEncoding(28591)
$utf8   = New-Object Text.UTF8Encoding $false

# ======================================================================
#  VERSIONS GEREES : pour ajouter/retirer une version d'InDesign, editer
#  CETTE SEULE LIGNE (la SDK C:\SDKs\adobe_indesign_ccXXXX doit exister),
#  puis relancer le script pour chaque plugin. Tout le reste en decoule.
$versions = @('2021','2022','2023','2024','2025','2026')
# ======================================================================
$cfgNames = foreach ($v in $versions) { "Client-$v"; "Server-$v" }   # configs solution

$pcLines = ($cfgNames | ForEach-Object {
    "    <ProjectConfiguration Include=`"$_|x64`"><Configuration>$_</Configuration><Platform>x64</Platform></ProjectConfiguration>"
}) -join "`r`n"
$projConfigs = "<ItemGroup Label=`"ProjectConfigurations`">`r`n$pcLines`r`n  </ItemGroup>"

function Cond([string]$cfg) { ' Condition="''$(Configuration)|$(Platform)''==''' + $cfg + '|x64''"' }

function Parse-Sln([string]$slnPath) {
    $t = [IO.File]::ReadAllText($slnPath, $latin1)
    [regex]::Matches($t, 'Project\("\{[^}]+\}"\)\s*=\s*"([^"]+)",\s*"([^"]+\.vcxproj)",\s*"(\{[^}]+\})"') |
        ForEach-Object {
            [pscustomobject]@{ Name=$_.Groups[1].Value; Rel=$_.Groups[2].Value; Guid=$_.Groups[3].Value }
        }
}

function Convert-Rsp([string]$rspPath, [string]$flag) {
    if (-not $rspPath -or -not (Test-Path $rspPath)) { return @() }
    Get-Content $rspPath | Where-Object { $_ -match "$flag\s+`"(.+)`"" } | ForEach-Object {
        $p = $_ -replace "^\s*$flag\s+`"", '' -replace "`"\s*$", ''
        $p -replace '(?i)C:\\SDKs\\adobe_indesign_CC2026', '$(ID_SDK_DIR)'
    }
}

# Entrees RELATIVES (cross-projet) d'un .rsp : tout ce qui n'est pas un chemin absolu C:\
function Rsp-Relatives([string]$rspPath, [string]$flag) {
    if (-not $rspPath -or -not (Test-Path $rspPath -PathType Leaf)) { return @() }
    Get-Content $rspPath | Where-Object { $_ -match "$flag\s+`"([^`"]+)`"" } | ForEach-Object {
        $p = $_ -replace "^\s*$flag\s+`"", '' -replace "`"\s*$", ''
        if ($p -notmatch '^[A-Za-z]:\\') { $p }
    }
}

function Process-Vcxproj([string]$vcxPath) {
    $pdir = Split-Path $vcxPath
    $v = [IO.File]::ReadAllText($vcxPath, $latin1)

    # noms des .rsp REELLEMENT references par ce vcxproj (avant suppression des refs)
    $cppRspName   = ([regex]::Match($v, '(?:@|%40)"?([^\s"<]*?CPP\.rsp)',   $reIC)).Groups[1].Value
    $odfrcRspName = ([regex]::Match($v, '(?:@|%40)"?([^\s"<]*?ODFRC\.rsp)', $reIC)).Groups[1].Value

    $present = [regex]::Matches($v, 'ProjectConfiguration Include="([^"|]+)\|x64"') |
               ForEach-Object { $_.Groups[1].Value } | Select-Object -Unique
    if     ($present -contains 'InDesign') { $keep = 'InDesign' }
    elseif ($present -contains 'Release')  { $keep = 'Release' }
    else   { throw "Pas de config InDesign/Release dans $vcxPath" }
    $drop = $present | Where-Object { $_ -ne $keep }

    $v = [regex]::Replace($v, '<ItemGroup Label="ProjectConfigurations">.*?</ItemGroup>', { $projConfigs }, $reSL)

    foreach ($d in $drop) {
        $esc = [regex]::Escape((Cond $d))
        foreach ($tag in 'PropertyGroup','ImportGroup','ItemDefinitionGroup') {
            $v = [regex]::Replace($v, "<$tag$esc[^>]*>.*?</$tag>", '', $reSL)
        }
        $v = [regex]::Replace($v, "<(\w+)$esc[^>]*>.*?</\1>", '', $reSL)
        $v = [regex]::Replace($v, "<\w+$esc[^>]*/>", '', $reSL)
    }

    $v = $v.Replace((Cond $keep), '')
    $v = [regex]::Replace($v, '<PropertyGroup>\s*<_ProjectFileVersion>.*?</PropertyGroup>', '', $reSL)

    $v = $v.Replace($OUTLIT, '$(OutDir)')
    $v = [regex]::Replace($v, '(?i)C:\\tmp\\[^<"]*?CC2026\\64\\', { '$(IntDir)' })
    $v = $v.Replace($SDKLIT, '$(ID_SDK_DIR)')

    $v = [regex]::Replace($v, '\s*(?:@|%40)"?[^\s"<]*?CPP\.rsp"?', '', $reIC)
    $v = [regex]::Replace($v, '(?:@|%40)"?[^\s"<]*?ODFRC\.rsp"?', { '$(ODFRC_INC)' }, $reIC)
    # SERVER doit aussi atteindre odfrc (les .fr font #if SERVER -> kInDesignServerProduct/kModelPlugIn). Le C++ l'a via ClCompile, pas odfrc.
    $v = $v.Replace('odfrc ', 'odfrc -d SERVER=$(SERVER_VALUE) ')

    $v = [regex]::Replace($v, '<AdditionalIncludeDirectories>(.*?)</AdditionalIncludeDirectories>', {
        param($m)
        if ($m.Groups[1].Value -match '%\(AdditionalIncludeDirectories\)') { return $m.Value }
        '<AdditionalIncludeDirectories>' + $m.Groups[1].Value + ';%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>'
    }, $reSL)

    [IO.File]::WriteAllText($vcxPath, $v, $latin1)

    # props importe par le projet (1er <Import> .props non-macro)
    $mp = [regex]::Match($v, '<Import Project="([^"$][^"]*\.props)" />')
    $propName = if ($mp.Success) { $mp.Groups[1].Value } else { ((Split-Path $vcxPath -LeafBase) + 'ReleaseX64.props') }
    # includes croisés (relatifs) propres au projet : cl via AdditionalIncludeDirectories, odfrc via ODFRC_INC
    $cppRel = if ($cppRspName)   { Rsp-Relatives (Join-Path $pdir $cppRspName)   '/I' } else { @() }
    $odfRel = if ($odfrcRspName) { Rsp-Relatives (Join-Path $pdir $odfrcRspName) '-i' } else { @() }
    $aiid   = (@('source') + $cppRel) -join ';'                       # 'source' = repertoire propre du projet
    $odfStr = (($odfRel | ForEach-Object { '-i "' + $_ + '"' }) -join ' ')
    $pp = @"
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003" ToolsVersion="4.0">
  <ImportGroup Label="PropertySheets">
    <Import Project="..\..\build\Gaia.Common.props" />
  </ImportGroup>
  <PropertyGroup>
    <ODFRC_INC>$odfStr `$(ID_ODFRC_SDK)</ODFRC_INC>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <AdditionalIncludeDirectories>$aiid;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
  </ItemDefinitionGroup>
</Project>
"@
    [IO.File]::WriteAllText((Join-Path $pdir $propName), $pp, $utf8)
    # menage : rsp inlines + props orphelins (Debug, projet mort...)
    Get-ChildItem $pdir -Filter *.rsp | Remove-Item -Force
    Get-ChildItem $pdir -Filter *.props | Where-Object { $_.Name -ne $propName } | Remove-Item -Force
}

# --- 1. Copie de l'arbre source (exclut le bruit de build) -----------
Write-Host "== Copie $src -> $dst"
robocopy $src $dst /E /XD .vs release_cocoa64 build /XF *.user .DS_Store /NFL /NDL /NJH /NJS /NP | Out-Null

# --- 2. Common props -------------------------------------------------
#  Liste SDK canonique = union des includes ABSOLUS (-> $(ID_SDK_DIR)) de
#  tous les .rsp. Les .rsp a chemins relatifs (XPage) sont ainsi ignores.
New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
$plugins = 'Pagemakeup','XRail','XPage','XRailClient'
$cppCanon = $plugins | ForEach-Object {
        Get-ChildItem -Recurse (Join-Path $srcRoot $_) -Filter *CPP.rsp -ErrorAction SilentlyContinue |
        ForEach-Object { Convert-Rsp $_.FullName '/I' }
    } | Where-Object { $_ -like '$(ID_SDK_DIR)*' } | Select-Object -Unique
$odfrcCanon = $plugins | ForEach-Object {
        Get-ChildItem -Recurse (Join-Path $srcRoot $_) -Filter *ODFRC.rsp -ErrorAction SilentlyContinue |
        ForEach-Object { Convert-Rsp $_.FullName '-i' }
    } | Where-Object { $_ -like '$(ID_SDK_DIR)*' } | Select-Object -Unique
$cppCanonStr   = $cppCanon -join ';'
$odfrcCanonStr = ($odfrcCanon | ForEach-Object { '-i "' + $_ + '"' }) -join ' '

$common = @'
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003" ToolsVersion="4.0">
  <PropertyGroup Label="GaiaAxes">
    <!-- Axe version : les 4 derniers caracteres de la Configuration = l'annee -->
    <ID_VER>$(Configuration.Substring($([MSBuild]::Subtract($(Configuration.Length), 4))))</ID_VER>
    <ID_SDK_DIR>C:\SDKs\adobe_indesign_cc$(ID_VER)</ID_SDK_DIR>
    <ID_INT_ROOT>C:\tmp\Gaia\$(Configuration)\</ID_INT_ROOT>
    <!-- Axe Client/Server -->
    <SERVER_VALUE Condition="$(Configuration.StartsWith('Client'))">0</SERVER_VALUE>
    <SERVER_VALUE Condition="$(Configuration.StartsWith('Server'))">1</SERVER_VALUE>
    <!-- Sortie : Trias (Client) ou TriasServeur (Server) -->
    <ID_TRIAS_DIR Condition="$(Configuration.StartsWith('Client'))">Trias</ID_TRIAS_DIR>
    <ID_TRIAS_DIR Condition="$(Configuration.StartsWith('Server'))">TriasServeur</ID_TRIAS_DIR>
    <ID_OUTPUT_DIR>C:\Program Files\Adobe\Adobe InDesign $(ID_VER)\Plug-Ins\$(ID_TRIAS_DIR)\</ID_OUTPUT_DIR>
    <!-- Includes SDK canoniques (cl et odfrc) -->
    <ID_SDK_INC>@@CPP_INC@@</ID_SDK_INC>
    <ID_ODFRC_SDK>@@ODFRC_INC@@</ID_ODFRC_SDK>
  </PropertyGroup>
  <ImportGroup Label="SDK">
    <Import Project="$(ID_SDK_DIR)\build\win\prj\ReleaseX64.props" />
  </ImportGroup>
  <PropertyGroup Label="GaiaOutput">
    <OutDir>$(ID_OUTPUT_DIR)</OutDir>
    <IntDir>$(ID_INT_ROOT)$(ProjectName)\</IntDir>
    <TargetName>$(ProjectName)</TargetName>
    <TargetExt>.pln</TargetExt>
    <ExecutablePath>$(ID_SDK_DIR)\devtools\bin;$(ExecutablePath)</ExecutablePath>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <!-- Le SDK 2024 impose C++14 ; le code (ecrit pour le SDK 2026) exige C++17 -->
      <LanguageStandard>stdcpp17</LanguageStandard>
      <!-- ...mais les libs ASL du SDK 2024 utilisent std::unary/binary_function (gardes par _HAS_AUTO_PTR_ETC dans la STL MSVC), supprimes en C++17 : on les restaure -->
      <!-- _WINSOCKAPI_ VIDE : empeche windows.h de tirer winsock.h v1 (conflit winsock2.h / XRailClient). Vide = identique au #define du SDK/winsock2.h -> pas de C4005. Rend la rustine SDK manuelle inutile. -->
      <PreprocessorDefinitions>SERVER=$(SERVER_VALUE);_HAS_AUTO_PTR_ETC=1;_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS;_WINSOCKAPI_=;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories>$(ID_SDK_INC);%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
  </ItemDefinitionGroup>
</Project>
'@
$common = $common.Replace('@@CPP_INC@@', $cppCanonStr).Replace('@@ODFRC_INC@@', $odfrcCanonStr)
[IO.File]::WriteAllText((Join-Path $buildDir 'Gaia.Common.props'), $common, $utf8)

# --- 3. Transforme les membres reels du .sln du plugin ---------------
$pluginSln = Get-ChildItem $src -Filter *.sln | Select-Object -First 1
$members = @{}
foreach ($m in (Parse-Sln $pluginSln.FullName)) {
    $vcxPath = Join-Path $dst $m.Rel
    if (-not (Test-Path $vcxPath)) { Write-Host "  !! membre manquant : $($m.Rel)"; continue }
    $members[((Get-Item $vcxPath).FullName).ToLower()] = $true
    Write-Host "== Projet $($m.Name)"
    Process-Vcxproj $vcxPath
}
# nettoie les vcxproj morts (non membres de la sln, ex: XPage\Core\UI.vcxproj)
Get-ChildItem -Recurse $dst -Filter *.vcxproj | Where-Object { -not $members[$_.FullName.ToLower()] } | ForEach-Object {
    Write-Host "   (mort) supprime $($_.Name)"
    Remove-Item $_.FullName -Force
    Remove-Item ($_.FullName + '.filters') -Force -ErrorAction SilentlyContinue
    Remove-Item ($_.FullName + '.user') -Force -ErrorAction SilentlyContinue
}

# --- 4. GlobalDefs.h : SERVER pilote par la config (si present) -------
$gd = Join-Path $dst 'Parser\source\GlobalDefs.h'
if (Test-Path $gd) {
    $g = [IO.File]::ReadAllText($gd, $latin1)
    $g = [regex]::Replace($g, '#define\s+SERVER\s+[01]', "#ifndef SERVER`r`n#define SERVER 0`r`n#endif")
    [IO.File]::WriteAllText($gd, $g, $latin1)
    Write-Host "== GlobalDefs.h patche (SERVER)"
}

# --- 4b. K2Vector<TypeMaison> -> std::vector -----------------------------
#  Le SDK 2026 active K2_BE_GONE=1 -> K2Vector<T> = std::vector<T>. Le code
#  est donc ecrit pour la semantique std::vector (ex: .clear() minuscule).
#  Les SDK <2026 ont le vieux K2Vector (exige data_type, methodes non liees
#  pour types custom). Ces conteneurs sont purement internes (jamais passes
#  a la SDK) -> on les bascule en std::vector partout : marche dans TOUTES
#  les versions, zero risque ABI. Si un nouveau type casse, l'ajouter ici.
$k2types = 'EtatImage','EtatArticle','ImageData','UnitEntry','SavedFormeData','XRLUIComment'
Get-ChildItem -Recurse $dst -Include *.h,*.cpp | ForEach-Object {
    $c0 = [IO.File]::ReadAllText($_.FullName, $latin1); $c = $c0
    foreach ($t in $k2types) { $c = [regex]::Replace($c, "K2Vector<\s*((?:\w+::)*$t)\s*>", 'std::vector<$1>') }
    if ($c -ne $c0) {
        if ($c -notmatch '#include\s*<vector>') {   # garantir <vector> (apres le 1er #include)
            $c = ([regex]'#include[^\r\n]*\r?\n').Replace($c, "`$0#include <vector>`r`n", 1)
        }
        [IO.File]::WriteAllText($_.FullName, $c, $latin1)
        Write-Host "== K2->std::vector : $($_.Name)"
    }
}

# --- 5. Gaia.sln : solution maitre, calquee sur CC2026\Gaia\Gaia.sln -
#  (agrege les plugins via ..\ ; vit dans CCGaia\Gaia\ comme en CC2026)
$typeGuid = '{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}'
$cfgs = $cfgNames
$gaiaDir = Join-Path $dstRoot 'Gaia'
New-Item -ItemType Directory -Force -Path $gaiaDir | Out-Null
$all = Parse-Sln (Join-Path $srcRoot 'Gaia\Gaia.sln') |
       Where-Object { Test-Path (Join-Path $gaiaDir $_.Rel) } |   # garde les projets reellement presents
       Sort-Object Name
# nettoie un eventuel ancien Gaia.sln place a la racine par erreur
Remove-Item (Join-Path $dstRoot 'Gaia.sln') -Force -ErrorAction SilentlyContinue

$sb = New-Object Text.StringBuilder
[void]$sb.AppendLine('')
[void]$sb.AppendLine('Microsoft Visual Studio Solution File, Format Version 12.00')
[void]$sb.AppendLine('# Visual Studio 14')
foreach ($p in $all) {
    [void]$sb.AppendLine("Project(`"$typeGuid`") = `"$($p.Name)`", `"$($p.Rel)`", `"$($p.Guid)`"")
    [void]$sb.AppendLine('EndProject')
}
[void]$sb.AppendLine('Global')
[void]$sb.AppendLine("`tGlobalSection(SolutionConfigurationPlatforms) = preSolution")
foreach ($c in $cfgs) { [void]$sb.AppendLine("`t`t$c|x64 = $c|x64") }
[void]$sb.AppendLine("`tEndGlobalSection")
# Projets non buildes en Server (UI + Pagemakeup) : ActiveCfg mais pas Build.0
$serverSkip = @('Pagemakeup','XPageUI','XRailUI')
[void]$sb.AppendLine("`tGlobalSection(ProjectConfigurationPlatforms) = postSolution")
foreach ($p in $all) {
    foreach ($c in $cfgs) {
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
Write-Host "== Gaia\Gaia.sln : $($all.Count) projets"
Write-Host "OK."
