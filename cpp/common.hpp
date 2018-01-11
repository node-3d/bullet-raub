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

#define SET_I(ARR, I, VAL) ARR->Set(I, VAL);

// Bullet stuff

#define VEC3_TO_OBJ(VEC3, NAME)                                     \
	Local<Array> NAME = Nan::New<Array>(3);                         \
	SET_I(NAME, 0, JS_NUM(VEC3.getX()));                            \
	SET_I(NAME, 1, JS_NUM(VEC3.getY()));                            \
	SET_I(NAME, 2, JS_NUM(VEC3.getZ()));                            \
	SET_PROP(NAME, "x", JS_NUM(VEC3.getX()));                       \
	SET_PROP(NAME, "y", JS_NUM(VEC3.getY()));                       \
	SET_PROP(NAME, "z", JS_NUM(VEC3.getZ()));

#define OBJ_TO_VEC3(OBJ, NAME)                                                \
	btVector3 NAME;                                                           \
	if (OBJ->Has(JS_STR("x"))) {                                              \
		NAME.setX(static_cast<float>(OBJ->Get(JS_STR("x"))->NumberValue()));  \
		NAME.setY(static_cast<float>(OBJ->Get(JS_STR("y"))->NumberValue()));  \
		NAME.setZ(static_cast<float>(OBJ->Get(JS_STR("z"))->NumberValue()));  \
	} else {                                                                  \
		NAME.setX(static_cast<float>(OBJ->Get(0)->NumberValue()));            \
		NAME.setY(static_cast<float>(OBJ->Get(1)->NumberValue()));            \
		NAME.setZ(static_cast<float>(OBJ->Get(2)->NumberValue()));            \
	}

#define REQ_VEC3_ARG(I, VAR)                                        \
	REQ_OBJ_ARG(0, _obj_##VAR);                                     \
	OBJ_TO_VEC3(_obj_##VAR, VAR);


// TODO: transfer to addon tools

#define SETTER_CHECK(C, T)                                              \
	if ( ! value->C )                                                   \
		Nan::ThrowTypeError("Value must be " T);


#define SETTER_UTF8_ARG                                                 \
	SETTER_CHECK(IsString(), "string");                                 \
	Nan::Utf8String v(value);

#define SETTER_INT32_ARG                                                \
	SETTER_CHECK(IsInt32(), "int32");                                   \
	int v = value->Int32Value();

#define SETTER_BOOL_ARG                                                 \
	SETTER_CHECK(IsBoolean(), "bool");                                  \
	bool v = value->BooleanValue(); // TODO APPLY bool TO REQ_BOOL_ARG

#define SETTER_UINT32_ARG                                               \
	SETTER_CHECK(IsUint32(), "uint32");                                 \
	unsigned int v = value->Uint32Value();

#define SETTER_OFFS_ARG                                                 \
	SETTER_CHECK(IsNumber(), "number");                                 \
	size_t v = static_cast<size_t>(value->IntegerValue());

#define SETTER_DOUBLE_ARG                                               \
	SETTER_CHECK(IsNumber(), "number");                                 \
	double v = value->NumberValue();

#define SETTER_FLOAT_ARG                                                \
	SETTER_CHECK(IsNumber(), "number");                                 \
	float v = static_cast<float>(value->NumberValue());

#define SETTER_EXT_ARG                                                  \
	SETTER_CHECK(IsExternal(), "void*");                                \
	Local<External> v = Local<External>::Cast(value);

#define SETTER_FUN_ARG                                                  \
	SETTER_CHECK(IsFunction(), "function");                             \
	Local<Function> v = Local<Function>::Cast(value);

#define SETTER_OBJ_ARG                                                  \
	SETTER_CHECK(IsObject(), "object");                                 \
	Local<Object> v = Local<Object>::Cast(value);

#define SETTER_VEC3_ARG                                                 \
	SETTER_CHECK(IsObject(), "object");                                 \
	Local<Object> _obj_v = Local<Object>::Cast(value);                  \
	OBJ_TO_VEC3(_obj_v, v);


// Fix bad defines

#undef True
#undef False


// Cool macros

#include <addon-tools.hpp>


#endif /* _COMMON_HPP_ */
