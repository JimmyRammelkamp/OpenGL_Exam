/*********************************************************************************
3DGL WizardAI Class created by Jarek Francik for Kingston University students
Copyright (C) 2022 Jarek Francik, Kingston University, London, UK
*********************************************************************************/

#ifndef __wizardAI_h_
#define __wizardAI_h_

#include "GL/3dgl.h"

// This class inherits all functionality after conventional C3dglModel
// but overwrites getAnimData to provide specific animations for the wizard:
// - call AnimateIdle(time) to initiate IDLE animation
// - call AnimateYouShallNotPass(time) to initiate YOU_SHALL_NOT_PASS animation.
//
// This class is also supposed to implement a State Machine
// We have defined two states for you: IDLE and YOU_SHALL_NOT_PASS.
// We have also created the ChangeState function - you should be familar we the concept.
// But this function is just a stub and you need to complete its implementation.
//
// To use this class, simply locate the definition of the "wizard" object in main.cpp, like
// C3dglModel wizard;
// and subsequently change it to:
// CWizardAI wizard;
//
// then you can use wizard.ChangeState (when you detect appropriate triggers).
// If properly done, calling wizard.getAnimData(...) will play the appropriate animation for you!

class CWizardAI : public _3dgl::C3dglModel
{
public:
	enum STATE { IDLE, YOU_SHALL_NOT_PASS };
private:
	enum STATE state;
	float t1, t2;

public:
	CWizardAI() : C3dglModel() { state = IDLE; t1 = t2 = 0; }

	void ChangeState(STATE newState, float time)
	{
		if (newState == state)
			return;

		state = newState;

		switch (state)
		{
		case IDLE:
			AnimateIdle(time);
			break;
		case YOU_SHALL_NOT_PASS:
			AnimateYouShallNotPass(time);
		}
		



		// TO DO: Implement this function
		// Note - you will need current time (provided as a parameter) to start animations!
	}

	void getAnimData(unsigned iAnim, float time, std::vector<float>& transforms);

	// START ANIMATIONS:
	void AnimateIdle(float time);
	void AnimateYouShallNotPass(float time);
};

#endif