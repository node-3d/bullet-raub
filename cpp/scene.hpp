#ifndef _SCENE_HPP_
#define _SCENE_HPP_


#include <nan.h>

#include <LinearMath/btVector3.h>

#include "common.hpp"


class btBroadphaseInterface;
class btCollisionShape;
class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
struct btCollisionAlgorithmCreateFunc;
class btDefaultCollisionConfiguration;
class btDynamicsWorld;

class Trace;
class Body;


class Scene : public node::ObjectWrap {
	
public:
	
	static void Initialize (v8::Handle<v8::Object> target);
	static void AtExit();
	
	btDynamicsWorld *getWorld() { return physWorld; }
	
	const btVector3 &getGravity() { return cacheGrav; }
	void setGravity(const btVector3 &v);
	
	void doUpdate(float dt);
	Trace *doHit(const btVector3 &from, const btVector3 &to);
	vector<Trace*> doTrace(const btVector3 &from, const btVector3 &to);
	
	
protected:
	
	explicit Scene();
	virtual ~Scene();
	
	static NAN_METHOD(newCtor);
	
	static NAN_GETTER(gravityGetter);
	static NAN_SETTER(gravitySetter);
	
	static NAN_METHOD(update);
	static NAN_METHOD(trace);
	static NAN_METHOD(hit);
	
	
private:
	
	static v8::Persistent<v8::Function> constructor_template;
	
	btDefaultCollisionConfiguration *physConfig;
	btCollisionDispatcher *physDispatcher;
	btBroadphaseInterface *physBroadphase;
	btConstraintSolver *physSolver;
	btDynamicsWorld *physWorld;
	
	btVector3 cacheGrav;
	
};


#endif // _SCENE_HPP_
