#ifndef _BODY_HPP_
#define _BODY_HPP_

#include "common.hpp"


class btRigidBody;
class btCollisionShape;

class Heightmap;
class Joint;
class Scene;
class Trimesh;


ATTRIBUTE_ALIGNED16(class) Body {
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
	
	bool _isDestroyed;
	
	void _rebuild();
	
	btVector3 _calcScale() const;
	
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
	
	JS_DECLARE_METHOD(newCtor);
	JS_DECLARE_METHOD(destroy);
	JS_DECLARE_GETTER(isDestroyedGetter);
	
	JS_DECLARE_GETTER(typeGetter);
	JS_DECLARE_SETTER(typeSetter);
	
	JS_DECLARE_GETTER(posGetter);
	JS_DECLARE_SETTER(posSetter);
	
	JS_DECLARE_GETTER(rotGetter);
	JS_DECLARE_SETTER(rotSetter);
	
	JS_DECLARE_GETTER(vellGetter);
	JS_DECLARE_SETTER(vellSetter);
	
	JS_DECLARE_GETTER(velaGetter);
	JS_DECLARE_SETTER(velaSetter);
	
	JS_DECLARE_GETTER(sizeGetter);
	JS_DECLARE_SETTER(sizeSetter);
	
	JS_DECLARE_GETTER(mapGetter);
	JS_DECLARE_SETTER(mapSetter);
	
	JS_DECLARE_GETTER(meshGetter);
	JS_DECLARE_SETTER(meshSetter);
	
	JS_DECLARE_GETTER(massGetter);
	JS_DECLARE_SETTER(massSetter);
	
	JS_DECLARE_GETTER(restGetter);
	JS_DECLARE_SETTER(restSetter);
	
	JS_DECLARE_GETTER(damplGetter);
	JS_DECLARE_SETTER(damplSetter);
	
	JS_DECLARE_GETTER(dampaGetter);
	JS_DECLARE_SETTER(dampaSetter);
	
	JS_DECLARE_GETTER(factlGetter);
	JS_DECLARE_SETTER(factlSetter);
	
	JS_DECLARE_GETTER(factaGetter);
	JS_DECLARE_SETTER(factaSetter);
	
	JS_DECLARE_GETTER(frictGetter);
	JS_DECLARE_SETTER(frictSetter);
	
	JS_DECLARE_GETTER(sleepyGetter);
	JS_DECLARE_SETTER(sleepySetter);
	
};


#endif // _BODY_HPP_
