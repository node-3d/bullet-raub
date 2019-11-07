#ifndef BODY_HPP
#define BODY_HPP

#include "common.hpp"


class btRigidBody;
class btCollisionShape;

class Heightmap;
class Joint;
class Scene;
class Trimesh;


ATTRIBUTE_ALIGNED16(class) Body : public Common {
DECLARE_ES5_CLASS(Body, Body);
	
public:
	static void init(Napi::Env env, Napi::Object exports);
	
	~Body();
	explicit Body(const Napi::CallbackInfo &info);
	
	void _destroy();
	
	void *operator new (size_t size) { return btAlignedAlloc(size, 16); }
	void operator delete (void *ptr) { btAlignedFree(ptr); }
	
	void refJoint(Joint *joint);
	void unrefJoint(Joint *joint);
	
	btDynamicsWorld *getWorld();
	btRigidBody *getBody() { return _body; }
	const btVector3 &getPos() { return _cachePos; }
	Napi::Object asJsObject() { return _that.Value(); }
	
	// called within engine
	void __update();
	
	
private:
	
	void _rebuild();
	
	btVector3 _calcScale() const;
	
	Napi::ObjectReference _sceneObj;
	Scene *_scene;
	btAlignedObjectArray<Joint*> _joints;
	
	btCollisionShape *_cshape;
	btRigidBody *_body;
	
	std::string _cacheType;
	btVector3 _cachePos;
	btQuaternion _cacheRot;
	btVector3 _cacheVell;
	btVector3 _cacheVela;
	btVector3 _cacheSize;
	Heightmap *_cacheMap;
	Trimesh *_cacheMesh;
	float _cacheMass;
	float _cacheRest;
	float _cacheDampl;
	float _cacheDampa;
	btVector3 _cacheFactl;
	btVector3 _cacheFacta;
	float _cacheFrict;
	bool _cacheSleepy;
	
	JS_DECLARE_METHOD(Body, destroy);
	JS_DECLARE_GETTER(Body, isDestroyed);
	
	JS_DECLARE_GETTER(Body, type);
	JS_DECLARE_SETTER(Body, type);
	
	JS_DECLARE_GETTER(Body, pos);
	JS_DECLARE_SETTER(Body, pos);
	
	JS_DECLARE_GETTER(Body, rot);
	JS_DECLARE_SETTER(Body, rot);
	
	JS_DECLARE_GETTER(Body, vell);
	JS_DECLARE_SETTER(Body, vell);
	
	JS_DECLARE_GETTER(Body, vela);
	JS_DECLARE_SETTER(Body, vela);
	
	JS_DECLARE_GETTER(Body, size);
	JS_DECLARE_SETTER(Body, size);
	
	JS_DECLARE_GETTER(Body, map);
	JS_DECLARE_SETTER(Body, map);
	
	JS_DECLARE_GETTER(Body, mesh);
	JS_DECLARE_SETTER(Body, mesh);
	
	JS_DECLARE_GETTER(Body, mass);
	JS_DECLARE_SETTER(Body, mass);
	
	JS_DECLARE_GETTER(Body, rest);
	JS_DECLARE_SETTER(Body, rest);
	
	JS_DECLARE_GETTER(Body, dampl);
	JS_DECLARE_SETTER(Body, dampl);
	
	JS_DECLARE_GETTER(Body, dampa);
	JS_DECLARE_SETTER(Body, dampa);
	
	JS_DECLARE_GETTER(Body, factl);
	JS_DECLARE_SETTER(Body, factl);
	
	JS_DECLARE_GETTER(Body, facta);
	JS_DECLARE_SETTER(Body, facta);
	
	JS_DECLARE_GETTER(Body, frict);
	JS_DECLARE_SETTER(Body, frict);
	
	JS_DECLARE_GETTER(Body, sleepy);
	JS_DECLARE_SETTER(Body, sleepy);
	
};


#endif // BODY_HPP
