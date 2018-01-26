#include <cstdlib>
#include <iostream>

#include <btDynamicsWorld.h>
#include <btRigidBody.h>
#include <btDefaultMotionState.h>

#include <btBoxShape.h>
#include <btCapsuleShape.h>
#include <btCollisionShape.h>
#include <btConvexTriangleMeshShape.h>
#include <btCylinderShape.h>
#include <btHeightfieldTerrainShape.h>
#include <btSphereShape.h>
#include <btStaticPlaneShape.h>

#include "scene.hpp"
#include "joint.hpp"
#include "body.hpp"

using namespace v8;
using namespace node;
using namespace std;

#define THIS_BODY                                                 \
	Body *body = ObjectWrap::Unwrap<Body>(info.This());

#define V3_GETTER(NAME, CACHE)                                    \
	NAN_GETTER(Body::NAME ## Getter) { THIS_BODY;                 \
		VEC3_TO_OBJ(body->CACHE, NAME);                           \
		RET_VALUE(NAME);                                          \
	}

#define CACHE_CAS(CACHE, V)                                       \
	if (body->CACHE == V) {                                       \
		return;                                                   \
	}                                                             \
	body->CACHE = V;




Nan::Persistent<v8::Function> Body::_constructor;


void Body::init(Handle<Object> target) {
	
	Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(newCtor);
	
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(JS_STR("Body"));
	
	// prototype
	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
	ACCESSOR_RW(proto, type);
	ACCESSOR_RW(proto, pos);
	ACCESSOR_RW(proto, rot);
	ACCESSOR_RW(proto, vell);
	ACCESSOR_RW(proto, vela);
	ACCESSOR_RW(proto, size);
	ACCESSOR_RW(proto, map);
	ACCESSOR_RW(proto, mesh);
	ACCESSOR_RW(proto, mass);
	ACCESSOR_RW(proto, rest);
	ACCESSOR_RW(proto, dampl);
	ACCESSOR_RW(proto, dampa);
	ACCESSOR_RW(proto, factl);
	ACCESSOR_RW(proto, facta);
	ACCESSOR_RW(proto, frict);
	ACCESSOR_RW(proto, sleepy);
	
	_constructor.Reset(Nan::GetFunction(ctor).ToLocalChecked());
	Nan::Set(target, JS_STR("Body"), Nan::GetFunction(ctor).ToLocalChecked());
	
}


Nan::Persistent<Object> &Body::getJsWrapper() {
	
	return _emitter;
	
}


void Body::_emit(int argc, Local<Value> argv[]) {
	
	if ( ! Nan::New(_emitter)->Has(JS_STR("emit")) ) {
		return;
	}
	
	Nan::Callback callback(Nan::New(_emitter)->Get(JS_STR("emit")).As<Function>());
	
	if ( ! callback.IsEmpty() ) {
		callback.Call(argc, argv);
	}
	
}


NAN_METHOD(Body::newCtor) {
	
	CTOR_CHECK("Body");
	
	REQ_OBJ_ARG(0, emitter);
	REQ_OBJ_ARG(1, owner);
	
	Scene *scene = ObjectWrap::Unwrap<Scene>(owner);
	Body *body = new Body(scene);
	body->_emitter.Reset(emitter);
	
	body->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


Body::Body(Scene *scene) {
	
	_scene = scene;
	
	_cshape = NULL;
	_body = NULL;
	
	_cacheType = "box";
	_cachePos = btVector3(0, 0, 0);
	_cacheRot = btQuaternion(0, 0, 0, 1);
	_cacheSize = btVector3(1, 1, 1);
	_cacheVell = btVector3(0, 0, 0);
	_cacheVela = btVector3(0, 0, 0);
	_cacheMass = 0.0f;
	_cacheRest = 0.0f;
	_cacheDampl = 0.1f;
	_cacheDampa = 0.1f;
	_cacheFactl = btVector3(1, 1, 1);
	_cacheFacta = btVector3(1, 1, 1);
	_cacheFrict = 0.5f;
	_cacheSleepy = true;
	_cacheMap = NULL;
	_cacheMesh = NULL;
	
	_rebuild();
	
	_scene->refBody(this);
	
}


Body::~Body() {
	
	vector<Joint*>::iterator it = _joints.begin();
	while (it != _joints.end()) {
		(*it)->_dropBody(this);
		it++;
	}
	_joints.clear();
	
	_scene->unrefBody(this);
	
	btMotionState *motion = _body->getMotionState();
	if (motion) {
		delete motion;
	}
	_scene->getWorld()->removeCollisionObject(_body);
	
	delete _body;
	_body = NULL;
	
	delete _cshape;
	_cshape = NULL;
	
}


void Body::refJoint(Joint *joint) {
	_joints.push_back(joint);
}

void Body::unrefJoint(Joint* joint) {
	
	vector<Joint*>::iterator it = _joints.begin();
	
	while (it != _joints.end()) {
		
		if (*it == joint) {
			_joints.erase(it);
			break;
		}
		
		it++;
		
	}
	
}


btDynamicsWorld *Body::getWorld() {
	return _scene->getWorld();
}


void Body::__update() {
	
	if (_body->isStaticObject() || ! _body->isActive()) {
		vector<Joint*>::iterator it = _joints.begin();
		while (it != _joints.end()) {
			(*it)->__update(true);
			it++;
		}
		return;
	}
	
	btTransform transform = _body->getCenterOfMassTransform();
	_cachePos = transform.getOrigin();
	_cacheRot = transform.getRotation();
	_cacheVell = _body->getLinearVelocity();
	_cacheVela = _body->getAngularVelocity();
	
	// Emit "update"
	VEC3_TO_OBJ(_cachePos, pos);
	QUAT_TO_OBJ(_cacheRot, quat);
	VEC3_TO_OBJ(_cacheVell, vell);
	VEC3_TO_OBJ(_cacheVela, vela);
	
	Local<Object> obj = Nan::New<Object>();
	SET_PROP(obj, "pos", pos);
	SET_PROP(obj, "quat", quat);
	SET_PROP(obj, "vell", vell);
	SET_PROP(obj, "vela", vela);
	
	Local<Value> argv[2] = { JS_STR("update"), obj };
	_emit(2, argv);
	
	vector<Joint*>::iterator it = _joints.begin();
	while (it != _joints.end()) {
		(*it)->__update();
		it++;
	}
	
}


NAN_GETTER(Body::typeGetter) { THIS_BODY;
	
	RET_VALUE(JS_STR(body->_cacheType.c_str()));
	
}


NAN_SETTER(Body::typeSetter) { THIS_BODY; SETTER_UTF8_ARG;
	
	CACHE_CAS(_cacheType, std::string(*v))
	
	body->_rebuild();
	
	// Emit "type"
	Local<Value> argv[2] = { JS_STR("type"), value };
	body->_emit(2, argv);
	
}


V3_GETTER(pos, _cachePos);
V3_GETTER(vell, _cacheVell);
V3_GETTER(vela, _cacheVela);
V3_GETTER(size, _cacheSize);
V3_GETTER(factl, _cacheFactl);
V3_GETTER(facta, _cacheFacta);


NAN_SETTER(Body::posSetter) { THIS_BODY; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cachePos, v);
	
	body->_rebuild(); // FIXME: ???
	
	// Emit "pos"
	Local<Value> argv[2] = { JS_STR("pos"), value };
	body->_emit(2, argv);
	
}


NAN_GETTER(Body::rotGetter) { THIS_BODY;
	
	btScalar w = body->_cacheRot.getW();
	btScalar x = body->_cacheRot.getX();
	btScalar y = body->_cacheRot.getY();
	btScalar z = body->_cacheRot.getZ();
	float wSquared = w * w;
	float xSquared = x * x;
	float ySquared = y * y;
	float zSquared = z * z;
	
	btVector3 r;
	r.setX(atan2f(2.0f * (y * z + x * w), -xSquared - ySquared + zSquared + wSquared));
	r.setY(asinf(-2.0f * (x * z - y * w)));
	r.setZ(atan2f(2.0f * (x * y + z * w), xSquared - ySquared - zSquared + wSquared));
	r *= 57.29577f;
	
	VEC3_TO_OBJ(r, rot)
	
	RET_VALUE(rot);
	
}


NAN_SETTER(Body::rotSetter) { THIS_BODY; SETTER_VEC3_ARG;
	
	btQuaternion q;
	q.setEuler(v.getY() * 0.01745329f, v.getX() * 0.01745329f, v.getZ() * 0.01745329f);
	
	CACHE_CAS(_cacheRot, q);
	
	btTransform transform = body->_body->getCenterOfMassTransform();
	transform.setRotation(body->_cacheRot);
	body->_body->setCenterOfMassTransform(transform);
	
	// Emit "rot"
	Local<Value> argv[2] = { JS_STR("rot"), value };
	body->_emit(2, argv);
	
}


NAN_SETTER(Body::vellSetter) { THIS_BODY; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheVell, v);
	
	if (body->_cacheSleepy) {
		body->_body->setActivationState(ACTIVE_TAG);
	}
	
	body->_body->setLinearVelocity(body->_cacheVell);
	
	// Emit "vell"
	Local<Value> argv[2] = { JS_STR("vell"), value };
	body->_emit(2, argv);
	
}


NAN_SETTER(Body::velaSetter) { THIS_BODY; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheVela, v);
	
	if (body->_cacheSleepy) {
		body->_body->setActivationState(ACTIVE_TAG);
	}
	
	body->_body->setAngularVelocity(body->_cacheVela);
	
	// Emit "vela"
	Local<Value> argv[2] = { JS_STR("vela"), value };
	body->_emit(2, argv);
	
}


NAN_SETTER(Body::sizeSetter) { THIS_BODY; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheSize, v);
	
	body->_rebuild(); // ??
	
	// Emit "size"
	Local<Value> argv[2] = { JS_STR("size"), value };
	body->_emit(2, argv);
	
}


NAN_SETTER(Body::factlSetter) { THIS_BODY; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheFactl, v);
	
	body->_body->setLinearFactor(body->_cacheFactl);
	
	// Emit "factl"
	Local<Value> argv[2] = { JS_STR("factl"), value };
	body->_emit(2, argv);
	
}


NAN_SETTER(Body::factaSetter) { THIS_BODY; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheFacta, v);
	
	body->_body->setAngularFactor(body->_cacheFacta);
	
	// Emit "facta"
	Local<Value> argv[2] = { JS_STR("facta"), value };
	body->_emit(2, argv);
	
}


NAN_SETTER(Body::mapSetter) { THIS_BODY; SETTER_OBJ_ARG;
	
	// TODO
	
	// Emit "map"
	Local<Value> argv[2] = { JS_STR("map"), value };
	body->_emit(2, argv);
	
}

NAN_GETTER(Body::mapGetter) { THIS_BODY;
	
	Local<Object> obj = Nan::New<Object>();
	
	RET_VALUE(obj);
	
}


NAN_SETTER(Body::meshSetter) { THIS_BODY; SETTER_OBJ_ARG;
	
	// TODO
	
	// Emit "mest"
	Local<Value> argv[2] = { JS_STR("mest"), value };
	body->_emit(2, argv);
	
}

NAN_GETTER(Body::meshGetter) { THIS_BODY;
	
	Local<Object> obj = Nan::New<Object>();
	
	RET_VALUE(obj);
	
}


NAN_SETTER(Body::massSetter) { THIS_BODY; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheMass, v);
	
	body->_rebuild();
	
	// Emit "mass"
	Local<Value> argv[2] = { JS_STR("mass"), value };
	body->_emit(2, argv);
	
}

NAN_GETTER(Body::massGetter) { THIS_BODY;
	
	RET_VALUE(JS_NUM(body->_cacheMass));
	
}


NAN_SETTER(Body::restSetter) { THIS_BODY; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheRest, v);
	
	body->_body->setRestitution(body->_cacheRest);
	
	// Emit "rest"
	Local<Value> argv[2] = { JS_STR("rest"), value };
	body->_emit(2, argv);
	
}

NAN_GETTER(Body::restGetter) { THIS_BODY;
	
	RET_VALUE(JS_NUM(body->_cacheRest));
	
}


NAN_SETTER(Body::damplSetter) { THIS_BODY; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheDampl, v);
	
	body->_body->setDamping(body->_cacheDampl, body->_cacheDampa);
	
	// Emit "dampl"
	Local<Value> argv[2] = { JS_STR("dampl"), value };
	body->_emit(2, argv);
	
}

NAN_GETTER(Body::damplGetter) { THIS_BODY;
	
	RET_VALUE(JS_NUM(body->_cacheDampl));
	
}


NAN_SETTER(Body::dampaSetter) { THIS_BODY; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheDampa, v);
	
	body->_body->setDamping(body->_cacheDampl, body->_cacheDampa);
	
	// Emit "dampa"
	Local<Value> argv[2] = { JS_STR("dampa"), value };
	body->_emit(2, argv);
	
}

NAN_GETTER(Body::dampaGetter) { THIS_BODY;
	
	RET_VALUE(JS_NUM(body->_cacheDampa));
	
}


NAN_SETTER(Body::frictSetter) { THIS_BODY; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheFrict, v);
	
	body->_body->setFriction(body->_cacheFrict);
	
	// Emit "frict"
	Local<Value> argv[2] = { JS_STR("frict"), value };
	body->_emit(2, argv);
	
}

NAN_GETTER(Body::frictGetter) { THIS_BODY;
	
	RET_VALUE(JS_NUM(body->_cacheFrict));
	
}


NAN_SETTER(Body::sleepySetter) { THIS_BODY; SETTER_BOOL_ARG;
	
	CACHE_CAS(_cacheSleepy, v);
	
	body->_body->setActivationState(body->_cacheSleepy ? ACTIVE_TAG : DISABLE_DEACTIVATION);
	
	// Emit "sleepy"
	Local<Value> argv[2] = { JS_STR("sleepy"), value };
	body->_emit(2, argv);
	
}

NAN_GETTER(Body::sleepyGetter) { THIS_BODY;
	
	RET_VALUE(JS_BOOL(body->_cacheSleepy));
	
}


void Body::_rebuild() {
	
	btCollisionShape *oldShape = _cshape;
	
	if (_cacheType == "ball") {
		_cshape = new btSphereShape(0.5f);
	} else if (_cacheType == "roll") {
		_cshape = new btCylinderShape(btVector3(0.5f, 0.5f, 0.5f));
	} else if (_cacheType == "caps") {
		_cshape = new btCapsuleShape(0.5f, 1);
	} else if (_cacheType == "plane") {
		_cshape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 0);
	} /*else if (_cacheType == "map" && _cacheMap) {
		_cshape = new btHeightfieldTerrainShape(
			_cacheMap->w(), _cacheMap->h(),
			_cacheMap->data(),
			1, 1,
			true, false
		);
	} */else /*if (type == "box")*/ {
		_cshape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
	}
	
	_cshape->setLocalScaling(_calcScale());
	
	btRigidBody *oldb = _body;
	
	bool isDynamic = _cacheMass != 0.f && _cacheType != "plane";
	
	btVector3 localInertia(0,0,0);
	if (isDynamic) {
		_cshape->calculateLocalInertia(_cacheMass, localInertia);
	}
	
	btDefaultMotionState* myMotionState = new btDefaultMotionState();
	
	btTransform transform;
	myMotionState->getWorldTransform(transform);
	transform.setOrigin(_cachePos);
	transform.setRotation(_cacheRot);
	myMotionState->setWorldTransform(transform);
	
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		_cacheType != "plane" ? _cacheMass : 0.f, myMotionState, _cshape, localInertia
	);
	
	_body = new btRigidBody(rbInfo);
	
	// reapply cached setup
	_body->setRestitution(_cacheRest);
	_body->setDamping(_cacheDampl, _cacheDampa);
	_body->setLinearFactor(_cacheFactl);
	_body->setAngularFactor(_cacheFacta);
	_body->setFriction(_cacheFrict);
	
	_body->setActivationState(_cacheSleepy ? ACTIVE_TAG : DISABLE_DEACTIVATION);
	_body->setSleepingThresholds(1.f, 1.f);
	
	_body->setUserPointer(this);
	
	_scene->getWorld()->addRigidBody(_body);
	
	vector<Joint*>::iterator it = _joints.begin();
	while (it != _joints.end()) {
		(*it)->_rebuild();
	}
	
	if (oldb) {
		_scene->getWorld()->removeRigidBody(oldb);
	}
	
	delete oldb;
	delete oldShape;
	
}


btVector3 Body::_calcScale() const {
	
	if (_cacheType != "map" || ! _cacheMap) {
		return _cacheSize;
	}
	
	btVector3 sz;
	// sz.setValue( TODO
	// 	_cacheSize.getX() / (_cacheMap->w() - 1),
	// 	_cacheSize.getY(),
	// 	_cacheSize.getZ() / (_cacheMap->h() - 1)
	// );
	
	return sz;
	
}
