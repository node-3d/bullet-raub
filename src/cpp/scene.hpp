#pragma once

#include "common.hpp"


class btBroadphaseInterface;
class btCollisionDispatcher;
class btCollisionShape;
class btConstraintSolver;
class btDefaultCollisionConfiguration;
class btDynamicsWorld;
class btOverlappingPairCache;
class btClock;

class Body;
class Trace;


ATTRIBUTE_ALIGNED16(class) Scene : public Common {
DECLARE_ES5_CLASS(Scene, Scene);
public:
	static void init(Napi::Env env, Napi::Object exports);
	
	~Scene();
	explicit Scene(const Napi::CallbackInfo &info);
	
	void _destroy();
	
	void *operator new (size_t size) { return btAlignedAlloc(size, 16); }
	void operator delete (void *ptr) { btAlignedFree(ptr); }
	
	void refBody(Body* body);
	void unrefBody(Body* body);
	
	btDynamicsWorld *getWorld() { return _physWorld; }
	
	void doUpdate(float dt);
	void doUpdate();
	
	Napi::Object doHit(const btVector3 &from, const btVector3 &to);
	ObjVec doTrace(const btVector3 &from, const btVector3 &to);
	
private:
	btClock *_clock;
	std::vector<Body*> _bodies;
	
	btDefaultCollisionConfiguration *_physConfig;
	btCollisionDispatcher *_physDispatcher;
	btBroadphaseInterface *_physBroadphase;
	btConstraintSolver *_physSolver;
	btDynamicsWorld *_physWorld;
	
	btVector3 _cacheGrav;
	
	JS_DECLARE_METHOD(Scene, newCtor);
	JS_DECLARE_METHOD(Scene, destroy);
	JS_DECLARE_GETTER(Scene, isDestroyed);
	
	JS_DECLARE_GETTER(Scene, gravity);
	JS_DECLARE_SETTER(Scene, gravity);
	
	JS_DECLARE_METHOD(Scene, update);
	JS_DECLARE_METHOD(Scene, hit);
	JS_DECLARE_METHOD(Scene, trace);
};
