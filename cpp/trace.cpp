#include <cstdlib>
#include <iostream>

#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <btBulletDynamicsCommon.h>
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
	NAN_GETTER(Trace::NAME ## Getter) { NAN_HS; THIS_TRACE;        \
		VEC3_TO_OBJ(trace->CACHE, NAME);                           \
		RET_VALUE(NAME);                                           \
	}

#define CACHE_CAS(CACHE, V)                                        \
	if (trace->CACHE == V) {                                       \
		return;                                                    \
	}                                                              \
	trace->CACHE = V;


Persistent<Function> Trace::_constructor;


void Trace::init(Handle<Object> target) { NAN_HS;
	
	Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(newCtor);
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(JS_STR("Trace"));
	
	// prototype
	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
	ACCESSOR_R(proto, hit);
	ACCESSOR_R(proto, body);
	ACCESSOR_R(proto, pos);
	ACCESSOR_R(proto, norm);
	
	Nan::Set(target, JS_STR("Trace"), ctor->GetFunction());
	
	_constructor.Reset(Isolate::GetCurrent(), ctor->GetFunction());
	
}


NAN_METHOD(Trace::newCtor) { NAN_HS;
	
	REQ_OBJ_ARG(0, owner);
	Scene *scene = ObjectWrap::Unwrap<Scene>(owner);
	REQ_VEC3_ARG(1, f);
	REQ_VEC3_ARG(2, t);
	
	Trace *traceResult = scene->doHit(f, t);
	
	Local<Object> result = Nan::New<Object>();
	traceResult->Wrap(result);
	
	RET_VALUE(result);
	
}


Trace::Trace(bool hit, Body *body, const btVector3 &pos, const btVector3 &norm) {
	
	_cacheHit = hit;
	_cacheBody = body;
	_cachePos = pos;
	_cacheNorm = norm;
	
}

Trace::~Trace() {
	
}


V3_GETTER(pos, _cachePos);
V3_GETTER(norm, _cacheNorm);
