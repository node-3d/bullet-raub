#ifndef _TRACE_HPP_
#define _TRACE_HPP_


#include "common.hpp"


class btGeneric6DofSpringConstraint;

class Body;
class Scene;


ATTRIBUTE_ALIGNED16(class) Trace : public EventEmitter {
	
public:
	
	static void init(V8_VAR_OBJ target);
	static bool isTrace(V8_VAR_OBJ obj);
	
	~Trace();
	
	// Make a new instance from C++ land
	static V8_VAR_OBJ getNew(bool hit, Body *body, const btVector3 &pos, const btVector3 &norm);
	static V8_VAR_OBJ getNew(Scene *scene, const btVector3 &from, const btVector3 &to);
	
	void _destroy();
	
	
protected:
	
	Trace();
	Trace(Scene *scene, const btVector3 &from, const btVector3 &to);
	
	static V8_STORE_FT _protoTrace; // for inheritance
	static V8_STORE_FUNC _ctorTrace;
	
	bool _isDestroyed;
	
	void _rebuild();
	
	bool _cacheHit;
	Body *_cacheBody;
	btVector3 _cachePos;
	btVector3 _cacheNorm;
	
	
private:
	
	static NAN_METHOD(newCtor);
	static NAN_METHOD(destroy);
	static NAN_GETTER(isDestroyedGetter);
	
	static NAN_GETTER(hitGetter);
	static NAN_GETTER(bodyGetter);
	static NAN_GETTER(posGetter);
	static NAN_GETTER(normGetter);
	
};


#endif // _TRACE_HPP_
