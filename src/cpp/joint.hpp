#ifndef _JOINT_HPP_
#define _JOINT_HPP_

#include "common.hpp"


class btGeneric6DofSpringConstraint;

class Body;


ATTRIBUTE_ALIGNED16(class) Joint {
DECLARE_ES5_CLASS(Joint, Joint);
	
public:
	
	static void init(Napi::Env env, Napi::Object exports);
	
	~Joint();
	explicit Joint(const Napi::CallbackInfo &info);
	
	void _destroy();
	
	void *operator new (size_t size) { return btAlignedAlloc(size, 16); }
	void operator delete (void *ptr) { btAlignedFree(ptr); }
	
	void _rebuild(); // called internally and by Body
	void _dropBody(Body *body); // called by Body
	
	void __update(bool asleep = false);
	
	
private:
	
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
	
	JS_DECLARE_METHOD(newCtor);
	JS_DECLARE_METHOD(destroy);
	JS_DECLARE_GETTER(isDestroyedGetter);
	
	JS_DECLARE_GETTER(aGetter);
	JS_DECLARE_SETTER(aSetter);
	
	JS_DECLARE_GETTER(bGetter);
	JS_DECLARE_SETTER(bSetter);
	
	JS_DECLARE_GETTER(brokenGetter);
	JS_DECLARE_SETTER(brokenSetter);
	
	JS_DECLARE_GETTER(posaGetter);
	JS_DECLARE_SETTER(posaSetter);
	
	JS_DECLARE_GETTER(posbGetter);
	JS_DECLARE_SETTER(posbSetter);
	
	JS_DECLARE_GETTER(rotaGetter);
	JS_DECLARE_SETTER(rotaSetter);
	
	JS_DECLARE_GETTER(rotbGetter);
	JS_DECLARE_SETTER(rotbSetter);
	
	JS_DECLARE_GETTER(minlGetter);
	JS_DECLARE_SETTER(minlSetter);
	
	JS_DECLARE_GETTER(maxlGetter);
	JS_DECLARE_SETTER(maxlSetter);
	
	JS_DECLARE_GETTER(minaGetter);
	JS_DECLARE_SETTER(minaSetter);
	
	JS_DECLARE_GETTER(maxaGetter);
	JS_DECLARE_SETTER(maxaSetter);
	
	JS_DECLARE_GETTER(maximpGetter);
	JS_DECLARE_SETTER(maximpSetter);
	
	JS_DECLARE_GETTER(damplGetter);
	JS_DECLARE_SETTER(damplSetter);
	
	JS_DECLARE_GETTER(dampaGetter);
	JS_DECLARE_SETTER(dampaSetter);
	
	JS_DECLARE_GETTER(stiflGetter);
	JS_DECLARE_SETTER(stiflSetter);
	
	JS_DECLARE_GETTER(stifaGetter);
	JS_DECLARE_SETTER(stifaSetter);
	
	JS_DECLARE_GETTER(springlGetter);
	JS_DECLARE_SETTER(springlSetter);
	
	JS_DECLARE_GETTER(springaGetter);
	JS_DECLARE_SETTER(springaSetter);
	
	JS_DECLARE_GETTER(motorlGetter);
	JS_DECLARE_SETTER(motorlSetter);
	
	JS_DECLARE_GETTER(motoraGetter);
	JS_DECLARE_SETTER(motoraSetter);
	
	JS_DECLARE_GETTER(motorlfGetter);
	JS_DECLARE_SETTER(motorlfSetter);
	
	JS_DECLARE_GETTER(motorafGetter);
	JS_DECLARE_SETTER(motorafSetter);
	
	JS_DECLARE_GETTER(motorlvGetter);
	JS_DECLARE_SETTER(motorlvSetter);
	
	JS_DECLARE_GETTER(motoravGetter);
	JS_DECLARE_SETTER(motoravSetter);
	
};


#endif // _JOINT_HPP_
