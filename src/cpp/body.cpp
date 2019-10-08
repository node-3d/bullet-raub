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
	
	REQ_OBJ_ARG(0, sceneObj);
	_sceneObj.Reset(sceneObj);
	
	_scene = _sceneObj;
	
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
	
	btMotionState *motion = _getMotionState();
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
	
	if (_isStaticObject() || ! _isActive()) {
		
		EACH(_joints) {
			_joints[i]->__update(true);
		}
		
		return;
		
	}
	
	btTransform transform = _getCenterOfMassTransform();
	
	_cachePos = transform.getOrigin();
	_cacheRot = transform.getRotation();
	
	_cacheVell = _getLinearVelocity();
	_cacheVela = _getAngularVelocity();
	
	// Emit "update"
	VEC3_TO_OBJ(_cachePos, pos);
	QUAT_TO_OBJ(_cacheRot, quat);
	VEC3_TO_OBJ(_cacheVell, vell);
	VEC3_TO_OBJ(_cacheVela, vela);
	Napi::Object obj = Napi::Object::New(env);
	SET_PROP(obj, "pos", pos);
	SET_PROP(obj, "quat", quat);
	SET_PROP(obj, "vell", vell);
	SET_PROP(obj, "vela", vela);
	Napi::Value objVal = obj;
	emit("update", 1, &objVal);
	
	EACH(_joints) {
		_joints[i]->__update();
	}
	
}


JS_IMPLEMENT_GETTER(Body, typeGetter) { THIS_CHECK;
	
	RET_STR(_cacheType.c_str());
	
}


JS_IMPLEMENT_SETTER(Body, typeSetter) { THIS_CHECK; SETTER_STR_ARG;
	
	CACHE_CAS(_cacheType, v);
	
	_rebuild();
	
	emit("type", 1, &value);
	
}


V3_GETTER(pos, _cachePos);
V3_GETTER(vell, _cacheVell);
V3_GETTER(vela, _cacheVela);
V3_GETTER(size, _cacheSize);
V3_GETTER(factl, _cacheFactl);
V3_GETTER(facta, _cacheFacta);
NUM_GETTER(mass, _cacheMass);
NUM_GETTER(rest, _cacheRest);
NUM_GETTER(dampl, _cacheDampl);
NUM_GETTER(dampa, _cacheDampa);
NUM_GETTER(frict, _cacheFrict);



JS_IMPLEMENT_SETTER(Body, posSetter) { THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cachePos, v);
	
	_rebuild(); // FIXME: ???
	
	emit("pos", 1, &value);
	
}


JS_IMPLEMENT_GETTER(Body, rotGetter) { THIS_CHECK;
	
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


JS_IMPLEMENT_SETTER(Body, rotSetter) { THIS_CHECK; SETTER_VEC3_ARG;
	
	btQuaternion q;
	q.setEuler(v.getY() * 0.01745329f, v.getX() * 0.01745329f, v.getZ() * 0.01745329f);
	
	CACHE_CAS(_cacheRot, q);
	
	btTransform transform = _getCenterOfMassTransform();
	transform.setRotation(_cacheRot);
	_setCenterOfMassTransform(transform);
	
	emit("rot", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, vellSetter) { THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheVell, v);
	
	if (_cacheSleepy) {
		_setActivationState(ACTIVE_TAG);
	}
	
	_setLinearVelocity(_cacheVell);
	
	emit("vell", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, velaSetter) { THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheVela, v);
	
	if (_cacheSleepy) {
		_setActivationState(ACTIVE_TAG);
	}
	
	_setAngularVelocity(_cacheVela);
	
	emit("vela", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, sizeSetter) { THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheSize, v);
	
	_rebuild(); // ??
	
	emit("size", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, factlSetter) { THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheFactl, v);
	
	_setLinearFactor(_cacheFactl);
	
	emit("factl", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, factaSetter) { THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheFacta, v);
	
	_setAngularFactor(_cacheFacta);
	
	emit("facta", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, mapSetter) { THIS_CHECK; SETTER_OBJ_ARG;
	
	// TODO
	
	emit("map", 1, &value);
	
}

JS_IMPLEMENT_GETTER(Body, mapGetter) { THIS_CHECK;
	
	Napi::Value obj = Napi::Object::New(env);
	
	RET_VALUE(obj);
	
}


JS_IMPLEMENT_SETTER(Body, meshSetter) { THIS_CHECK; SETTER_OBJ_ARG;
	
	// TODO
	
	emit("mest", 1, &value);
	
}

JS_IMPLEMENT_GETTER(Body, meshGetter) { THIS_CHECK;
	
	Napi::Value obj = Napi::Object::New(env);
	
	RET_VALUE(obj);
	
}


JS_IMPLEMENT_SETTER(Body, massSetter) { THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheMass, v);
	
	_rebuild();
	
	emit("mass", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, restSetter) { THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheRest, v);
	
	_setRestitution(_cacheRest);
	
	emit("rest", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, damplSetter) { THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheDampl, v);
	
	_setDamping(_cacheDampl, _cacheDampa);
	
	emit("dampl", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, dampaSetter) { THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheDampa, v);
	
	_setDamping(_cacheDampl, _cacheDampa);
	
	emit("dampa", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, frictSetter) { THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheFrict, v);
	
	_setFriction(_cacheFrict);
	
	emit("frict", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Body, sleepySetter) { THIS_CHECK; SETTER_BOOL_ARG;
	
	CACHE_CAS(_cacheSleepy, v);
	
	_setActivationState(_cacheSleepy ? ACTIVE_TAG : DISABLE_DEACTIVATION);
	
	emit("sleepy", 1, &value);
	
}

JS_IMPLEMENT_GETTER(Body, sleepyGetter) { THIS_CHECK;
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
	_setRestitution(_cacheRest);
	_setDamping(_cacheDampl, _cacheDampa);
	_setLinearFactor(_cacheFactl);
	_setAngularFactor(_cacheFacta);
	_setFriction(_cacheFrict);
	
	_setActivationState(_cacheSleepy ? ACTIVE_TAG : DISABLE_DEACTIVATION);
	_setSleepingThresholds(1.f, 1.f);
	
	_setUserPointer(this);
	
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
	_destroy();
}


JS_IMPLEMENT_GETTER(Body, isDestroyedGetter) { THIS_BODY;
	RET_BOOL(_isDestroyed);
}
