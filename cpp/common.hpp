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


// TODO: transfer to addon tools

#define SETTER_CHECK(C, T)                                              \
	if ( ! value->C )                                                   \
		Nan::ThrowTypeError("Value must be " T);

#define SETTER_UTF8_ARG                                                 \
	SETTER_CHECK(IsString(), "string");

#define SETTER_INT32_ARG                                                \
	SETTER_CHECK(IsInt32(), "int32");

#define SETTER_BOOL_ARG                                                 \
	SETTER_CHECK(IsBoolean(), "bool");

#define SETTER_UINT32_ARG                                               \
	SETTER_CHECK(IsUint32(), "uint32");

#define SETTER_OFFS_ARG                                                 \
	SETTER_CHECK(IsNumber(), "number");

#define SETTER_DOUBLE_ARG                                               \
	SETTER_CHECK(IsNumber(), "number");

#define SETTER_FLOAT_ARG                                                \
	SETTER_CHECK(IsNumber(), "number");

#define SETTER_EXT_ARG                                                  \
	SETTER_CHECK(IsExternal(), "void*");

#define SETTER_FUN_ARG                                                  \
	SETTER_CHECK(IsFunction(), "function");

#define SETTER_OBJ_ARG                                                  \
	SETTER_CHECK(IsObject(), "object");



// Fix bad defines

#undef True
#undef False


// Cool macros

#include <addon-tools.hpp>


#endif /* _COMMON_HPP_ */
