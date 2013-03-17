#include "maxcpp5.h"


class Example : public MspCpp5<Example> {
public:
	Example(t_symbol * sym, long ac, t_atom * av) { 
		setupIO(&Example::perform, 2, 2); 
		post("created an example"); 
	}
	~Example() { post("freed an example"); }	
	
	// methods:
	void bang(long inlet) { 
		post("bang in inlet %i!", inlet); 
	}
	void test(long inlet, t_symbol * s, long ac, t_atom * av) { 
		post("%s in inlet %i (%i args)", s->s_name, inlet, ac);
	}
	
	// optional method: gets called when the dsp chain is modified
	void dsp() { post("user-dsp"); }
	
	// signal processing example: inverts sign of inputs
	void perform(int vs, t_sample ** inputs, t_sample ** outputs) {
		for (int channel = 0; channel < 2; channel++) {
			t_sample * in = inputs[channel];
			t_sample * out = outputs[channel];
			for (int i=0; i<vs; i++) {
				out[i] = -in[i];
			}
		}
	}
};

extern "C" int main(void) {
	// create a class with the given name:
	Example::makeMaxClass("example~");
	REGISTER_METHOD(Example, bang);
	REGISTER_METHOD_GIMME(Example, test);
}