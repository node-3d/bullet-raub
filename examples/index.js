'use strict';

process.on('uncaughtException', function (err) {
	console.log('Caught exception: ' + err);
});

const SegfaultHandler = require('segfault-handler');

SegfaultHandler.registerHandler("crash.log"); // With no argument, SegfaultHandler will generate a generic log file name

const { Scene, Body, Joint, Trace } = require('..');


const scene = new Scene();
console.log('SCENE', scene);
scene.on('gravity', g => console.log('gravity event', g));
scene.gravity = [0, -9.8, 0];

const plane = new Body({ scene });
plane.type = 'plane';
console.log('PLANE', plane);


const bodyA = new Body({ scene });
bodyA.pos = [0, 100, 0];
bodyA.mass = 1;
bodyA.on('update', u => console.log('update A', u.pos.y));
const bodyB = new Body({ scene });
bodyB.pos = [0, 101, 0];
bodyB.mass = 1;

console.log('A', bodyA);
console.log('B', bodyB);


const joint = new Joint();
console.log('JOINT', joint);
joint.on('a', a => console.log('joint A', a));
joint.on('b', a => console.log('joint B', a));
joint.on('update', u => console.log('update J', u.posa.y, u.posb.y));

joint.a = bodyA;
console.log('J-A', joint.a);

joint.b = bodyB;
console.log('J-B', joint.b);


const trace1 = new Trace({ scene, from: [0, 200, 0], to: [0, -10, 0] });
console.log('TRACE 1', trace1);

const trace2 = new Trace();
console.log('TRACE 2', trace2);


const hitRes = scene.hit([0, 200, 0], [0, -10, 0]);
console.log('hitRes', hitRes);

const traceRes = scene.trace([0, 200, 0], [0, -10, 0]);
console.log('traceRes', traceRes);



function frame() {
	
	scene.update();
	
	setTimeout(frame, 15);
	
}

frame();
