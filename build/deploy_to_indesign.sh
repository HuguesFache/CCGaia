#!/bin/sh
# ============================================================================
#  deploy_to_indesign.sh — copie le plugin construit dans le dossier Plug-Ins
#  d'InDesign (toujours l'app InDesign normale), sous-dossier deduit de $CONFIGURATION :
#     Client-*       -> /Applications/Adobe InDesign <ver>/Plug-Ins/Trias
#     Server-*       -> /Applications/Adobe InDesign <ver>/Plug-Ins/TriasServeur
#     Client-*-Intel -> /Applications/Adobe InDesign <ver>/Plug-Ins/TriasIntel
#  Lance en POST-ACTION de build (apres signature Xcode). Variables fournies
#  par Xcode quand l'EnvironmentBuildable du scheme pointe la cible.
#  Ne fait jamais echouer le build (exit 0) : si la cible n'est pas installee,
#  on laisse simplement le plugin dans le dossier de build.
# ============================================================================
set -u

IDVER=$(printf '%s' "${CONFIGURATION:-}" | grep -oE '20[0-9][0-9]' | head -1)
[ -n "$IDVER" ] || { echo "deploy: version inconnue pour '${CONFIGURATION:-}' - ignore"; exit 0; }

APP="/Applications/Adobe InDesign $IDVER"   # toujours l'app InDesign normale
case "${CONFIGURATION:-}" in
  Server-*)  SUB="TriasServeur" ;;
  *-Intel)   SUB="TriasIntel" ;;
  *)         SUB="Trias" ;;
esac

PLUGIN="${BUILT_PRODUCTS_DIR:-}/${FULL_PRODUCT_NAME:-}"
DEST="$APP/Plug-Ins/$SUB"

if [ ! -d "$APP" ]; then
  echo "deploy: '$APP' introuvable - deploiement ignore (plugin dans ${BUILT_PRODUCTS_DIR:-?})"
  exit 0
fi
if [ ! -d "$PLUGIN" ]; then
  echo "deploy: produit introuvable: $PLUGIN - ignore"
  exit 0
fi

mkdir -p "$DEST" || { echo "deploy: impossible de creer $DEST"; exit 0; }
rm -rf "$DEST/${FULL_PRODUCT_NAME}"
if cp -R "$PLUGIN" "$DEST/"; then
  echo "deploy: OK -> $DEST/${FULL_PRODUCT_NAME}"
else
  echo "deploy: echec de copie vers $DEST"
fi
exit 0
