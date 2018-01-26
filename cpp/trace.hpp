#ifndef _TRACE_HPP_
#define _TRACE_HPP_


#include <nan.h>

#include <LinearMath/btVector3.h>

#include "common.hpp"


class btGeneric6DofSpringConstraint;

class Body;
class Scene;


class Trace : public Nan::ObjectWrap {
	
public:
	
	static void init(v8::Handle<v8::Object> target);
	
	static v8::Local<v8::Object> instance(bool hit, Body *body, const btVector3 &pos, const btVector3 &norm);
	static v8::Local<v8::Object> instance(Scene *scene, const btVector3 &from, const btVector3 &to);
	
	Trace(bool hit, Body *body, const btVector3 &pos, const btVector3 &norm);
	Trace(Scene *scene, const btVector3 &from, const btVector3 &to);
	
	// apply(Scene *scene, const btVector3 &from, const btVector3 &to);
	
	
protected:
	
	virtual ~Trace();
	
	static NAN_METHOD(newCtor);
	
	static NAN_GETTER(hitGetter);
	static NAN_GETTER(bodyGetter);
	static NAN_GETTER(posGetter);
	static NAN_GETTER(normGetter);
	
	
private:
	
	static Nan::Persistent<v8::Function> _constructor;
	
	
private: // prop cache
	bool _cacheHit;
	Body *_cacheBody;
	btVector3 _cachePos;
	btVector3 _cacheNorm;
	
	
private:
	void _rebuild();
	
};


#endif // _TRACE_HPP_
