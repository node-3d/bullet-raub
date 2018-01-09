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


// Bullet stuff

// TODO


// Fix bad defines

#undef True
#undef False


// Cool macros

#include <addon-tools.hpp>


#endif /* _COMMON_HPP_ */
