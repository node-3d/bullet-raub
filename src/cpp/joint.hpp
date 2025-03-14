#pragma once

#include "common.hpp"


class btGeneric6DofSpringConstraint;
class Body;


ATTRIBUTE_ALIGNED16(class) Joint : public Common {
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
	
	JS_DECLARE_METHOD(Joint, newCtor);
	JS_DECLARE_METHOD(Joint, destroy);
	JS_DECLARE_GETTER(Joint, isDestroyed);
	
	JS_DECLARE_GETTER(Joint, a);
	JS_DECLARE_SETTER(Joint, a);
	
	JS_DECLARE_GETTER(Joint, b);
	JS_DECLARE_SETTER(Joint, b);
	
	JS_DECLARE_GETTER(Joint, broken);
	JS_DECLARE_SETTER(Joint, broken);
	
	JS_DECLARE_GETTER(Joint, posa);
	JS_DECLARE_SETTER(Joint, posa);
	
	JS_DECLARE_GETTER(Joint, posb);
	JS_DECLARE_SETTER(Joint, posb);
	
	JS_DECLARE_GETTER(Joint, rota);
	JS_DECLARE_SETTER(Joint, rota);
	
	JS_DECLARE_GETTER(Joint, rotb);
	JS_DECLARE_SETTER(Joint, rotb);
	
	JS_DECLARE_GETTER(Joint, minl);
	JS_DECLARE_SETTER(Joint, minl);
	
	JS_DECLARE_GETTER(Joint, maxl);
	JS_DECLARE_SETTER(Joint, maxl);
	
	JS_DECLARE_GETTER(Joint, mina);
	JS_DECLARE_SETTER(Joint, mina);
	
	JS_DECLARE_GETTER(Joint, maxa);
	JS_DECLARE_SETTER(Joint, maxa);
	
	JS_DECLARE_GETTER(Joint, maximp);
	JS_DECLARE_SETTER(Joint, maximp);
	
	JS_DECLARE_GETTER(Joint, dampl);
	JS_DECLARE_SETTER(Joint, dampl);
	
	JS_DECLARE_GETTER(Joint, dampa);
	JS_DECLARE_SETTER(Joint, dampa);
	
	JS_DECLARE_GETTER(Joint, stifl);
	JS_DECLARE_SETTER(Joint, stifl);
	
	JS_DECLARE_GETTER(Joint, stifa);
	JS_DECLARE_SETTER(Joint, stifa);
	
	JS_DECLARE_GETTER(Joint, springl);
	JS_DECLARE_SETTER(Joint, springl);
	
	JS_DECLARE_GETTER(Joint, springa);
	JS_DECLARE_SETTER(Joint, springa);
	
	JS_DECLARE_GETTER(Joint, motorl);
	JS_DECLARE_SETTER(Joint, motorl);
	
	JS_DECLARE_GETTER(Joint, motora);
	JS_DECLARE_SETTER(Joint, motora);
	
	JS_DECLARE_GETTER(Joint, motorlf);
	JS_DECLARE_SETTER(Joint, motorlf);
	
	JS_DECLARE_GETTER(Joint, motoraf);
	JS_DECLARE_SETTER(Joint, motoraf);
	
	JS_DECLARE_GETTER(Joint, motorlv);
	JS_DECLARE_SETTER(Joint, motorlv);
	
	JS_DECLARE_GETTER(Joint, motorav);
	JS_DECLARE_SETTER(Joint, motorav);
};
