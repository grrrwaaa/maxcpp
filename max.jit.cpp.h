/*	max.jit.cpp.h
 *
 *	MAV Framework, 2008 ICSRiM Leeds
 *	Thijs Koerselman
 *
 *	Based on original Cpp Template code by Graham Wakefield
 */

#ifndef MAX_JIT_CPP_H
#define MAX_JIT_CPP_H

#include "jit.common.h"

#define calcoffset_cpp(OBJECT, MEMBER) \
((size_t) ((char *)&(OBJECT->MEMBER) - (char*)OBJECT))


template <class T>
class MaxJitCpp
{

protected:
       t_object					maxOb;			// Max 'instance' pointer
       void *					maxObex;		// Max Obex container
       static void *			maxClass;		// Max 'class' pointer
       static char *			jitClassName;	// our own reference

private:
       static void *			maxAlloc(t_symbol *s, long argc, t_atom *argv);
       static void				maxFree(T *x);
       static void *			operator new(size_t);
       static void				operator delete(void *);

public:

       // called from 'main' to create class ptr
       static void			maxMakeClass(char *name);      // use this for Max objects

       // use normal C++ constructors & destructors:
       MaxJitCpp();
       ~MaxJitCpp();
};


// forward decl static variables
template <class T>
void * MaxJitCpp<T>::maxClass = 0;
template <class T>
char * MaxJitCpp<T>::jitClassName = NULL;



template <class T>
inline void MaxJitCpp<T>::maxMakeClass(char *max_class_name)
{
	void *p, *q;
	T * cp = NULL;

	setup(
		(t_messlist **)&T::maxClass, // reference to store class info
		(method)T::maxAlloc,
		(method)T::maxFree,
		(short)sizeof(T),
		0L,
		A_GIMME,
		0);


	// store jitter object name in static member
	jitClassName = max_class_name;

	// allocate max_jit_classex data (incl jitter object struct?)
	p = max_jit_classex_setup(calcoffset_cpp(cp, maxObex));
	// lookup the registered class name to get info on methods and attributes
	q = jit_class_findbyname(gensym(jitClassName));
	// add standard jitter methods to object
	max_jit_classex_standard_wrap(p,q,0);
}

template <class T>
inline void * MaxJitCpp<T> :: maxAlloc(t_symbol *sym, long ac, t_atom *av)
{
	return new T(sym, ac, av);
}

template <class T>
inline void MaxJitCpp<T> :: maxFree(T *x)
{
	delete x;
}

template <class T>
inline void * MaxJitCpp<T> :: operator new(size_t t)
{
	// allocate wrapper class memory
	return (void *)max_jit_obex_new(T::maxClass,gensym(jitClassName));
}

template <class T>
inline void MaxJitCpp<T> :: operator delete(void *)
{
	// nothing to do - Max will manage the memory
}

template <class T>
inline MaxJitCpp<T>::MaxJitCpp(){}

template <class T>
inline MaxJitCpp<T> :: ~MaxJitCpp()
{
	// lookup the internal Jitter object instance and free
	//jit_object_free(max_jit_obex_jitob_get(this));

	// free resources associated with the obex entry
	//max_jit_obex_free(this);
}


#endif //MAX_JIT_CPP_H