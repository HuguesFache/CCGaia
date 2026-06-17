#ifndef __XRLUICommentPinStore__
#define __XRLUICommentPinStore__

#include "PMString.h"
#include "PMReal.h"
#include <vector>

/** Un pin de commentaire prêt à dessiner. Coordonnées déjà normalisées en
	% de la page seule (Format A), origine coin haut-gauche.
*/
struct XRLUICommentPin
{
	int32     idPage;     // page Gaia portant le pin
	PMReal    xPct;       // % largeur page (0–100)
	PMReal    yPct;       // % hauteur page (0–100)
	PMString  header;     // ex. "Hugues - 16/06 à 10:22"
	PMString  text;       // texte du commentaire
};

/** Canal de données simple entre l'observer de la palette Commentaires
	(écrivain) et l'adornment qui dessine les pins (lecteur). Les deux vivent
	dans le plugin XRailUI et tournent sur le main thread, donc un singleton
	process suffit — inutile de passer par des données de session.

	L'observer remplit les pins + le flag d'affichage ; l'adornment lit. Le
	flag fShow reflète la case "tout afficher" de la palette : si false,
	l'adornment ne dessine rien.
*/
class XRLUICommentPinStore
{
public:
	static XRLUICommentPinStore& Instance();

	bool GetShow() const   { return fShow; }
	void SetShow(bool s)   { fShow = s; }

	void SetPins(const std::vector<XRLUICommentPin>& pins) { fPins = pins; }
	const std::vector<XRLUICommentPin>& GetPins() const    { return fPins; }
	void Clear()           { fPins.clear(); }

private:
	XRLUICommentPinStore() : fShow(false) {}

	bool fShow;
	std::vector<XRLUICommentPin> fPins;
};

#endif // __XRLUICommentPinStore__
