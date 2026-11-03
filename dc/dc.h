#pragma once

#include "audioeffectx.h"

class DCorrect : public AudioEffectX {
public:
	DCorrect(audioMasterCallback am);
	~DCorrect();

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
	short size, psize, head;
	float buff[2048][2] = { {0.f, 0.f} }, invs, dc[2] = { 0.f }, x[2] = { 0.f };
	char pn[kVstMaxProgNameLen + 1];
};