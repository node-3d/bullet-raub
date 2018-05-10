#ifndef _BODY_HPP_
#define _BODY_HPP_


#include <vector>
// #include <cstdlib>

#include <LinearMath/btVector3.h>
#include <LinearMath/btQuaternion.h>

#include <event-emitter.hpp>

#include "common.hpp"


class btRigidBody;
class btCollisionShape;

class Heightmap;
class Joint;
class Scene;
class Trimesh;


/*__declspec(align(16))*/ class Body : public EventEmitter {
	
public:
	
	// void *operator new(size_t size) { return std::aligned_alloc(16, size); }
	// void operator delete(void *p) { std::free(p); }
	
	static void init(V8_VAR_OBJ target);
	static bool isBody(V8_VAR_OBJ obj);
	
	~Body();
	
	void _destroy();
	
	void refJoint(Joint *joint);
	void unrefJoint(Joint *joint);
	
	btDynamicsWorld *getWorld();
	btRigidBody *getBody() { return _body; }
	const btVector3 &getPos() { return _cachePos; }
	
	// called within engine
	void __update();
	
	
protected:
	
	explicit Body(Scene *scene);
	
	static V8_STORE_FT _protoBody; // for inheritance
	static V8_STORE_FUNC _ctorBody;
	
	bool _isDestroyed;
	
	void _rebuild();
	
	btVector3 _calcScale() const;
	
	Scene *_scene;
	std::vector<Joint*> _joints;
	
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
	
	
private:
	
	static NAN_METHOD(newCtor);
	static NAN_METHOD(destroy);
	static NAN_GETTER(isDestroyedGetter);
	
	static NAN_GETTER(typeGetter);
	static NAN_SETTER(typeSetter);
	
	static NAN_GETTER(posGetter);
	static NAN_SETTER(posSetter);
	
	static NAN_GETTER(rotGetter);
	static NAN_SETTER(rotSetter);
	
	static NAN_GETTER(vellGetter);
	static NAN_SETTER(vellSetter);
	
	static NAN_GETTER(velaGetter);
	static NAN_SETTER(velaSetter);
	
	static NAN_GETTER(sizeGetter);
	static NAN_SETTER(sizeSetter);
	
	static NAN_GETTER(mapGetter);
	static NAN_SETTER(mapSetter);
	
	static NAN_GETTER(meshGetter);
	static NAN_SETTER(meshSetter);
	
	static NAN_GETTER(massGetter);
	static NAN_SETTER(massSetter);
	
	static NAN_GETTER(restGetter);
	static NAN_SETTER(restSetter);
	
	static NAN_GETTER(damplGetter);
	static NAN_SETTER(damplSetter);
	
	static NAN_GETTER(dampaGetter);
	static NAN_SETTER(dampaSetter);
	
	static NAN_GETTER(factlGetter);
	static NAN_SETTER(factlSetter);
	
	static NAN_GETTER(factaGetter);
	static NAN_SETTER(factaSetter);
	
	static NAN_GETTER(frictGetter);
	static NAN_SETTER(frictSetter);
	
	static NAN_GETTER(sleepyGetter);
	static NAN_SETTER(sleepySetter);
	
};


#endif // _BODY_HPP_
