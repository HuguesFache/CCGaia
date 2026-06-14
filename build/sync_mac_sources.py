#!/usr/bin/env python3
# ============================================================================
#  sync_mac_sources.py — aligne la liste de sources d'un .xcodeproj Mac sur le
#  .vcxproj Windows (qui fait foi). Ajoute les source/*.cpp compiles cote Windows
#  mais absents du build Mac (PBXFileReference + PBXBuildFile + Sources phase).
#  Usage: sync_mac_sources.py <projdir> <XcodeName> <VcxprojName>
#     ex: sync_mac_sources.py Pagemakeup/Pagemakeup Pagemakeup PageMakeUp
# ============================================================================
import sys, os, re

CC = '/Users/macmini2024/deve/InDesign/Sources/CCGaia'

def main():
    projdir, xname, vname = sys.argv[1], sys.argv[2], sys.argv[3]
    pdir = os.path.join(CC, projdir)
    vcx = open(os.path.join(pdir, vname+'.vcxproj')).read()
    pbxp = os.path.join(pdir, xname+'.xcodeproj/project.pbxproj')
    s = open(pbxp).read()

    # sources projet cote Windows : ClCompile Include="source\X.cpp"
    win = re.findall(r'ClCompile Include="source\\([^"]+\.cpp)"', vcx)
    # .cpp deja compiles cote Mac (in Sources)
    mac = set(re.findall(r'/\* ([\w.]+\.cpp) in Sources \*/', s))
    # presents sur disque
    missing = [c for c in win if c not in mac and os.path.exists(os.path.join(pdir,'source',c))]
    # dedoublonne en gardant l'ordre
    seen=set(); missing=[c for c in missing if not (c in seen or seen.add(c))]
    if not missing:
        print('%s : rien a ajouter' % xname); return

    # IDs uniques (base + compteur hex)
    base = 'C0DE'  # prefixe
    def mkid(n): return ('%s%020X' % (base, n))[:24]
    fileref_lines, buildfile_lines, phase_lines = [], [], []
    for i, cpp in enumerate(missing):
        fid = mkid(0x5005000 + i*2); bid = mkid(0x5005001 + i*2)
        fileref_lines.append('\t\t%s /* %s */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = sourcecode.cpp.cpp; name = %s; path = source/%s; sourceTree = "<group>"; };\n' % (fid, cpp, cpp, cpp))
        buildfile_lines.append('\t\t%s /* %s in Sources */ = {isa = PBXBuildFile; fileRef = %s /* %s */; };\n' % (bid, cpp, fid, cpp))
        phase_lines.append('\t\t\t\t%s /* %s in Sources */,\n' % (bid, cpp))

    # injecter : apres "Begin PBXBuildFile section" / "Begin PBXFileReference section" / dans la Sources phase
    s = s.replace('/* Begin PBXBuildFile section */\n', '/* Begin PBXBuildFile section */\n'+''.join(buildfile_lines), 1)
    s = s.replace('/* Begin PBXFileReference section */\n', '/* Begin PBXFileReference section */\n'+''.join(fileref_lines), 1)
    s = re.sub(r'(isa = PBXSourcesBuildPhase;.*?files = \(\n)', lambda m: m.group(1)+''.join(phase_lines), s, count=1, flags=re.S)
    open(pbxp,'w').write(s)
    print('%s : %d source(s) ajoutee(s) -> %s' % (xname, len(missing), ', '.join(missing)))

if __name__ == '__main__':
    main()
