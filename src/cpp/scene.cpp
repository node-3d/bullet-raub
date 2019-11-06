#include <btQuickprof.h>
#include <btDefaultCollisionConfiguration.h>
#include <btDbvtBroadphase.h>
#include <btSequentialImpulseConstraintSolver.h>
#include <btDiscreteDynamicsWorld.h>
#include <btConstraintSolver.h>
#include <btRaycastCallback.h>
#include <btSolverConstraint.h>
#include <btDynamicsWorld.h>
#include <btAabbUtil2.h>
#include <btAlignedObjectArray.h>

#include "body.hpp"
#include "scene.hpp"


IMPLEMENT_ES5_CLASS(Scene);


void Scene::init(Napi::Env env, Napi::Object exports) {
	
	Napi::Function ctor = wrap(env);
	
	JS_ASSIGN_METHOD(destroy);
	JS_ASSIGN_GETTER(isDestroyed);
	
	JS_ASSIGN_METHOD(update);
	JS_ASSIGN_METHOD(trace);
	JS_ASSIGN_METHOD(hit);
	
	JS_ASSIGN_SETTER(gravity);
	
	exports.Set("Scene", ctor);
	
}


Scene::Scene(const Napi::CallbackInfo &info):
Common(info.This(), "Scene") { NAPI_ENV;
	
	super(info);
	
	_clock = ALIGNED_NEW(btClock);
	_clock->reset();
	
	_physConfig = ALIGNED_NEW(btDefaultCollisionConfiguration);
	_physDispatcher = ALIGNED_NEW(btCollisionDispatcher, _physConfig);
	_physBroadphase = ALIGNED_NEW(btDbvtBroadphase);
	_physSolver = ALIGNED_NEW(btSequentialImpulseConstraintSolver);
	_physWorld = ALIGNED_NEW(
		btDiscreteDynamicsWorld,
		_physDispatcher,
		_physBroadphase,
		_physSolver,
		_physConfig
	);
	
	_cacheGrav.setValue(0, -10, 0);
	_physWorld->setGravity(_cacheGrav);
	
}


Scene::~Scene() {
	_destroy();
}


void Scene::_destroy() { DES_CHECK;
	
	emit("destroy");
	
	EACH(_bodies) {
		Body *b = _bodies[i];
		b->_destroy();
	}
	
	_bodies.clear();
	
	ALIGNED_DELETE(btDiscreteDynamicsWorld, _physWorld);
	
	ALIGNED_DELETE(btSequentialImpulseConstraintSolver, _physSolver);
	
	ALIGNED_DELETE(btDbvtBroadphase, _physBroadphase);
	
	ALIGNED_DELETE(btCollisionDispatcher, _physDispatcher);
	
	ALIGNED_DELETE(btDefaultCollisionConfiguration, _physConfig);
	
	Common::_destroy();
	
}


inline Napi::Object fillTraceObject(
	Napi::Env env,
	const btVector3 &cppPos,
	const btVector3 &cppNorm,
	const btCollisionObject *m_collisionObject
) {
	
	Napi::Object trace = Napi::Object::New(env);
	
	trace.Set("hit", true);
	
	Body *b = reinterpret_cast<Body *>(m_collisionObject->getUserPointer());
	trace.Set("body", b->asJsObject());
	
	VEC3_TO_OBJ(cppPos, pos);
	trace.Set("pos", pos);
	VEC3_TO_OBJ(cppNorm, norm);
	trace.Set("norm", norm);
	
	return trace;
	
}

inline Napi::Object fillTraceObject(Napi::Env env) {
	
	Napi::Object trace = Napi::Object::New(env);
	
	trace.Set("hit", false);
	trace.Set("body", env.Null());
	
	btVector3 cppPos = btVector3(0, 0, 0);
	btVector3 cppNorm = btVector3(0, 1, 0);
	
	VEC3_TO_OBJ(cppPos, pos);
	trace.Set("pos", pos);
	VEC3_TO_OBJ(cppNorm, norm);
	trace.Set("norm", norm);
	
	return trace;
	
}

// ------ Methods and props

void Scene::refBody(Body *body) { DES_CHECK;
	
	_bodies.push_back(body);
	
}


void Scene::unrefBody(Body* body) { DES_CHECK;
	
	_bodies.remove(body);
	
}


void Scene::doUpdate(float dt) { DES_CHECK;
	
	_physWorld->stepSimulation(dt, 10, 1.f / 120.f);
	
	EACH(_bodies) {
		_bodies[i]->__update();
	}
	
}


void Scene::doUpdate() { DES_CHECK;
	
	btScalar dt = static_cast<btScalar>(_clock->getTimeMicroseconds())* 0.000001f;
	_clock->reset();
	
	doUpdate(dt);
	
}

V3_GETTER(Scene, gravity, _cacheGrav);


JS_IMPLEMENT_SETTER(Scene, gravity) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheGrav, v);
	
	_physWorld->setGravity(_cacheGrav);
	
	emit("gravity", 1, &value);
	
}


JS_IMPLEMENT_METHOD(Scene, update) { THIS_CHECK;
	
	LET_FLOAT_ARG(0, dt);
	
	if (dt > 0.f) {
		doUpdate(dt);
	} else {
		doUpdate();
	}
	
	RET_UNDEFINED;
	
}


JS_IMPLEMENT_METHOD(Scene, hit) { THIS_CHECK;
	
	REQ_VEC3_ARG(0, f);
	REQ_VEC3_ARG(1, t);
	
	btCollisionWorld::ClosestRayResultCallback closestResults(f, t);
	_physWorld->rayTest(f, t, closestResults);
	
	Napi::Object trace;
	if (closestResults.hasHit()) {
		trace = fillTraceObject(
			env,
			f.lerp(t, closestResults.m_closestHitFraction),
			closestResults.m_hitNormalWorld,
			closestResults.m_collisionObject
		);
	} else {
		trace = fillTraceObject(env);
	}
	
	RET_VALUE(trace);
	
}


JS_IMPLEMENT_METHOD(Scene, trace) { THIS_CHECK;
	
	REQ_VEC3_ARG(0, f);
	REQ_VEC3_ARG(1, t);
	
	btCollisionWorld::AllHitsRayResultCallback allResults(f, t);
	_physWorld->rayTest(f, t, allResults);
	
	ObjVec traceList = ObjVec(allResults.m_collisionObjects.size());
	
	EACH(allResults.m_collisionObjects) {
		traceList[i] = fillTraceObject(
			env,
			allResults.m_hitPointWorld[i],
			allResults.m_hitNormalWorld[i],
			allResults.m_collisionObjects[i]
		);
	}
	
	int size = traceList.size();
	
	Napi::Array result = Napi::Array::New(env);
	
	for (int i = 0; i < size; i++) {
		result.Set(i, traceList[i]);
	}
	
	RET_VALUE(result);
	
}


JS_IMPLEMENT_METHOD(Scene, destroy) { THIS_CHECK;
	emit("destroy");
	_destroy();
	RET_UNDEFINED;
}


JS_IMPLEMENT_GETTER(Scene, isDestroyed) { THIS_CHECK;
	RET_BOOL(_isDestroyed);
}
