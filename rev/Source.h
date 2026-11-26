#pragma once

#include "audioeffectx.h"

class RevIN : public AudioEffectX {
public:
	RevIN(audioMasterCallback a);
	~RevIN();

	virtual void processReplacing(float** i, float** o, VstInt32 f);

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
	float lc[2] = { 1.f }; // pre-matrix low cut, post-matrix low cut
	float m[8][8] = { 0.f }; // matrix
	unsigned char r[8]; // feedback routing
	float D[8] = { 0.f }; // output mix coeffs
	float M[8] = { 1.f };
	float l[8][844]; // dleay
	float s = 0.f; // size
	float d = 0.f, invd = 1.f; // decay
	unsigned short L[8] = { 0 }; // length
	unsigned short h[8] = { 0 }; // head
	float mix = 0.f;
};