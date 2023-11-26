#ifndef COMMON_HPP
#define COMMON_HPP

#include <vector>

#include <LinearMath/btVector3.h>
#include <LinearMath/btQuaternion.h>

#include <addon-tools.hpp>


// Bullet stuff

#define VEC3_TO_OBJ(VEC3, NAME)                                               \
	Napi::Array NAME = Napi::Array::New(env);                                 \
	NAME.Set(0U, VEC3.getX());                                                \
	NAME.Set(1U, VEC3.getY());                                                \
	NAME.Set(2U, VEC3.getZ());                                                \
	NAME.Set("x", VEC3.getX());                                               \
	NAME.Set("y", VEC3.getY());                                               \
	NAME.Set("z", VEC3.getZ());

#define QUAT_TO_OBJ(QUAT, NAME)                                               \
	Napi::Array NAME = Napi::Array::New(env);                                 \
	NAME.Set(0U, QUAT.getX());                                                \
	NAME.Set(1U, QUAT.getY());                                                \
	NAME.Set(2U, QUAT.getZ());                                                \
	NAME.Set(3U, QUAT.getW());                                                \
	NAME.Set("x", QUAT.getX());                                               \
	NAME.Set("y", QUAT.getY());                                               \
	NAME.Set("z", QUAT.getZ());                                               \
	NAME.Set("w", QUAT.getW());


#define OBJ_TO_VEC3(OBJ, NAME)                                                \
	btVector3 NAME;                                                           \
	if (OBJ.Has("x")) {                                                       \
		NAME.setX(OBJ.Get("x").ToNumber().FloatValue());                      \
		NAME.setY(OBJ.Get("y").ToNumber().FloatValue());                      \
		NAME.setZ(OBJ.Get("z").ToNumber().FloatValue());                      \
	} else {                                                                  \
		NAME.setX(OBJ.Get(0U).ToNumber().FloatValue());                       \
		NAME.setY(OBJ.Get(1U).ToNumber().FloatValue());                       \
		NAME.setZ(OBJ.Get(2U).ToNumber().FloatValue());                       \
	}

#define OBJ_TO_QUAT(OBJ, NAME)                                                \
	btQuaternion NAME;                                                        \
	if (OBJ.Has("x")) {                                                       \
		NAME.setX(OBJ.Get("x").ToNumber().FloatValue());                      \
		NAME.setY(OBJ.Get("y").ToNumber().FloatValue());                      \
		NAME.setZ(OBJ.Get("z").ToNumber().FloatValue());                      \
		NAME.setW(OBJ.Get("w").ToNumber().FloatValue());                      \
	} else {                                                                  \
		NAME.setX(OBJ.Get(0U).ToNumber().FloatValue());                       \
		NAME.setY(OBJ.Get(1U).ToNumber().FloatValue());                       \
		NAME.setZ(OBJ.Get(2U).ToNumber().FloatValue());                       \
		NAME.setW(OBJ.Get(3U).ToNumber().FloatValue());                       \
	}


#define REQ_VEC3_ARG(I, VAR)                                                  \
	REQ_OBJ_ARG(I, _obj_##VAR);                                               \
	OBJ_TO_VEC3(_obj_##VAR, VAR);

#define REQ_QUAT_ARG(I, VAR)                                                  \
	REQ_OBJ_ARG(I, _obj_##VAR);                                               \
	OBJ_TO_QUAT(_obj_##VAR, VAR);


#define SETTER_VEC3_ARG                                                       \
	SETTER_CHECK(IsObject(), "Object");                                       \
	Napi::Object _obj_v = value.As<Napi::Object>();                           \
	OBJ_TO_VEC3(_obj_v, v);

#define SETTER_QUAT_ARG                                                       \
	SETTER_CHECK(IsObject(), "Object");                                       \
	Napi::Object _obj_v = value.As<Napi::Object>();                           \
	OBJ_TO_QUAT(_obj_v, v);


#define ALIGNED_NEW(C, ...) new (btAlignedAlloc(sizeof(C), 16)) C(__VA_ARGS__)


#define ALIGNED_DELETE(C, V)                                                  \
	if (V) {                                                                  \
		static_cast<C*>(V)->~C();                                             \
		btAlignedFree(V);                                                     \
		V = nullptr;                                                          \
	}


#define EACH(V) for (int i = V.size() - 1; i >= 0; i--)

typedef std::vector<Napi::Object> ObjVec;


#define V3_GETTER(CLASS, NAME, CACHE)                                         \
	JS_IMPLEMENT_GETTER(CLASS, NAME) { THIS_CHECK;                            \
		VEC3_TO_OBJ(CACHE, NAME);                                             \
		RET_VALUE(NAME);                                                      \
	}

#define NUM_GETTER(CLASS, NAME, CACHE)                                        \
	JS_IMPLEMENT_GETTER(CLASS, NAME) { THIS_CHECK;                            \
		RET_VALUE(JS_NUM(CACHE));                                             \
	}


struct Common {
	Common(Napi::Value that, const char *name):
	_asyncCtx(that.Env(), name) {
		_that.Reset(that.As<Napi::Object>());
		_isDestroyed = false;
	}
	
	~Common() { _destroy(); }
	
	void emit(
		const char* name,
		int argc = 0,
		const Napi::Value *argv = nullptr
	) { DES_CHECK;
		eventEmit(_that.Value(), name, argc, argv);
	}
	
	void _destroy() { DES_CHECK;
		_isDestroyed = true;
		_that.Reset();
	}
	
	bool _isDestroyed;
	
	Napi::ObjectReference _that;
	Napi::AsyncContext _asyncCtx;
};


// Fix bad defines

#undef True
#undef False


#endif /* COMMON_HPP */
