#include <cstdlib>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include <btRigidBody.h>
#include <btTypedConstraint.h>
#include <btDynamicsWorld.h>
#include <btGeneric6DofSpringConstraint.h>
#include <btGeneric6DofConstraint.h>

#include "scene.hpp"
#include "body.hpp"
#include "joint.hpp"

using namespace v8;
using namespace node;
using namespace std;


// ------ Aux macros

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


// ------ Constructor and Destructor

Joint::Joint() {
	
	_isDestroyed = false;
	
	_constraint = nullptr;
	
	_cacheA = nullptr;
	_cacheB = nullptr;
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
	
	if (_cacheA) {
		_removeConstraint(_cacheA->getWorld());
	} else if (_cacheB) {
		_removeConstraint(_cacheB->getWorld());
	}
	
	_cacheA = nullptr;
	_cacheB = nullptr;
	
	_isDestroyed = true;
	
}


// ------ Methods and props

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
	
	VEC3_TO_OBJ(a, posa);
	VEC3_TO_OBJ(b, posb);
	
	V8_VAR_OBJ obj = Nan::New<Object>();
	SET_PROP(obj, "posa", posa);
	SET_PROP(obj, "posb", posb);
	SET_PROP(obj, "broken", JS_BOOL(_cacheBroken));
	
	V8_VAR_VAL objVal = obj;
	emit("update", 1, &objVal);
	
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
	
	if (joint->_cacheA) {
		joint->emit("a", 1, &value);
	} else {
		V8_VAR_VAL nullVal = Nan::Null();
		joint->emit("a", 1, &nullVal);
	}
	
}


NAN_GETTER(Joint::aGetter) { THIS_JOINT; THIS_CHECK;
	
	if (joint->_cacheA) {
		RET_VALUE(joint->_cacheA->handle());
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
	
	if (joint->_cacheB) {
		joint->emit("b", 1, &value);
	} else {
		V8_VAR_VAL nullVal = Nan::Null();
		joint->emit("b", 1, &nullVal);
	}
	
}

NAN_GETTER(Joint::bGetter) { THIS_JOINT; THIS_CHECK;
	
	if (joint->_cacheB) {
		RET_VALUE(joint->_cacheB->handle());
	} else {
		RET_VALUE(Nan::Null());
	}
	
}


NAN_SETTER(Joint::brokenSetter) { THIS_JOINT; THIS_CHECK; SETTER_BOOL_ARG;
	
	CACHE_CAS(_cacheBroken, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setEnabled( ! joint->_cacheBroken );
	
	joint->emit("broken", 1, &value);
	
}


NAN_GETTER(Joint::brokenGetter) { THIS_JOINT; THIS_CHECK;
	
	RET_VALUE(JS_BOOL(joint->_cacheBroken));
	
}


NAN_SETTER(Joint::maximpSetter) { THIS_JOINT; THIS_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheMaximp, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setBreakingImpulseThreshold(joint->_cacheMaximp);
	
	joint->emit("maximp", 1, &value);
	
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
	
	joint->emit("posa", 1, &value);
	
}


NAN_SETTER(Joint::posbSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cachePosb, v);
	CHECK_CONSTRAINT;
	
	btTransform transformB = joint->_constraint->getFrameOffsetB();
	transformB.setOrigin(joint->_cachePosb);
	joint->_constraint->setFrames(joint->_constraint->getFrameOffsetA(), transformB);
	
	joint->emit("posb", 1, &value);
	
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
	
	joint->emit("rota", 1, &value);
	
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
	
	joint->emit("rotb", 1, &value);
	
}


NAN_SETTER(Joint::minlSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMinl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setLinearLowerLimit(v);
	
	joint->emit("minl", 1, &value);
	
}


NAN_SETTER(Joint::maxlSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMaxl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setLinearUpperLimit(v);
	
	joint->emit("maxl", 1, &value);
	
}


NAN_SETTER(Joint::minaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMina, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setAngularLowerLimit(static_cast<float>(M_PI) * v);
	
	joint->emit("mina", 1, &value);
	
}


NAN_SETTER(Joint::maxaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMaxa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setAngularUpperLimit(static_cast<float>(M_PI) * v);
	
	joint->emit("maxa", 1, &value);
	
}


NAN_SETTER(Joint::damplSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheDampl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setDamping(0, v.x());
	joint->_constraint->setDamping(1, v.y());
	joint->_constraint->setDamping(2, v.z());
	
	joint->emit("dampl", 1, &value);
	
}


NAN_SETTER(Joint::dampaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheDampa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setDamping(3, v.x());
	joint->_constraint->setDamping(4, v.y());
	joint->_constraint->setDamping(5, v.z());
	
	joint->emit("dampa", 1, &value);
	
}


NAN_SETTER(Joint::stiflSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheStifl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setStiffness(0, v.x());
	joint->_constraint->setStiffness(1, v.y());
	joint->_constraint->setStiffness(2, v.z());
	
	joint->emit("stifl", 1, &value);
	
}


NAN_SETTER(Joint::stifaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheStifa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setStiffness(3, v.x());
	joint->_constraint->setStiffness(4, v.y());
	joint->_constraint->setStiffness(5, v.z());
	
	joint->emit("stifa", 1, &value);
	
}


NAN_SETTER(Joint::springlSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheSpringl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->enableSpring(0, v.x());
	joint->_constraint->enableSpring(1, v.y());
	joint->_constraint->enableSpring(2, v.z());
	
	joint->emit("springl", 1, &value);
	
}


NAN_SETTER(Joint::springaSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheSpringa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->enableSpring(3, v.x());
	joint->_constraint->enableSpring(4, v.y());
	joint->_constraint->enableSpring(5, v.z());
	
	joint->emit("springa", 1, &value);
	
}


NAN_SETTER(Joint::motorlSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getTranslationalLimitMotor()->m_enableMotor[0] = v.x();
	joint->_constraint->getTranslationalLimitMotor()->m_enableMotor[1] = v.y();
	joint->_constraint->getTranslationalLimitMotor()->m_enableMotor[2] = v.z();
	
	joint->emit("motorl", 1, &value);
	
}


NAN_SETTER(Joint::motoraSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotora, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getRotationalLimitMotor(0)->m_enableMotor = v.x() != 0;
	joint->_constraint->getRotationalLimitMotor(1)->m_enableMotor = v.y() != 0;
	joint->_constraint->getRotationalLimitMotor(2)->m_enableMotor = v.z() != 0;
	
	joint->emit("motora", 1, &value);
	
}


NAN_SETTER(Joint::motorlfSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorlf, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getTranslationalLimitMotor()->m_maxMotorForce = v;
	
	joint->emit("motorlf", 1, &value);
	
}


NAN_SETTER(Joint::motorafSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotoraf, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getRotationalLimitMotor(0)->m_maxMotorForce = v.x();
	joint->_constraint->getRotationalLimitMotor(1)->m_maxMotorForce = v.y();
	joint->_constraint->getRotationalLimitMotor(2)->m_maxMotorForce = v.z();
	
	joint->emit("motoraf", 1, &value);
	
}


NAN_SETTER(Joint::motorlvSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorlv, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getTranslationalLimitMotor()->m_targetVelocity = v;
	
	joint->emit("motorlv", 1, &value);
	
}


NAN_SETTER(Joint::motoravSetter) { THIS_JOINT; THIS_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorav, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getRotationalLimitMotor(0)->m_targetVelocity = v.x();
	joint->_constraint->getRotationalLimitMotor(1)->m_targetVelocity = v.y();
	joint->_constraint->getRotationalLimitMotor(2)->m_targetVelocity = v.z();
	
	joint->emit("motorav", 1, &value);
	
}



void Joint::_dropBody(Body *body) { DES_CHECK;
	
	if ( ! body ) {
		return;
	}
	
	if (body == _cacheA) {
		_removeConstraint(_cacheA->getWorld());
		_cacheA = nullptr;
	} else if (body == _cacheB) {
		_removeConstraint(_cacheB->getWorld());
		_cacheB = nullptr;
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
	
	_constraint = new (
		btAlignedAlloc(sizeof(btGeneric6DofSpringConstraint), 16)
	) btGeneric6DofSpringConstraint(
		*rb1, *rb2, transformA, transformB, true
	);
	
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
	}
	
	ALIGNED_DELETE(btGeneric6DofSpringConstraint, _constraint);
	
}


// ------ System methods and props for ObjectWrap

V8_STORE_FT Joint::_protoJoint;
V8_STORE_FUNC Joint::_ctorJoint;


void Joint::init(V8_VAR_OBJ target) {
	
	V8_VAR_FT proto = Nan::New<FunctionTemplate>(newCtor);
	
	// class Joint inherits EventEmitter
	V8_VAR_FT parent = Nan::New(EventEmitter::_protoEventEmitter);
	proto->Inherit(parent);
	
	proto->InstanceTemplate()->SetInternalFieldCount(1);
	proto->SetClassName(JS_STR("Joint"));
	
	
	// Accessors
	
	V8_VAR_OT obj = proto->PrototypeTemplate();
	
	ACCESSOR_R(obj, isDestroyed);
	
	ACCESSOR_RW(obj, a);
	ACCESSOR_RW(obj, b);
	ACCESSOR_RW(obj, broken);
	ACCESSOR_RW(obj, posa);
	ACCESSOR_RW(obj, posb);
	ACCESSOR_RW(obj, rota);
	ACCESSOR_RW(obj, rotb);
	ACCESSOR_RW(obj, minl);
	ACCESSOR_RW(obj, maxl);
	ACCESSOR_RW(obj, mina);
	ACCESSOR_RW(obj, maxa);
	ACCESSOR_RW(obj, maximp);
	ACCESSOR_RW(obj, dampl);
	ACCESSOR_RW(obj, dampa);
	ACCESSOR_RW(obj, stifl);
	ACCESSOR_RW(obj, stifa);
	ACCESSOR_RW(obj, springl);
	ACCESSOR_RW(obj, springa);
	ACCESSOR_RW(obj, motorl);
	ACCESSOR_RW(obj, motora);
	ACCESSOR_RW(obj, motorlf);
	ACCESSOR_RW(obj, motoraf);
	ACCESSOR_RW(obj, motorlv);
	ACCESSOR_RW(obj, motorav);
	
	// -------- dynamic
	
	Nan::SetPrototypeMethod(proto, "destroy", destroy);
	
	
	// -------- static
	
	V8_VAR_FUNC ctor = Nan::GetFunction(proto).ToLocalChecked();
	
	_protoJoint.Reset(proto);
	_ctorJoint.Reset(ctor);
	
	Nan::Set(target, JS_STR("Joint"), ctor);
	
}


bool Joint::isJoint(V8_VAR_OBJ obj) {
	return Nan::New(_protoJoint)->HasInstance(obj);
}


NAN_METHOD(Joint::newCtor) {
	
	CTOR_CHECK("Joint");
	
	Joint *joint = new Joint();
	joint->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}



NAN_METHOD(Joint::destroy) { THIS_JOINT; THIS_CHECK;
	
	joint->emit("destroy");
	
	joint->_destroy();
	
}


NAN_GETTER(Joint::isDestroyedGetter) { THIS_JOINT;
	
	RET_VALUE(JS_BOOL(joint->_isDestroyed));
	
}

