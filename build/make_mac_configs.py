#!/usr/bin/env python3
# ============================================================================
#  make_mac_configs.py — generateur Mac (equivalent de sync_versions.ps1).
#  Applique a UN projet le schema valide sur Parser :
#   - xcconfig projet/release => "communs" (sans version) + 4 wrappers de version
#   - pbxproj : framework+lib via OTHER_LDFLAGS (version-correct), chemins SDK
#     en $(ID_SDK_ROOT), odfrc -d SERVER, N configs (6 ou 4) a baseConfig par version
#   - N schemes partages avec post-action de deploiement
#  S'appuie sur les IDs d'objets partages par tous les projets (template Dolly).
#
#  Usage: make_mac_configs.py <projdir> <Name> <Model|ModelAndUI> <server|noserver> [extra_ldflags...]
#     ex: make_mac_configs.py Pagemakeup/DocLib DocLib Model server
#         make_mac_configs.py XPage/UI XPageUI ModelAndUI noserver
#         make_mac_configs.py XRailClient/XRailClient XRailClient Model server -lcurl
# ============================================================================
import sys, os, re

ROOTSDK = '/Users/macmini2024/AdobeSDKs/Adobe_InDesign_CC2026_Products_SDK'
CC = '/Users/macmini2024/deve/InDesign/Sources/CCGaia'

# IDs partages (template Dolly) — identiques dans tous les .pbxproj
ID = dict(projList='4B05EEB008840E8D00D809E1', tgtList='F957087015C90B3E00C95573',
          projDef='4B05EEB108840E8D00D809E1', tgtDef='F957087115C90B3E00C95573',
          fwPhase='F957086B15C90B3E00C95573', target='F957086215C90B3E00C95573',
          projXcref='7FE5845A08AD6DB8007DB654', relXcref='77D687F4113861B500D56A9C')
# filerefs wrappers (uniques dans chaque pbxproj)
WP = dict(P2026='5E54076200000000C0DE2026', P2025='5E54076200000000C0DE2025',
          R2026='5E54076300000000C0DE2026', R2025='5E54076300000000C0DE2025')
# (projId, tgtId) par config
CID = {'Client-2026':('4B05EEB108840E8D00D809E1','F957087115C90B3E00C95573'),
       'Server-2026':('5E5407C000000000C0DE2026','5E5407D000000000C0DE2026'),
       'Client-2025':('5E5407C000000000C0DE2025','5E5407D000000000C0DE2025'),
       'Server-2025':('5E5407C100000000C0DE2025','5E5407D100000000C0DE2025'),
       'Client-2026-Intel':('5E5407C200000000C0DE2026','5E5407D200000000C0DE2026'),
       'Client-2025-Intel':('5E5407C300000000C0DE2025','5E5407D300000000C0DE2025')}
ALL6 = ['Client-2026','Server-2026','Client-2025','Server-2025','Client-2026-Intel','Client-2025-Intel']
CLIENT4 = ['Client-2026','Client-2025','Client-2026-Intel','Client-2025-Intel']

def vof(cfg):  # version d'une config
    return '2025' if '2025' in cfg else '2026'

def main():
    projdir, name, fw, srv = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
    extra = ' '.join(sys.argv[5:])
    configs = ALL6 if srv == 'server' else CLIENT4
    pdir = os.path.join(CC, projdir)
    pbxp = os.path.join(pdir, '%s.xcodeproj/project.pbxproj' % name)
    s = open(pbxp).read()

    # noms des xcconfig communs (lus depuis les filerefs partages)
    mproj = re.search(r'%s /\* ([^ ]+\.xcconfig) \*/ = \{[^}]*path = ([^;]+);' % ID['projXcref'], s)
    mrel  = re.search(r'%s /\* ([^ ]+\.xcconfig) \*/ = \{[^}]*path = ([^;]+);' % ID['relXcref'], s)
    projcfg, relcfg = mproj.group(2).strip(), mrel.group(2).strip()  # ex DocLibPlugin.xcconfig

    # ---------- 1) xcconfig communs ----------
    pf = os.path.join(pdir, projcfg); t = open(pf).read()
    t = re.sub(r'#include "[^"]*_shared_build_settings/common\.xcconfig"\n', '', t)
    t = re.sub(r'ID_SDK_ROOT = /Developer/[^\n]*\n', '', t)
    if 'SERVER=$(SERVER_VALUE)' not in t:
        t = re.sub(r'(GCC_PREPROCESSOR_DEFINITIONS = [^\n]*)', r'\1 SERVER=$(SERVER_VALUE)', t)
    if 'PRODUCT_BUNDLE_IDENTIFIER' not in t:
        t = t.replace('WRAPPER_EXTENSION = InDesignPlugin\n',
                      'WRAPPER_EXTENSION = InDesignPlugin\nPRODUCT_BUNDLE_IDENTIFIER = com.adobe.InDesign.$(PRODUCT_NAME)\n', 1)
    if '../../build/Gaia.Common.xcconfig' not in t:
        t = '#include "../../build/Gaia.Common.xcconfig"\n' + t
    open(pf, 'w').write(t)

    rf = os.path.join(pdir, relcfg); t = open(rf).read()
    t = re.sub(r'#include "[^"]*_shared_build_settings/common_release\.xcconfig"\n', '', t)
    open(rf, 'w').write(t)

    # ---------- 2) wrappers de version ----------
    pstem = projcfg[:-9]   # 'DocLibPlugin'  (sans .xcconfig)
    rstem = relcfg[:-9]    # 'DocLibPlugin_release'
    def wrap(incl, root, common):
        return ('#include "%s/build/mac/prj/_shared_build_settings/%s"\n'
                'ID_SDK_ROOT = %s\n#include "%s"\n') % (root, incl, root, common)
    R25 = ROOTSDK.replace('CC2026','CC2025')
    open(os.path.join(pdir,'%s_2026.xcconfig'%name),'w').write(wrap('common.xcconfig',ROOTSDK,projcfg))
    open(os.path.join(pdir,'%s_2025.xcconfig'%name),'w').write(wrap('common.xcconfig',R25,projcfg))
    open(os.path.join(pdir,'%sRelease_2026.xcconfig'%name),'w').write(wrap('common_release.xcconfig',ROOTSDK,relcfg))
    open(os.path.join(pdir,'%sRelease_2025.xcconfig'%name),'w').write(wrap('common_release.xcconfig',R25,relcfg))
    BN = {WP['P2026']:'%s_2026.xcconfig'%name, WP['P2025']:'%s_2025.xcconfig'%name,
          WP['R2026']:'%sRelease_2026.xcconfig'%name, WP['R2025']:'%sRelease_2025.xcconfig'%name}

    # ---------- 3) pbxproj ----------
    # 3a) auto-detecter TOUTES les frameworks/libs de la phase, PUIS la vider
    mfw = re.search(r'%s /\* Frameworks \*/ = \{.*?files = \((.*?)\);' % ID['fwPhase'], s, re.S)
    names = re.findall(r'/\* ([^*]+) in Frameworks \*/', mfw.group(1))
    ldparts = []
    for n in names:
        n = n.strip()
        if n.endswith('.framework'): ldparts += ['-framework', n[:-len('.framework')]]
        elif n.startswith('lib') and n.endswith('.a'): ldparts.append('-l'+n[3:-2])
        else: ldparts.append(n)  # cas exotique : on garde tel quel
    assert ldparts, 'aucune framework/lib detectee dans la phase Frameworks'
    s = re.sub(r'(%s /\* Frameworks \*/ = \{.*?files = \()(.*?)(\);)' % ID['fwPhase'],
               lambda m: m.group(1)+'\n\t\t\t'+m.group(3), s, flags=re.S)
    # 3b) OTHER_LDFLAGS : re-lier toutes les frameworks/libs (version-correct via search paths)
    ld = ''.join('\t\t\t\t\t"%s",\n' % p for p in ldparts)
    for e in (extra.split() if extra else []):
        ld += '\t\t\t\t\t"%s",\n' % e
    pat = re.compile(r'(OTHER_LDFLAGS = \(\n(?:\t\t\t\t\t[^\n]*\n)*)(\t\t\t\t\);)')
    assert pat.search(s), 'OTHER_LDFLAGS pattern introuvable'
    s = pat.sub(lambda m: m.group(1)+ld+m.group(2), s, count=1)
    # 3c) genericiser les chemins SDK (entrees de liste) + retirer override ID_SDK_ROOT
    s = re.sub(r'^(\t+)('+re.escape(ROOTSDK)+r'[^,\n]*),$',
               lambda m: '%s"%s",'%(m.group(1), m.group(2).replace(ROOTSDK,'$(ID_SDK_ROOT)')), s, flags=re.M)
    s = s.replace('\t\t\t\tID_SDK_ROOT = %s;\n' % ROOTSDK, '')
    # 3d) odfrc -d SERVER
    s = s.replace('odfrc-cmd -d RELEASE $ODFRC_ADDITIONAL_FLAGS',
                  'odfrc-cmd -d RELEASE -d SERVER=$SERVER_VALUE $ODFRC_ADDITIONAL_FLAGS')
    # 3d') sources communes SDK : version-correctes via $(ID_SDK_ROOT) (sinon on compile
    #      le .cpp 2026 sous les headers 2025 -> mismatch K2Vector etc.)
    s = re.sub(r'path = /Users/macmini2024/AdobeSDKs/adobe_indesign_CC2026_Products_SDK(/source/sdksamples/common/SDK\w+\.cpp); sourceTree = "<absolute>";',
               r'path = "$(ID_SDK_ROOT)\1"; sourceTree = "<absolute>";', s)
    # 3e) filerefs wrappers
    anchor = re.search(r'\t\t%s /\* [^\n]*\n' % ID['relXcref'], s).group(0)
    refs = ''.join('\t\t%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = text.xcconfig; path = %s; sourceTree = "<group>"; };\n'
                   % (i,BN[i],BN[i]) for i in (WP['P2026'],WP['P2025'],WP['R2026'],WP['R2025']))
    s = s.replace(anchor, anchor+refs)

    # 3f) cloner les blocs config Default -> N configs
    def block(text, oid):
        st=text.index('\t\t%s /* '%oid); en=text.index('\n\t\t};\n',st)+len('\n\t\t};\n'); return text[st:en]
    projT, tgtT = block(s, ID['projDef']), block(s, ID['tgtDef'])
    def mkproj(cfg):
        pid = CID[cfg][0]; base = WP['P'+vof(cfg)]
        b = projT.replace(ID['projDef'], pid)
        b = re.sub(r'baseConfigurationReference = %s /\* [^*]*\*/;'%ID['projXcref'],
                   'baseConfigurationReference = %s /* %s */;'%(base,BN[base]), b)
        return b.replace('name = Default;','name = %s;'%cfg)
    def mktgt(cfg):
        tid = CID[cfg][1]; base = WP['R'+vof(cfg)]
        b = tgtT.replace(ID['tgtDef'], tid)
        b = re.sub(r'baseConfigurationReference = %s /\* [^*]*\*/;'%ID['relXcref'],
                   'baseConfigurationReference = %s /* %s */;'%(base,BN[base]), b)
        b = b.replace('name = Default;','name = %s;'%cfg)
        if 'Intel' in cfg: b = b.replace('buildSettings = {\n','buildSettings = {\n\t\t\t\tARCHS = x86_64;\n',1)
        return b
    newsec = ''.join(mkproj(c) for c in configs)+''.join(mktgt(c) for c in configs)
    a = s.index('/* Begin XCBuildConfiguration section */\n')+len('/* Begin XCBuildConfiguration section */\n')
    b = s.index('/* End XCBuildConfiguration section */')
    s = s[:a]+newsec+s[b:]
    # 3g) listes de configs
    def arr(items): return '\t\t\tbuildConfigurations = (\n'+''.join('\t\t\t\t%s /* %s */,\n'%(i,c) for c,i in items)+'\t\t\t);'
    s = re.sub(r'\t\t\tbuildConfigurations = \(\n\t\t\t\t%s /\* Default \*/,\n\t\t\t\);'%ID['projDef'],
               arr([(c,CID[c][0]) for c in configs]), s)
    s = re.sub(r'\t\t\tbuildConfigurations = \(\n\t\t\t\t%s /\* Default \*/,\n\t\t\t\);'%ID['tgtDef'],
               arr([(c,CID[c][1]) for c in configs]), s)
    s = s.replace('defaultConfigurationName = Default;','defaultConfigurationName = %s;'%configs[0])
    open(pbxp,'w').write(s)

    # ---------- 4) schemes partages + post-action deploiement ----------
    sdir = os.path.join(pdir, '%s.xcodeproj/xcshareddata/xcschemes'%name)
    os.makedirs(sdir, exist_ok=True)
    # purge anciens schemes parasites du projet
    for f in os.listdir(sdir):
        if f.endswith('.xcscheme') and not f.startswith('%s '%name): os.remove(os.path.join(sdir,f))
    label = {'Client-2026':'CC2026 Client','Server-2026':'CC2026 Server','Client-2025':'CC2025 Client',
             'Server-2025':'CC2025 Server','Client-2026-Intel':'CC2026 Client Intel','Client-2025-Intel':'CC2025 Client Intel'}
    bref = ('<BuildableReference BuildableIdentifier="primary" BlueprintIdentifier="%s" BuildableName="%s.InDesignPlugin" '
            'BlueprintName="Release_Cocoa64" ReferencedContainer="container:%s.xcodeproj"></BuildableReference>') % (ID['target'],name,name)
    for cfg in configs:
        xml = ('<?xml version="1.0" encoding="UTF-8"?>\n<Scheme LastUpgradeVersion="1620" version="1.7">\n'
          '   <BuildAction parallelizeBuildables="YES" buildImplicitDependencies="YES">\n'
          '      <PostActions>\n'
          '         <ExecutionAction ActionType="Xcode.IDEStandardExecutionActionsCore.ExecutionActionType.ShellScriptAction">\n'
          '            <ActionContent title="Deployer vers les Plug-Ins d\'InDesign" scriptText="&quot;$PROJECT_DIR/../../build/deploy_to_indesign.sh&quot;&#10;">\n'
          '               <EnvironmentBuildable>\n                  %s\n               </EnvironmentBuildable>\n'
          '            </ActionContent>\n         </ExecutionAction>\n      </PostActions>\n'
          '      <BuildActionEntries>\n         <BuildActionEntry buildForTesting="YES" buildForRunning="YES" buildForProfiling="YES" buildForArchiving="YES" buildForAnalyzing="YES">\n'
          '            %s\n         </BuildActionEntry>\n      </BuildActionEntries>\n   </BuildAction>\n'
          '   <TestAction buildConfiguration="%s" selectedDebuggerIdentifier="Xcode.DebuggerFoundation.Debugger.LLDB" selectedLauncherIdentifier="Xcode.DebuggerFoundation.Launcher.LLDB" shouldUseLaunchSchemeArgsEnv="YES"></TestAction>\n'
          '   <LaunchAction buildConfiguration="%s" selectedDebuggerIdentifier="Xcode.DebuggerFoundation.Debugger.LLDB" selectedLauncherIdentifier="Xcode.DebuggerFoundation.Launcher.LLDB" launchStyle="0" useCustomWorkingDirectory="NO" ignoresPersistentStateOnLaunch="NO" debugDocumentVersioning="YES" debugServiceExtension="internal" allowLocationSimulation="YES"></LaunchAction>\n'
          '   <ProfileAction buildConfiguration="%s" shouldUseLaunchSchemeArgsEnv="YES" savedToolIdentifier="" useCustomWorkingDirectory="NO" debugDocumentVersioning="YES"></ProfileAction>\n'
          '   <AnalyzeAction buildConfiguration="%s"></AnalyzeAction>\n'
          '   <ArchiveAction buildConfiguration="%s" revealArchiveInOrganizer="YES"></ArchiveAction>\n</Scheme>\n'
          ) % (bref, bref, cfg, cfg, cfg, cfg, cfg)
        open(os.path.join(sdir, '%s %s.xcscheme'%(name, label[cfg])), 'w').write(xml)

    print('OK %s : %d configs, ld=[%s], server=%s' % (name, len(configs), ' '.join(ldparts), srv))

if __name__ == '__main__':
    main()
