#include <cstdlib>
#include <iostream>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
#include <BulletDynamics/ConstraintSolver/btSolverConstraint.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <LinearMath/btAabbUtil2.h>
#include <LinearMath/btAlignedObjectArray.h>

#include "body.hpp"
#include "trace.hpp"
#include "scene.hpp"

using namespace v8;
using namespace node;
using namespace std;

#define THIS_SCENE                                                 \
	Scene *scene = ObjectWrap::Unwrap<Scene>(info.This());

#define V3_GETTER(NAME, CACHE)                                     \
	NAN_GETTER(Scene::NAME ## Getter) { THIS_SCENE;        \
		VEC3_TO_OBJ(scene->CACHE, NAME);                           \
		RET_VALUE(NAME);                                           \
	}

#define CACHE_CAS(CACHE, V)                                        \
	if (scene->CACHE == V) {                                       \
		return;                                                    \
	}                                                              \
	scene->CACHE = V;


vector<Scene*> Scene::_scenes;
Persistent<Function> Scene::_constructor;


void Scene::init(Handle<Object> target) {
	
	Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(newCtor);
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(JS_STR("Scene"));
	
	// prototype
	Nan::SetPrototypeMethod(ctor, "update", update);
	Nan::SetPrototypeMethod(ctor, "trace", trace);
	Nan::SetPrototypeMethod(ctor, "hit", hit);
	
	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
	ACCESSOR_RW(proto, gravity);
	
	Nan::Set(target, JS_STR("Image"), ctor->GetFunction());
	
	_constructor.Reset(Isolate::GetCurrent(), ctor->GetFunction());
	
}

void Scene::deinit() {
	
	vector<Scene*>::iterator it = _scenes.begin();
	
	while (it != _scenes.end()) {
		delete (*it);
		it++;
	}
	
	_scenes.clear();
	
}


void Scene::remember(Scene *scene) {
	_scenes.push_back(scene);
}

void Scene::forget(Scene* scene) {
	
	vector<Scene*>::iterator it = _scenes.begin();
	
	while (it != _scenes.end()) {
		
		if (*it == scene) {
			_scenes.erase(it);
			break;
		}
		
		it++;
		
	}
	
}


NAN_METHOD(Scene::newCtor) {
	
	Scene *scene = new Scene();
	scene->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


Scene::Scene() {
	
	_clock = new btClock();
	_clock->reset();
	
	_physConfig = new btDefaultCollisionConfiguration();
	_physDispatcher = new btCollisionDispatcher(_physConfig);
	_physBroadphase = new btDbvtBroadphase();
	_physSolver = new btSequentialImpulseConstraintSolver();
	_physWorld = new btDiscreteDynamicsWorld(_physDispatcher, _physBroadphase, _physSolver, _physConfig);
	
	_cacheGrav.setValue(0, -10, 0);
	_physWorld->setGravity(_cacheGrav);
	
	remember(this);
	
	// A SUDDEN STATIC PLANE
	// btStaticPlaneShape *cshape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 0);
	// btVector3 localInertia(0,0,0);
	// btDefaultMotionState* myMotionState = new btDefaultMotionState();
	// btRigidBody::btRigidBodyConstructionInfo rbInfo(0, myMotionState, cshape, localInertia);
	// btRigidBody *body = new btRigidBody(rbInfo);
	// _physWorld->addRigidBody(body);
	
}


Scene::~Scene() {
	
	vector<Body*>::iterator it = _bodies.begin();
	while (it != _bodies.end()) {
		delete (*it);
		it++;
	}
	_bodies.clear();
	
	forget(this);
	
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
	
}


void Scene::refBody(Body *body) {
	_bodies.push_back(body);
}

void Scene::unrefBody(Body* body) {
	
	vector<Body*>::iterator it = _bodies.begin();
	
	while (it != _bodies.end()) {
		
		if (*it == body) {
			_bodies.erase(it);
			break;
		}
		
		it++;
		
	}
	
}


void Scene::doUpdate(float dt) {
	
	_physWorld->stepSimulation(dt * 0.5f, 10, 1.f / 120.f);
	
	vector<Body*>::iterator it = _bodies.begin();
	while (it != _bodies.end()) {
		(*it)->__update();
		it++;
	}
	
	
}


void Scene::doUpdate() {
	
	btScalar dt = static_cast<btScalar>(_clock->getTimeMicroseconds())* 0.000001f;
	_clock->reset();
	
	doUpdate(dt);
	
}


const vector< Local<Value> > &Scene::doTrace(const btVector3 &from, const btVector3 &to) {
	
	btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
	_physWorld->rayTest(from, to, allResults);
	
	vector< Local<Value> > list;
	
	for (int i = 0; i < allResults.m_collisionObjects.size(); i++) {
		
		Body *b = reinterpret_cast<Body *>(allResults.m_collisionObjects[i]->getUserPointer());
		
		list.push_back(Trace::instance(
			true, b,
			allResults.m_hitPointWorld[i],
			allResults.m_hitNormalWorld[i]
		));
		
	}
	
	return list;
	
}


V3_GETTER(gravity, _cacheGrav);

NAN_SETTER(Scene::gravitySetter) { THIS_SCENE; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheGrav, v);
	
	scene->_physWorld->setGravity(scene->_cacheGrav);
	
	// EMIT
	
}


NAN_METHOD(Scene::update) { THIS_SCENE;
	
	LET_FLOAT_ARG(0, dt);
	
	if (dt > 0.f) {
		scene->doUpdate(dt);
	} else {
		scene->doUpdate();
	}
	
}


NAN_METHOD(Scene::hit) { THIS_SCENE;
	
	REQ_VEC3_ARG(0, f);
	REQ_VEC3_ARG(1, t);
	
	RET_VALUE(Trace::instance(scene, f, t));
	
}


NAN_METHOD(Scene::trace) { THIS_SCENE;
	
	REQ_VEC3_ARG(0, f);
	REQ_VEC3_ARG(1, t);
	
	const vector< Local<Value> > &traceList = scene->doTrace(f, t);
	int size = traceList.size();
	
	Local<Array> result = Nan::New<Array>(size);
	
	for (int i = 0; i < size; i++) {
		SET_I(result, i, traceList[i]);
	}
	
	RET_VALUE(result);
	
}
