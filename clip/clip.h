#pragma once

#include "audioeffectx.h"

class Clipz : public AudioEffectX {
public:
	Clipz(audioMasterCallback audioMaster);
	~Clipz();

	virtual void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames);

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
	float mix = 0.0f;
	float thr = 1.0f, inv = 1.0f, inv1 = 1.0f;
	float clg = 0.5f;
	float z = 0.f, x = 0.f;
	float c = 0.f, v = 0.f;
	char programName[kVstMaxProgNameLen + 1];
};