#include "limit.h"

const char* labels[6] = {
	"MIX", "THRESH", "ATTACK", "RELEASE", "IN GAIN", "LINKED"
};

const char units[6] = {
	'%', 'A', '%', '%', 'A', '?'
};

#define min(x,m) (((x)>(m))?(m):(x))
#define max(x,m) (((x)<(m))?(m):(x))
#define clamp(x,a,b) min(max(x,b),a) // min and max already parenthesizes so no need
#define sign(x) (((x) < 0.f)?-1.f:1.f)
#define abs(x) ((x)<0.f?-(x):(x))


AudioEffect* createEffectInstance(audioMasterCallback audioMaster) {
	return new Limee(audioMaster);
}

Limee::Limee(audioMasterCallback audioMaster) : AudioEffectX(audioMaster, 1, 6) {
	setNumInputs(2); setNumOutputs(2);
	canProcessReplacing();
	setUniqueID('Lmit');

	mix = thr = ign  = atk = rel = 1.f;
	nmi = 0.f;
	link = 0;
	vst_strncpy(programName, "Limee", 5);
}

Limee::~Limee() {}

void Limee::setProgramName(char* name) { vst_strncpy(programName, name, kVstMaxProgNameLen); }
void Limee::getProgramName(char* name) { vst_strncpy(name, programName, kVstMaxProgNameLen); }

void Limee::setParameter(VstInt32 index, float value) {
	value = clamp(value, 1, 0);
	switch (index) {
	case 0:
		mix = value; 
		nmi = 1.f - mix;
		break;
	case 1:
		thr = value; break;
	case 2:
		atk = value; break;
	case 3:
		rel = value; break;
	case 4:
		ign = value * 4.f + 1.f; break;
	case 5:
		link = (char)(value * 2.f); break;
	}
}
float Limee::getParameter(VstInt32 index) {
	float o = 0.f;
	switch (index) {
	case 0:
		o = mix; break;
	case 1:
		o = thr; break;
	case 2:
		o = atk; break;
	case 3:
		o = rel; break;
	case 4:
		o = (ign - 1.f) * 0.25f; break;
	case 5:
		o = (float)link * 0.5f; break;
	}
	return o;
}
void Limee::getParameterLabel(VstInt32 index, char* label) { vst_strncpy(label, (char*)&units[index], 1); }
void Limee::getParameterDisplay(VstInt32 index, char* text) { 
	switch (index) {
	case 0:
		float2string(mix, text, 4); break;
	case 1:
		float2string(thr, text, 4); break;
	case 2:
		float2string(atk, text, 4); break;
	case 3:
		float2string(rel, text, 4); break;
	case 4:
		float2string(ign, text, 4); break;
	case 5:
		text[0] = (link + '0'); 
		text[1] = '\0';
		break;
	}
}
void Limee::getParameterName(VstInt32 index, char* text) { vst_strncpy(text, labels[index], 7); }

VstPlugCategory Limee::getPlugCategory() { return kPlugCategEffect; }
bool Limee::getVendorString(char* text) {
	vst_strncpy(text, "butte", 5);
	return true;
}
bool Limee::getEffectName(char* name) {
	vst_strncpy(name, programName, kVstMaxEffectNameLen);
	return true;
}
bool Limee::getProductString(char* text) {
	vst_strncpy(text, programName, kVstMaxProductStrLen);
	return true;
}
VstInt32 Limee::getVendorVersion() { return 1000; }

void Limee::processReplacing(float** in, float** out, VstInt32 frames) {
	float* i0 = in[0];
	float* i1 = in[1];
	float* o0 = out[0];
	float* o1 = out[1];

	float g[2] = { 1.f,1.f };

	while (frames--) {
		if (abs(*i0 * ign) > pik[0]) pik[0] += (abs(*i0 * ign) - pik[0]) * atk;
		if (abs(*i1 * ign) > pik[1]) pik[1] += (abs(*i1 * ign) - pik[1]) * atk;
		if (thr > 0.01f) {
			g[0] = (pik[0] > thr) ? thr / pik[0] : 1.f;
			g[1] = (pik[1] > thr) ? thr / pik[1] : 1.f;
		} else g[0] = g[1] = 0.f;

		if (link == 1) {
			g[1] = g[0];
		}
		else if (link == 2) {
			g[0] = g[1];
		}

		g[0] *= mix;
		g[0] += nmi;
		g[1] *= mix;
		g[1] += nmi;

		*o0++ = *i0++ * g[0] * ign;
		*o1++ = *i1++ * g[1] * ign;

		pik[0] *= rel;
		pik[1] *= rel;
	}
}