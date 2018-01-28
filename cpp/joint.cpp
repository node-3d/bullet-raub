#include <cstdlib>
#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/ConstraintSolver/btTypedConstraint.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.h>
#include <BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h>

#include "scene.hpp"
#include "body.hpp"
#include "joint.hpp"

using namespace v8;
using namespace node;
using namespace std;

#define THIS_JOINT                                                            \
	Joint *joint = ObjectWrap::Unwrap<Joint>(info.This());

#define THIS_CHECK                                                            \
	if (joint->_isDestroyed) return;
	
#define V3_GETTER(NAME, CACHE)                                                \
	NAN_GETTER(Joint::NAME ## Getter) { THIS_JOINT; THIS_CHECK;               \
		VEC3_TO_OBJ(joint->CACHE, NAME);                                      \
		RET_VALUE(NAME);                                                      \
	}

#define CACHE_CAS(CACHE, V)                                                   \
	if (joint->CACHE == V) {                                                  \
		return;                                                               \
	}                                                                         \
	joint->CACHE = V;

#define CHECK_CONSTRAINT                                                      \
	if ( ! joint->_constraint ) {                                             \
		return;                                                               \
	}




Nan::Persistent<v8::Function> Joint::_constructor;


void Joint::init(Handle<Object> target) {
	
	Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(newCtor);
	
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(JS_STR("Joint"));
	
	// prototype
	Nan::SetPrototypeMethod(ctor, "destroy", destroy);
	
	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
	ACCESSOR_RW(proto, a);
	ACCESSOR_RW(proto, b);
	ACCESSOR_RW(proto, broken);
	ACCESSOR_RW(proto, posa);
	ACCESSOR_RW(proto, posb);
	ACCESSOR_RW(proto, rota);
	ACCESSOR_RW(proto, rotb);
	ACCESSOR_RW(proto, minl);
	ACCESSOR_RW(proto, maxl);
	ACCESSOR_RW(proto, mina);
	ACCESSOR_RW(proto, maxa);
	ACCESSOR_RW(proto, maximp);
	ACCESSOR_RW(proto, dampl);
	ACCESSOR_RW(proto, dampa);
	ACCESSOR_RW(proto, stifl);
	ACCESSOR_RW(proto, stifa);
	ACCESSOR_RW(proto, springl);
	ACCESSOR_RW(proto, springa);
	ACCESSOR_RW(proto, motorl);
	ACCESSOR_RW(proto, motora);
	ACCESSOR_RW(proto, motorlf);
	ACCESSOR_RW(proto, motoraf);
	ACCESSOR_RW(proto, motorlv);
	ACCESSOR_RW(proto, motorav);
	
	_constructor.Reset(Nan::GetFunction(ctor).ToLocalChecked());
	Nan::Set(target, JS_STR("Joint"), Nan::GetFunction(ctor).ToLocalChecked());
	
}


NAN_METHOD(Joint::newCtor) {
	
	CTOR_CHECK("Joint");
	
	REQ_OBJ_ARG(0, emitter);
	
	Joint *joint = new Joint();
	joint->_emitter.Reset(emitter);
	joint->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


void Joint::_emit(int argc, Local<Value> argv[]) {
	
	if ( ! Nan::New(_emitter)->Has(JS_STR("emit")) ) {
		return;
	}
	
	Nan::Callback callback(Nan::New(_emitter)->Get(JS_STR("emit")).As<Function>());
	
	if ( ! callback.IsEmpty() ) {
		callback.Call(argc, argv);
	}
	
}


Joint::Joint() {
	
	_isDestroyed = false;
	
	_constraint = NULL;
	
	_cacheA = NULL;
	_cacheB = NULL;
	_cacheBroken = false;
	_cacheMaximp = 9001.f*9001.f;
	_cachePosa = btVector3(0.f, 0.f, 0.f);
	_cachePosb = btVector3(0.f, 0.f, 0.f);
	_cacheRota = btVector3(0.f, 0.f, 0.f);
	_cacheRotb = btVector3(0.f, 0.f, 0.f);
	_cacheMinl = btVector3(0.f, 0.f, 0.f);
	_cacheMaxl = btVector3(0.f, 0.f, 0.f);
	_cacheMina = btVector3(0.f, 0.f, 0.f);
	_cacheMaxa = btVector3(0.f, 0.f, 0.f);
	_cacheDampl = btVector3(1.f, 1.f, 1.f);
	_cacheDampa = btVector3(1.f, 1.f, 1.f);
	_cacheStifl = btVector3(0.f, 0.f, 0.f);
	_cacheStifa = btVector3(0.f, 0.f, 0.f);
	_cacheSpringl = btVector3(0.f, 0.f, 0.f);
	_cacheSpringa = btVector3(0.f, 0.f, 0.f);
	_cacheMotorl = btVector3(0.f, 0.f, 0.f);
	_cacheMotora = btVector3(0.f, 0.f, 0.f);
	_cacheMotorlv = btVector3(0.f, 0.f, 0.f);
	_cacheMotorav = btVector3(0.f, 0.f, 0.f);
	_cacheMotorlf = btVector3(0.f, 0.f, 0.f);
	_cacheMotoraf = btVector3(0.f, 0.f, 0.f);
	
	_throttle = false;
	_asleep = false;
	
}


Joint::~Joint() {
	
	_destroy();
	
}

void Joint::_destroy() { DES_CHECK;
	
	if (_cacheA) {
		_cacheA->unrefJoint(this);
	}
	
	if (_cacheB) {
		_cacheB->unrefJoint(this);
	}
	
	if (_constraint) {
		_cacheA->getWorld()->removeConstraint(_constraint);
	}
	
	_cacheA = NULL;
	_cacheB = NULL;
	
	delete _constraint;
	_constraint = NULL;
	
	_isDestroyed = true;
	
	// Emit "destroy"
	Local<Value> argv = JS_STR("destroy");
	_emit(1, &argv);
	
}


void Joint::__update(bool asleep) { DES_CHECK;
	
	if ( ! _constraint ) {
		return;
	}
	
	_throttle = !_throttle;
	
	if (_throttle) {
		_asleep = asleep;
		return;
	}
	
	if (_asleep && asleep) {
		return;
	}
	
	const btVector3 &a = _cacheA->getPos();
	const btVector3 &b = _cacheB->getPos();
	
	// Emit "update"
	VEC3_TO_OBJ(a, posa);
	VEC3_TO_OBJ(b, posb);
	
	Local<Object> obj = Nan::New<Object>();
	SET_PROP(obj, "posa", posa);
	SET_PROP(obj, "posb", posb);
	SET_PROP(obj, "broken", JS_BOOL(_cacheBroken));
	
	Local<Value> argv[2] = { JS_STR("update"), obj };
	_emit(2, argv);
	
}


NAN_METHOD(Joint::destroy) { THIS_JOINT; THIS_CHECK;
	
	joint->_destroy();
	
}


V3_GETTER(posa, _cachePosa);
V3_GETTER(posb, _cachePosb);
V3_GETTER(rota, _cacheRota);
V3_GETTER(rotb, _cacheRotb);
V3_GETTER(minl, _cacheMinl);
V3_GETTER(maxl, _cacheMaxl);
V3_GETTER(mina, _cacheMina);
V3_GETTER(maxa, _cacheMaxa);
V3_GETTER(dampl, _cacheDampl);
V3_GETTER(dampa, _cacheDampa);
V3_GETTER(stifl, _cacheStifl);
V3_GETTER(stifa, _cacheStifa);
V3_GETTER(springl, _cacheSpringl);
V3_GETTER(springa, _cacheSpringa);
V3_GETTER(motorl, _cacheMotorl);
V3_GETTER(motora, _cacheMotora);
V3_GETTER(motorlf, _cacheMotorlf);
V3_GETTER(motoraf, _cacheMotoraf);
V3_GETTER(motorlv, _cacheMotorlv);
V3_GETTER(motorav, _cacheMotorav);



NAN_SETTER(Joint::aSetter) { THIS_JOINT; THIS_CHECK; SETTER_OBJ_ARG;
	
	Body *body = ObjectWrap::Unwrap<Body>(v);
	
	if (joint->_cacheA == body) {
		return;
	}
	
	if (joint->_cacheA) {
		joint->_cacheA->unrefJoint(joint);
		joint->_removeConstraint(joint->_cacheA->getWorld());
	}
	
	joint->_cacheA = body;
	
	if (joint->_cacheA) {
		joint->_cacheA->refJoint(joint);
	}
	
	joint->_rebuild();
	
	// Emit "a"
	if (joint->_cacheA) {
		Local<Value> argv[2] = {
			JS_STR("a"), Nan::New(joint->_cacheA->getEmitter())
		};
		joint->_emit(2, argv);
	} else {
		Local<Value> argv[2] = { JS_STR("a"), Nan::Null() };
		joint->_emit(2, argv);
	}
	
}

NAN_GETTER(Joint::aGetter) { THIS_JOINT; THIS_CHECK;
	
	if (joint->_cacheA) {
		RET_VALUE(Nan::New(joint->_cacheA->getEmitter()));
	} else {
		RET_VALUE(Nan::Null());
	}
	
}


NAN_SETTER(Joint::bSetter) { THIS_JOINT; THIS_CHECK; SETTER_OBJ_ARG;
	
	Body *body = ObjectWrap::Unwrap<Body>(v);
	
	if (joint->_cacheB == body) {
		return;
	}
	
	if (joint->_cacheB) {
		joint->_cacheB->unrefJoint(joint);
		joint->_removeConstraint(joint->_cacheB->getWorld());
	}
	
	joint->_cacheB = body;
	
	if (joint->_cacheB) {
		joint->_cacheB->refJoint(joint);
	}
	
	joint->_rebuild();
	
	// Emit "b"
	if (joint->_cacheB) {
		Local<Value> argv[2] = {
			JS_STR("b"), Nan::New(joint->_cacheB->getEmitter())
		};
		joint->_emit(2, argv);
	} else {
		Local<Value> argv[2] = { JS_STR("b"), Nan::Null() };
		joint->_emit(2, argv);
	}
	
}

NAN_GETTER(Joint::bGetter) { THIS_JOINT; THIS_CHECK;
	
	if (joint->_cacheB) {
		RET_VALUE(Nan::New(joint->_cacheB->getEmitter()));
	} else {
		RET_VALUE(Nan::Null());
	}
	
}


NAN_SETTER(Joint::brokenSetter) { THIS_JOINT; THIS_CHECK; SETTER_BOOL_ARG;
	
	CACHE_CAS(_cacheBroken, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setEnabled( ! joint->_cacheBroken );
	
	// Emit "broken"
	Local<Value> argv[2] = { JS_STR("broken"), value };
	joint->_emit(2, argv);
	
}

NAN_GETTER(Joint::brokenGetter) { THIS_JOINT; THIS_CHECK;
	
	RET_VALUE(JS_BOOL(joint->_cacheBroken));
	
}


NAN_SETTER(Joint::maximpSetter) { THIS_JOINT; THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheMaximp, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setBreakingImpulseThreshold(joint->_cacheMaximp);
	
	// Emit "maximp"
	Local<Value> argv[2] = { JS_STR("maximp"), value };
	joint->_emit(2, argv);
	
}

NAN_GETTER(Joint::maximpGetter) { THIS_JOINT; THIS_CHECK;
	
	RET_VALUE(JS_NUM(joint->_cacheMaximp));
	
}


NAN_SETTER(Joint::posaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cachePosa, v);
	CHECK_CONSTRAINT;
	
	btTransform transformA = joint->_constraint->getFrameOffsetA();
	transformA.setOrigin(joint->_cachePosa);
	joint->_constraint->setFrames(transformA, joint->_constraint->getFrameOffsetB());
	
	// Emit "posa"
	Local<Value> argv[2] = { JS_STR("posa"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::posbSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cachePosb, v);
	CHECK_CONSTRAINT;
	
	btTransform transformB = joint->_constraint->getFrameOffsetB();
	transformB.setOrigin(joint->_cachePosb);
	joint->_constraint->setFrames(joint->_constraint->getFrameOffsetA(), transformB);
	
	// Emit "posb"
	Local<Value> argv[2] = { JS_STR("posb"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::rotaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheRota, v);
	CHECK_CONSTRAINT;
	
	v *= 0.01745329f;
	btQuaternion q;
	q.setEuler(v.y(), v.x(), v.z());
	
	btTransform transformA = joint->_constraint->getFrameOffsetA();
	transformA.setRotation(q);
	joint->_constraint->setFrames(transformA, joint->_constraint->getFrameOffsetB());
	
	// Emit "rota"
	Local<Value> argv[2] = { JS_STR("rota"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::rotbSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheRotb, v);
	CHECK_CONSTRAINT;
	
	v *= 0.01745329f;
	btQuaternion q;
	q.setEuler(v.y(), v.x(), v.z());
	
	btTransform transformB = joint->_constraint->getFrameOffsetB();
	transformB.setRotation(q);
	joint->_constraint->setFrames(joint->_constraint->getFrameOffsetA(), transformB);
	
	// Emit "rotb"
	Local<Value> argv[2] = { JS_STR("rotb"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::minlSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMinl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setLinearLowerLimit(v);
	
	// Emit "minl"
	Local<Value> argv[2] = { JS_STR("minl"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::maxlSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMaxl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setLinearUpperLimit(v);
	
	// Emit "maxl"
	Local<Value> argv[2] = { JS_STR("maxl"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::minaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMina, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setAngularLowerLimit(static_cast<float>(M_PI) * v);
	
	// Emit "mina"
	Local<Value> argv[2] = { JS_STR("mina"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::maxaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMaxa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setAngularUpperLimit(static_cast<float>(M_PI) * v);
	
	// Emit "maxa"
	Local<Value> argv[2] = { JS_STR("maxa"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::damplSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheDampl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setDamping(0, v.x());
	joint->_constraint->setDamping(1, v.y());
	joint->_constraint->setDamping(2, v.z());
	
	// Emit "dampl"
	Local<Value> argv[2] = { JS_STR("dampl"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::dampaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheDampa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setDamping(3, v.x());
	joint->_constraint->setDamping(4, v.y());
	joint->_constraint->setDamping(5, v.z());
	
	// Emit "dampa"
	Local<Value> argv[2] = { JS_STR("dampa"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::stiflSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheStifl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setStiffness(0, v.x());
	joint->_constraint->setStiffness(1, v.y());
	joint->_constraint->setStiffness(2, v.z());
	
	// Emit "stifl"
	Local<Value> argv[2] = { JS_STR("stifl"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::stifaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheStifa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setStiffness(3, v.x());
	joint->_constraint->setStiffness(4, v.y());
	joint->_constraint->setStiffness(5, v.z());
	
	// Emit "stifa"
	Local<Value> argv[2] = { JS_STR("stifa"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::springlSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheSpringl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->enableSpring(0, v.x());
	joint->_constraint->enableSpring(1, v.y());
	joint->_constraint->enableSpring(2, v.z());
	
	// Emit "springl"
	Local<Value> argv[2] = { JS_STR("springl"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::springaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheSpringa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->enableSpring(3, v.x());
	joint->_constraint->enableSpring(4, v.y());
	joint->_constraint->enableSpring(5, v.z());
	
	// Emit "springa"
	Local<Value> argv[2] = { JS_STR("springa"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::motorlSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getTranslationalLimitMotor()->m_enableMotor[0] = v.x();
	joint->_constraint->getTranslationalLimitMotor()->m_enableMotor[1] = v.y();
	joint->_constraint->getTranslationalLimitMotor()->m_enableMotor[2] = v.z();
	
	// Emit "motorl"
	Local<Value> argv[2] = { JS_STR("motorl"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::motoraSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotora, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getRotationalLimitMotor(0)->m_enableMotor = v.x() != 0;
	joint->_constraint->getRotationalLimitMotor(1)->m_enableMotor = v.y() != 0;
	joint->_constraint->getRotationalLimitMotor(2)->m_enableMotor = v.z() != 0;
	
	// Emit "motora"
	Local<Value> argv[2] = { JS_STR("motora"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::motorlfSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorlf, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getTranslationalLimitMotor()->m_maxMotorForce = v;
	
	// Emit "motorlf"
	Local<Value> argv[2] = { JS_STR("motorlf"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::motorafSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotoraf, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getRotationalLimitMotor(0)->m_maxMotorForce = v.x();
	joint->_constraint->getRotationalLimitMotor(1)->m_maxMotorForce = v.y();
	joint->_constraint->getRotationalLimitMotor(2)->m_maxMotorForce = v.z();
	
	// Emit "motoraf"
	Local<Value> argv[2] = { JS_STR("motoraf"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::motorlvSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorlv, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getTranslationalLimitMotor()->m_targetVelocity = v;
	
	// Emit "motorlv"
	Local<Value> argv[2] = { JS_STR("motorlv"), value };
	joint->_emit(2, argv);
	
}


NAN_SETTER(Joint::motoravSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorav, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getRotationalLimitMotor(0)->m_targetVelocity = v.x();
	joint->_constraint->getRotationalLimitMotor(1)->m_targetVelocity = v.y();
	joint->_constraint->getRotationalLimitMotor(2)->m_targetVelocity = v.z();
	
	// Emit "motorav"
	Local<Value> argv[2] = { JS_STR("motorav"), value };
	joint->_emit(2, argv);
	
}



void Joint::_dropBody(Body *body) { DES_CHECK;
	
	if ( ! body ) {
		return;
	}
	
	if (body == _cacheA) {
		_removeConstraint(_cacheA->getWorld());
		_cacheA = NULL;
	} else if (body == _cacheB) {
		_removeConstraint(_cacheB->getWorld());
		_cacheB = NULL;
	} else {
		return;
	}
	
	_rebuild();
	
}


void Joint::_rebuild() { DES_CHECK;
	
	if (_cacheA) {
		_removeConstraint(_cacheA->getWorld());
	} else if (_cacheB) {
		_removeConstraint(_cacheB->getWorld());
	}
	
	if( ! (_cacheA && _cacheB) ) {
		return;
	}
	
	btRigidBody *rb1 = _cacheA->getBody();
	btRigidBody *rb2 = _cacheB->getBody();
	
	btTransform transformA;
	transformA.setIdentity();
	transformA.setOrigin(_cachePosa);
	btVector3 r = _cacheRota * 0.01745329f;
	btQuaternion q;
	q.setEuler(r.getY(), r.getX(), r.getZ());
	transformA.setRotation(q);
	
	btTransform transformB;
	transformB.setIdentity();
	transformB.setOrigin(_cachePosb);
	r = _cacheRotb * 0.01745329f;
	q.setEuler(r.getY(), r.getX(), r.getZ());
	transformB.setRotation(q);
	
	_constraint = new btGeneric6DofSpringConstraint(*rb1, *rb2, transformA, transformB, true);
	_cacheA->getWorld()->addConstraint(_constraint, true);
	_constraint->setEnabled( ! _cacheBroken );
	
	_constraint->setLinearLowerLimit(_cacheMinl);
	_constraint->setLinearUpperLimit(_cacheMaxl);
	_constraint->setAngularLowerLimit(static_cast<float>(M_PI) * _cacheMina);
	_constraint->setAngularUpperLimit(static_cast<float>(M_PI) * _cacheMaxa);
	
	// Holy fucking props
	_constraint->setBreakingImpulseThreshold(_cacheMaximp);
	
	_constraint->setDamping(0, _cacheDampl.getX());
	_constraint->setDamping(1, _cacheDampl.getY());
	_constraint->setDamping(2, _cacheDampl.getZ());
	_constraint->setDamping(3, _cacheDampa.getX());
	_constraint->setDamping(4, _cacheDampa.getY());
	_constraint->setDamping(5, _cacheDampa.getZ());
	
	_constraint->setStiffness(0, _cacheStifl.getX());
	_constraint->setStiffness(1, _cacheStifl.getY());
	_constraint->setStiffness(2, _cacheStifl.getZ());
	_constraint->setStiffness(3, _cacheStifa.getX());
	_constraint->setStiffness(4, _cacheStifa.getY());
	_constraint->setStiffness(5, _cacheStifa.getZ());
	
	_constraint->enableSpring(0, _cacheSpringl.getX() > 0);
	_constraint->enableSpring(1, _cacheSpringl.getY() > 0);
	_constraint->enableSpring(2, _cacheSpringl.getZ() > 0);
	_constraint->enableSpring(3, _cacheSpringa.getX() > 0);
	_constraint->enableSpring(4, _cacheSpringa.getY() > 0);
	_constraint->enableSpring(5, _cacheSpringa.getZ() > 0);
	
	_constraint->getTranslationalLimitMotor()->m_enableMotor[0] = _cacheMotorl.getX() > 0;
	_constraint->getTranslationalLimitMotor()->m_enableMotor[1] = _cacheMotorl.getY() > 0;
	_constraint->getTranslationalLimitMotor()->m_enableMotor[2] = _cacheMotorl.getZ() > 0;
	
	_constraint->getRotationalLimitMotor(0)->m_enableMotor = _cacheMotora.getX() > 0;
	_constraint->getRotationalLimitMotor(1)->m_enableMotor = _cacheMotora.getY() > 0;
	_constraint->getRotationalLimitMotor(2)->m_enableMotor = _cacheMotora.getZ() > 0;
	
	_constraint->getTranslationalLimitMotor()->m_maxMotorForce = _cacheMotorlf;
	_constraint->getRotationalLimitMotor(0)->m_maxMotorForce = _cacheMotoraf.getX();
	_constraint->getRotationalLimitMotor(1)->m_maxMotorForce = _cacheMotoraf.getY();
	_constraint->getRotationalLimitMotor(2)->m_maxMotorForce = _cacheMotoraf.getZ();
	
	_constraint->getTranslationalLimitMotor()->m_targetVelocity = _cacheMotorlv;
	_constraint->getRotationalLimitMotor(0)->m_targetVelocity = _cacheMotorav.getX();
	_constraint->getRotationalLimitMotor(1)->m_targetVelocity = _cacheMotorav.getY();
	_constraint->getRotationalLimitMotor(2)->m_targetVelocity = _cacheMotorav.getZ();
	
}


void Joint::_removeConstraint(btDynamicsWorld *world) { DES_CHECK;
	
	if (_constraint) {
		world->removeConstraint(_constraint);
		delete _constraint;
		_constraint = NULL;
	}
	
}
