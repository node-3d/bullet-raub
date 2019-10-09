
#include "scene.hpp"
#include "body.hpp"
#include "joint.hpp"


Napi::Object initModule(Napi::Env env, Napi::Object exports) {
	
	Scene::init(env, exports);
	Body::init(env, exports);
	Joint::init(env, exports);
	
	return exports;
	
}


NODE_API_MODULE(bullet, initModule)
