#include <cstdlib>

#include <event-emitter.hpp>

#include "scene.hpp"
#include "body.hpp"
#include "joint.hpp"
#include "trace.hpp"


extern "C" {


void init(V8_VAR_OBJ target) {
	
	EventEmitter::init(target);
	
	Scene::init(target);
	Body::init(target);
	Joint::init(target);
	Trace::init(target);
	
}


NODE_MODULE(bullet, init);


} // extern "C"
