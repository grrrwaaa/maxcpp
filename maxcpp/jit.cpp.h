/*	jit.cpp.h
 *
 *	MAV Framework, 2008 ICSRiM Leeds
 *	Thijs Koerselman
 *
 *	Based on original Cpp Template code by Graham Wakefield
 */


#ifndef JIT_CPP_H
#define JIT_CPP_H

#include "jit.common.h"

#define calcoffset_cpp(OBJECT, MEMBER) \
((size_t) ((char *)&(OBJECT->MEMBER) - (char*)OBJECT))


template <class T>
class JitCpp
{

protected:
		// these 2 are fixed for every jit module
		t_jit_object			jitOb;	// 1st in struct
		static void *           jitClass;     // Jit 'class' pointer
		static char *			jitClassName;

private:
		static void *           jitAlloc(t_symbol *s, long argc, t_atom *argv);
		static void             jitFree(T *x);
		static void *           operator new(size_t);
		static void             operator delete(void *);

public:
       // called from 'main' to create class ptr
       static void *        jitMakeClass(char *jit_class_name);     

       // use normal C++ constructors & destructors:
       JitCpp();
       ~JitCpp();
};

// singleton class object
template <class T>
void * JitCpp<T>::jitClass = 0;
template <class T>
char * JitCpp<T>::jitClassName = NULL;


template <class T>
inline void * JitCpp<T>::jitMakeClass(char *jit_class_name)
{
	// create class and allocate memory
	void * _class = jit_class_new(
					jit_class_name,
					(method)T::jitAlloc,
					(method)T::jitFree,
					(short)sizeof(T),
					A_GIMME,
					0L);


	// store static ref
	JitCpp<T>::jitClass = _class;
	JitCpp<T>::jitClassName = jit_class_name;

	return _class;
}

template <class T>
inline void * JitCpp<T> :: jitAlloc(t_symbol * sym, long ac, t_atom *av)
{
	return new T(sym, ac, av);
}

template <class T>
inline void JitCpp<T> :: jitFree(T * x)
{
	delete x;
}

template <class T>
inline void * JitCpp<T> :: operator new(size_t t)
{
	// use jitter object allocation and then call
	// class constructor
	return jit_object_alloc(JitCpp<T>::jitClass);
}

template <class T>
inline void JitCpp<T> :: operator delete(void *)
{
	// nothing to do - Max will manage the memory
}

template <class T>
inline JitCpp<T>::JitCpp()
{
	// nothing to do
}

template <class T>
inline JitCpp<T> :: ~JitCpp()
{
	jit_object_free(this);
}

#endif //JIT_CPP_H