#include <cstdlib>
#include <iostream>

#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <LinearMath/btAabbUtil2.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

#include "scene.hpp"
#include "body.hpp"
#include "trace.hpp"

using namespace v8;
using namespace node;
using namespace std;

#define THIS_TRACE                                                 \
	Trace *trace = ObjectWrap::Unwrap<Trace>(info.This());

#define V3_GETTER(NAME, CACHE)                                     \
	NAN_GETTER(Trace::NAME ## Getter) { THIS_TRACE;                \
		VEC3_TO_OBJ(trace->CACHE, NAME);                           \
		RET_VALUE(NAME);                                           \
	}

#define CACHE_CAS(CACHE, V)                                        \
	if (trace->CACHE == V) {                                       \
		return;                                                    \
	}                                                              \
	trace->CACHE = V;


Nan::Persistent<v8::Function> Trace::_constructor;


void Trace::init(Handle<Object> target) {
	
	Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(newCtor);
	
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(JS_STR("Trace"));
	
	// prototype
	Nan::SetPrototypeMethod(ctor, "toString", toString);
	
	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
	ACCESSOR_R(proto, hit);
	ACCESSOR_R(proto, body);
	ACCESSOR_R(proto, pos);
	ACCESSOR_R(proto, norm);
	
	_constructor.Reset(Nan::GetFunction(ctor).ToLocalChecked());
	Nan::Set(target, JS_STR("Trace"), Nan::GetFunction(ctor).ToLocalChecked());
	
}


Local<Object> Trace::instance(bool hit, Body *body, const btVector3 &pos, const btVector3 &norm) {
	
	Local<Function> cons = Nan::New(_constructor);
	
	VEC3_TO_OBJ(pos, p);
	VEC3_TO_OBJ(norm, n);
	
	const int argc = 4;
	Local<Value> argv[argc] = { JS_BOOL(hit), Nan::New<External>(body), p, n };
	
	Local<Object> inst = cons->NewInstance(argc, argv);
	
	return inst;
	
}


Local<Object> Trace::instance(Scene *scene, const btVector3 &from, const btVector3 &to) {
	
	Trace helper(scene, from, to);
	
	return instance(helper._cacheHit, helper._cacheBody, helper._cachePos, helper._cacheNorm);
	
}


NAN_METHOD(Trace::newCtor) {
	
	CTOR_CHECK("Trace");
	
	Trace *traceResult = NULL;
	
	if (info[0]->IsObject()) {
		
		REQ_OBJ_ARG(0, owner);
		Scene *scene = ObjectWrap::Unwrap<Scene>(owner);
		REQ_VEC3_ARG(1, f);
		REQ_VEC3_ARG(2, t);
		
		traceResult = new Trace(scene, f, t);
		
	} else {
		
		REQ_BOOL_ARG(0, hit);
		REQ_EXT_ARG(1, body);
		REQ_VEC3_ARG(2, pos);
		REQ_VEC3_ARG(3, norm);
		
		Body *external = reinterpret_cast<Body*>(body->Value());
		
		traceResult = new Trace(hit, external, pos, norm);
		
	}
	
	traceResult->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


Trace::Trace(bool hit, Body *body, const btVector3 &pos, const btVector3 &norm) {
	
	_cacheHit = hit;
	_cacheBody = body;
	_cachePos = pos;
	_cacheNorm = norm;
	
}


Trace::Trace(Scene *scene, const btVector3 &from, const btVector3 &to) {
	
	btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
	scene->getWorld()->rayTest(from, to, closestResults);
	
	if (closestResults.hasHit()) {
		_cacheHit = true;
		_cacheBody = reinterpret_cast<Body *>(closestResults.m_collisionObject->getUserPointer());
		_cachePos = from.lerp(to, closestResults.m_closestHitFraction);
		_cacheNorm = closestResults.m_hitNormalWorld;
	} else {
		_cacheHit = false;
		_cacheBody = NULL;
		_cachePos = btVector3(0, 0, 0);
		_cacheNorm = btVector3(0, 1, 0);
	}
	
}


Trace::~Trace() {
	
}


NAN_METHOD(Trace::toString) { THIS_TRACE;
	
	RET_VALUE(JS_STR("oh hai!"));
	
}


V3_GETTER(pos, _cachePos);
V3_GETTER(norm, _cacheNorm);

NAN_GETTER(Trace::bodyGetter) { THIS_TRACE;
	
	Local<Object> obj = Nan::New<Object>();
	
	RET_VALUE(obj);
	
}

NAN_GETTER(Trace::hitGetter) { THIS_TRACE;
	
	RET_VALUE(JS_BOOL(trace->_cacheHit));
	
}
