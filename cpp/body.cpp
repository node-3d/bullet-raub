#include <cstdlib>
#include <iostream>

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btDefaultMotionState.h>

#include "scene.hpp"
#include "body.hpp"

using namespace v8;
using namespace node;
using namespace std;

#define THIS_BODY                                                 \
	Body *body = ObjectWrap::Unwrap<Body>(info.This());

#define V3_GETTER(NAME, CACHE)                                    \
	NAN_GETTER(Body::posGetter) { NAN_HS; THIS_BODY;              \
		VEC3_TO_OBJ(body->CACHE, NAME);                           \
		RET_VALUE(NAME);                                          \
	}

#define CACHE_CAS(CACHE, V)                                       \
	if (body->CACHE == V) {                                        \
		return;                                                   \
	}                                                             \
	body->CACHE = V;




Persistent<Function> Body::_constructor;


void Body::init(Handle<Object> target) { NAN_HS;
	
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
	
	Nan::Set(target, JS_STR("Body"), ctor->GetFunction());
	
	_constructor.Reset(Isolate::GetCurrent(), ctor->GetFunction());
	
}


NAN_METHOD(Body::newCtor) { NAN_HS;
	
	REQ_OBJ_ARG(0, owner);
	Scene *scene = ObjectWrap::Unwrap<Scene>(owner);
	
	Body *body = new Body(scene);
	body->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


Body::Body(Scene *scene) {
	
	_scene = scene;
	
	_cshape = NULL;
	_body = NULL;
	
	_cacheType = "box";
	_cachePos = btVector3(0,0,0);
	_cacheRot = btQuaternion(0,0,0,1);
	_cacheSize = btVector3(1,1,1);
	_cacheVell = btVector3(0,0,0);
	_cacheVela = btVector3(0,0,0);
	_cacheMass = 0;
	_cacheRest = 0.0f;
	_cacheDampl = 0.1f;
	_cacheDampa = 0.1f;
	_cacheFactl = btVector3(1,1,1);
	_cacheFacta = btVector3(1,1,1);
	_cacheFrict = 0.5f;
	_cacheSleepy = true;
	_cacheMap = NULL;
	_cacheMesh = NULL;
	
	rebuild();
	
	_scene->refBody(this);
	
}


Body::~Body() {
	
	vector<Joint*>::iterator it = _joints.begin();
	while (it != _joints.end()) {
		(*it)->dropBody(this);
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


void Body::__update()
{
	if (_body->isStaticObject() || ! _body->isActive()) {
		return;
	}
	
	btTransform  transform = _body->getCenterOfMassTransform();
	_cachePos = transform.getOrigin();
	_cacheRot = transform.getRotation();
	_cacheVell = _body->getLinearVelocity();
	_cacheVela = _body->getAngularVelocity();
	
	// EMIT
	
}


NAN_GETTER(Body::typeGetter) { NAN_HS; THIS_BODY;
	
	RET_VALUE(JS_STR(body->_cacheType.c_str()));
	
}


NAN_SETTER(Body::typeSetter) { NAN_HS; THIS_BODY;
	
	REQ_UTF8_ARG(0, str);
	
	CACHE_CAS(_cacheType, std::string(*str))
	
	body->_rebuild();
	
	// EMIT
	
}


V3_GETTER(pos, _cachePos);
V3_GETTER(vell, _cacheVell);
V3_GETTER(vela, _cacheVela);
V3_GETTER(size, _cacheSize);
V3_GETTER(factl, _cacheFactl);
V3_GETTER(facta, _cacheFacta);


NAN_SETTER(Body::posSetter) { NAN_HS; THIS_BODY;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cachePos, v);
	
	_rebuild(); // FIXME: ???
	
	// EMIT
	
}


NAN_GETTER(Body::rotGetter) { NAN_HS; THIS_BODY;
	
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
	
	RET_VALUE(pos);
	
}


NAN_SETTER(Body::rotSetter) { NAN_HS; THIS_BODY;
	
	REQ_VEC3_ARG(0, v);
	
	btQuaternion q;
	q.setEuler(v.getY() * 0.01745329, v.getX() * 0.01745329, v.getZ() * 0.01745329);
	
	CACHE_CAS(_cacheRot, q);
	
	btTransform transform = body->_body->getCenterOfMassTransform();
	transform.setRotation(body->_cacheRot);
	body->_body->setCenterOfMassTransform(transform);
	
	// EMIT
	
}


NAN_SETTER(Body::vellSetter) { NAN_HS; THIS_BODY;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheVell, v);
	
	if (cacheSleepy) {
		body->body->setActivationState(ACTIVE_TAG);
	}
	
	body->body->setLinearVelocity(body->_cacheVell);
	
	// EMIT
	
}


NAN_SETTER(Body::velaSetter) { NAN_HS; THIS_BODY;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheVela, v);
	
	if (cacheSleepy) {
		body->body->setActivationState(ACTIVE_TAG);
	}
	
	body->body->setAngularVelocity(body->_cacheVela);
	
	// EMIT
	
}


NAN_SETTER(Body::sizeSetter) { NAN_HS; THIS_BODY;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheSize, v);
	
	body->_rebuild(); // ??
	
	// EMIT
	
}


NAN_SETTER(Body::factlSetter) { NAN_HS; THIS_BODY;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheFactl, v);
	
	body->body->setLinearFactor(body->_cacheFactl);
	
	// EMIT
	
}


NAN_SETTER(Body::factaSetter) { NAN_HS; THIS_BODY;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheFacta, v);
	
	body->body->setAngularFactor(body->_cacheFacta);
	
	// EMIT
	
}


NAN_SETTER(Body::mapSetter) { NAN_HS; THIS_BODY;
	
	REQ_OBJ_ARG(0, v);
	
	// TODO
	
	// EMIT
	
}


NAN_SETTER(Body::meshSetter) { NAN_HS; THIS_BODY;
	
	REQ_OBJ_ARG(0, v);
	
	// TODO
	
	// EMIT
	
}


NAN_SETTER(Body::massSetter) { NAN_HS; THIS_BODY;
	
	REQ_FLOAT_ARG(0, v);
	
	CACHE_CAS(_cacheMass, v);
	
	body->_rebuild();
	
	// EMIT
	
}


NAN_SETTER(Body::restSetter) { NAN_HS; THIS_BODY;
	
	REQ_FLOAT_ARG(0, v);
	
	CACHE_CAS(_cacheRest, v);
	
	body->_body->setRestitution();
	
	// EMIT
	
}


NAN_SETTER(Body::damplSetter) { NAN_HS; THIS_BODY;
	
	REQ_FLOAT_ARG(0, v);
	
	CACHE_CAS(_cacheDampl, v);
	
	body->_body->setDamping(body->_cacheDampl, body->_cacheDampa);
	
	// EMIT
	
}


NAN_SETTER(Body::dampaSetter) { NAN_HS; THIS_BODY;
	
	REQ_FLOAT_ARG(0, v);
	
	CACHE_CAS(_cacheDampa, v);
	
	body->_body->setDamping(body->_cacheDampl, body->_cacheDampa);
	
	// EMIT
	
}


NAN_SETTER(Body::frictSetter) { NAN_HS; THIS_BODY;
	
	REQ_FLOAT_ARG(0, v);
	
	CACHE_CAS(_cacheFrict, v);
	
	body->_body->setDamping(body->_cacheFrict);
	
	// EMIT
	
}


NAN_SETTER(Body::sleepySetter) { NAN_HS; THIS_BODY;
	
	REQ_BOOL_ARG(0, v);
	
	CACHE_CAS(_cacheSleepy, v);
	
	body->_body->setActivationState(body->_cacheSleepy ? ACTIVE_TAG : DISABLE_DEACTIVATION);
	
	// EMIT
	
}



void Body::_rebuild() {
	
	btCollisionShape *oldShape = _cshape;
	
	if (_cacheType == "ball") {
		_cshape = new btSphereShape(0.5f);
	} else if (_cacheType == "roll") {
		_cshape = new btCylinderShape(btVector3(0.5f, 0.5f, 0.5f));
	} else if (_cacheType == "caps") {
		_cshape = new btCapsuleShape(0.5f, 1);
	} else if (_cacheType == "map" && _cacheMap) {
		_cshape = new btHeightfieldTerrainShape(
			_cacheMap->w(), _cacheMap->h(),
			_cacheMap->data(),
			1, 1,
			true, false
		);
	} else /*if (type == "box")*/ {
		_cshape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
	}
	
	_cshape->setLocalScaling(_calcScale());
	
	btRigidBody *oldb = _body;
	
	bool isDynamic = (_cacheMass != 0.f);
	
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
	
	btRigidBody::btRigidBodyConstructionInfo rbInfo(_cacheMass, myMotionState, _cshape, localInertia);
	
	_body = new btRigidBody(rbInfo);
	
	// reapply cached setup
	_body->setRestitution(_cacheRest);
	_body->setDamping(_cacheDampl, _cacheDampa);
	_body->setLinearFactor(_cacheFactl);
	_body->setAngularFactor(_cacheFacta);
	_body->setFriction(_cacheFrict);
	_body->setActivationState(_cacheSleepy ? ACTIVE_TAG : DISABLE_DEACTIVATION);
	
	_body->setUserPointer(this);
	
	_scene->getWorld()->addRigidBody(_body);
	
	vector<Joint*>::iterator it = _joints.begin();
	while (it != _joints.end()) {
		(*it)->rebuild();
	}
	
	if (oldb) {
		_scene->getWorld()->removeRigidBody(oldb);
	}
	
	delete oldb;
	delete oldShape;
	
}


const btVector3 &VecBody::calcScale() const {
	
	if (_cacheType != "map" || ! cacheMap) {
		return cacheSizeBt;
	}
	
	btVector3 sz;
	sz.setValue(
		cacheSizeBt.getX() / (cacheMap->w() - 1),
		cacheSizeBt.getY(),
		cacheSizeBt.getZ() / (cacheMap->h() - 1)
	);
	
	return sz;
	
}
