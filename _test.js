'use strict';

const util = require('util');

const { Scene, Body, Joint, Trace } = require('.');


const scene = new Scene();
console.log('SCENE', scene);


const bodyA = new Body(scene);
const bodyB = new Body(scene);
console.log('A', bodyA);
console.log('B', bodyB);


const joint = new Joint(scene);
console.log('JOINT', scene);

joint.enta = bodyA;
console.log('J-A', joint.enta);

joint.entb = bodyB;
console.log('J-B', joint.entb);


const trace = new Trace(scene, [0, 10, 0], [0, -10, 0]);
console.log('TRACE', trace, trace.toString());


const hitRes = scene.hit([0, 10, 0], [0, -10, 0]);
console.log('hitRes', hitRes);

const traceRes = scene.trace([0, 10, 0], [0, -10, 0]);
console.log('traceRes', traceRes);



