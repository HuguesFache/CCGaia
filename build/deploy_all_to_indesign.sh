#!/bin/sh
# ============================================================================
#  deploy_all_to_indesign.sh — post-action des SCHEMES AGREGES.
#  Deploie TOUS les plugins construits pour la variante choisie vers le dossier
#  Plug-Ins d'InDesign (toujours l'app InDesign normale). Sous-dossier deduit de
#  $CONFIGURATION (fourni par Xcode via l'EnvironmentBuildable). Jamais d'echec (exit 0).
#     Client-*       -> /Applications/Adobe InDesign <ver>/Plug-Ins/Trias
#     Server-*       -> /Applications/Adobe InDesign <ver>/Plug-Ins/TriasServeur (exclut UI)
#     Client-*-Intel -> /Applications/Adobe InDesign <ver>/Plug-Ins/TriasIntel
# ============================================================================
set -u
case "${CONFIGURATION:-}" in *2025*) V=2025 ;; *2026*) V=2026 ;; *) echo "deploy-all: version inconnue '${CONFIGURATION:-}'"; exit 0 ;; esac
APP="/Applications/Adobe InDesign $V"   # toujours l'app InDesign normale
case "${CONFIGURATION:-}" in
  Server-*)  SUB="TriasServeur" ;;
  *-Intel)   SUB="TriasIntel" ;;
  *)         SUB="Trias" ;;
esac
# PROJECT_DIR = <CCGaia>/<categorie>/<projet> -> racine CCGaia = ../..
ROOT="${PROJECT_DIR:-}/../.."
DEST="$APP/Plug-Ins/$SUB"
SKIP_SERVER="Pagemakeup.InDesignPlugin XPageUI.InDesignPlugin XRailUI.InDesignPlugin"

if [ ! -d "$APP" ]; then echo "deploy-all: '$APP' introuvable - deploiement ignore"; exit 0; fi
mkdir -p "$DEST" || { echo "deploy-all: impossible de creer $DEST"; exit 0; }

n=0
for p in "$ROOT"/*/release_cocoa64/SDK/*.InDesignPlugin; do
  [ -d "$p" ] || continue
  bn=$(basename "$p")
  if [ "$SUB" = "TriasServeur" ]; then
    case " $SKIP_SERVER " in *" $bn "*) continue ;; esac
  fi
  rm -rf "$DEST/$bn"
  if cp -R "$p" "$DEST/"; then n=$((n+1)); fi
done
echo "deploy-all: $n plugin(s) deploye(s) -> $DEST"
exit 0
