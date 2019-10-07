#ifndef _BODY_HPP_
#define _BODY_HPP_

#include "common.hpp"


class btRigidBody;
class btCollisionShape;

class Heightmap;
class Joint;
class Scene;
class Trimesh;


ATTRIBUTE_ALIGNED16(class) Body : Common {
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
	
	JS_DECLARE_METHOD(destroy);
	JS_DECLARE_GETTER(isDestroyed);
	
	JS_DECLARE_GETTER(type);
	JS_DECLARE_SETTER(type);
	
	JS_DECLARE_GETTER(pos);
	JS_DECLARE_SETTER(pos);
	
	JS_DECLARE_GETTER(rot);
	JS_DECLARE_SETTER(rot);
	
	JS_DECLARE_GETTER(vell);
	JS_DECLARE_SETTER(vell);
	
	JS_DECLARE_GETTER(vela);
	JS_DECLARE_SETTER(vela);
	
	JS_DECLARE_GETTER(size);
	JS_DECLARE_SETTER(size);
	
	JS_DECLARE_GETTER(map);
	JS_DECLARE_SETTER(map);
	
	JS_DECLARE_GETTER(mesh);
	JS_DECLARE_SETTER(mesh);
	
	JS_DECLARE_GETTER(mass);
	JS_DECLARE_SETTER(mass);
	
	JS_DECLARE_GETTER(rest);
	JS_DECLARE_SETTER(rest);
	
	JS_DECLARE_GETTER(dampl);
	JS_DECLARE_SETTER(dampl);
	
	JS_DECLARE_GETTER(dampa);
	JS_DECLARE_SETTER(dampa);
	
	JS_DECLARE_GETTER(factl);
	JS_DECLARE_SETTER(factl);
	
	JS_DECLARE_GETTER(facta);
	JS_DECLARE_SETTER(facta);
	
	JS_DECLARE_GETTER(frict);
	JS_DECLARE_SETTER(frict);
	
	JS_DECLARE_GETTER(sleepy);
	JS_DECLARE_SETTER(sleepy);
	
};


#endif // _BODY_HPP_
