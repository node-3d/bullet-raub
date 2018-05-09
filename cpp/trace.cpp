#include <cstdlib>

#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <LinearMath/btAabbUtil2.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

#include "scene.hpp"
#include "body.hpp"
#include "trace.hpp"

using namespace v8;
using namespace node;
using namespace std;


// ------ Aux macros

#define THIS_TRACE                                                            \
	Trace *trace = ObjectWrap::Unwrap<Trace>(info.This());

#define V3_GETTER(NAME, CACHE)                                                \
	NAN_GETTER(Trace::NAME ## Getter) { THIS_TRACE;                           \
		VEC3_TO_OBJ(trace->CACHE, NAME);                                      \
		RET_VALUE(NAME);                                                      \
	}

#define CACHE_CAS(CACHE, V)                                                   \
	if (trace->CACHE == V) {                                                  \
		return;                                                               \
	}                                                                         \
	trace->CACHE = V;


// ------ Constructor and Destructor

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
	
	_destroy();
	
}


void Trace::_destroy() { DES_CHECK;
	
}


// ------ Methods and props


V8_VAR_OBJ Trace::instance(bool hit, Body *body, const btVector3 &pos, const btVector3 &norm) {
	
	Local<Function> cons = Nan::New(_constructor);
	
	VEC3_TO_OBJ(pos, p);
	VEC3_TO_OBJ(norm, n);
	
	const int argc = 4;
	Local<Value> argv[argc] = { JS_BOOL(hit), Nan::New<External>(body), p, n };
	
	V8_VAR_OBJ inst = cons->NewInstance(argc, argv);
	// Nan::NewInstance(cons, argc, argv).ToLocalChecked();
	
	return inst;
	
}


V8_VAR_OBJ Trace::instance(Scene *scene, const btVector3 &from, const btVector3 &to) {
	
	Trace helper(scene, from, to);
	
	return instance(helper._cacheHit, helper._cacheBody, helper._cachePos, helper._cacheNorm);
	
}


V3_GETTER(pos, _cachePos);
V3_GETTER(norm, _cacheNorm);

NAN_GETTER(Trace::bodyGetter) { THIS_TRACE;
	
	if (trace->_cacheBody) {
		RET_VALUE(Nan::New(trace->_cacheBody->getJsWrapper()));
	} else {
		RET_VALUE(Nan::Null());
	}
	
}

NAN_GETTER(Trace::hitGetter) { THIS_TRACE;
	
	RET_VALUE(JS_BOOL(Trace->_cacheHit));
	
}


// ------ System methods and props for ObjectWrap

V8_STORE_FT Trace::_protoTrace;
V8_STORE_FUNC Trace::_ctorTrace;


void Trace::init(V8_VAR_OBJ target) {
	
	V8_VAR_FT proto = Nan::New<FunctionTemplate>(newCtor);
	
	proto->InstanceTemplate()->SetInternalFieldCount(1);
	proto->SetClassName(JS_STR("Trace"));
	
	
	// Accessors
	
	V8_VAR_OT obj = proto->PrototypeTemplate();
	
	ACCESSOR_R(obj, isDestroyed);
	
	ACCESSOR_R(obj, hit);
	ACCESSOR_R(obj, body);
	ACCESSOR_R(obj, pos);
	ACCESSOR_R(obj, norm);
	
	// -------- dynamic
	
	Nan::SetPrototypeMethod(proto, "destroy", destroy);
	
	
	// -------- static
	
	V8_VAR_FUNC ctor = Nan::GetFunction(proto).ToLocalChecked();
	
	_protoTrace.Reset(proto);
	_ctorTrace.Reset(ctor);
	
	Nan::Set(target, JS_STR("Trace"), ctor);
	
}


bool Trace::isTrace(V8_VAR_OBJ obj) {
	return Nan::New(_protoTrace)->HasInstance(obj);
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
		LET_EXT_ARG(1, body);
		REQ_VEC3_ARG(2, pos);
		REQ_VEC3_ARG(3, norm);
		
		Body *external = reinterpret_cast<Body*>(body->Value());
		
		traceResult = new Trace(hit, external, pos, norm);
		
	}
	
	traceResult->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


NAN_METHOD(Trace::destroy) { THIS_TRACE; THIS_CHECK;
	
	trace->emit("destroy");
	
	trace->_destroy();
	
}


NAN_GETTER(Trace::isDestroyedGetter) { THIS_TRACE;
	
	RET_VALUE(JS_BOOL(trace->_isDestroyed));
	
}

