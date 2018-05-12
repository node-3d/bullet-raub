#include <cstdlib>

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


// ------ Aux macros

#define THIS_BODY                                                             \
	Body *body = ObjectWrap::Unwrap<Body>(info.This());

#define THIS_CHECK                                                            \
	if (body->_isDestroyed) return;

#define V3_GETTER(NAME, CACHE)                                                \
	NAN_GETTER(Body::NAME ## Getter) { THIS_BODY; THIS_CHECK;                 \
		VEC3_TO_OBJ(body->CACHE, NAME);                                       \
		RET_VALUE(NAME);                                                      \
	}

#define CACHE_CAS(CACHE, V)                                                   \
	if (body->CACHE == V) {                                                   \
		return;                                                               \
	}                                                                         \
	body->CACHE = V;


// ------ Constructor and Destructor

Body::Body(Scene *scene) {
	
	_isDestroyed = false;
	
	_scene = scene;
	
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
	
	_isDestroyed = true;
	
}


// ------ Methods and props

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
		
		// EACH(_joints) {
		// 	_joints[i]->__update(true);
		// }
		
		return;
		
	}
	
	// btTransform transform = _body->getCenterOfMassTransform();
	
	// _cachePos = transform.getOrigin();
	// _cacheRot = transform.getRotation();
	
	// _cacheVell = _body->getLinearVelocity();
	// _cacheVela = _body->getAngularVelocity();
	
	// // Emit "update"
	// VEC3_TO_OBJ(_cachePos, pos);
	// QUAT_TO_OBJ(_cacheRot, quat);
	// VEC3_TO_OBJ(_cacheVell, vell);
	// VEC3_TO_OBJ(_cacheVela, vela);
	V8_VAR_OBJ obj = Nan::New<Object>();
	// SET_PROP(obj, "pos", pos);
	// SET_PROP(obj, "quat", quat);
	// SET_PROP(obj, "vell", vell);
	// SET_PROP(obj, "vela", vela);
	V8_VAR_VAL objVal = obj;
	emit("update", 1, &objVal);
	
	// EACH(_joints) {
	// 	_joints[i]->__update();
	// }
	
}


NAN_GETTER(Body::typeGetter) { THIS_BODY; THIS_CHECK;
	
	RET_VALUE(JS_STR(body->_cacheType.c_str()));
	
}


NAN_SETTER(Body::typeSetter) { THIS_BODY; THIS_CHECK; SETTER_UTF8_ARG;
	
	CACHE_CAS(_cacheType, std::string(*v));
	
	body->_rebuild();
	
	body->emit("type", 1, &value);
	
}


V3_GETTER(pos, _cachePos);
V3_GETTER(vell, _cacheVell);
V3_GETTER(vela, _cacheVela);
V3_GETTER(size, _cacheSize);
V3_GETTER(factl, _cacheFactl);
V3_GETTER(facta, _cacheFacta);


NAN_SETTER(Body::posSetter) { THIS_BODY; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cachePos, v);
	
	body->_rebuild(); // FIXME: ???
	
	body->emit("pos", 1, &value);
	
}


NAN_GETTER(Body::rotGetter) { THIS_BODY; THIS_CHECK;
	
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


NAN_SETTER(Body::rotSetter) { THIS_BODY; THIS_CHECK; SETTER_VEC3_ARG;
	
	btQuaternion q;
	q.setEuler(v.getY() * 0.01745329f, v.getX() * 0.01745329f, v.getZ() * 0.01745329f);
	
	CACHE_CAS(_cacheRot, q);
	
	btTransform transform = body->_body->getCenterOfMassTransform();
	transform.setRotation(body->_cacheRot);
	body->_body->setCenterOfMassTransform(transform);
	
	body->emit("rot", 1, &value);
	
}


NAN_SETTER(Body::vellSetter) { THIS_BODY; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheVell, v);
	
	if (body->_cacheSleepy) {
		body->_body->setActivationState(ACTIVE_TAG);
	}
	
	body->_body->setLinearVelocity(body->_cacheVell);
	
	body->emit("vell", 1, &value);
	
}


NAN_SETTER(Body::velaSetter) { THIS_BODY; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheVela, v);
	
	if (body->_cacheSleepy) {
		body->_body->setActivationState(ACTIVE_TAG);
	}
	
	body->_body->setAngularVelocity(body->_cacheVela);
	
	body->emit("vela", 1, &value);
	
}


NAN_SETTER(Body::sizeSetter) { THIS_BODY; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheSize, v);
	
	body->_rebuild(); // ??
	
	body->emit("size", 1, &value);
	
}


NAN_SETTER(Body::factlSetter) { THIS_BODY; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheFactl, v);
	
	body->_body->setLinearFactor(body->_cacheFactl);
	
	body->emit("factl", 1, &value);
	
}


NAN_SETTER(Body::factaSetter) { THIS_BODY; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheFacta, v);
	
	body->_body->setAngularFactor(body->_cacheFacta);
	
	body->emit("facta", 1, &value);
	
}


NAN_SETTER(Body::mapSetter) { THIS_BODY; THIS_CHECK; SETTER_OBJ_ARG;
	
	// TODO
	
	body->emit("map", 1, &value);
	
}

NAN_GETTER(Body::mapGetter) { THIS_BODY; THIS_CHECK;
	
	V8_VAR_VAL obj = Nan::New<Object>();
	
	RET_VALUE(obj);
	
}


NAN_SETTER(Body::meshSetter) { THIS_BODY; THIS_CHECK; SETTER_OBJ_ARG;
	
	// TODO
	
	body->emit("mest", 1, &value);
	
}

NAN_GETTER(Body::meshGetter) { THIS_BODY; THIS_CHECK;
	
	V8_VAR_VAL obj = Nan::New<Object>();
	
	RET_VALUE(obj);
	
}


NAN_SETTER(Body::massSetter) { THIS_BODY; THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheMass, v);
	
	body->_rebuild();
	
	body->emit("mass", 1, &value);
	
}

NAN_GETTER(Body::massGetter) { THIS_BODY; THIS_CHECK;
	
	RET_VALUE(JS_NUM(body->_cacheMass));
	
}


NAN_SETTER(Body::restSetter) { THIS_BODY; THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheRest, v);
	
	body->_body->setRestitution(body->_cacheRest);
	
	body->emit("rest", 1, &value);
	
}

NAN_GETTER(Body::restGetter) { THIS_BODY; THIS_CHECK;
	
	RET_VALUE(JS_NUM(body->_cacheRest));
	
}


NAN_SETTER(Body::damplSetter) { THIS_BODY; THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheDampl, v);
	
	body->_body->setDamping(body->_cacheDampl, body->_cacheDampa);
	
	body->emit("dampl", 1, &value);
	
}

NAN_GETTER(Body::damplGetter) { THIS_BODY; THIS_CHECK;
	
	RET_VALUE(JS_NUM(body->_cacheDampl));
	
}


NAN_SETTER(Body::dampaSetter) { THIS_BODY; THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheDampa, v);
	
	body->_body->setDamping(body->_cacheDampl, body->_cacheDampa);
	
	body->emit("dampa", 1, &value);
	
}

NAN_GETTER(Body::dampaGetter) { THIS_BODY; THIS_CHECK;
	
	RET_VALUE(JS_NUM(body->_cacheDampa));
	
}


NAN_SETTER(Body::frictSetter) { THIS_BODY; THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheFrict, v);
	
	body->_body->setFriction(body->_cacheFrict);
	
	body->emit("frict", 1, &value);
	
}

NAN_GETTER(Body::frictGetter) { THIS_BODY;
	
	RET_VALUE(JS_NUM(body->_cacheFrict));
	
}


NAN_SETTER(Body::sleepySetter) { THIS_BODY; THIS_CHECK; SETTER_BOOL_ARG;
	
	CACHE_CAS(_cacheSleepy, v);
	
	body->_body->setActivationState(body->_cacheSleepy ? ACTIVE_TAG : DISABLE_DEACTIVATION);
	
	body->emit("sleepy", 1, &value);
	
}

NAN_GETTER(Body::sleepyGetter) { THIS_BODY; THIS_CHECK;
	
	RET_VALUE(JS_BOOL(body->_cacheSleepy));
	
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
		
		oldb->~btRigidBody();
		btAlignedFree(oldb);
	}
	
	if (oldShape) {
		oldShape->~btCollisionShape();
		btAlignedFree(oldShape);
	}
	
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



// ------ System methods and props for ObjectWrap

V8_STORE_FT Body::_protoBody;
V8_STORE_FUNC Body::_ctorBody;


void Body::init(V8_VAR_OBJ target) {
	
	V8_VAR_FT proto = Nan::New<FunctionTemplate>(newCtor);
	
	// class Body inherits EventEmitter
	V8_VAR_FT parent = Nan::New(EventEmitter::_protoEventEmitter);
	proto->Inherit(parent);
	
	proto->InstanceTemplate()->SetInternalFieldCount(1);
	proto->SetClassName(JS_STR("Body"));
	
	
	// Accessors
	
	V8_VAR_OT obj = proto->PrototypeTemplate();
	
	ACCESSOR_R(obj, isDestroyed);
	
	ACCESSOR_RW(obj, type);
	ACCESSOR_RW(obj, pos);
	ACCESSOR_RW(obj, rot);
	ACCESSOR_RW(obj, vell);
	ACCESSOR_RW(obj, vela);
	ACCESSOR_RW(obj, size);
	ACCESSOR_RW(obj, map);
	ACCESSOR_RW(obj, mesh);
	ACCESSOR_RW(obj, mass);
	ACCESSOR_RW(obj, rest);
	ACCESSOR_RW(obj, dampl);
	ACCESSOR_RW(obj, dampa);
	ACCESSOR_RW(obj, factl);
	ACCESSOR_RW(obj, facta);
	ACCESSOR_RW(obj, frict);
	ACCESSOR_RW(obj, sleepy);
	
	// -------- dynamic
	
	Nan::SetPrototypeMethod(proto, "destroy", destroy);
	
	
	// -------- static
	
	V8_VAR_FUNC ctor = Nan::GetFunction(proto).ToLocalChecked();
	
	_protoBody.Reset(proto);
	_ctorBody.Reset(ctor);
	
	Nan::Set(target, JS_STR("Body"), ctor);
	
}


bool Body::isBody(V8_VAR_OBJ obj) {
	return Nan::New(_protoBody)->HasInstance(obj);
}


NAN_METHOD(Body::newCtor) {
	
	CTOR_CHECK("Body");
	
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
	
	Body *body = ALIGNED_NEW(Body, scene);
	
	// TODO: opts
	
	body->Wrap(info.This());
	// RET_VALUE(info.This());
	
}


NAN_METHOD(Body::destroy) { THIS_BODY; THIS_CHECK;
	
	body->emit("destroy");
	
	body->_destroy();
	
}


NAN_GETTER(Body::isDestroyedGetter) { THIS_BODY;
	
	RET_VALUE(JS_BOOL(body->_isDestroyed));
	
}

