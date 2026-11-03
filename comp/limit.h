#pragma once

#include "audioeffectx.h"

class Limee : public AudioEffectX {
public:
	Limee(audioMasterCallback audioMaster);
	~Limee();
	
	virtual void processReplacing(float** in, float** out, VstInt32 frames);

	virtual void setProgramName(char* name);
	virtual void getProgramName(char* name);

	virtual void setParameter(VstInt32 index, float value);
	virtual float getParameter(VstInt32 index);
	virtual void getParameterLabel(VstInt32 index, char* label);
	virtual void getParameterDisplay(VstInt32 index, char* text);
	virtual void getParameterName(VstInt32 index, char* text);

	virtual bool getEffectName(char* name);
	virtual VstPlugCategory getPlugCategory();
	virtual bool getVendorString(char* text);
	virtual bool getProductString(char* text);
	virtual VstInt32 getVendorVersion();

protected:
	float mix, nmi, thr, pik[2] = { 0.f, 0.f }, ign;
	float atk, rel;
	char link;
	char programName[kVstMaxProgNameLen+1];
};