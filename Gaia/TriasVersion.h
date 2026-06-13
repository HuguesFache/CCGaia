// =============================================================================
//   TriasVersion.h
//
//   Single source of truth for the Trias plugin family version. Included by
//   every plugin from its ***ID.h (kPMUVersion, kXPGVersion, ...).
//
//   The MAJOR number tracks the InDesign version and is injected by the build
//   (GAIA_IDMAJOR = year - 2005 : 2021->16, 2024->19, 2026->21, 2027->22 ...).
//   So you ONLY bump kTriasVersionMinor / kTriasVersionRev below at each Trias
//   family release; the major follows the InDesign version automatically.
//
//   (Per-plugin override still possible: a plugin that needs its own cadence
//   can keep its k***Version literal in its ***ID.h instead of pulling this.)
// =============================================================================

#ifndef __TriasVersion_h__
#define __TriasVersion_h__

// Fallback when built outside the Gaia solution (no GAIA_IDMAJOR define).
#define GAIA_IDMAJOR 26

// ---- Edit these two at each Trias release -----------------------------------
#define kTriasVersionMinor   06
#define kTriasVersionRev     11
// -----------------------------------------------------------------------------

#define kTriasVersionMajor   GAIA_IDMAJOR

// Compose the dotted string "MAJOR.MINOR.REV" (e.g. "19.3.0") from the numerics.
#define TRIAS__STR2(x) #x
#define TRIAS__STR(x)  TRIAS__STR2(x)
#define kTriasVersionString  TRIAS__STR(kTriasVersionMajor) "." TRIAS__STR(kTriasVersionMinor) "." TRIAS__STR(kTriasVersionRev)

#endif // __TriasVersion_h__
