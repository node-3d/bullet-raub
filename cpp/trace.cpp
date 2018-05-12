#include <cstdlib>

#include <btDynamicsWorld.h>
#include <btAabbUtil2.h>
#include <btRaycastCallback.h>

#include "scene.hpp"
#include "body.hpp"
#include "trace.hpp"

using namespace v8;
using namespace node;
using namespace std;


// ------ Aux macros

#define THIS_TRACE                                                            \
	Trace *trace = ObjectWrap::Unwrap<Trace>(info.This());

#define THIS_CHECK                                                            \
	if (trace->_isDestroyed) return;

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

Trace::Trace() {
	
	_cacheHit = false;
	_cacheBody = nullptr;
	_cachePos = btVector3();
	_cacheNorm = btVector3();
	
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
		_cacheBody = nullptr;
		_cachePos = btVector3(0, 0, 0);
		_cacheNorm = btVector3(0, 1, 0);
	}
	
}


Trace::~Trace() {
	
	_destroy();
	
}


void Trace::_destroy() { DES_CHECK;
	
	_isDestroyed = true;
	
}


// ------ Methods and props


V8_VAR_OBJ Trace::getNew(bool hit, Body *body, const btVector3 &pos, const btVector3 &norm) {
	
	V8_VAR_FUNC ctor = Nan::New(_ctorTrace);
	
	V8_VAR_OBJ traceObj = Nan::NewInstance(ctor).ToLocalChecked();
	
	Trace *trace = ObjectWrap::Unwrap<Trace>(traceObj);
	trace->_cacheHit = hit;
	trace->_cacheBody = body;
	trace->_cachePos = pos;
	trace->_cacheNorm = norm;
	
	return traceObj;
	
}


V8_VAR_OBJ Trace::getNew(Scene *scene, const btVector3 &from, const btVector3 &to) {
	
	V8_VAR_FUNC ctor = Nan::New(_ctorTrace);
	
	VEC3_TO_OBJ(from, fromObj);
	VEC3_TO_OBJ(to, toObj);
	
	V8_VAR_OBJ args = Nan::New<Object>();
	SET_PROP(args, "scene", scene->handle());
	SET_PROP(args, "from", fromObj);
	SET_PROP(args, "to", toObj);
	
	V8_VAR_VAL argsVal = args;
	
	return Nan::NewInstance(ctor, 1, &argsVal).ToLocalChecked();
	
}


V3_GETTER(pos, _cachePos);
V3_GETTER(norm, _cacheNorm);

NAN_GETTER(Trace::bodyGetter) { THIS_TRACE;
	
	if (trace->_cacheBody) {
		RET_VALUE(trace->_cacheBody->handle());
	} else {
		RET_VALUE(Nan::Null());
	}
	
}

NAN_GETTER(Trace::hitGetter) { THIS_TRACE;
	
	RET_VALUE(JS_BOOL(trace->_cacheHit));
	
}


// ------ System methods and props for ObjectWrap

V8_STORE_FT Trace::_protoTrace;
V8_STORE_FUNC Trace::_ctorTrace;


void Trace::init(V8_VAR_OBJ target) {
	
	V8_VAR_FT proto = Nan::New<FunctionTemplate>(newCtor);
	
	// class Trace inherits EventEmitter
	V8_VAR_FT parent = Nan::New(EventEmitter::_protoEventEmitter);
	proto->Inherit(parent);
	
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
	
	Trace *traceResult = nullptr;
	
	if (info.Length() > 0) {
		
		REQ_OBJ_ARG(0, opts);
		
		if ( ! opts->Has(JS_STR("scene")) ) {
			return Nan::ThrowTypeError("Missing 'opts.scene' argument.");
		}
		
		V8_VAR_VAL ownerVal = opts->Get(JS_STR("scene"));
		
		if ( ! ownerVal->IsObject() ) {
			return Nan::ThrowTypeError("Type of 'opts.scene' must be 'object'.");
		}
		
		V8_VAR_OBJ owner = V8_VAR_OBJ::Cast(ownerVal);
		Scene *scene = ObjectWrap::Unwrap<Scene>(owner);
		
		if ( ! (opts->Has(JS_STR("from")) && opts->Has(JS_STR("to")))) {
			return Nan::ThrowTypeError("Missing 'opts.from' or 'opts.to' argument.");
		}
		
		V8_VAR_VAL fromVal = opts->Get(JS_STR("from"));
		V8_VAR_VAL toVal = opts->Get(JS_STR("to"));
		
		if ( ! (fromVal->IsObject() && toVal->IsObject()) ) {
			return Nan::ThrowTypeError("Type of 'opts.from' and 'opts.to' must be 'object'.");
		}
		
		V8_VAR_OBJ fromObj = V8_VAR_OBJ::Cast(fromVal);
		V8_VAR_OBJ toObj = V8_VAR_OBJ::Cast(toVal);
		
		OBJ_TO_VEC3(fromObj, from);
		OBJ_TO_VEC3(toObj, to);
		
		traceResult = ALIGNED_NEW(Trace, scene, from, to);
		
	} else {
		
		traceResult = ALIGNED_NEW(Trace);
		
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

