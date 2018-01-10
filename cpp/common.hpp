#ifndef _COMMON_HPP_
#define _COMMON_HPP_


#include <node.h>

#ifdef _WIN32
	#pragma warning(push)
	#pragma warning(disable:4244)
#endif
#include <nan.h>
#ifdef _WIN32
	#pragma warning(pop)
#endif

// TODO: transfer to addon-tools

#define ACCESSOR_RW(OBJ, NAME)                                      \
	Nan::SetAccessor(OBJ, JS_STR(#NAME), NAME ## Getter, NAME ## Setter);

#define ACCESSOR_R(OBJ, NAME)                                       \
	Nan::SetAccessor(OBJ, JS_STR(#NAME), NAME ## Getter);


// Bullet stuff

#define VEC3_TO_OBJ(VEC3, NAME)                                     \
	Local<Array> NAME = Nan::New<Array>(3);                         \
	SET_PROP(NAME, 0, JS_NUM(VEC3.getX()));                         \
	SET_PROP(NAME, 1, JS_NUM(VEC3.getY()));                         \
	SET_PROP(NAME, 2, JS_NUM(VEC3.getZ()));                         \
	SET_PROP(NAME, JS_STR("x"), JS_NUM(VEC3.getX()));               \
	SET_PROP(NAME, JS_STR("y"), JS_NUM(VEC3.getY()));               \
	SET_PROP(NAME, JS_STR("z"), JS_NUM(VEC3.getZ()));

#define OBJ_TO_VEC3(OBJ, NAME)                                     \
	btVector3 NAME;                                                \
	if (OBJ->Has(JS_STR("x"))) {                                   \
		NAME.setX(OBJ->Get(JS_STR("x")));                          \
		NAME.setY(OBJ->Get(JS_STR("y")));                          \
		NAME.setZ(OBJ->Get(JS_STR("z")));                          \
	} else {                                                       \
		NAME.setX(OBJ->Get(JS_STR("0")));                          \
		NAME.setY(OBJ->Get(JS_STR("1")));                          \
		NAME.setZ(OBJ->Get(JS_STR("2")));                          \
	}

#define REQ_VEC3_ARG(I, VAR)                                       \
	REQ_OBJ_ARG(0, _obj_##VAR);                                    \
	OBJ_TO_VEC3(_obj_##VAR, VAR);


// Fix bad defines

#undef True
#undef False


// Cool macros

#include <addon-tools.hpp>


#endif /* _COMMON_HPP_ */
