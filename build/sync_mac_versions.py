#!/usr/bin/env python3
# ============================================================================
#  sync_mac_versions.py — equivalent Mac de sync_versions.ps1.
#  Regenere EN PLACE, pour les 12 projets : configurations Xcode, wrappers
#  xcconfig de version, schemes (par projet + agreges workspace) et
#  build/Gaia.Common.xcconfig — a partir de la liste VERSIONS ci-dessous.
#  Idempotent : relancable a volonte. Ne touche jamais aux sources.
#
#  AJOUTER DES VERSIONS (Mac 2021-2024) : decommenter/ajuster les lignes
#  VERSIONS (chemin SDK absolu propre au Mac qui possede la version) puis
#  relancer. Les chemins absolus par version rendent l'arbre valide sur les 2 Macs
#  (une version ne se compile que sur le Mac ou son SDK est present).
# ============================================================================
import os, re, hashlib

CC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# --- (annee, racine SDK absolue, Intel?) ------------------------------------
VERSIONS = [
    # Mac macmini2021 (Big Sur, Xcode 13) — SDK universels x86_64+arm64, pas de variante Intel separee
    ('2021', '/Users/macmini2021/AdobeSDKs/Adobe_InDesign_CC2021_Products_SDK', False),
    ('2022', '/Users/macmini2021/AdobeSDKs/Adobe_InDesign_CC2022_Products_SDK', False),
    ('2023', '/Users/macmini2021/AdobeSDKs/Adobe_InDesign_CC2023_Products_SDK', False),
    ('2024', '/Users/macmini2021/AdobeSDKs/Adobe_InDesign_CC2024_Products_SDK', False),
    # Mac macmini2024 (Sequoia, Xcode 16) — universels + variante Intel (x86_64) pour InDesign Intel
    ('2025', '/Users/macmini2024/AdobeSDKs/Adobe_InDesign_CC2025_Products_SDK', True),
    ('2026', '/Users/macmini2024/AdobeSDKs/Adobe_InDesign_CC2026_Products_SDK', True),
]

# --- (projdir, Name, has_server) --------------------------------------------
PROJECTS = [
    ('Pagemakeup/Parser','Parser',True), ('Pagemakeup/DocLib','DocLib',True),
    ('Pagemakeup/FrameLib','FrameLib',True), ('Pagemakeup/PageLib','PageLib',True),
    ('Pagemakeup/TextLib','TextLib',True), ('Pagemakeup/Autopagelink','Autopagelink',True),
    ('Pagemakeup/Pagemakeup','Pagemakeup',False), ('XPage/Core','XPage',True),
    ('XPage/UI','XPageUI',False), ('XRail/XRail','XRail',True),
    ('XRail/XRailUI','XRailUI',False), ('XRailClient/XRailClient','XRailClient',True),
]
# Includes _shared_build_settings SUPPLEMENTAIRES (au-dela de common.xcconfig) que certains
# commons hardcodaient sur le SDK 2026 -> deplaces dans les wrappers (version-correct).
EXTRA_SDK_INC = {'XPageUI':['plugin.sdk.xcconfig'], 'XRailUI':['plugin.sdk.xcconfig'],
                 'XRail':['plugin.sdk.xcconfig'], 'XRailClient':['plugin.sdk.xcconfig']}
TARGET='F957086215C90B3E00C95573'   # PBXNativeTarget id partage
PLIST='4B05EEB008840E8D00D809E1'    # XCConfigurationList projet
TLIST='F957087015C90B3E00C95573'    # XCConfigurationList target

def oid(*p): return hashlib.md5('|'.join(p).encode()).hexdigest()[:24].upper()
def vof(c): return re.search(r'(20\d\d)', c).group(1)
def is_intel(c): return c.endswith('-Intel')
def label(c):
    role='Server' if c.startswith('Server') else 'Client'
    return 'CC%s %s%s'%(vof(c), role, ' Intel' if is_intel(c) else '')

def configs_for(has_server):
    out=[]
    for ver,_,intel in VERSIONS:
        out.append('Client-%s'%ver)
        if has_server: out.append('Server-%s'%ver)
        if intel: out.append('Client-%s-Intel'%ver)
    return out

# ---------------------------------------------------------------- Gaia.Common
def gaia_common():
    L=['// GENERE par build/sync_mac_versions.py — NE PAS EDITER A LA MAIN.',
       '// Axe CLIENT/SERVER derive du nom de configuration.','','SERVER_VALUE = 0']
    L+=['SERVER_VALUE[config=Server-%s] = 1'%v for v,_,_ in VERSIONS]
    L+=['','ID_MODEL_FRAMEWORK = InDesignModelAndUI']
    L+=['ID_MODEL_FRAMEWORK[config=Server-%s] = InDesignModel'%v for v,_,_ in VERSIONS]
    # -ld_classic : requis par le nouveau linker (Xcode 15+, SDK 2025+), INEXISTANT sur Xcode 13 (2021-2024).
    L+=['','GAIA_LD_CLASSIC =']
    for ver,_,intel in VERSIONS:
        if int(ver) >= 2025:
            cc=['Client-%s'%ver,'Server-%s'%ver]+(['Client-%s-Intel'%ver] if intel else [])
            L+=['GAIA_LD_CLASSIC[config=%s] = -ld_classic'%c for c in cc]
    open(os.path.join(CC,'build','Gaia.Common.xcconfig'),'w').write('\n'.join(L)+'\n')

# ---------------------------------------------------------------- schemes
def bref(container,name):
    return ('<BuildableReference BuildableIdentifier="primary" BlueprintIdentifier="%s" '
            'BuildableName="%s.InDesignPlugin" BlueprintName="Release_Cocoa64" '
            'ReferencedContainer="container:%s"></BuildableReference>')%(TARGET,name,container)

def scheme_xml(projs, cfg, deploy_script):
    entries=''.join('         <BuildActionEntry buildForTesting="YES" buildForRunning="YES" buildForProfiling="YES" buildForArchiving="YES" buildForAnalyzing="YES">\n            %s\n         </BuildActionEntry>\n'%bref(c,n) for c,n in projs)
    post=('         <ExecutionAction ActionType="Xcode.IDEStandardExecutionActionsCore.ExecutionActionType.ShellScriptAction">\n'
          '            <ActionContent title="Deployer vers InDesign" scriptText="&quot;$PROJECT_DIR/../../build/%s&quot;&#10;">\n'
          '               <EnvironmentBuildable>\n                  %s\n               </EnvironmentBuildable>\n'
          '            </ActionContent>\n         </ExecutionAction>\n')%(deploy_script, bref(projs[0][0],projs[0][1]))
    return ('<?xml version="1.0" encoding="UTF-8"?>\n<Scheme LastUpgradeVersion="1620" version="1.7">\n'
      '   <BuildAction parallelizeBuildables="YES" buildImplicitDependencies="YES">\n'
      '      <PostActions>\n%s      </PostActions>\n'
      '      <BuildActionEntries>\n%s      </BuildActionEntries>\n   </BuildAction>\n'
      '   <TestAction buildConfiguration="%s" selectedDebuggerIdentifier="Xcode.DebuggerFoundation.Debugger.LLDB" selectedLauncherIdentifier="Xcode.DebuggerFoundation.Launcher.LLDB" shouldUseLaunchSchemeArgsEnv="YES"></TestAction>\n'
      '   <LaunchAction buildConfiguration="%s" selectedDebuggerIdentifier="Xcode.DebuggerFoundation.Debugger.LLDB" selectedLauncherIdentifier="Xcode.DebuggerFoundation.Launcher.LLDB" launchStyle="0" useCustomWorkingDirectory="NO" ignoresPersistentStateOnLaunch="NO" debugDocumentVersioning="YES" debugServiceExtension="internal" allowLocationSimulation="YES"></LaunchAction>\n'
      '   <ProfileAction buildConfiguration="%s" shouldUseLaunchSchemeArgsEnv="YES" savedToolIdentifier="" useCustomWorkingDirectory="NO" debugDocumentVersioning="YES"></ProfileAction>\n'
      '   <AnalyzeAction buildConfiguration="%s"></AnalyzeAction>\n'
      '   <ArchiveAction buildConfiguration="%s" revealArchiveInOrganizer="YES"></ArchiveAction>\n</Scheme>\n')%(post,entries,cfg,cfg,cfg,cfg,cfg)

# ---------------------------------------------------------------- pbxproj util
def cfg_blocks(s):
    """Tous les XCBuildConfiguration -> liste (texte, name, baseconf-comment)."""
    a=s.index('/* Begin XCBuildConfiguration section */'); b=s.index('/* End XCBuildConfiguration section */')
    sec=s[a:b]; out=[]
    for m in re.finditer(r'\t\t[0-9A-F]{24} /\* [^*]+ \*/ = \{', sec):
        st=m.start(); en=sec.index('\n\t\t};\n',st)+len('\n\t\t};\n'); blk=sec[st:en]
        nm=re.search(r'name = ([^;]+);',blk); base=re.search(r'baseConfigurationReference = \w+ /\* ([^*]+) \*/',blk)
        out.append((blk, nm.group(1) if nm else '', base.group(1).strip() if base else ''))
    return out

def stamp(template, newid, cfg, basefile, baseid, intel):
    b=re.sub(r'\t\t[0-9A-F]{24} /\* [^*]+ \*/ = \{', '\t\t%s /* %s */ = {'%(newid,cfg), template, count=1)
    b=re.sub(r'baseConfigurationReference = \w+ /\* [^*]+ \*/;',
             'baseConfigurationReference = %s /* %s */;'%(baseid,basefile), b, count=1)
    b=re.sub(r'\n\t\t\t\tARCHS = x86_64;','',b)
    b=re.sub(r'name = [^;]+;','name = %s;'%cfg, b, count=1)
    b=b.replace('"-ld_classic",','"$(GAIA_LD_CLASSIC)",')   # -ld_classic version-conditionnel
    if intel: b=b.replace('buildSettings = {\n','buildSettings = {\n\t\t\t\tARCHS = x86_64;\n',1)
    return b

def sync_project(projdir, name, has_server):
    pdir=os.path.join(CC,projdir); pbxp=os.path.join(pdir,name+'.xcodeproj/project.pbxproj')
    s=open(pbxp).read(); cfgs=configs_for(has_server)

    blks=cfg_blocks(s)
    projT=next(b for b,nm,base in blks if nm.startswith('Client-') and not nm.endswith('-Intel') and 'Release_' not in base)
    tgtT =next(b for b,nm,base in blks if nm.startswith('Client-') and not nm.endswith('-Intel') and 'Release_' in base)

    # noms reels des xcconfig communs (lus depuis les filerefs partages)
    projcommon=re.search(r'7FE5845A08AD6DB8007DB654 /\* [^*]+ \*/ = \{[^}]*?path = ([^;]+);', s).group(1).strip()
    relcommon =re.search(r'77D687F4113861B500D56A9C /\* [^*]+ \*/ = \{[^}]*?path = ([^;]+);', s).group(1).strip()

    # nettoyer les includes SDK residuels (ex plugin.sdk.xcconfig hardcode 2026) dans les commons
    for cf in (projcommon, relcommon):
        cp=os.path.join(pdir,cf); ct=open(cp).read()
        ct2=re.sub(r'#include "[^"]*/_shared_build_settings/[^"]+"\n','',ct)
        if ct2!=ct: open(cp,'w').write(ct2)
    extras=EXTRA_SDK_INC.get(name, [])

    # wrappers + filerefs
    wref={}; wfiles=[]
    for ver,root,_ in VERSIONS:
        for kind,inc,suff,common,exs in (('P','common.xcconfig','',projcommon,extras),
                                          ('R','common_release.xcconfig','Release',relcommon,[])):
            wn='%s%s_%s.xcconfig'%(name,suff,ver)
            incs='#include "%s/build/mac/prj/_shared_build_settings/%s"\n'%(root,inc)
            for e in exs: incs+='#include "%s/build/mac/prj/_shared_build_settings/%s"\n'%(root,e)
            open(os.path.join(pdir,wn),'w').write('// GENERE par sync_mac_versions.py\n'+incs+'ID_SDK_ROOT = %s\n#include "%s"\n'%(root,common))
            fid=oid('wref',name,kind,ver); wref[(kind,ver)]=fid; wfiles.append((fid,wn))

    # section XCBuildConfiguration
    sec=''
    for c in cfgs: sec+=stamp(projT, oid('cfgP',name,c), c, '%s_%s.xcconfig'%(name,vof(c)), wref[('P',vof(c))], False)
    for c in cfgs: sec+=stamp(tgtT,  oid('cfgT',name,c), c, '%sRelease_%s.xcconfig'%(name,vof(c)), wref[('R',vof(c))], is_intel(c))
    a=s.index('/* Begin XCBuildConfiguration section */\n')+len('/* Begin XCBuildConfiguration section */\n')
    b=s.index('/* End XCBuildConfiguration section */'); s=s[:a]+sec+s[b:]

    # listes de configs (par id de liste connu)
    def arr(kind): return 'buildConfigurations = (\n'+''.join('\t\t\t\t%s /* %s */,\n'%(oid(kind,name,c),c) for c in cfgs)+'\t\t\t);'
    s=re.sub(r'(%s /\* [^\n]*\n\t\t\tisa = XCConfigurationList;\n\t\t\t)buildConfigurations = \([^;]*?\t\t\t\);'%PLIST,
             lambda m:m.group(1)+arr('cfgP'), s, flags=re.S)
    s=re.sub(r'(%s /\* [^\n]*\n\t\t\tisa = XCConfigurationList;\n\t\t\t)buildConfigurations = \([^;]*?\t\t\t\);'%TLIST,
             lambda m:m.group(1)+arr('cfgT'), s, flags=re.S)
    s=re.sub(r'defaultConfigurationName = [^;]+;','defaultConfigurationName = %s;'%cfgs[0], s)

    # filerefs wrappers : purge + reinjecte
    s=re.sub(r'\t\t[0-9A-F]{24} /\* %s(Release)?_20\d\d\.xcconfig \*/ = \{isa = PBXFileReference[^\n]*\n'%re.escape(name),'',s)
    s=re.sub(r'\t\t\t\t[0-9A-F]{24} /\* %s(Release)?_20\d\d\.xcconfig \*/,\n'%re.escape(name),'',s)
    refs=''.join('\t\t%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = text.xcconfig; path = %s; sourceTree = "<group>"; };\n'%(i,n,n) for i,n in wfiles)
    s=s.replace('/* Begin PBXFileReference section */\n','/* Begin PBXFileReference section */\n'+refs,1)
    open(pbxp,'w').write(s)

    # schemes par projet
    sdir=os.path.join(pdir,name+'.xcodeproj/xcshareddata/xcschemes'); os.makedirs(sdir,exist_ok=True)
    for f in os.listdir(sdir):
        if f.endswith('.xcscheme'): os.remove(os.path.join(sdir,f))
    for c in cfgs:
        open(os.path.join(sdir,'%s %s.xcscheme'%(name,label(c))),'w').write(
            scheme_xml([(name+'.xcodeproj',name)], c, 'deploy_to_indesign.sh'))
    return len(cfgs)

WS=[('../Pagemakeup/Parser/Parser.xcodeproj','Parser',False),('../Pagemakeup/DocLib/DocLib.xcodeproj','DocLib',False),
 ('../Pagemakeup/FrameLib/FrameLib.xcodeproj','FrameLib',False),('../Pagemakeup/PageLib/PageLib.xcodeproj','PageLib',False),
 ('../Pagemakeup/TextLib/TextLib.xcodeproj','TextLib',False),('../Pagemakeup/Autopagelink/Autopagelink.xcodeproj','Autopagelink',False),
 ('../Pagemakeup/Pagemakeup/Pagemakeup.xcodeproj','Pagemakeup',True),('../XPage/Core/XPage.xcodeproj','XPage',False),
 ('../XPage/UI/XPageUI.xcodeproj','XPageUI',True),('../XRail/XRail/XRail.xcodeproj','XRail',False),
 ('../XRail/XRailUI/XRailUI.xcodeproj','XRailUI',True),('../XRailClient/XRailClient/XRailClient.xcodeproj','XRailClient',False)]

def workspace_aggregate():
    sdir=os.path.join(CC,'Gaia/Gaia.xcworkspace/xcshareddata/xcschemes'); os.makedirs(sdir,exist_ok=True)
    for f in os.listdir(sdir):
        if f.startswith('ALL ') and f.endswith('.xcscheme'): os.remove(os.path.join(sdir,f))
    for ver,_,intel in VERSIONS:
        vs=[('Client-%s'%ver,False),('Server-%s'%ver,True)]
        if intel: vs.append(('Client-%s-Intel'%ver,False))
        for cfg,server in vs:
            projs=[(c,n) for c,n,ui in WS if not (server and ui)]
            open(os.path.join(sdir,'ALL — %s.xcscheme'%label(cfg)),'w').write(
                scheme_xml(projs, cfg, 'deploy_all_to_indesign.sh'))

def main():
    gaia_common()
    for projdir,name,hs in PROJECTS: print('%-14s %d configs'%(name, sync_project(projdir,name,hs)))
    workspace_aggregate()
    print('OK — versions:', ','.join(v for v,_,_ in VERSIONS))

if __name__=='__main__': main()
