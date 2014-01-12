#include "maxcpp6.h"

// inherit from the MSP base class, template-specialized for myself:

class Example : public MspCpp6<Example> {
public:

	Example(t_symbol * sym, long ac, t_atom * av) { 
		setupIO(2, 2); 
		post("object created"); 
	}
	
	~Example() { 
		post("object freed"); 
	}	
	
	// methods:
	void bang(long inlet) { 
		post("bang in inlet %i!", inlet); 
	}
	void test(long inlet, t_symbol * s, long ac, t_atom * av) { 
		post("%s in inlet %i (%i args)", s->s_name, inlet, ac);
	}
	
	// default signal processing method is called 'perform'
	void perform(double **ins, long numins, double **outs, long numouts, long sampleframes) {
		// example code to invert inputs
		for (long channel = 0; channel < numouts; channel++) {
			double * in = ins[channel];
			double * out = outs[channel];
			for (long i=0; i < sampleframes; i++) {
				out[i] = -in[i];
			}
		}
	}
	
//	// optional method: gets called when the dsp chain is modified
//	// if not given, the MspCpp will use Example::perform by default
//	void dsp(t_object * dsp64, short *count, double samplerate, long maxvectorsize, long flags) { 
//		// specify a perform method manually:
//		REGISTER_PERFORM(Example, perform);
//	}
};

C74_EXPORT int main(void) {
	// create a class with the given name:
	Example::makeMaxClass("example~");
	REGISTER_METHOD(Example, bang);
	REGISTER_METHOD_GIMME(Example, test);
}