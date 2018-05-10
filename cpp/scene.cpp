#include <cstdlib>

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
#include "trace.hpp"
#include "scene.hpp"

using namespace v8;
using namespace node;
using namespace std;


// ------ Aux macros

#define THIS_SCENE                                                            \
	Scene *scene = ObjectWrap::Unwrap<Scene>(info.This());

#define THIS_CHECK                                                            \
	if (scene->_isDestroyed) return;

#define V3_GETTER(NAME, CACHE)                                                \
	NAN_GETTER(Scene::NAME ## Getter) { THIS_SCENE; THIS_CHECK;               \
		VEC3_TO_OBJ(scene->CACHE, NAME);                                      \
		RET_VALUE(NAME);                                                      \
	}

#define CACHE_CAS(CACHE, V)                                                   \
	if (scene->CACHE == V) {                                                  \
		return;                                                               \
	}                                                                         \
	scene->CACHE = V;


// ------ Constructor and Destructor

vector<Scene*> Scene::_scenes;


Scene::Scene() {
	
	_isDestroyed = false;
	
	_clock = new btClock();
	_clock->reset();
	
	_physConfig = new btDefaultCollisionConfiguration();
	_physDispatcher = new btCollisionDispatcher(_physConfig);
	_physBroadphase = new btDbvtBroadphase();
	_physSolver = new btSequentialImpulseConstraintSolver();
	_physWorld = new btDiscreteDynamicsWorld(_physDispatcher, _physBroadphase, _physSolver, _physConfig);
	
	_cacheGrav.setValue(0, -10, 0);
	_physWorld->setGravity(_cacheGrav);
	
}


Scene::~Scene() {
	
	_destroy();
	
}


void Scene::_destroy() { DES_CHECK;
	
	vector<Body*>::iterator it = _bodies.begin();
	while (it != _bodies.end()) {
		delete (*it);
		it++;
	}
	_bodies.clear();
	
	delete _physWorld;
	_physWorld = NULL;
	
	delete _physSolver;
	_physSolver = NULL;
	
	delete _physBroadphase;
	_physBroadphase = NULL;
	
	delete _physDispatcher;
	_physDispatcher = NULL;
	
	delete _physConfig;
	_physConfig = NULL;
	
	_isDestroyed = true;
	
}


// ------ Methods and props

void Scene::refBody(Body *body) { DES_CHECK;
	_bodies.push_back(body);
}


void Scene::unrefBody(Body* body) { DES_CHECK;
	
	vector<Body*>::iterator it = _bodies.begin();
	
	while (it != _bodies.end()) {
		
		if (*it == body) {
			_bodies.erase(it);
			break;
		}
		
		it++;
		
	}
	
}


void Scene::doUpdate(float dt) { DES_CHECK;
	
	_physWorld->stepSimulation(dt, 10, 1.f / 120.f);
	
	vector<Body*>::iterator it = _bodies.begin();
	while (it != _bodies.end()) {
		(*it)->__update();
		consoleLog("i++");
		it++;
	}
	
}


void Scene::doUpdate() { DES_CHECK;
	
	btScalar dt = static_cast<btScalar>(_clock->getTimeMicroseconds())* 0.000001f;
	_clock->reset();
	
	doUpdate(dt);
	
}


vector< V8_VAR_OBJ > Scene::doTrace(const btVector3 &from, const btVector3 &to) {
	
	btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
	_physWorld->rayTest(from, to, allResults);
	
	vector< V8_VAR_OBJ > list;
	
	for (int i = 0; i < allResults.m_collisionObjects.size(); i++) {
		
		Body *b = reinterpret_cast<Body *>(allResults.m_collisionObjects[i]->getUserPointer());
		
		list.push_back(Trace::getNew(
			true, b,
			allResults.m_hitPointWorld[i],
			allResults.m_hitNormalWorld[i]
		));
		
	}
	
	return list;
	
}


V3_GETTER(gravity, _cacheGrav);

NAN_SETTER(Scene::gravitySetter) { THIS_SCENE; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheGrav, v);
	
	scene->_physWorld->setGravity(scene->_cacheGrav);
	
	// Emit "gravity"
	scene->emit("gravity", 1, &value);
	
}


NAN_METHOD(Scene::update) { THIS_SCENE; THIS_CHECK;
	consoleLog("++ U0");
	
	try {
	LET_FLOAT_ARG(0, dt);
	
	if (dt > 0.f) {
		scene->doUpdate(dt);
	} else {
		scene->doUpdate();
	}
} catch (...) {
	consoleLog(">>>>> BLEAT");
}
	consoleLog("-- U1");
}


NAN_METHOD(Scene::hit) { THIS_SCENE; THIS_CHECK;
	
	REQ_VEC3_ARG(0, f);
	REQ_VEC3_ARG(1, t);
	
	V8_VAR_OBJ trace = Trace::getNew(scene, f, t);
	
	RET_VALUE(trace);
}


NAN_METHOD(Scene::trace) { THIS_SCENE; THIS_CHECK;
	
	REQ_VEC3_ARG(0, f);
	REQ_VEC3_ARG(1, t);
	
	const vector< V8_VAR_OBJ > &traceList = scene->doTrace(f, t);
	int size = traceList.size();
	
	V8_VAR_ARR result = Nan::New<Array>(size);
	
	for (int i = 0; i < size; i++) {
		SET_I(result, i, traceList[i]);
	}
	
	RET_VALUE(result);
	
}



// ------ System methods and props for ObjectWrap

V8_STORE_FT Scene::_protoScene;
V8_STORE_FUNC Scene::_ctorScene;


void Scene::init(V8_VAR_OBJ target) {
	
	V8_VAR_FT proto = Nan::New<FunctionTemplate>(newCtor);
	
	// class Scene inherits EventEmitter
	V8_VAR_FT parent = Nan::New(EventEmitter::_protoEventEmitter);
	proto->Inherit(parent);
	
	proto->InstanceTemplate()->SetInternalFieldCount(1);
	proto->SetClassName(JS_STR("Scene"));
	
	
	// Accessors
	
	V8_VAR_OT obj = proto->PrototypeTemplate();
	
	ACCESSOR_R(obj, isDestroyed);
	
	ACCESSOR_RW(obj, gravity);
	
	
	// -------- dynamic
	
	Nan::SetPrototypeMethod(proto, "destroy", destroy);
	
	Nan::SetPrototypeMethod(proto, "update", update);
	Nan::SetPrototypeMethod(proto, "trace", trace);
	Nan::SetPrototypeMethod(proto, "hit", hit);
	Nan::SetPrototypeMethod(proto, "destroy", destroy);
	
	
	// -------- static
	
	V8_VAR_FUNC ctor = Nan::GetFunction(proto).ToLocalChecked();
	
	_protoScene.Reset(proto);
	_ctorScene.Reset(ctor);
	
	Nan::Set(target, JS_STR("Scene"), ctor);
	
}


bool Scene::isScene(V8_VAR_OBJ obj) {
	return Nan::New(_protoScene)->HasInstance(obj);
}


NAN_METHOD(Scene::newCtor) {
	
	CTOR_CHECK("Scene");
	
	Scene *scene = new Scene();
	scene->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


NAN_METHOD(Scene::destroy) { THIS_SCENE; THIS_CHECK;
	
	scene->emit("destroy");
	
	scene->_destroy();
	
}


NAN_GETTER(Scene::isDestroyedGetter) { THIS_SCENE;
	
	RET_VALUE(JS_BOOL(scene->_isDestroyed));
	
}

