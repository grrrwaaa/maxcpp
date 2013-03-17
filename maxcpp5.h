/*

Write MaxMSP (v5) objects in C++

-- general --

Allows the definition of a max object as a C++ class. 

Most of the mechanics of typical max objects are encapsulated by the curiously recurring template pattern. 

See the example.cpp and example~.cpp for max and msp example objects.

note: be sure to include this header only once, or the linker will complain of duplicate symbols

note 2: this probably won't work with virtuals -- and definitely won't work with multiple inheritance!

-- licence --
maxcpp is distributed under the permissive BSD-like MIT licence:

Copyright (c) 2009 Graham Wakefield

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
#ifndef MAXMSP_CPP_5_H
#define MAXMSP_CPP_5_H

#include "ext.h"
#include "ext_obex.h"
#include "ext_common.h"
#include "commonsyms.h"
#include "z_dsp.h"

#include <new>

#define MAX_CPP_VERSION	0.005
#define MAX_CPP_MAX_DSP_SIGNALS (16) // max. no. signal ins/outs

// for A_NOTHING methods (e.g. bang):
#define REGISTER_METHOD(CLASS, METHOD)	class_addmethod(	\
	(t_class *)CLASS::m_class,								\
	(method)CLASS::MaxMethod<&CLASS::METHOD>::call,			\
	#METHOD,												\
	0);	
// for A_GIMME methods (t_symbol * s, long argc, void * argv):
#define REGISTER_METHOD_GIMME(CLASS, METHOD)	class_addmethod(	\
	(t_class *)CLASS::m_class,								\
	(method)CLASS::MaxMethodGimme<&CLASS::METHOD>::call,	\
	#METHOD,												\
	A_GIMME,												\
	0);	
	
// for A_FLOAT methods (double v):
#define REGISTER_METHOD_FLOAT(CLASS, METHOD)	class_addmethod(	\
	(t_class *)CLASS::m_class,								\
	(method)CLASS::MaxMethodFloat<&CLASS::METHOD>::call,	\
	#METHOD,												\
	A_FLOAT,												\
	0);	
	
// for A_INT methods (long v):
#define REGISTER_METHOD_LONG(CLASS, METHOD)	class_addmethod(	\
	(t_class *)CLASS::m_class,								\
	(method)CLASS::MaxMethodLong<&CLASS::METHOD>::call,	\
	#METHOD,												\
	A_LONG,												\
	0);	
	
// a purely static base class for Max and MSP objects:
template <typename T>
class MaxCppBase {
public:	
	static t_class * m_class;
	
	// template functors to forward Max messages to class methods:
	
	typedef void (T::*maxmethodgimme)(long inlet, t_symbol * s, long ac, t_atom * av);
	template<maxmethodgimme F>
	struct MaxMethodGimme {
		static void call(T * x, t_symbol * s, long ac, t_atom * av) { ((x)->*F)(proxy_getinlet((t_object *)x), s, ac, av); }
	};
	
	typedef void (T::*maxmethod)(long inlet);
	template<maxmethod F>
	struct MaxMethod {
		static void call(T * x) { ((x)->*F)(proxy_getinlet((t_object *)x)); }
	};
	
	typedef void (T::*maxmethodlong)(long inlet, long v);
	template<maxmethodlong F>
	struct MaxMethodLong {
		static void call(T * x, long v) { ((x)->*F)(proxy_getinlet((t_object *)x), v); }
	};
	
	typedef void (T::*maxmethodfloat)(long inlet, double v);
	template<maxmethodfloat F>
	struct MaxMethodFloat {
		static void call(T * x, double v) { ((x)->*F)(proxy_getinlet((t_object *)x), v); }
	};
};

// inherit from this one for non-audio objects
template <typename T>
class MaxCpp5 : public MaxCppBase<T> {
public:
	t_object m_ob;
	int m_outlets, m_inlets;
	void **	m_outlet;
	void **	m_inletproxy;
	long m_whichinlet;
	
	static t_class * makeMaxClass(const char * name);
	static void * create(t_symbol * sym, long ac, t_atom * av);
	static void destroy(t_object * x);
	
	void setupIO(unsigned int numinlets, unsigned int numoutlets);
	
	// C++ operator overload to treat MaxCpp5 objects as t_objects
	operator t_object & () { return m_ob; }
	
	
};

// inherit from this one for audio objects
template <typename T>
class MspCpp5 : public MaxCppBase<T> {
public:
	typedef void (T::*maxmethodperform)(int vs, t_sample ** inputs, t_sample ** outputs);
	
	t_pxobject m_ob;
	int m_siginlets, m_sigoutlets;
	maxmethodperform m_perform;
	
	static t_class * makeMaxClass(const char * name);
	static void * create(t_symbol * sym, long ac, t_atom * av);
	static void destroy(t_object * x);
	
	void setupIO(maxmethodperform meth, unsigned int siginlets, unsigned int sigoutlets);
	
	// use this to change which perform method is being used:
	void changePerformMethod(maxmethodperform meth) { m_perform = meth; }
	
	static void internal_dsp(MspCpp5<T> * x, t_signal **sp, short *count);
	static t_int * internal_perform(t_int *);
	
	// stub function in case the user doesn't supply one
	void dsp() {}
		
	struct PerformData {
		T * x;
		int vs;
		t_sample * inputs[MAX_CPP_MAX_DSP_SIGNALS];
		t_sample * outputs[MAX_CPP_MAX_DSP_SIGNALS];
	};
};

// note: only include this file once to prevent linker errors!
template<typename T> t_class * MaxCppBase<T>::m_class = 0;

template<typename T> t_class * MaxCpp5<T>::makeMaxClass(const char * name) {
	common_symbols_init();
	MaxCppBase<T>::m_class = class_new((char *)name, (method)MaxCpp5<T>::create, (method)MaxCpp5<T>::destroy, sizeof(T), 0L, A_GIMME, 0);   
	class_register(CLASS_BOX, MaxCppBase<T>::m_class);
	return MaxCppBase<T>::m_class;
}

template<typename T> void * MaxCpp5<T>::create(t_symbol * sym, long ac, t_atom * av) {
	void * x = object_alloc(MaxCppBase<T>::m_class);
	new(x) T(sym, ac, av);
	return (T *)x; 
}

template<typename T> void MaxCpp5<T>::destroy(t_object * x) {
	T * t = (T *)x;
	t->~T();
	// free outlets:
	sysmem_freeptr(t->m_outlet);
	sysmem_freeptr(t->m_inletproxy);
}

template<typename T> void MaxCpp5<T>::setupIO(unsigned int numinlets, unsigned int numoutlets) {
	if (numinlets > 0) {
		m_inlets = (long)numinlets - 1;
		m_inletproxy = (void **)sysmem_newptr(sizeof(void *) * m_inlets);
		for (long i=1; i<=m_inlets; i++)
			m_inletproxy[i] = proxy_new(this, i, &this->m_whichinlet); // generic outlet
	}
	
	m_outlets = (long)numoutlets;
	m_outlet = (void **)sysmem_newptr(sizeof(void *) * m_outlets);
	for (long i=0; i<m_outlets; i++)
		m_outlet[m_outlets - i - 1] = outlet_new(this, NULL); // generic outlet
}

template<typename T> t_class * MspCpp5<T>::makeMaxClass(const char * name) {
	common_symbols_init();
	MaxCppBase<T>::m_class = class_new((char *)name, (method)MspCpp5<T>::create, (method)MspCpp5<T>::destroy, sizeof(T), 0L, A_GIMME, 0);   
	class_dspinit(MaxCppBase<T>::m_class);
	class_addmethod(MaxCppBase<T>::m_class, (method)MspCpp5<T>::internal_dsp, "dsp", A_CANT, 0);
	class_register(CLASS_BOX, MaxCppBase<T>::m_class);
	return MaxCppBase<T>::m_class;
}

template<typename T> void * MspCpp5<T>::create(t_symbol * sym, long ac, t_atom * av) {
	void * x = object_alloc(MaxCppBase<T>::m_class);
	new(x) T(sym, ac, av);
	return (T *)x; 
}

template<typename T> void MspCpp5<T>::destroy(t_object * x) {
	dsp_free((t_pxobject *)x);
	T * t = (T *)x;
	t->~T();
}

template<typename T> void MspCpp5<T>::setupIO(maxmethodperform meth, unsigned int siginlets, unsigned int sigoutlets) {
	m_perform = meth;
	m_siginlets = MIN(siginlets, MAX_CPP_MAX_DSP_SIGNALS);
	m_sigoutlets = MIN(sigoutlets, MAX_CPP_MAX_DSP_SIGNALS);
	dsp_setup((t_pxobject *)this, m_siginlets);
	// prevent recycling of inputs for outputs
	m_ob.z_misc = Z_NO_INPLACE;
	for (int i=0; i < m_sigoutlets; i++)
		outlet_new(this, "signal");
}

template<typename T> void MspCpp5<T>::internal_dsp(MspCpp5<T> * x, t_signal **sp, short *count) {
	int i=0;
	int s=0;
	PerformData pdata;
	pdata.x = (T *)x;	
	pdata.vs = sp[i]->s_n;
	for (i=0; i< x->m_siginlets; i++, s++) {
		pdata.inputs[i] = sp[s]->s_vec;
	}
	for (i=0; i< x->m_sigoutlets; i++, s++) {
		pdata.outputs[i] = sp[s]->s_vec;
	}
	dsp_addv(MspCpp5<T>::internal_perform, sizeof(PerformData)/sizeof(t_int), (void **)&pdata);
	x->dsp();
}

template<typename T> t_int * MspCpp5<T>::internal_perform(t_int * w) {
	PerformData * pdata = (PerformData *)(w+1);
	MspCpp5<T> * x = pdata->x;
	T * self = pdata->x;
	if (!x->m_ob.z_disabled) {
		// forward this to the user dsp routine:
		((self)->*(self->m_perform))(pdata->vs, pdata->inputs, pdata->outputs);
	}
	return w + sizeof(PerformData)/sizeof(t_int) + 1;
}

#endif //MAXMSP_CPP_5_H
