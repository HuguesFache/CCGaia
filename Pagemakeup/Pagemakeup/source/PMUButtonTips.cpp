/*
//	File:	PMUButtonTips.cpp
//
//	Tooltip provider for the three main palette buttons that need a hint:
//	"Exec TCL", "Insert hidden reference", "Identify a block".
//
//	Aggregated on kPMUButtonWithTipBoss (a custom rollover icon button boss
//	declared in PMU.fr that derives from kRollOverIconButtonBoss). The
//	widget instance picks the boss via the PMUIconButtonWithTipWidget type
//	defined in PMU.fr. AbstractTip::GetTipText is called by the framework
//	whenever the user hovers over the button; we read the widget ID and
//	return the matching localized string key (the leading '#' tells the
//	framework to look it up in the StringTable).
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IControlView.h"

// General includes:
#include "AbstractTip.h"
#include "CPMUnknown.h"

// Project includes:
#include "PMUID.h"


/** PMUButtonTips
	Returns a localized tooltip string for the supported palette buttons.
*/
class PMUButtonTips : public AbstractTip
{
public:
	PMUButtonTips(IPMUnknown* boss) : AbstractTip(boss) {}
	virtual ~PMUButtonTips() {}

	virtual PMString GetTipText(const PMPoint& mouseLocation);
};

CREATE_PMINTERFACE(PMUButtonTips, kPMUButtonTipsImpl)


PMString PMUButtonTips::GetTipText(const PMPoint& /*mouseLocation*/)
{
	InterfacePtr<const IControlView> view(this, UseDefaultIID());
	if (view == nil)
		return kNullString;

	// PMString constructor with kTranslateDuringCall does the StringTable
	// lookup right away — we return the already-translated text. (The "#"
	// prefix convention used by some Adobe samples requires an explicit
	// .Translate() call, which we'd need to do anyway, so we skip it.)
	WidgetID widgetID = view->GetWidgetID();
	switch (widgetID.Get())
	{
		case kPMUExecTCLWidgetID:
			return PMString(kPMUExecTCLTipKey, PMString::kTranslateDuringCall);

		case kPMUAddPermRefWidgetID:
			return PMString(kPMUAddPermRefTipKey, PMString::kTranslateDuringCall);

		case kPMUAddTCLRefWidgetID:
			return PMString(kPMUAddTCLRefTipKey, PMString::kTranslateDuringCall);

		default:
			return kNullString;
	}
}

// End, PMUButtonTips.cpp.
