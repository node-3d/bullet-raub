#ifndef _TRACE_HPP_
#define _TRACE_HPP_

#include "common.hpp"


class btGeneric6DofSpringConstraint;

class Body;
class Scene;


ATTRIBUTE_ALIGNED16(class) Trace {
DECLARE_ES5_CLASS(Trace, Trace);
	
public:
	static void init(Napi::Env env, Napi::Object exports);
	
	~Trace();
	explicit Trace(const Napi::CallbackInfo &info);
	
	void _destroy();
	
	void *operator new (size_t size) { return btAlignedAlloc(size, 16); }
	void operator delete (void *ptr) { btAlignedFree(ptr); }
	
	void _destroy();
	
	
private:
	
	bool _isDestroyed;
	
	void _rebuild();
	
	bool _cacheHit;
	Body *_cacheBody;
	btVector3 _cachePos;
	btVector3 _cacheNorm;
	
	JS_DECLARE_METHOD(Trace, newCtor);
	JS_DECLARE_METHOD(Trace, destroy);
	JS_DECLARE_GETTER(Trace, isDestroyed);
	
	JS_DECLARE_GETTER(Trace, hit);
	JS_DECLARE_GETTER(Trace, body);
	JS_DECLARE_GETTER(Trace, pos);
	JS_DECLARE_GETTER(Trace, norm);
	
};


#endif // _TRACE_HPP_
