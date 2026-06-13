# CCGaia — build unifié des plugins InDesign (Trias)

**CCGaia est la source unique.** Un seul arbre de sources construit tous les plugins
pour toutes les versions d'InDesign (2021→2026, extensible), en Client et en Serveur.
Les anciens dossiers par version (CC2024, CC2026…) sont **archivés** — ne plus les éditer.

Plugins : **Pagemakeup** (Parser, Doclib, Framelib, Textlib, Pagelib, Pagemakeup, Autopagelink),
**XRail** (XRail, XRailUI), **XPage** (XPage, XPageUI), **XRailClient** — 12 projets.

---

## Windows — opérationnel ✅

Ouvrir **`Gaia\Gaia.sln`**, choisir une configuration et builder :

| Configuration | InDesign | Sortie |
|---|---|---|
| `Client-2024\|x64` | 2024 desktop | `…\Adobe InDesign 2024\Plug-Ins\Trias\` |
| `Server-2024\|x64` | 2024 Server | `…\Adobe InDesign 2024\Plug-Ins\TriasServeur\` |
| `Client-2026`, `Server-2026`, … | idem autres années | idem |

- **Jamais de Debug.** Deux axes : Client/Server et l'année.
- **Server** : `Pagemakeup`, `XPageUI`, `XRailUI` ne sont **pas** buildés (pas d'UI) ; sortie dans `TriasServeur\`.
- Pré-requis : SDK dans `C:\SDKs\adobe_indesign_ccXXXX` + InDesign XXXX installé (pour la sortie).

### Mécanique (tout dans `build\Gaia.Common.props`, importé par chaque projet)
- **Axe version** : `ID_VER` = les 4 derniers caractères du nom de config → `ID_SDK_DIR`, sortie, `IntDir`. Générique : aucune version codée en dur.
- **Axe Client/Server** : préfixe du nom de config → `SERVER=0/1`.
  - `SERVER` est injecté pour le **C++** (`ClCompile`) **ET** pour les **ressources** (odfrc : `-d SERVER=$(SERVER_VALUE)`). ⚠️ Indispensable : `prs.fr` fait `#if SERVER` pour déclarer `kInDesignServerProduct`/`kModelPlugIn` (vs `kInDesignProduct`/`kUIPlugIn`). Sans le define côté odfrc, le plugin se déclare client et **InDesign Server ne l'active pas** (bug TCL serveur déjà rencontré).
- **Norme C++ forcée `stdcpp17`** + `_HAS_AUTO_PTR_ETC=1` + `_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS` : le code récent exige C++17 ; or les SDK ≤2025 imposent C++14 et leurs libs ASL utilisent `std::unary/binary_function` (supprimés en C++17) → on les restaure.
- **`_WINSOCKAPI_=`** (vide) : empêche `windows.h` de tirer winsock.h v1 (conflit avec winsock2.h de XRailClient). Vide = identique au `#define` du SDK → pas de C4005. Remplace l'ancienne rustine SDK manuelle.
- **K2Vector** : les SDK <2026 ont l'ancien `K2Vector` (exige `data_type` par type) ; le 2026 active `K2_BE_GONE=1` → `K2Vector`=`std::vector`. Les conteneurs maison (EtatImage, EtatArticle, ImageData, UnitEntry, SavedFormeData, XRLUIComment) ont été basculés en `std::vector` (internes, jamais passés à la SDK).
- Les includes SDK sont centralisés (`ID_SDK_INC` / `ID_ODFRC_SDK`) ; chaque projet ajoute ses includes croisés relatifs.

### Ajouter une version (ex. 2027)
1. Installer le SDK `C:\SDKs\adobe_indesign_cc2027` (+ InDesign 2027 pour la sortie).
2. Éditer **une ligne** : `$versions` en haut de **`build\sync_versions.ps1`**.
3. Lancer `build\sync_versions.ps1`. → ajoute `Client-2027`/`Server-2027` à chaque `.vcxproj` + au `.sln`. **Aucune source touchée.**

### Version du plugin
`Gaia\TriasVersion.h` — **manuel** : MAJOR=année, MINOR=mois, REV=jour de la compil.

### Scripts (`build\`)
- **`sync_versions.ps1`** — OUTIL VIVANT. Régénère en place les configs des `.vcxproj` + `Gaia.sln`. Ne touche jamais aux sources. À relancer à volonté.
- **`make_gaia.ps1`** — BOOTSTRAP one-shot (a créé CCGaia depuis CC2026). **Ne plus relancer** (écraserait les sources). Conservé pour documentation.

---

## Mac — à faire ⏳

Objectif : mêmes contraintes (source unique, Client/Server, pas de Debug, mêmes plugins serveur),
sortie dans le **Plug-Ins d'InDesign** (au lieu de `SDK_Release`).

Spécificités Mac connues :
- **Deux Macs / deux Xcode** : 2021→2024 sur le Mac dont les SDK sont sous `/Users/macmini2021/AdobeSDKs/`, 2025/2026 sous `/Users/macmini2024/AdobeSDKs/`. Dossiers SDK nommés `Adobe_InDesign_CC20XX_Products_SDK` (majuscules). La version → détermine le Mac → un mapping version→chemin SDK unique marche sur les deux.
- **Plusieurs projets OK** tant que les sources restent uniques (CCGaia). Pas besoin d'un seul pbxproj géant.
- Le paramétrage vit dans les `.xcconfig` (équivalent Mac des `.props`) ; le `pbxproj` (fragile) porte configs, frameworks, archs.
- **Variante Intel** nécessaire pour 2025/2026 (certains Mac Intel ne lancent pas la version « normale » même en universal) → config à `ARCHS = x86_64`.
- **Parser serveur** : désactiver le framework `InDesignModelAndUI`, activer la lib `InDesignModel`. Le client garde `InDesignModelAndUI` (adornments TCL = UI vivante, cf. `PrsTCLAdornmentShape.cpp`). Le dialogue série (`PrsSerialNumberDialogController`, déjà `#if !SERVER`) est à supprimer (inutile).
- ⚠️ « Ça compile ≠ ça se lance » sur Mac : valider le **chargement** dans InDesign, pas seulement la compil.

Approche retenue : PoC sur **Parser** d'abord (il porte le cas serveur framework/lib), validé client+serveur sur un Mac, puis industrialisation. Idéalement piloté **depuis le Mac** (pour pouvoir lancer `xcodebuild` et tester).

---

## Pour Claude (contexte inter-machines)
Les conversations ne se transfèrent pas entre PC et Mac. Pour retrouver le contexte côté Mac :
copier le dossier mémoire de l'utilisateur (`~/.claude/projects/<projet>/memory/`) **et** lire ce `BUILD.md`.
