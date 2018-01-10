#include <cstdlib>
#include <iostream>
#include <vector>

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

#define THIS_JOINT                                                 \
	Joint *joint = ObjectWrap::Unwrap<Joint>(info.This());

#define V3_GETTER(NAME, CACHE)                                     \
	NAN_GETTER(Joint::NAME ## Getter) { NAN_HS; THIS_JOINT;        \
		VEC3_TO_OBJ(joint->CACHE, NAME);                           \
		RET_VALUE(NAME);                                           \
	}

#define CACHE_CAS(CACHE, V)                                        \
	if (joint->CACHE == V) {                                       \
		return;                                                    \
	}                                                              \
	joint->CACHE = V;

#define CHECK_CONSTRAINT                                           \
	if ( ! joint->_constraint ) {                                  \
		return;                                                    \
	}




Persistent<Function> Joint::_constructor;


void Joint::init(Handle<Object> target) { NAN_HS;
	
	Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(newCtor);
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(JS_STR("Joint"));
	
	// prototype
	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
	ACCESSOR_RW(proto, enta);
	ACCESSOR_RW(proto, entb);
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
	
	Nan::Set(target, JS_STR("Joint"), ctor->GetFunction());
	
	_constructor.Reset(Isolate::GetCurrent(), ctor->GetFunction());
	
}


NAN_METHOD(Joint::newCtor) { NAN_HS;
	
	Joint *joint = new Joint();
	joint->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


Joint::Joint() {
	
	_constraint = NULL;
	
	_cacheEnta = NULL;
	_cacheEntb = NULL;
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
	
}


Joint::~Joint() {
	
	if (_cacheEnta) {
		_cacheEnta->unrefJoint(this);
	}
	
	if (_cacheEntb) {
		_cacheEntb->unrefJoint(this);
	}
	
	if (_constraint) {
		_cacheEnta->getWorld()->removeConstraint(_constraint);
	}
	
	_cacheEnta = NULL;
	_cacheEntb = NULL;
	
	delete _constraint;
	_constraint = NULL;
	
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



NAN_SETTER(Joint::entaSetter) { THIS_JOINT;
	
	REQ_OBJ_ARG(0, a);
	Body *body = ObjectWrap::Unwrap<Body>(a);
	
	if (_cacheEnta == body) {
		return;
	}
	
	if (_cacheEnta) {
		_cacheEnta->unrefJoint(this);
		_removeConstraint(_cacheEnta->getWorld());
	}
	
	_cacheEnta = body;
	
	if (_cacheEnta) {
		_cacheEnta->refJoint(this);
	}
	
	rebuild();
	
	// EMIT
	
}


NAN_SETTER(Joint::entbSetter) { THIS_JOINT;
	
	REQ_OBJ_ARG(0, b);
	Body *body = ObjectWrap::Unwrap<Body>(b);
	
	if (_cacheEntb == body) {
		return;
	}
	
	if (_cacheEntb) {
		_cacheEntb->unrefJoint(this);
		_removeConstraint(_cacheEntb->getWorld());
	}
	
	_cacheEntb = body;
	
	if (_cacheEntb) {
		_cacheEntb->refJoint(this);
	}
	
	rebuild();
	
	// EMIT
	
}


NAN_SETTER(Joint::brokenSetter) { THIS_JOINT;
	
	REQ_BOOL_ARG(0, v);
	
	CACHE_CAS(_cacheBroken, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setEnabled( ! joint->_cacheBroken );
	
	// EMIT
	
}


NAN_SETTER(Joint::maximpSetter) { THIS_JOINT;
	
	REQ_FLOAT_ARG(0, v);
	
	CACHE_CAS(_cacheMaximp, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setBreakingImpulseThreshold(joint->_cacheMaximp);
	
	// EMIT
	
}


NAN_SETTER(Joint::posaSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cachePosa, v);
	CHECK_CONSTRAINT;
	
	btTransform transformA = _constraint->getFrameOffsetA();
	transformA.setOrigin(_cachePosa);
	_constraint->setFrames(transformA, _constraint->getFrameOffsetB());
	
	// EMIT
	
}


NAN_SETTER(Joint::posbSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cachePosb, v);
	CHECK_CONSTRAINT;
	
	btTransform transformB = _constraint->getFrameOffsetB();
	transformA.setOrigin(_cachePosb);
	joint->_constraint->setFrames(joint->_constraint->getFrameOffsetA(), transformB);
	
	// EMIT
	
}


NAN_SETTER(Joint::rotaSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheRota, v);
	CHECK_CONSTRAINT;
	
	v *= 0.01745329f;
	btQuaternion q;
	q.setEuler(v.y(), v.x(), v.z());
	
	btTransform transformA = joint->_constraint->getFrameOffsetA();
	transformA.setRotation(q);
	joint->_constraint->setFrames(transformA, joint->_constraint->getFrameOffsetB());
	
	// EMIT
	
}


NAN_SETTER(Joint::rotbSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheRotb, v);
	CHECK_CONSTRAINT;
	
	v *= 0.01745329f;
	btQuaternion q;
	q.setEuler(v.y(), v.x(), v.z());
	
	btTransform transformB = joint->_constraint->getFrameOffsetB();
	transformB.setRotation(q);
	joint->_constraint->setFrames(joint->_constraint->getFrameOffsetA(), transformB);
	
	// EMIT
	
}


NAN_SETTER(Joint::minlSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheMinl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setLinearLowerLimit(v);
	
	// EMIT
	
}


NAN_SETTER(Joint::maxlSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheMaxl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setLinearUpperLimit(v);
	
	// EMIT
	
}


NAN_SETTER(Joint::minaSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheMina, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setAngularLowerLimit(M_PI * v);
	
	// EMIT
	
}


NAN_SETTER(Joint::maxaSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheMaxa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setAngularUpperLimit(M_PI * v);
	
	// EMIT
	
}


NAN_SETTER(Joint::damplSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheDampl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setDamping(0, v.x());
	joint->_constraint->setDamping(1, v.y());
	joint->_constraint->setDamping(2, v.z());
	
	// EMIT
	
}


NAN_SETTER(Joint::dampaSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheDampa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setDamping(3, v.x());
	joint->_constraint->setDamping(4, v.y());
	joint->_constraint->setDamping(5, v.z());
	
	// EMIT
	
}


NAN_SETTER(Joint::stiflSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheStifl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setStiffness(0, v.x());
	joint->_constraint->setStiffness(1, v.y());
	joint->_constraint->setStiffness(2, v.z());
	
	// EMIT
	
}


NAN_SETTER(Joint::stifaSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheStifa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->setStiffness(3, v.x());
	joint->_constraint->setStiffness(4, v.y());
	joint->_constraint->setStiffness(5, v.z());
	
	// EMIT
	
}


NAN_SETTER(Joint::springlSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheSpringl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->enableSpring(0, v.x());
	joint->_constraint->enableSpring(1, v.y());
	joint->_constraint->enableSpring(2, v.z());
	
	// EMIT
	
}


NAN_SETTER(Joint::springaSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheSpringa, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->enableSpring(3, v.x());
	joint->_constraint->enableSpring(4, v.y());
	joint->_constraint->enableSpring(5, v.z());
	
	// EMIT
	
}


NAN_SETTER(Joint::motorlSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheMotorl, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getTranslationalLimitMotor()->m_enableMotor[0] = v.x();
	joint->_constraint->getTranslationalLimitMotor()->m_enableMotor[1] = v.y();
	joint->_constraint->getTranslationalLimitMotor()->m_enableMotor[2] = v.z();
	
	// EMIT
	
}


NAN_SETTER(Joint::motoraSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheMotora, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getRotationalLimitMotor(0)->m_enableMotor = value.x() != 0;
	joint->_constraint->getRotationalLimitMotor(1)->m_enableMotor = value.y() != 0;
	joint->_constraint->getRotationalLimitMotor(2)->m_enableMotor = value.z() != 0;
	
	// EMIT
	
}


NAN_SETTER(Joint::motorlfSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheMotorlf, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getTranslationalLimitMotor()->m_maxMotorForce = v;
	
	// EMIT
	
}


NAN_SETTER(Joint::motorafSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheMotoraf, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getRotationalLimitMotor(0)->m_maxMotorForce = value.x();
	joint->_constraint->getRotationalLimitMotor(1)->m_maxMotorForce = value.y();
	joint->_constraint->getRotationalLimitMotor(2)->m_maxMotorForce = value.z();
	
	// EMIT
	
}


NAN_SETTER(Joint::motorlvSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheMotorlv, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getTranslationalLimitMotor()->m_targetVelocity = v;
	
	// EMIT
	
}


NAN_SETTER(Joint::motoravSetter) { THIS_JOINT;
	
	REQ_VEC3_ARG(0, v);
	
	CACHE_CAS(_cacheMotorav, v);
	CHECK_CONSTRAINT;
	
	joint->_constraint->getRotationalLimitMotor(0)->m_targetVelocity = value.x();
	joint->_constraint->getRotationalLimitMotor(1)->m_targetVelocity = value.y();
	joint->_constraint->getRotationalLimitMotor(2)->m_targetVelocity = value.z();
	
	// EMIT
	
}



void Joint::_dropBody(Body *body) {
	
	if ( ! body ) {
		return;
	}
	
	if (body == _cacheEnta) {
		_removeConstraint(_cacheEnta->getWorld());
		_cacheEnta = NULL;
	} else if (body == _cacheEntb) {
		_removeConstraint(_cacheEntb->getWorld());
		_cacheEntb = NULL;
	} else {
		return;
	}
	
	_rebuild();
	
}


void Joint::_rebuild() {
	
	if (_cacheEnta) {
		_removeConstraint();
	} else if (_cacheEntb) {
		_removeConstraint();
	}
	
	if( ! (_cacheEnta && _cacheEntb) ) {
		return;
	}
	
	btRigidBody *rb1 = _cacheEnta->getBody();
	btRigidBody *rb2 = _cacheEntb->getBody();
	
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
	_cacheEnta->getWorld()->addConstraint(_constraint, true);
	_constraint->setEnabled( ! _cacheBroken );
	
	_constraint->setLinearLowerLimit(_cacheMinl);
	_constraint->setLinearUpperLimit(_cacheMaxl);
	_constraint->setAngularLowerLimit(M_PI * _cacheMina);
	_constraint->setAngularUpperLimit(M_PI * _cacheMaxa);
	
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


void Joint::_removeConstraint(btDynamicsWorld *world) {
	
	if (_constraint) {
		world->removeConstraint(_constraint);
		delete _constraint;
		_constraint = NULL;
	}
	
}
