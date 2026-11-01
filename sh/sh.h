#pragma once

#include "audioeffectx.h"

class Bity : public AudioEffectX {
public:
	Bity(audioMasterCallback audioMaster);
	~Bity();

	virtual void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames);

	virtual void setProgramName(char* name);
	virtual void getProgramName(char* name);

	virtual void setParameter(VstInt32 index, float value);
	virtual float getParameter(VstInt32 index);
	virtual void getParameterLabel(VstInt32 index, char* label);
	virtual void getParameterDisplay(VstInt32 index, char* text);
	virtual void getParameterName(VstInt32 index, char* text);

	virtual bool getEffectName(char* name);
	virtual bool getVendorString(char* text);
	virtual bool getProductString(char* text);
	virtual VstInt32 getVendorVersion();

protected:
	float fMix = 0.f;
	short updt = 1;
	float lerp = 0.f;
	float noiz = 0.f;
	float z = 0.f, z1 = 0.f;
	float x = 0.f, x1 = 0.f;
	float r = 0.f;
	unsigned int rs = 0;
	short c = 0;
	char programName[kVstMaxProgNameLen + 1];
};