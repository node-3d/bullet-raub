#include <cstdlib>

#include <btDynamicsWorld.h>
#include <btAabbUtil2.h>
#include <btRaycastCallback.h>

#include "scene.hpp"
#include "body.hpp"
#include "trace.hpp"


IMPLEMENT_ES5_CLASS(Trace);


void Body::init(Napi::Env env, Napi::Object exports) {
	
	Napi::Function ctor = wrap(env);
	
	JS_ASSIGN_METHOD(Body, destroy);
	
	JS_ASSIGN_GETTER(Body, isDestroyed);
	JS_ASSIGN_GETTER(Body, type);
	JS_ASSIGN_GETTER(Body, pos);
	JS_ASSIGN_GETTER(Body, rot);
	JS_ASSIGN_GETTER(Body, vell);
	JS_ASSIGN_GETTER(Body, vela);
	JS_ASSIGN_GETTER(Body, size);
	JS_ASSIGN_GETTER(Body, map);
	JS_ASSIGN_GETTER(Body, mesh);
	JS_ASSIGN_GETTER(Body, mass);
	JS_ASSIGN_GETTER(Body, rest);
	JS_ASSIGN_GETTER(Body, dampl);
	JS_ASSIGN_GETTER(Body, dampa);
	JS_ASSIGN_GETTER(Body, factl);
	JS_ASSIGN_GETTER(Body, facta);
	JS_ASSIGN_GETTER(Body, frict);
	JS_ASSIGN_GETTER(Body, sleepy);
	
	JS_ASSIGN_SETTER(Body, type);
	JS_ASSIGN_SETTER(Body, pos);
	JS_ASSIGN_SETTER(Body, rot);
	JS_ASSIGN_SETTER(Body, vell);
	JS_ASSIGN_SETTER(Body, vela);
	JS_ASSIGN_SETTER(Body, size);
	JS_ASSIGN_SETTER(Body, map);
	JS_ASSIGN_SETTER(Body, mesh);
	JS_ASSIGN_SETTER(Body, mass);
	JS_ASSIGN_SETTER(Body, rest);
	JS_ASSIGN_SETTER(Body, dampl);
	JS_ASSIGN_SETTER(Body, dampa);
	JS_ASSIGN_SETTER(Body, factl);
	JS_ASSIGN_SETTER(Body, facta);
	JS_ASSIGN_SETTER(Body, frict);
	JS_ASSIGN_SETTER(Body, sleepy);
	
	exports.Set("Body", ctor);
	
}


Body::Body(const Napi::CallbackInfo &info):
Common(info.This(), "Body") { NAPI_ENV;
	
	super(info);
	
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
		
		Napi::Object owner = Napi::Object::Cast(ownerVal);
		Scene *scene = ObjectWrap::Unwrap<Scene>(owner);
		
		if ( ! (opts->Has(JS_STR("from")) && opts->Has(JS_STR("to")))) {
			return Nan::ThrowTypeError("Missing 'opts.from' or 'opts.to' argument.");
		}
		
		V8_VAR_VAL fromVal = opts->Get(JS_STR("from"));
		V8_VAR_VAL toVal = opts->Get(JS_STR("to"));
		
		if ( ! (fromVal->IsObject() && toVal->IsObject()) ) {
			return Nan::ThrowTypeError("Type of 'opts.from' and 'opts.to' must be 'object'.");
		}
		
		Napi::Object fromObj = Napi::Object::Cast(fromVal);
		Napi::Object toObj = Napi::Object::Cast(toVal);
		
		OBJ_TO_VEC3(fromObj, from);
		OBJ_TO_VEC3(toObj, to);
		
		traceResult = new Trace(scene, from, to);
		
	} else {
		
		traceResult = new Trace();
		
	}
	
}



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


// ------ Methods and props


Napi::Object Trace::getNew(bool hit, Body *body, const btVector3 &pos, const btVector3 &norm) {
	
	V8_VAR_FUNC ctor = Nan::New(_ctorTrace);
	
	Napi::Object traceObj = Nan::NewInstance(ctor).ToLocalChecked();
	
	Trace *trace = ObjectWrap::Unwrap<Trace>(traceObj);
	_cacheHit = hit;
	_cacheBody = body;
	_cachePos = pos;
	_cacheNorm = norm;
	
	return traceObj;
	
}


Napi::Object Trace::getNew(Scene *scene, const btVector3 &from, const btVector3 &to) {
	
	V8_VAR_FUNC ctor = Nan::New(_ctorTrace);
	
	VEC3_TO_OBJ(from, fromObj);
	VEC3_TO_OBJ(to, toObj);
	
	Napi::Object args = Nan::New<Object>();
	SET_PROP(args, "scene", scene->handle());
	SET_PROP(args, "from", fromObj);
	SET_PROP(args, "to", toObj);
	
	V8_VAR_VAL argsVal = args;
	
	return Nan::NewInstance(ctor, 1, &argsVal).ToLocalChecked();
	
}


V3_GETTER(pos, _cachePos);
V3_GETTER(norm, _cacheNorm);

JS_IMPLEMENT_GETTER(Trace, bodyGetter) { THIS_TRACE;
	
	if (_cacheBody) {
		RET_VALUE(_cacheBody->handle());
	} else {
		RET_VALUE(Nan::Null());
	}
	
}

JS_IMPLEMENT_GETTER(Trace, hitGetter) { THIS_TRACE;
	
	RET_VALUE(JS_BOOL(_cacheHit));
	
}


// ------ System methods and props for ObjectWrap


void Trace::init(Napi::Object target) {
	
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



JS_IMPLEMENT_METHOD(Trace, newCtor) {
	
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
		
		Napi::Object owner = Napi::Object::Cast(ownerVal);
		Scene *scene = ObjectWrap::Unwrap<Scene>(owner);
		
		if ( ! (opts->Has(JS_STR("from")) && opts->Has(JS_STR("to")))) {
			return Nan::ThrowTypeError("Missing 'opts.from' or 'opts.to' argument.");
		}
		
		V8_VAR_VAL fromVal = opts->Get(JS_STR("from"));
		V8_VAR_VAL toVal = opts->Get(JS_STR("to"));
		
		if ( ! (fromVal->IsObject() && toVal->IsObject()) ) {
			return Nan::ThrowTypeError("Type of 'opts.from' and 'opts.to' must be 'object'.");
		}
		
		Napi::Object fromObj = Napi::Object::Cast(fromVal);
		Napi::Object toObj = Napi::Object::Cast(toVal);
		
		OBJ_TO_VEC3(fromObj, from);
		OBJ_TO_VEC3(toObj, to);
		
		traceResult = new Trace(scene, from, to);
		
	} else {
		
		traceResult = new Trace();
		
	}
	
	traceResult->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


JS_IMPLEMENT_METHOD(Trace, destroy) { THIS_TRACE; THIS_CHECK;
	
	emit("destroy");
	
	_destroy();
	
}


JS_IMPLEMENT_GETTER(Trace, isDestroyedGetter) { THIS_TRACE;
	
	RET_BOOL(_isDestroyed);
	
}
