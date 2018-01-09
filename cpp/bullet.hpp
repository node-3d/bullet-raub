#ifndef _BULLET_HPP_
#define _BULLET_HPP_

#include "common.hpp"


namespace NodeBullet {
	
	void AtExit();
	
	void registerScene(Scene *scene);
	void unregisterScene(Scene *scene);
	
	NAN_METHOD(Init);
	
}

#endif /* _BULLET_HPP_ */
