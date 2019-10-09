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


IMPLEMENT_ES5_CLASS(Body);


void Body::init(Napi::Env env, Napi::Object exports) {
	
	Napi::Function ctor = wrap(env);
	
	JS_ASSIGN_METHOD(destroy);
	
	JS_ASSIGN_GETTER(isDestroyed);
	JS_ASSIGN_GETTER(type);
	JS_ASSIGN_GETTER(pos);
	JS_ASSIGN_GETTER(rot);
	JS_ASSIGN_GETTER(vell);
	JS_ASSIGN_GETTER(vela);
	JS_ASSIGN_GETTER(size);
	JS_ASSIGN_GETTER(map);
	JS_ASSIGN_GETTER(mesh);
	JS_ASSIGN_GETTER(mass);
	JS_ASSIGN_GETTER(rest);
	JS_ASSIGN_GETTER(dampl);
	JS_ASSIGN_GETTER(dampa);
	JS_ASSIGN_GETTER(factl);
	JS_ASSIGN_GETTER(facta);
	JS_ASSIGN_GETTER(frict);
	JS_ASSIGN_GETTER(sleepy);
	
	JS_ASSIGN_SETTER(type);
	JS_ASSIGN_SETTER(pos);
	JS_ASSIGN_SETTER(rot);
	JS_ASSIGN_SETTER(vell);
	JS_ASSIGN_SETTER(vela);
	JS_ASSIGN_SETTER(size);
	JS_ASSIGN_SETTER(map);
	JS_ASSIGN_SETTER(mesh);
	JS_ASSIGN_SETTER(mass);
	JS_ASSIGN_SETTER(rest);
	JS_ASSIGN_SETTER(dampl);
	JS_ASSIGN_SETTER(dampa);
	JS_ASSIGN_SETTER(factl);
	JS_ASSIGN_SETTER(facta);
	JS_ASSIGN_SETTER(frict);
	JS_ASSIGN_SETTER(sleepy);
	
	exports.Set("Body", ctor);
	
}


Body::Body(const Napi::CallbackInfo &info):
Common(info.This(), "Body") { NAPI_ENV;
	
	super(info);
	
	REQ_OBJ_ARG(0, sceneObj);
	_sceneObj.Reset(sceneObj);
	
	_scene = Scene::unwrap(sceneObj);
	
	_cshape = nullptr;
	_body = nullptr;
	
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
	_cacheMap = nullptr;
	_cacheMesh = nullptr;
	
	_rebuild();
	_scene->refBody(this);
	
}


Body::~Body() {
	_destroy();
}


void Body::_destroy() { DES_CHECK;
	
	emit("destroy");
	
	EACH(_joints) {
		_joints[i]->_dropBody(this);
	}
	
	_joints.clear();
	_scene->unrefBody(this);
	
	btMotionState *motion = _body->getMotionState();
	ALIGNED_DELETE(btMotionState, motion);
	
	if (_body) {
		_scene->getWorld()->removeCollisionObject(_body);
	}
	
	ALIGNED_DELETE(btRigidBody, _body);
	ALIGNED_DELETE(btCollisionShape, _cshape);
	
	_scene = nullptr;
	
	Common::_destroy();
	
}


void Body::refJoint(Joint *joint) { DES_CHECK;
	_joints.push_back(joint);
}


void Body::unrefJoint(Joint* joint) { DES_CHECK;
	_joints.remove(joint);
}


btDynamicsWorld *Body::getWorld() {
	return _scene->getWorld();
}


void Body::__update() { DES_CHECK;
	
	if (_body->isStaticObject() || ! _body->isActive()) {
		
		EACH(_joints) {
			_joints[i]->__update(true);
		}
		
		return;
		
	}
	
	btTransform transform = _body->getCenterOfMassTransform();
	
	_cachePos = transform.getOrigin();
	_cacheRot = transform.getRotation();
	
	_cacheVell = _body->getLinearVelocity();
	_cacheVela = _body->getAngularVelocity();
	
	Napi::Env env = _that.Env();
	
	VEC3_TO_OBJ(_cachePos, pos);
	QUAT_TO_OBJ(_cacheRot, quat);
	VEC3_TO_OBJ(_cacheVell, vell);
	VEC3_TO_OBJ(_cacheVela, vela);
	
	Napi::Object obj = Napi::Object::New(env);
	obj.Set("pos", pos);
	obj.Set("quat", quat);
	obj.Set("vell", vell);
	obj.Set("vela", vela);
	
	Napi::Value objVal = obj;
	emit("update", 1, &objVal);
	
	EACH(_joints) {
		_joints[i]->__update();
	}
	
}


JS_IMPLEMENT_GETTER(Body, type) { THIS_CHECK;
	
	RET_STR(_cacheType.c_str());
	
}


JS_IMPLEMENT_SETTER(Body, type) { THIS_SETTER_CHECK; SETTER_STR_ARG;
	
	CACHE_CAS(_cacheType, v);
	
	_rebuild();
	
	emit("type", 1, &value);
	
}


V3_GETTER(Body, pos, _cachePos);
V3_GETTER(Body, vell, _cacheVell);
V3_GETTER(Body, vela, _cacheVela);
V3_GETTER(Body, size, _cacheSize);
V3_GETTER(Body, factl, _cacheFactl);
V3_GETTER(Body, facta, _cacheFacta);
NUM_GETTER(Body, mass, _cacheMass);
NUM_GETTER(Body, rest, _cacheRest);
NUM_GETTER(Body, dampl, _cacheDampl);
NUM_GETTER(Body, dampa, _cacheDampa);
NUM_GETTER(Body, frict, _cacheFrict);



JS_IMPLEMENT_SETTER(Body, pos) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cachePos, v);
	
	_rebuild(); // FIXME: ???
	
	emit("pos", 1, &value);
	
}


JS_IMPLEMENT_GETTER(Body, rot) { THIS_CHECK;
	
	btScalar w = _cacheRot.getW();
	btScalar x = _cacheRot.getX();
	btScalar y = _cacheRot.getY();
	btScalar z = _cacheRot.getZ();
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


JS_IMPLEMENT_SETTER(Body, rot) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	btQuaternion q;
	q.setEuler(v.getY() * 0.01745329f, v.getX() * 0.01745329f, v.getZ() * 0.01745329f);
	
	CACHE_CAS(_cacheRot, q);
	
	btTransform transform = _body->getCenterOfMassTransform();
	transform.setRotation(_cacheRot);
	_body->setCenterOfMassTransform(transform);
	
	emit("rot", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, vell) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheVell, v);
	
	if (_cacheSleepy) {
		_body->setActivationState(ACTIVE_TAG);
	}
	
	_body->setLinearVelocity(_cacheVell);
	
	emit("vell", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, vela) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheVela, v);
	
	if (_cacheSleepy) {
		_body->setActivationState(ACTIVE_TAG);
	}
	
	_body->setAngularVelocity(_cacheVela);
	
	emit("vela", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, size) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheSize, v);
	
	_rebuild(); // ??
	
	emit("size", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, factl) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheFactl, v);
	
	_body->setLinearFactor(_cacheFactl);
	
	emit("factl", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, facta) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheFacta, v);
	
	_body->setAngularFactor(_cacheFacta);
	
	emit("facta", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, map) { THIS_SETTER_CHECK; SETTER_OBJ_ARG;
	
	// TODO
	
	emit("map", 1, &value);
	
}

JS_IMPLEMENT_GETTER(Body, map) { THIS_CHECK;
	
	Napi::Value obj = Napi::Object::New(env);
	
	RET_VALUE(obj);
	
}


JS_IMPLEMENT_SETTER(Body, mesh) { THIS_SETTER_CHECK; SETTER_OBJ_ARG;
	
	// TODO
	
	emit("mest", 1, &value);
	
}

JS_IMPLEMENT_GETTER(Body, mesh) { THIS_CHECK;
	
	Napi::Value obj = Napi::Object::New(env);
	
	RET_VALUE(obj);
	
}


JS_IMPLEMENT_SETTER(Body, mass) { THIS_SETTER_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheMass, v);
	
	_rebuild();
	
	emit("mass", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, rest) { THIS_SETTER_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheRest, v);
	
	_body->setRestitution(_cacheRest);
	
	emit("rest", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, dampl) { THIS_SETTER_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheDampl, v);
	
	_body->setDamping(_cacheDampl, _cacheDampa);
	
	emit("dampl", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, dampa) { THIS_SETTER_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheDampa, v);
	
	_body->setDamping(_cacheDampl, _cacheDampa);
	
	emit("dampa", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, frict) { THIS_SETTER_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheFrict, v);
	
	_body->setFriction(_cacheFrict);
	
	emit("frict", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, sleepy) { THIS_SETTER_CHECK; SETTER_BOOL_ARG;
	
	CACHE_CAS(_cacheSleepy, v);
	
	_body->setActivationState(_cacheSleepy ? ACTIVE_TAG : DISABLE_DEACTIVATION);
	
	emit("sleepy", 1, &value);
	
}

JS_IMPLEMENT_GETTER(Body, sleepy) { THIS_CHECK;
	RET_BOOL(_cacheSleepy);
}


void Body::_rebuild() { DES_CHECK;
	
	btCollisionShape *oldShape = _cshape;
	
	if (_cacheType == "ball") {
		_cshape = ALIGNED_NEW(btSphereShape, 0.5f);
	} else if (_cacheType == "roll") {
		_cshape = ALIGNED_NEW(btCylinderShape, btVector3(0.5f, 0.5f, 0.5f));
	} else if (_cacheType == "caps") {
		_cshape = ALIGNED_NEW(btCapsuleShape, 0.5f, 1);
	} else if (_cacheType == "plane") {
		_cshape = ALIGNED_NEW(btStaticPlaneShape, btVector3(0.0f, 1.0f, 0.0f), 0);
	/*} else if (_cacheType == "map" && _cacheMap) {
		_cshape = ALIGNED_NEW(
			btHeightfieldTerrainShape,
			_cacheMap->w(), _cacheMap->h(),
			_cacheMap->data(),
			1, 1,
			true, false
		);
	} */
	} else /*if (type == "box")*/ {
		_cshape = ALIGNED_NEW(btBoxShape, btVector3(0.5f, 0.5f, 0.5f));
	}
	
	_cshape->setLocalScaling(_calcScale());
	
	btRigidBody *oldb = _body;
	
	bool isDynamic = _cacheMass != 0.f && _cacheType != "plane";
	
	btVector3 localInertia(0, 0, 0);
	if (isDynamic) {
		_cshape->calculateLocalInertia(_cacheMass, localInertia);
	}
	
	btDefaultMotionState* myMotionState = ALIGNED_NEW(btDefaultMotionState);
	
	btTransform transform;
	myMotionState->getWorldTransform(transform);
	transform.setOrigin(_cachePos);
	transform.setRotation(_cacheRot);
	myMotionState->setWorldTransform(transform);
	
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		_cacheType != "plane" ? _cacheMass : 0.f, myMotionState, _cshape, localInertia
	);
	_body = ALIGNED_NEW(btRigidBody, rbInfo);
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
	
	EACH(_joints) {
		_joints[i]->_rebuild();
	}
	
	if (oldb) {
		_scene->getWorld()->removeRigidBody(oldb);
	}
	
	ALIGNED_DELETE(btRigidBody, oldb);
	ALIGNED_DELETE(btCollisionShape, oldShape);
	
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


JS_IMPLEMENT_METHOD(Body, destroy) { THIS_CHECK;
	emit("destroy");
	_destroy();
	RET_UNDEFINED;
}


JS_IMPLEMENT_GETTER(Body, isDestroyed) { THIS_CHECK;
	RET_BOOL(_isDestroyed);
}
