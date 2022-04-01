#include "WizardAI.h"

// DO NOT CHANGE THIS FILE
// DO NOT CHANGE THIS FILE
// DO NOT CHANGE THIS FILE
// DO NOT CHANGE THIS FILE
// DO NOT CHANGE THIS FILE
// Instead - edit WizardAI.h header file!

///////////////////////////////////////////////////////////////////////////////////
// CWizardAI

void CWizardAI::getAnimData(unsigned iAnim, float time, std::vector<float>& transforms)
{
	if (time - t1 > (float)GetAnimDuration(0)) t1 = 0;
	float t = t1 + t2;
	C3dglModel::getAnimData(iAnim, t ? time - t : 0, transforms);
}

void CWizardAI::AnimateIdle(float time)
{
	t1 = time - fmodf(time - t2, (float)GetAnimDuration(0));
	t2 = 0;
}

void CWizardAI::AnimateYouShallNotPass(float time)
{
	t1 = 0;
	t2 = time;
}

