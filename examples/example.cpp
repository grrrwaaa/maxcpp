#include "maxcpp5.h"

class Example : public MaxCpp5<Example> {
public:
	Example(t_symbol * sym, long ac, t_atom * av) { 
		setupIO(2, 2); // inlets / outlets
	}
	~Example() {}	
	
	// methods:
	void bang(long inlet) { 
		outlet_bang(m_outlet[0]);
	}
	void testfloat(long inlet, double v) { 
		outlet_float(m_outlet[0], v);
	}
	void testint(long inlet, long v) { 
		outlet_int(m_outlet[0], v);
	}
	void test(long inlet, t_symbol * s, long ac, t_atom * av) { 
		outlet_anything(m_outlet[1], gensym("test"), ac, av); 
	}
};

extern "C" int main(void) {
	// create a class with the given name:
	Example::makeMaxClass("example");
	REGISTER_METHOD(Example, bang);
	REGISTER_METHOD_FLOAT(Example, testfloat);
	REGISTER_METHOD_LONG(Example, testint);
	REGISTER_METHOD_GIMME(Example, test);
}