#include <cstdlib>
#include <vector>
#include <iostream>

#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <LinearMath/btAlignedObjectArray.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/ConstraintSolver/btSolverConstraint.h>
#include <LinearMath/btAabbUtil2.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

#include "scene.hpp"

using namespace v8;
using namespace node;
using namespace std;


static vector<Scene*> scenes;


static void registerScene(Scene *obj) {
	scenes.push_back(obj);
}


static void unregisterScene(Scene* obj) {
	vector<Scene*>::iterator it = scenes.begin();
	while (it != scenes.end()) {
		if (*it == obj) {
			scenes.erase(it);
			break;
		}
		it++;
	}
}


Persistent<Function> Scene::constructor_template;


void Scene::Initialize(Handle<Object> target) { NAN_HS;
	
	// constructor
	Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(newCtor);
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(JS_STR("Scene"));
	
	// prototype
	Nan::SetPrototypeMethod(ctor, "update", update);
	Nan::SetPrototypeMethod(ctor, "trace", trace);
	Nan::SetPrototypeMethod(ctor, "hit", hit);
	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
	
	Nan::SetAccessor(proto,JS_STR("src"), gravityGetter, gravitySetter);
	Nan::Set(target, JS_STR("Image"), ctor->GetFunction());
	
	constructor_template.Reset(Isolate::GetCurrent(), ctor->GetFunction());
	
}


void Scene::AtExit() {
	
	vector<Scene*>::iterator it = scenes.begin();
	
	while (it != scenes.end()) {
		
		Scene *scene = *it;
		
		delete scene;
		
		it++;
	}
	
	scenes.clear();
	
}


NAN_METHOD(Scene::newCtor) { NAN_HS;
	
	Scene *scene = new Scene();
	scene->Wrap(info.This());
	registerScene(scene);
	
	RET_VALUE(info.This());
	
}


Scene::Scene() {
	
	physConfig = new btDefaultCollisionConfiguration();
	physDispatcher = new btCollisionDispatcher(physConfig);
	physBroadphase = new btDbvtBroadphase();
	physSolver = new btSequentialImpulseConstraintSolver();
	physWorld = new btDiscreteDynamicsWorld(physDispatcher, physBroadphase, physSolver, physConfig);
	
	cacheGrav.setValue(0, -10, 0);
	physWorld->setGravity(cacheGrav);
	
	// A SUDDEN STATIC PLANE
	// btStaticPlaneShape *cshape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 0);
	// btVector3 localInertia(0,0,0);
	// btDefaultMotionState* myMotionState = new btDefaultMotionState();
	// btRigidBody::btRigidBodyConstructionInfo rbInfo(0, myMotionState, cshape, localInertia);
	// btRigidBody *body = new btRigidBody(rbInfo);
	// physWorld->addRigidBody(body);
	
}


Scene::~Scene() {
	
	unregisterScene(this);
	
	delete physWorld;
	physWorld = NULL;
	
	delete physSolver;
	physSolver = NULL;
	
	delete physBroadphase;
	physBroadphase = NULL;
	
	delete physDispatcher;
	physDispatcher = NULL;
	
	delete physConfig;
	physConfig = NULL;
	
}


void Scene::setGravity(const btVector3 &v) {
	
	if(cacheGrav == v) {
		return;
	}
	
	cacheGrav = v;
	physWorld->setGravity(cacheGrav);
	
}


void Scene::doUpdate(float dt) {
	
	physWorld->stepSimulation(dt * 0.5f, 10, 1.f / 120.f);
	
}


Trace *Scene::doHit(const btVector3 &from, const btVector3 &to) {
	
	btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
	physWorld->rayTest(from, to, closestResults);
	
	Trace *result;
	if (closestResults.hasHit()) {
		Body *b = reinterpret_cast<Body *>(closestResults.m_collisionObject->getUserPointer());
		result = new Trace(
			NULL, true, b,
			from.lerp(to, closestResults.m_closestHitFraction),
			closestResults.m_hitNormalWorld
		);
	} else {
		result = new Trace(NULL, false, NULL, btVector3(0, 0, 0), btVector3(0, 1, 0));
	}
	
	return result;
	
}


vector<Trace*> Scene::doTrace(const btVector3 &from, const btVector3 &to) {
	
	btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
	physWorld->rayTest(from, to, allResults);
	
	vector<Trace*> list;
	for (int i = 0; i < allResults.m_collisionObjects.size(); i++) {
		Body *b = reinterpret_cast<Body *>(allResults.m_collisionObjects[i]->getUserPointer());
		list.push(new Trace(
			nullptr, true, b,
			allResults.m_hitPointWorld[i],
			allResults.m_hitNormalWorld[i]
		));
	}
	
	return list;
	
}



NAN_GETTER(Scene::gravityGetter) { NAN_HS;
	
	Scene *scene = ObjectWrap::Unwrap<Scene>(info.This());
	
	const btVector3 &gravity = scene->getGravity();
	
	Local<Array> arr = Nan::New<Array>(3);
	SET_PROP(arr, 0, JS_NUM(gravity.getX()));
	SET_PROP(arr, 1, JS_NUM(gravity.getY()));
	SET_PROP(arr, 2, JS_NUM(gravity.getZ()));
	
	RET_VALUE(arr);
	
}


NAN_SETTER(Image::gravitySetter) { NAN_HS;
	
	Nan::MaybeLocal<v8::Object> buffer;
	
	Image *image = ObjectWrap::Unwrap<Image>(info.This());
	String::Utf8Value filename_s(value->ToString());
	image->Load(*filename_s);
	
	// adjust internal fields
	size_t num_pixels = FreeImage_GetWidth(image->image_bmp) * FreeImage_GetHeight(image->image_bmp);
	BYTE *pixels = FreeImage_GetBits(image->image_bmp);
	size_t num_bytes = num_pixels * 4;
	
	// FreeImage stores data in BGR
	// Convert from BGR to RGB
	for (size_t i = 0; i < num_pixels; i++) {
		size_t i4 = i << 2;
		BYTE temp = pixels[i4 + 0];
		pixels[i4 + 0] = pixels[i4 + 2];
		pixels[i4 + 2] = temp;
	}
	
	buffer= Nan::NewBuffer((int)num_bytes);
	
	std::memcpy(node::Buffer::Data(buffer.ToLocalChecked()), pixels, (int)num_bytes);
	
	Nan::Set(info.This(), JS_STR("data"), buffer.ToLocalChecked());
	
	// emit event
	Nan::MaybeLocal<Value> emit_v = Nan::Get(info.This(), JS_STR("emit"));//info.This()->Get(Nan::New<String>("emit"));
	assert(emit_v.ToLocalChecked()->IsFunction());
	Local<Function> emit_f = emit_v.ToLocalChecked().As<Function>();
	
	Handle<Value> argv[2] = {
		JS_STR("load"), // event name
		value	// argument
	};
	
	Nan::TryCatch tc;
	
	emit_f->Call(info.This(), 2, argv);
	
	if (tc.HasCaught()) {
		Nan::FatalException(tc);
	}
	
}


NAN_METHOD(Scene::doUpdate) { NAN_HS;
	
	REQ_UTF8_ARG(0, filename)
	
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(*filename);
	
	void *buffer = node::Buffer::Data(info[1]);
	
	REQ_UINT32_ARG(2, width);
	REQ_UINT32_ARG(3, height);
	
	USE_UINT32_ARG(4, pitch, width * 4);
	USE_UINT32_ARG(5, bpp, 32);
	USE_UINT32_ARG(6, redMask, 0xFF000000);
	USE_UINT32_ARG(7, greenMask, 0x00FF0000);
	USE_UINT32_ARG(8, blueMask, 0x0000FF00);
	
	FIBITMAP *image = FreeImage_ConvertFromRawBits(
		(BYTE*)buffer,
		width, height,
		pitch, bpp,
		redMask, greenMask, blueMask
	);
	
	if (format == FIF_JPEG && bpp != 24) {
		FIBITMAP *old = image;
		image = FreeImage_ConvertTo24Bits(image);
		FreeImage_Unload(old);
	}
	
	bool ret = FreeImage_Save(format, image, *filename) == 1;
	FreeImage_Unload(image);
	
	RET_VALUE(Nan::New<Boolean>(ret));
	
}

NAN_METHOD(Scene::doUpdate) { NAN_HS;
	
	REQ_UTF8_ARG(0, filename)
	
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(*filename);
	
	void *buffer = node::Buffer::Data(info[1]);
	
	REQ_UINT32_ARG(2, width);
	REQ_UINT32_ARG(3, height);
	
	USE_UINT32_ARG(4, pitch, width * 4);
	USE_UINT32_ARG(5, bpp, 32);
	USE_UINT32_ARG(6, redMask, 0xFF000000);
	USE_UINT32_ARG(7, greenMask, 0x00FF0000);
	USE_UINT32_ARG(8, blueMask, 0x0000FF00);
	
	FIBITMAP *image = FreeImage_ConvertFromRawBits(
		(BYTE*)buffer,
		width, height,
		pitch, bpp,
		redMask, greenMask, blueMask
	);
	
	if (format == FIF_JPEG && bpp != 24) {
		FIBITMAP *old = image;
		image = FreeImage_ConvertTo24Bits(image);
		FreeImage_Unload(old);
	}
	
	bool ret = FreeImage_Save(format, image, *filename) == 1;
	FreeImage_Unload(image);
	
	RET_VALUE(Nan::New<Boolean>(ret));
	
}

