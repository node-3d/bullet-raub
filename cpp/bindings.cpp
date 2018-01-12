#include <cstdlib>

#include "scene.hpp"
#include "body.hpp"
#include "joint.hpp"
#include "trace.hpp"

using namespace v8;
using namespace node;
using namespace std;


extern "C" {


void init(Handle<Object> target) {
	
	Scene::init(target);
	Body::init(target);
	Joint::init(target);
	Trace::init(target);
	
}


NODE_MODULE(NODE_GYP_MODULE_NAME, init);


} // extern "C"
