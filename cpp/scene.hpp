#ifndef _SCENE_HPP_
#define _SCENE_HPP_


#include <vector>

#include <nan.h>

#include <LinearMath/btVector3.h>

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


class Scene : public Nan::ObjectWrap {
	
public:
	
	static void init(v8::Handle<v8::Object> target);
	static void deinit();
	
	static void remember(Scene *scene);
	static void forget(Scene* scene);
	
	void refBody(Body *body);
	void unrefBody(Body *body);
	
	btDynamicsWorld *getWorld() { return _physWorld; }
	
	void doUpdate(float dt);
	void doUpdate();
	Trace *doHit(const btVector3 &from, const btVector3 &to);
	const std::vector<Trace*> &doTrace(const btVector3 &from, const btVector3 &to);
	
	
protected:
	
	explicit Scene();
	virtual ~Scene();
	
	static NAN_METHOD(newCtor);
	
	static NAN_GETTER(gravityGetter);
	static NAN_SETTER(gravitySetter);
	
	static NAN_METHOD(update);
	static NAN_METHOD(hit);
	static NAN_METHOD(trace);
	
	
private:
	
	static std::vector<Scene*> _scenes;
	static v8::Persistent<v8::Function> _constructor;
	
	btClock *_clock;
	std::vector<Body*> _bodies;
	
	btDefaultCollisionConfiguration *_physConfig;
	btCollisionDispatcher *_physDispatcher;
	btBroadphaseInterface *_physBroadphase;
	btConstraintSolver *_physSolver;
	btDynamicsWorld *_physWorld;
	
	btVector3 _cacheGrav;
	
};


#endif // _SCENE_HPP_
