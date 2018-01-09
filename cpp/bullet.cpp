#include <cstring>
#include <vector>
#include <iostream>

#include <node.h>


#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <LinearMath/btAlignedObjectArray.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/ConstraintSolver/btSolverConstraint.h>
#include <LinearMath/btAabbUtil2.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>


#include "bullet.hpp"
#include "scene.hpp"

using namespace node;
using namespace v8;
using namespace std;


namespace NodeBullet {


NAN_METHOD(Init) { NAN_HS;
	
	
	
}


vector<GLObj*> globjs;
static bool atExit = false;

void registerScene(GLObjectType type, GLuint obj) {
	globjs.push_back(new GLObj(type,obj));
}


void unregisterGLObj(GLuint obj) {
	
	if (atExit) {
		return;
	}
	
	vector<GLObj*>::iterator it = globjs.begin();
	
	while (globjs.size() && it != globjs.end()) {
		
		GLObj *globj = *it;
		
		if (globj->obj == obj) {
			delete globj;
			globjs.erase(it);
			break;
		}
		
		++it;
		
	}
	
}

void AtExit() {
	
	atExit = true;
	
	vector<GLObj*>::iterator it;
	
	#ifdef LOGGING
		cout<<"WebGL AtExit() called"<<endl;
		cout<<"	# objects allocated: "<<globjs.size()<<endl;
		
		it = globjs.begin();
		while(globjs.size() && it != globjs.end()) {
			
			GLObj *obj=*it;
			cout<<"[";
			
			switch(obj->type) {
			case GLOBJECT_TYPE_BUFFER: cout<<"buffer"; break;
			case GLOBJECT_TYPE_FRAMEBUFFER: cout<<"framebuffer"; break;
			case GLOBJECT_TYPE_PROGRAM: cout<<"program"; break;
			case GLOBJECT_TYPE_RENDERBUFFER: cout<<"renderbuffer"; break;
			case GLOBJECT_TYPE_SHADER: cout<<"shader"; break;
			case GLOBJECT_TYPE_TEXTURE: cout<<"texture"; break;
			};
			
			cout<<": "<<obj->obj<<"] ";
			++it;
			
		}
		cout<<endl;
	#endif
	
	it = globjs.begin();
	while(globjs.size() && it != globjs.end()) {
		
		GLObj *globj = *it;
		GLuint obj = globj->obj;
		
		switch(globj->type) {
		case GLOBJECT_TYPE_PROGRAM:
			#ifdef LOGGING
			cout<<"	Destroying GL program "<<obj<<endl;
			#endif
			glDeleteProgram(obj);
			break;
		case GLOBJECT_TYPE_BUFFER:
			#ifdef LOGGING
			cout<<"	Destroying GL buffer "<<obj<<endl;
			#endif
			glDeleteBuffers(1, &obj);
			break;
		case GLOBJECT_TYPE_FRAMEBUFFER:
			#ifdef LOGGING
			cout<<"	Destroying GL frame buffer "<<obj<<endl;
			#endif
			glDeleteFramebuffers(1, &obj);
			break;
		case GLOBJECT_TYPE_RENDERBUFFER:
			#ifdef LOGGING
			cout<<"	Destroying GL render buffer "<<obj<<endl;
			#endif
			glDeleteRenderbuffers(1, &obj);
			break;
		case GLOBJECT_TYPE_SHADER:
			#ifdef LOGGING
			cout<<"	Destroying GL shader "<<obj<<endl;
			#endif
			glDeleteShader(obj);
			break;
		case GLOBJECT_TYPE_TEXTURE:
			#ifdef LOGGING
			cout<<"	Destroying GL texture "<<obj<<endl;
			#endif
			glDeleteTextures(1, &obj);
			break;
		default:
			#ifdef LOGGING
			cout<<"	Unknown object "<<obj<<endl;
			#endif
			break;
		}
		
		delete globj;
		++it;
		
	}
	
	globjs.clear();
	
}

} // namespace webgl
