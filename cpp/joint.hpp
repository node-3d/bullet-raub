#ifndef _JOINT_HPP_
#define _JOINT_HPP_


#include "common.hpp"


class btGeneric6DofSpringConstraint;

class Body;


ATTRIBUTE_ALIGNED16(class) Joint : public EventEmitter {
	
public:
	
	static void init(V8_VAR_OBJ target);
	static bool isJoint(V8_VAR_OBJ obj);
	
	~Joint();
	
	void _destroy();
	
	void _rebuild(); // called internally and by Body
	void _dropBody(Body *body); // called by Body
	
	void __update(bool asleep = false);
	
	
protected:
	
	Joint();
	
	static V8_STORE_FT _protoJoint; // for inheritance
	static V8_STORE_FUNC _ctorJoint;
	
	bool _isDestroyed;
	
	void _removeConstraint(btDynamicsWorld *world);
	
	btGeneric6DofSpringConstraint *_constraint;
	
	// Throttle every first __update, pass every second call
	bool _throttle;
	// Remember asleep every first __update
	// Compare every second call, both asleep -> nop
	bool _asleep;
	
	Body *_cacheA;
	Body *_cacheB;
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
	
	static NAN_METHOD(newCtor);
	static NAN_METHOD(destroy);
	static NAN_GETTER(isDestroyedGetter);
	
	static NAN_GETTER(aGetter);
	static NAN_SETTER(aSetter);
	
	static NAN_GETTER(bGetter);
	static NAN_SETTER(bSetter);
	
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
	
};


#endif // _JOINT_HPP_
