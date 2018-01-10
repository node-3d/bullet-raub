#include <cstdlib>
#include <iostream>

#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <btBulletDynamicsCommon.h>
#include <LinearMath/btAabbUtil2.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

#include "scene.hpp"
#include "body.hpp"
#include "trace.hpp"

using namespace v8;
using namespace node;
using namespace std;

#define THIS_TRACE                                                 \
	Trace *trace = ObjectWrap::Unwrap<Trace>(info.This());

#define V3_GETTER(NAME, CACHE)                                     \
	NAN_GETTER(Trace::NAME ## Getter) { NAN_HS; THIS_JOINT;        \
		VEC3_TO_OBJ(trace->CACHE, NAME);                           \
		RET_VALUE(NAME);                                           \
	}

#define CACHE_CAS(CACHE, V)                                        \
	if (trace->CACHE == V) {                                       \
		return;                                                    \
	}                                                              \
	trace->CACHE = V;




Persistent<Function> Trace::_constructor;


void Trace::init(Handle<Object> target) { NAN_HS;
	
	Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(newCtor);
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(JS_STR("Trace"));
	
	// prototype
	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
	ACCESSOR_RW(proto, hit);
	ACCESSOR_RW(proto, body);
	ACCESSOR_RW(proto, pos);
	ACCESSOR_RW(proto, norm);
	
	Nan::Set(target, JS_STR("Trace"), ctor->GetFunction());
	
	_constructor.Reset(Isolate::GetCurrent(), ctor->GetFunction());
	
}


NAN_METHOD(Trace::newCtor) { NAN_HS;
	
	Trace *trace = new Trace();
	trace->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


Trace::Trace() {
	
	_cacheHit = false;
	_cacheBody = NULL;
	_cachePos = btVector3(0, 0, 0);
	_cacheNorm = btVector3(0, 1, 0);
	
}

Trace::Trace(bool hit, Body *body, const btVector3 &pos, const btVector3 &norm) {
	
	_cacheHit = hit;
	_cacheBody = body;
	_cachePos = pos;
	_cacheNorm = norm;
	
}

Trace::~Trace() {
	
	if (_cacheEnta) {
		_cacheEnta->unrefTrace(this);
	}
	
	if (_cacheEntb) {
		_cacheEntb->unrefTrace(this);
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



NAN_SETTER(Trace::entaSetter) { NAN_HS; THIS_JOINT;
	
	REQ_OBJ_ARG(0, a);
	Body *body = ObjectWrap::Unwrap<Body>(a);
	
	if (_cacheEnta == body) {
		return;
	}
	
	if (_cacheEnta) {
		_cacheEnta->unrefTrace(this);
		_removeConstraint(_cacheEnta->getWorld());
	}
	
	_cacheEnta = body;
	
	if (_cacheEnta) {
		_cacheEnta->refTrace(this);
	}
	
	rebuild();
	
	// EMIT
	
}


NAN_SETTER(Trace::entbSetter) { NAN_HS; THIS_JOINT;
	
	REQ_OBJ_ARG(0, b);
	Body *body = ObjectWrap::Unwrap<Body>(b);
	
	if (_cacheEntb == body) {
		return;
	}
	
	if (_cacheEntb) {
		_cacheEntb->unrefTrace(this);
		_removeConstraint(_cacheEntb->getWorld());
	}
	
	_cacheEntb = body;
	
	if (_cacheEntb) {
		_cacheEntb->refTrace(this);
	}
	
	rebuild();
	
	// EMIT
	
}


NAN_SETTER(Trace::brokenSetter) { NAN_HS; THIS_JOINT;
	
	REQ_BOOL_ARG(0, v);
	
	CACHE_CAS(_cacheBroken, v);
	
	if (trace->_constraint) {
		trace->_constraint->setEnabled( ! trace->_cacheBroken );
	}
	
	// EMIT
	
}


NAN_SETTER(Trace::maximpSetter) { NAN_HS; THIS_JOINT;
	
	REQ_FLOAT_ARG(0, v);
	
	CACHE_CAS(_cacheMaximp, v);
	
	if (trace->_constraint) {
		trace->_constraint->setBreakingImpulseThreshold(trace->_cacheMaximp);
	}
	
	// EMIT
	
}






void Trace::_dropBody(Body *body) {
	
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


void Trace::_rebuild() {
	
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


void Trace::_removeConstraint(btDynamicsWorld *world) {
	
	if (_constraint) {
		world->removeConstraint(_constraint);
		delete _constraint;
		_constraint = NULL;
	}
	
}
