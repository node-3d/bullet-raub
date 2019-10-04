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
	JS_DECLARE_GETTER(isDestroyed);
	
	JS_DECLARE_GETTER(a);
	JS_DECLARE_SETTER(a);
	
	JS_DECLARE_GETTER(b);
	JS_DECLARE_SETTER(b);
	
	JS_DECLARE_GETTER(broken);
	JS_DECLARE_SETTER(broken);
	
	JS_DECLARE_GETTER(posa);
	JS_DECLARE_SETTER(posa);
	
	JS_DECLARE_GETTER(posb);
	JS_DECLARE_SETTER(posb);
	
	JS_DECLARE_GETTER(rota);
	JS_DECLARE_SETTER(rota);
	
	JS_DECLARE_GETTER(rotb);
	JS_DECLARE_SETTER(rotb);
	
	JS_DECLARE_GETTER(minl);
	JS_DECLARE_SETTER(minl);
	
	JS_DECLARE_GETTER(maxl);
	JS_DECLARE_SETTER(maxl);
	
	JS_DECLARE_GETTER(mina);
	JS_DECLARE_SETTER(mina);
	
	JS_DECLARE_GETTER(maxa);
	JS_DECLARE_SETTER(maxa);
	
	JS_DECLARE_GETTER(maximp);
	JS_DECLARE_SETTER(maximp);
	
	JS_DECLARE_GETTER(dampl);
	JS_DECLARE_SETTER(dampl);
	
	JS_DECLARE_GETTER(dampa);
	JS_DECLARE_SETTER(dampa);
	
	JS_DECLARE_GETTER(stifl);
	JS_DECLARE_SETTER(stifl);
	
	JS_DECLARE_GETTER(stifa);
	JS_DECLARE_SETTER(stifa);
	
	JS_DECLARE_GETTER(springl);
	JS_DECLARE_SETTER(springl);
	
	JS_DECLARE_GETTER(springa);
	JS_DECLARE_SETTER(springa);
	
	JS_DECLARE_GETTER(motorl);
	JS_DECLARE_SETTER(motorl);
	
	JS_DECLARE_GETTER(motora);
	JS_DECLARE_SETTER(motora);
	
	JS_DECLARE_GETTER(motorlf);
	JS_DECLARE_SETTER(motorlf);
	
	JS_DECLARE_GETTER(motoraf);
	JS_DECLARE_SETTER(motoraf);
	
	JS_DECLARE_GETTER(motorlv);
	JS_DECLARE_SETTER(motorlv);
	
	JS_DECLARE_GETTER(motorav);
	JS_DECLARE_SETTER(motorav);
	
};


#endif // _JOINT_HPP_
