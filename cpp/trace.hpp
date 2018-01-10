#ifndef _TRACE_HPP_
#define _TRACE_HPP_


#include <vector>

#include <nan.h>

#include <LinearMath/btVector3.h>

#include "common.hpp"


class btGeneric6DofSpringConstraint;

class Body;


class Trace : public Nan::ObjectWrap {
	
public:
	
	static void init(v8::Handle<v8::Object> target);
	static void deinit();
	
protected:
	
	Trace(bool hit, Body *body, const btVector3 &pos, const btVector3 &norm);
	Trace();
	virtual ~Trace();
	
	static NAN_METHOD(newCtor);
	
	static NAN_GETTER(hitGetter);
	static NAN_GETTER(bodyGetter);
	static NAN_GETTER(posGetter);
	static NAN_GETTER(normGetter);
	
	
private:
	
	static v8::Persistent<v8::Function> _constructor;
	
	
private: // prop cache
	bool _cacheHit;
	Body *_cacheBody;
	btVector3 _cachePos;
	btVector3 _cacheNorm;
	
	
private:
	void _rebuild();
	
};


#endif // _TRACE_HPP_
