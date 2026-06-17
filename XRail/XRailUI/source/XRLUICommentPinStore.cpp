#include "VCPlugInHeaders.h"
#include "XRLUICommentPinStore.h"

XRLUICommentPinStore& XRLUICommentPinStore::Instance()
{
	static XRLUICommentPinStore sInstance;
	return sInstance;
}
