#ifndef _JOINT_HPP_
#define _JOINT_HPP_


#include <nan.h>

#include <LinearMath/btVector3.h>

#include "common.hpp"


class btGeneric6DofSpringConstraint;

class Body;


class Joint : public Nan::ObjectWrap {
	
public:
	
	static void init(v8::Handle<v8::Object> target);
	
	void _rebuild(); // called internally and by Body
	void _dropBody(Body *body); // called by Body
	
protected:
	
	explicit Joint();
	virtual ~Joint();
	
	static NAN_METHOD(newCtor);
	
	static NAN_GETTER(entaGetter);
	static NAN_SETTER(entaSetter);
	
	static NAN_GETTER(entbGetter);
	static NAN_SETTER(entbSetter);
	
	static NAN_GETTER(brokenGetter);
	static NAN_SETTER(brokenSetter);
	
	static NAN_GETTER(posaGetter);
	static NAN_SETTER(posaSetter);
	
	static NAN_GETTER(posbGetter);
	static NAN_SETTER(posbSetter);
	
	static NAN_GETTER(rotaGetter);
	static NAN_SETTER(rotaSetter);
	
	static NAN_GETTER(rotbGetter);
	static NAN_SETTER(rotbSetter);
	
	static NAN_GETTER(minlGetter);
	static NAN_SETTER(minlSetter);
	
	static NAN_GETTER(maxlGetter);
	static NAN_SETTER(maxlSetter);
	
	static NAN_GETTER(minaGetter);
	static NAN_SETTER(minaSetter);
	
	static NAN_GETTER(maxaGetter);
	static NAN_SETTER(maxaSetter);
	
	static NAN_GETTER(maximpGetter);
	static NAN_SETTER(maximpSetter);
	
	static NAN_GETTER(damplGetter);
	static NAN_SETTER(damplSetter);
	
	static NAN_GETTER(dampaGetter);
	static NAN_SETTER(dampaSetter);
	
	static NAN_GETTER(stiflGetter);
	static NAN_SETTER(stiflSetter);
	
	static NAN_GETTER(stifaGetter);
	static NAN_SETTER(stifaSetter);
	
	static NAN_GETTER(springlGetter);
	static NAN_SETTER(springlSetter);
	
	static NAN_GETTER(springaGetter);
	static NAN_SETTER(springaSetter);
	
	static NAN_GETTER(motorlGetter);
	static NAN_SETTER(motorlSetter);
	
	static NAN_GETTER(motoraGetter);
	static NAN_SETTER(motoraSetter);
	
	static NAN_GETTER(motorlfGetter);
	static NAN_SETTER(motorlfSetter);
	
	static NAN_GETTER(motorafGetter);
	static NAN_SETTER(motorafSetter);
	
	static NAN_GETTER(motorlvGetter);
	static NAN_SETTER(motorlvSetter);
	
	static NAN_GETTER(motoravGetter);
	static NAN_SETTER(motoravSetter);
	
	
private:
	
	static Nan::Persistent<v8::Function> _constructor;
	
	btGeneric6DofSpringConstraint *_constraint;
	
	
private: // prop cache
	Body *_cacheEnta;
	Body *_cacheEntb;
	bool _cacheBroken;
	float _cacheMaximp;
	btVector3 _cachePosa;
	btVector3 _cachePosb;
	btVector3 _cacheRota;
	btVector3 _cacheRotb;
	btVector3 _cacheMinl;
	btVector3 _cacheMaxl;
	btVector3 _cacheMina;
	btVector3 _cacheMaxa;
	btVector3 _cacheDampl;
	btVector3 _cacheDampa;
	btVector3 _cacheStifl;
	btVector3 _cacheStifa;
	btVector3 _cacheSpringl;
	btVector3 _cacheSpringa;
	btVector3 _cacheMotorl;
	btVector3 _cacheMotora;
	btVector3 _cacheMotorlf;
	btVector3 _cacheMotoraf;
	btVector3 _cacheMotorlv;
	btVector3 _cacheMotorav;
	
	
private:
	void _removeConstraint(btDynamicsWorld *world);
	
};


#endif // _JOINT_HPP_
