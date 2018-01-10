#ifndef _BODY_HPP_
#define _BODY_HPP_


#include <vector>

#include <nan.h>

#include <LinearMath/btVector3.h>
#include <LinearMath/btQuaternion.h>

#include "common.hpp"


class btRigidBody;
class btCollisionShape;

class Heightmap;
class Joint;
class Scene;
class Trimesh;


class Body : public Nan::ObjectWrap {
	
public:
	
	static void init(v8::Handle<v8::Object> target);
	static void deinit();
	
	void refJoint(Joint *joint);
	void unrefJoint(Joint *joint);
	
	btDynamicsWorld *getWorld() { return _scene->getWorld(); }
	btRigidBody *getBody() { return _body; }
	
	// called within engine
	void __update();
	
	
protected:
	
	explicit Body(Scene *scene);
	virtual ~Body();
	
	static NAN_METHOD(newCtor);
	
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
	
	
private:
	
	static v8::Persistent<v8::Function> _constructor;
	
	Scene *_scene;
	std::vector<Joint*> _joints;
	
	btCollisionShape *_cshape;
	btRigidBody *_body;
	
	
private: // helpers
	void _rebuild();
	const btVector3 &_calcScale() const;
	
	
private: // prop cache
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
	
};


#endif // _BODY_HPP_
