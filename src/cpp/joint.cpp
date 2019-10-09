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


#define CHECK_CONSTRAINT                                                      \
	if ( ! _constraint ) {                                                    \
		return;                                                               \
	}

IMPLEMENT_ES5_CLASS(Joint);


void Joint::init(Napi::Env env, Napi::Object exports) {
	
	Napi::Function ctor = wrap(env);
	
	JS_ASSIGN_GETTER(isDestroyed);
	
	JS_ASSIGN_GETTER(a);
	JS_ASSIGN_GETTER(b);
	JS_ASSIGN_GETTER(broken);
	JS_ASSIGN_GETTER(posa);
	JS_ASSIGN_GETTER(posb);
	JS_ASSIGN_GETTER(rota);
	JS_ASSIGN_GETTER(rotb);
	JS_ASSIGN_GETTER(minl);
	JS_ASSIGN_GETTER(maxl);
	JS_ASSIGN_GETTER(mina);
	JS_ASSIGN_GETTER(maxa);
	JS_ASSIGN_GETTER(maximp);
	JS_ASSIGN_GETTER(dampl);
	JS_ASSIGN_GETTER(dampa);
	JS_ASSIGN_GETTER(stifl);
	JS_ASSIGN_GETTER(stifa);
	JS_ASSIGN_GETTER(springl);
	JS_ASSIGN_GETTER(springa);
	JS_ASSIGN_GETTER(motorl);
	JS_ASSIGN_GETTER(motora);
	JS_ASSIGN_GETTER(motorlf);
	JS_ASSIGN_GETTER(motoraf);
	JS_ASSIGN_GETTER(motorlv);
	JS_ASSIGN_GETTER(motorav);
	
	JS_ASSIGN_SETTER(a);
	JS_ASSIGN_SETTER(b);
	JS_ASSIGN_SETTER(broken);
	JS_ASSIGN_SETTER(posa);
	JS_ASSIGN_SETTER(posb);
	JS_ASSIGN_SETTER(rota);
	JS_ASSIGN_SETTER(rotb);
	JS_ASSIGN_SETTER(minl);
	JS_ASSIGN_SETTER(maxl);
	JS_ASSIGN_SETTER(mina);
	JS_ASSIGN_SETTER(maxa);
	JS_ASSIGN_SETTER(maximp);
	JS_ASSIGN_SETTER(dampl);
	JS_ASSIGN_SETTER(dampa);
	JS_ASSIGN_SETTER(stifl);
	JS_ASSIGN_SETTER(stifa);
	JS_ASSIGN_SETTER(springl);
	JS_ASSIGN_SETTER(springa);
	JS_ASSIGN_SETTER(motorl);
	JS_ASSIGN_SETTER(motora);
	JS_ASSIGN_SETTER(motorlf);
	JS_ASSIGN_SETTER(motoraf);
	JS_ASSIGN_SETTER(motorlv);
	JS_ASSIGN_SETTER(motorav);
	
	exports.Set("Joint", ctor);
	
}


Joint::Joint(const Napi::CallbackInfo &info):
Common(info.This(), "Joint") { NAPI_ENV;
	
	super(info);
	
	_constraint = nullptr;
	
	_cacheA = nullptr;
	_cacheB = nullptr;
	_cacheBroken = false;
	_cacheMaximp = 9001.f * 9001.f;
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
	
	emit("destroy");
	
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
	
	Common::_destroy();
	
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
	
	Napi::Env env = _that.Env();
	
	const btVector3 &a = _cacheA->getPos();
	const btVector3 &b = _cacheB->getPos();
	
	VEC3_TO_OBJ(a, posa);
	VEC3_TO_OBJ(b, posb);
	
	Napi::Object obj = Napi::Object::New(env);
	obj.Set("posa", posa);
	obj.Set("posb", posb);
	obj.Set("broken", _cacheBroken);
	
	Napi::Value objVal = obj;
	emit("update", 1, &objVal);
	
}


V3_GETTER(Joint, posa, _cachePosa);
V3_GETTER(Joint, posb, _cachePosb);
V3_GETTER(Joint, rota, _cacheRota);
V3_GETTER(Joint, rotb, _cacheRotb);
V3_GETTER(Joint, minl, _cacheMinl);
V3_GETTER(Joint, maxl, _cacheMaxl);
V3_GETTER(Joint, mina, _cacheMina);
V3_GETTER(Joint, maxa, _cacheMaxa);
V3_GETTER(Joint, dampl, _cacheDampl);
V3_GETTER(Joint, dampa, _cacheDampa);
V3_GETTER(Joint, stifl, _cacheStifl);
V3_GETTER(Joint, stifa, _cacheStifa);
V3_GETTER(Joint, springl, _cacheSpringl);
V3_GETTER(Joint, springa, _cacheSpringa);
V3_GETTER(Joint, motorl, _cacheMotorl);
V3_GETTER(Joint, motora, _cacheMotora);
V3_GETTER(Joint, motorlf, _cacheMotorlf);
V3_GETTER(Joint, motoraf, _cacheMotoraf);
V3_GETTER(Joint, motorlv, _cacheMotorlv);
V3_GETTER(Joint, motorav, _cacheMotorav);


JS_IMPLEMENT_SETTER(Joint, a) { THIS_SETTER_CHECK; SETTER_OBJ_ARG;
	
	Body *body = Body::unwrap(v);
	
	if (_cacheA == body) {
		return;
	}
	
	if (_cacheA) {
		_cacheA->unrefJoint(this);
		_removeConstraint(_cacheA->getWorld());
	}
	
	_cacheA = body;
	
	if (_cacheA) {
		_cacheA->refJoint(this);
	}
	
	_rebuild();
	
	if (_cacheA) {
		emit("a", 1, &value);
	} else {
		Napi::Value nullVal = env.Null();
		emit("a", 1, &nullVal);
	}
	
}


JS_IMPLEMENT_GETTER(Joint, a) { THIS_CHECK;
	
	if (_cacheA) {
		RET_VALUE(_cacheA->asJsObject());
	} else {
		RET_VALUE(env.Null());
	}
	
}


JS_IMPLEMENT_SETTER(Joint, b) { THIS_SETTER_CHECK; SETTER_OBJ_ARG;
	
	Body *body = Body::unwrap(v);
	
	if (_cacheB == body) {
		return;
	}
	
	if (_cacheB) {
		_cacheB->unrefJoint(this);
		_removeConstraint(_cacheB->getWorld());
	}
	
	_cacheB = body;
	
	if (_cacheB) {
		_cacheB->refJoint(this);
	}
	
	_rebuild();
	
	if (_cacheB) {
		emit("b", 1, &value);
	} else {
		Napi::Value nullVal = env.Null();
		emit("b", 1, &nullVal);
	}
	
}

JS_IMPLEMENT_GETTER(Joint, b) { THIS_CHECK;
	
	if (_cacheB) {
		RET_VALUE(_cacheB->asJsObject());
	} else {
		RET_VALUE(env.Null());
	}
	
}


JS_IMPLEMENT_SETTER(Joint, broken) { THIS_SETTER_CHECK; SETTER_BOOL_ARG;
	
	CACHE_CAS(_cacheBroken, v);
	CHECK_CONSTRAINT;
	
	_constraint->setEnabled( ! _cacheBroken );
	
	emit("broken", 1, &value);
	
}


JS_IMPLEMENT_GETTER(Joint, broken) { THIS_CHECK;
	
	RET_BOOL(_cacheBroken);
	
}


JS_IMPLEMENT_SETTER(Joint, maximp) { THIS_SETTER_CHECK; SETTER_FLOAT_ARG;
	
	CACHE_CAS(_cacheMaximp, v);
	CHECK_CONSTRAINT;
	
	_constraint->setBreakingImpulseThreshold(_cacheMaximp);
	
	emit("maximp", 1, &value);
	
}


JS_IMPLEMENT_GETTER(Joint, maximp) { THIS_CHECK;
	
	RET_NUM(_cacheMaximp);
	
}


JS_IMPLEMENT_SETTER(Joint, posa) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cachePosa, v);
	CHECK_CONSTRAINT;
	
	btTransform transformA = _constraint->getFrameOffsetA();
	transformA.setOrigin(_cachePosa);
	_constraint->setFrames(transformA, _constraint->getFrameOffsetB());
	
	emit("posa", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, posb) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cachePosb, v);
	CHECK_CONSTRAINT;
	
	btTransform transformB = _constraint->getFrameOffsetB();
	transformB.setOrigin(_cachePosb);
	_constraint->setFrames(_constraint->getFrameOffsetA(), transformB);
	
	emit("posb", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, rota) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheRota, v);
	CHECK_CONSTRAINT;
	
	v *= 0.01745329f;
	btQuaternion q;
	q.setEuler(v.y(), v.x(), v.z());
	
	btTransform transformA = _constraint->getFrameOffsetA();
	transformA.setRotation(q);
	_constraint->setFrames(transformA, _constraint->getFrameOffsetB());
	
	emit("rota", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, rotb) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheRotb, v);
	CHECK_CONSTRAINT;
	
	v *= 0.01745329f;
	btQuaternion q;
	q.setEuler(v.y(), v.x(), v.z());
	
	btTransform transformB = _constraint->getFrameOffsetB();
	transformB.setRotation(q);
	_constraint->setFrames(_constraint->getFrameOffsetA(), transformB);
	
	emit("rotb", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, minl) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMinl, v);
	CHECK_CONSTRAINT;
	
	_constraint->setLinearLowerLimit(v);
	
	emit("minl", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, maxl) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMaxl, v);
	CHECK_CONSTRAINT;
	
	_constraint->setLinearUpperLimit(v);
	
	emit("maxl", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, mina) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMina, v);
	CHECK_CONSTRAINT;
	
	_constraint->setAngularLowerLimit(static_cast<float>(M_PI) * v);
	
	emit("mina", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, maxa) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMaxa, v);
	CHECK_CONSTRAINT;
	
	_constraint->setAngularUpperLimit(static_cast<float>(M_PI) * v);
	
	emit("maxa", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, dampl) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheDampl, v);
	CHECK_CONSTRAINT;
	
	_constraint->setDamping(0, v.x());
	_constraint->setDamping(1, v.y());
	_constraint->setDamping(2, v.z());
	
	emit("dampl", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, dampa) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheDampa, v);
	CHECK_CONSTRAINT;
	
	_constraint->setDamping(3, v.x());
	_constraint->setDamping(4, v.y());
	_constraint->setDamping(5, v.z());
	
	emit("dampa", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, stifl) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheStifl, v);
	CHECK_CONSTRAINT;
	
	_constraint->setStiffness(0, v.x());
	_constraint->setStiffness(1, v.y());
	_constraint->setStiffness(2, v.z());
	
	emit("stifl", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, stifa) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheStifa, v);
	CHECK_CONSTRAINT;
	
	_constraint->setStiffness(3, v.x());
	_constraint->setStiffness(4, v.y());
	_constraint->setStiffness(5, v.z());
	
	emit("stifa", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, springl) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheSpringl, v);
	CHECK_CONSTRAINT;
	
	_constraint->enableSpring(0, v.x());
	_constraint->enableSpring(1, v.y());
	_constraint->enableSpring(2, v.z());
	
	emit("springl", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, springa) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheSpringa, v);
	CHECK_CONSTRAINT;
	
	_constraint->enableSpring(3, v.x());
	_constraint->enableSpring(4, v.y());
	_constraint->enableSpring(5, v.z());
	
	emit("springa", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, motorl) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorl, v);
	CHECK_CONSTRAINT;
	
	_constraint->getTranslationalLimitMotor()->m_enableMotor[0] = v.x();
	_constraint->getTranslationalLimitMotor()->m_enableMotor[1] = v.y();
	_constraint->getTranslationalLimitMotor()->m_enableMotor[2] = v.z();
	
	emit("motorl", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, motora) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotora, v);
	CHECK_CONSTRAINT;
	
	_constraint->getRotationalLimitMotor(0)->m_enableMotor = v.x() != 0;
	_constraint->getRotationalLimitMotor(1)->m_enableMotor = v.y() != 0;
	_constraint->getRotationalLimitMotor(2)->m_enableMotor = v.z() != 0;
	
	emit("motora", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, motorlf) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorlf, v);
	CHECK_CONSTRAINT;
	
	_constraint->getTranslationalLimitMotor()->m_maxMotorForce = v;
	
	emit("motorlf", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, motoraf) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotoraf, v);
	CHECK_CONSTRAINT;
	
	_constraint->getRotationalLimitMotor(0)->m_maxMotorForce = v.x();
	_constraint->getRotationalLimitMotor(1)->m_maxMotorForce = v.y();
	_constraint->getRotationalLimitMotor(2)->m_maxMotorForce = v.z();
	
	emit("motoraf", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, motorlv) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorlv, v);
	CHECK_CONSTRAINT;
	
	_constraint->getTranslationalLimitMotor()->m_targetVelocity = v;
	
	emit("motorlv", 1, &value);
	
}


JS_IMPLEMENT_SETTER(Joint, motorav) { THIS_SETTER_CHECK; SETTER_VEC3_ARG;
	
	CACHE_CAS(_cacheMotorav, v);
	CHECK_CONSTRAINT;
	
	_constraint->getRotationalLimitMotor(0)->m_targetVelocity = v.x();
	_constraint->getRotationalLimitMotor(1)->m_targetVelocity = v.y();
	_constraint->getRotationalLimitMotor(2)->m_targetVelocity = v.z();
	
	emit("motorav", 1, &value);
	
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


JS_IMPLEMENT_METHOD(Joint, destroy) { THIS_CHECK;
	emit("destroy");
	_destroy();
	RET_UNDEFINED;
}


JS_IMPLEMENT_GETTER(Joint, isDestroyed) { THIS_CHECK;
	RET_BOOL(_isDestroyed);
}
