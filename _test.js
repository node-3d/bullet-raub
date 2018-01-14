'use strict';

const { Scene, Body, Joint, Trace } = require('.');


const scene = new Scene();
console.log('SCENE', scene);
scene.on('gravity', g => console.log('gravity event', g));
scene.gravity = [0, -9.8, 0];


const plane = new Body({ scene });
plane.type = 'plane';
console.log('PLANE', plane);


const bodyA = new Body({ scene });
bodyA.pos = [10, 100, 10];
bodyA.mass = 1;
// bodyA.on('update', u => console.log('update A', u.pos.x, u.pos.y, u.pos.z));
const bodyB = new Body({ scene });
bodyB.pos = [10, 101, 10];
bodyB.mass = 1;

console.log('A', bodyA);
console.log('B', bodyB);


const joint = new Joint();
console.log('JOINT', scene);
joint.on('a', a => console.log('joint A', a));
joint.on('b', a => console.log('joint B', a));
joint.on('update', u => console.log('update J', u.posa, u.posb));

joint.enta = bodyA;
console.log('J-A', joint.enta);

joint.entb = bodyB;
console.log('J-B', joint.entb);


const trace1 = new Trace({ scene, from: [0, 10, 0], to: [0, -10, 0] });
console.log('TRACE 1', trace1);

const trace2 = new Trace();
console.log('TRACE 2', trace2);


const hitRes = scene.hit([0, 10, 0], [0, -10, 0]);
console.log('hitRes', hitRes);

const traceRes = scene.trace([0, 10, 0], [0, -10, 0]);
console.log('traceRes', traceRes);



function frame() {
	
	scene.update();
	
	setTimeout(frame, 15);
	
}

frame();
