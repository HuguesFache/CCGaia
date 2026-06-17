#include "VCPlugInHeaders.h"

// Interface includes
#include "IAdornmentShape.h"
#include "IShape.h"
#include "IGraphicsPort.h"
#include "IGeometry.h"
#include "ISpread.h"
#include "IHierarchy.h"
#include "IFontMgr.h"
#include "IPMFont.h"
#include "IFontInstance.h"
#include "ILayoutControlData.h"
#include "ILayoutUIUtils.h"
#include "IPanorama.h"
#include "ISession.h"

// General includes
#include "CPMUnknown.h"
#include "AutoGSave.h"
#include "Utils.h"
#include "PersistUtils.h"     // ::GetDataBase
#include "IDataBase.h"
#include "TransformUtils.h"   // ::TransformInnerPointToPasteboard / PasteboardPointToInner

// Project includes
#include "XRLUIID.h"
#include "IXRailPageSlugData.h"
#include "XRLUICommentPinStore.h"

/** Adornment des pins de commentaires. L'observer crée, quand la case "tout
	afficher" est cochée, un bloc-ancre 1x1 au premier plan sur chaque spread
	Gaia et y attache cet adornment. Comme le bloc est l'item le plus haut de la
	pile, kAfterShape se dessine APRÈS toute la maquette -> les pins passent
	au-dessus.

	Un page-item adornment classique (sur la page) se dessine sous les blocs
	(enfants du spread). Les pistes "au-dessus de tout" (kAfterSpread, spread,
	liste globale) ne déclenchent rien dans ce build : le bloc-ancre est le moyen
	fiable et déterministe d'être au premier plan.

	À kAfterShape, iShape = le bloc : on récupère son spread, on dessine les pins
	de ses pages en convertissant page-interne -> pasteboard -> repère du bloc.
	Données fournies par XRLUICommentPinStore (rempli par l'observer).
*/
class XRLUICommentsPinAdornment : public CPMUnknown<IAdornmentShape>
{
public:
	XRLUICommentsPinAdornment(IPMUnknown* boss) : CPMUnknown<IAdornmentShape>(boss) {}
	virtual ~XRLUICommentsPinAdornment() {}

	// Attaché aux pages Gaia. kAfterSpread / spread / liste globale ne
	// déclenchent rien dans ce build ; kAfterShape est le seul ordre qui
	// dessine. iShape = la page, port en coordonnées internes de la page.
	virtual AdornmentDrawOrder GetDrawOrderBits() { return (AdornmentDrawOrder)kAfterShape; }

	virtual void DrawAdornment(IShape* iShape, AdornmentDrawOrder drawOrder, GraphicsData* gd, int32 flags);

	virtual PMRect GetPaintedAdornmentBounds(IShape* iShape, AdornmentDrawOrder drawOrder,
		const PMRect& itemBounds, const PMMatrix& innertoview);
	virtual PMRect GetPrintedAdornmentBounds(IShape* iShape, AdornmentDrawOrder drawOrder,
		const PMRect& itemBounds, const PMMatrix& innertoview) { return itemBounds; }

	virtual void Inval(IShape*, AdornmentDrawOrder, GraphicsData*, ClassID, int32) {}
	virtual PMReal GetPriority() { return 0.0; }
	virtual void AddToContentInkBounds(IShape*, PMRect*) {}
	virtual bool16 WillDraw(IShape*, AdornmentDrawOrder drawOrder, GraphicsData*, int32 flags)
	{
		return (drawOrder == kAfterShape && !(flags & IShape::kPrinting)) ? kTrue : kFalse;
	}
	virtual bool16 WillPrint() { return kFalse; }
	virtual bool16 HitTest(IShape*, AdornmentDrawOrder, IControlView*, const PMRect&) { return kFalse; }

private:
	void  FillCircle(IGraphicsPort* gPort, const PMReal& cx, const PMReal& cy, const PMReal& r) const;
	void  DrawText(IGraphicsPort* gPort, IPMFont* font, const PMReal& fontSize,
		const PMReal& x, const PMReal& yBaseline, const PMString& text) const;
	void  DrawPin(IGraphicsPort* gPort, IPMFont* font, IFontInstance* fontInst,
		const PMReal& px, const PMReal& py, const PMReal& inv, const XRLUICommentPin& pin) const;
};

CREATE_PMINTERFACE(XRLUICommentsPinAdornment, kXRLUICommentsPinAdornmentImpl)

// Cercle plein approximé par 4 courbes de Bézier (kappa = 0.5522847498).
void XRLUICommentsPinAdornment::FillCircle(IGraphicsPort* gPort, const PMReal& cx, const PMReal& cy, const PMReal& r) const
{
	const PMReal k = r * 0.5522847498;
	gPort->newpath();
	gPort->moveto(cx + r, cy);
	gPort->curveto(cx + r, cy + k, cx + k, cy + r, cx, cy + r);
	gPort->curveto(cx - k, cy + r, cx - r, cy + k, cx - r, cy);
	gPort->curveto(cx - r, cy - k, cx - k, cy - r, cx, cy - r);
	gPort->curveto(cx + k, cy - r, cx + r, cy - k, cx + r, cy);
	gPort->closepath();
	gPort->fill();
}

void XRLUICommentsPinAdornment::DrawText(IGraphicsPort* gPort, IPMFont* font, const PMReal& fontSize,
	const PMReal& x, const PMReal& yBaseline, const PMString& text) const
{
	PMString s = text;
	int32 numUTF16 = 0;
	const textchar* buff = s.GrabUTF16Buffer(&numUTF16);
	if (buff == nil || numUTF16 == 0)
		return;
	gPort->selectfont(font, fontSize);
	gPort->show(x, yBaseline, numUTF16, buff);
}

void XRLUICommentsPinAdornment::DrawPin(IGraphicsPort* gPort, IPMFont* font, IFontInstance* fontInst,
	const PMReal& px, const PMReal& py, const PMReal& inv, const XRLUICommentPin& pin) const
{
	const PMReal pinR     = 5.0  * inv;   // rayon du point
	const PMReal fontSize = 11.0 * inv;   // texte bulle
	const PMReal pad      = 6.0  * inv;   // marge interne bulle
	const PMReal gap      = 8.0  * inv;   // écart pin -> bulle
	const PMReal lineGap  = 3.0  * inv;   // interligne

	// Mesure des deux lignes pour dimensionner la bulle.
	PMReal wHeader = 0, hHeader = 0, wText = 0, hText = 0;
	{
		int32 n = 0;
		PMString h = pin.header;
		const textchar* hb = h.GrabUTF16Buffer(&n);
		if (hb != nil) fontInst->MeasureWText(hb, n, wHeader, &hHeader);
		PMString t = pin.text;
		const textchar* tb = t.GrabUTF16Buffer(&n);
		if (tb != nil) fontInst->MeasureWText(tb, n, wText, &hText);
	}
	if (hHeader <= 0) hHeader = fontInst->GetAscent();
	if (hText   <= 0) hText   = fontInst->GetAscent();

	const PMReal lineH    = (hHeader > hText ? hHeader : hText);
	const PMReal contentW = (wHeader > wText ? wHeader : wText);
	const PMReal bubbleW  = contentW + 2 * pad;
	const PMReal bubbleH  = 2 * lineH + lineGap + 2 * pad;

	// Bulle ancrée à droite du pin, centrée verticalement dessus.
	const PMReal bLeft = px + gap;
	const PMReal bTop  = py - bubbleH / 2.0;
	const PMRect bubble(bLeft, bTop, bLeft + bubbleW, bTop + bubbleH);

	// Fond sombre.
	gPort->newpath();
	gPort->setrgbcolor(45 / 256.0, 52 / 256.0, 60 / 256.0);
	gPort->setopacity(0.92, kTrue);
	gPort->rectpath(bubble);
	gPort->fill();
	gPort->setopacity(1.0, kTrue);

	// Texte blanc : entête puis commentaire.
	gPort->setrgbcolor(1.0, 1.0, 1.0);
	const PMReal tx = bubble.Left() + pad;
	this->DrawText(gPort, font, fontSize, tx, bubble.Top() + pad + hHeader, pin.header);
	this->DrawText(gPort, font, fontSize, tx, bubble.Top() + pad + lineH + lineGap + hText, pin.text);

	// Pin rouge par-dessus (dessiné en dernier pour rester visible).
	gPort->setrgbcolor(0.85, 0.10, 0.10);
	gPort->setlinewidth(0);
	this->FillCircle(gPort, px, py, pinR);
}

void XRLUICommentsPinAdornment::DrawAdornment(IShape* iShape, AdornmentDrawOrder drawOrder, GraphicsData* gd, int32 flags)
{
	do
	{
		if (drawOrder != kAfterShape)
			break;
		if (flags & IShape::kPrinting)
			break;

		XRLUICommentPinStore& store = XRLUICommentPinStore::Instance();
		if (!store.GetShow())
			break;
		const std::vector<XRLUICommentPin>& pins = store.GetPins();
		if (pins.empty())
			break;

		// iShape = le bloc-ancre (1x1, premier plan) créé par l'observer. On
		// récupère son spread pour dessiner les pins des pages de ce spread.
		IDataBase* db = ::GetDataBase(iShape);
		if (db == nil)
			break;
		InterfacePtr<IHierarchy> blockHier(iShape, UseDefaultIID());
		if (blockHier == nil)
			break;
		InterfacePtr<ISpread> spread(db, blockHier->GetSpreadUID(), UseDefaultIID());
		if (spread == nil)
			break;
		InterfacePtr<IGeometry> blockGeo(iShape, UseDefaultIID());
		if (blockGeo == nil)
			break;

		IGraphicsPort* gPort = gd->GetGraphicsPort();
		if (gPort == nil)
			break;

		// Police par défaut pour le texte des bulles.
		InterfacePtr<IFontMgr> fontMgr(GetExecutionContextSession(), UseDefaultIID());
		if (fontMgr == nil)
			break;
		InterfacePtr<IPMFont> theFont(fontMgr->QueryFont(fontMgr->GetDefaultFontName()));
		if (theFont == nil)
			break;

		// Taille constante à l'écran quel que soit le zoom (cf. adornments XPage).
		PMReal zoom = 1.0;
		InterfacePtr<ILayoutControlData> layoutControlData(Utils<ILayoutUIUtils>()->QueryFrontLayoutData());
		InterfacePtr<IPanorama> panorama(layoutControlData, UseDefaultIID());
		if (panorama != nil)
			zoom = panorama->GetXScaleFactor();
		if (zoom <= 0)
			zoom = 1.0;
		const PMReal inv = 1.0 / zoom;

		PMMatrix fontMatrix(11.0 * inv, 0.0, 0.0, 11.0 * inv, 0.0, 0.0);
		InterfacePtr<IFontInstance> fontInst(fontMgr->QueryFontInstance(theFont, fontMatrix));
		if (fontInst == nil)
			break;

		AutoGSave autoGSave(gPort);

		// Pour chaque page Gaia du spread, dessine ses pins. Position : point
		// page-interne -> pasteboard -> repère interne du bloc-ancre (le port
		// est dans le repère du bloc à kAfterShape). Le bloc étant à l'échelle 1,
		// les tailles (point, texte) restent correctes.
		const int32 nPages = spread->GetNumPages();
		for (int32 p = 0; p < nPages; ++p)
		{
			const UID pageUID = spread->GetNthPageUID(p);
			InterfacePtr<IXRailPageSlugData> slug(db, pageUID, IID_PAGESLUGDATA);
			const int32 idPage = (slug != nil) ? slug->GetID() : 0;
			if (idPage <= 0)
				continue;

			InterfacePtr<IGeometry> pageGeo(db, pageUID, UseDefaultIID());
			if (pageGeo == nil)
				continue;
			const PMRect pageBounds = pageGeo->GetStrokeBoundingBox();
			const PMReal pw = pageBounds.Right() - pageBounds.Left();
			const PMReal ph = pageBounds.Bottom() - pageBounds.Top();
			if (pw <= 0 || ph <= 0)
				continue;

			for (size_t i = 0; i < pins.size(); ++i)
			{
				const XRLUICommentPin& pin = pins[i];
				if (pin.idPage != idPage)
					continue;

				PMPoint pt(pageBounds.Left() + (pin.xPct / 100.0) * pw,
				           pageBounds.Top()  + (pin.yPct / 100.0) * ph);
				::TransformInnerPointToPasteboard(pageGeo, &pt);
				::TransformPasteboardPointToInner(blockGeo, &pt);
				this->DrawPin(gPort, theFont, fontInst, pt.X(), pt.Y(), inv, pin);
			}
		}

	} while (kFalse);
}

PMRect XRLUICommentsPinAdornment::GetPaintedAdornmentBounds(IShape* /*iShape*/, AdornmentDrawOrder /*drawOrder*/,
	const PMRect& itemBounds, const PMMatrix& /*innertoview*/)
{
	// Le bloc-ancre est minuscule (1x1) et à l'origine, mais on dessine les pins
	// loin (sur tout le spread). On élargit donc très largement la zone peinte,
	// sinon les pins seraient clippés à la zone du bloc.
	const PMReal m = 20000.0;
	return PMRect(itemBounds.Left() - m, itemBounds.Top() - m, itemBounds.Right() + m, itemBounds.Bottom() + m);
}
