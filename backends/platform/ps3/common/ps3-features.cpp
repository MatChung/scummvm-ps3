#include "../ps3.h"


bool OSystem_PS3::hasFeature(Feature f)
{
	switch(f)
	{
	case kFeatureFullscreenMode:
		return false;
	case kFeatureAspectRatioCorrection:
		return true;
	case kFeatureVirtualKeyboard:
		return false;
	case kFeatureCursorHasPalette:
		return true;
	case kFeatureOverlaySupportsAlpha:
		return false;
	case kFeatureIconifyWindow:
		return false;
	case kFeatureDisableKeyFiltering:
		return false;
	}
	return false;
}

void OSystem_PS3::setFeatureState(Feature f, bool enable)
{
	if(f==kFeatureAspectRatioCorrection)
		_aspectRatioCorrection=enable;
	/*if(f==kFeatureVirtualKeyboard)
	{
		_virtkeybd_on=enable;
		showVirtualKeyboard(enable);
	}*/
}

bool OSystem_PS3::getFeatureState(Feature f)
{
	if(f==kFeatureAspectRatioCorrection)
		return _aspectRatioCorrection;
/*	if(f==kFeatureVirtualKeyboard)
		return _virtkeybd_on;
*/
	return false;
}
