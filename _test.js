'use strict';

const util = require('util');

const { Scene, Body, Joint, Trace } = require('.');


const scene = new Scene();
console.log('SCENE', scene);
scene.on('gravity', g => console.log('gravity event', g));
scene.gravity = [0, -5, 0];

const bodyA = new Body({scene});
const bodyB = new Body({scene});
console.log('A', bodyA);
console.log('B', bodyB);


const joint = new Joint();
console.log('JOINT', scene);

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



