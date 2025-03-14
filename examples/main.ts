import * as bullet from 'bullet-raub';
const { Scene, Body, Joint } = bullet;


const scene = new Scene();
scene.on('gravity', (g) => console.log('gravity event', g));
scene.gravity = [0, -9.8, 0];

const plane = new Body({ scene });
plane.type = 'plane';

const bodyA = new Body({ scene });
bodyA.pos = [0, 100, 0];
bodyA.mass = 1;

const bodyB = new Body({ scene });
bodyB.pos = [0, 101, 0];
bodyB.mass = 1;

const joint = new Joint();
joint.minl = [0, 1, 0];
joint.maxl = [0, 1, 0];
// joint.on('a', (a) => console.log('joint A', a));
// joint.on('b', (a) => console.log('joint B', a));

joint.a = bodyA;
joint.b = bodyB;


console.log('scene =', scene);
console.log('plane =', plane);
console.log('A =', bodyA);
console.log('B =', bodyB);
console.log('JOINT =', joint);


// TODO
// const trace1 = new Trace({ scene, from: [0, 200, 0], to: [0, -10, 0] });
// console.log('TRACE 1', trace1);

// const trace2 = new Trace();
// console.log('TRACE 2', trace2);


const hitRes = scene.hit([0, 200, 0], [0, -10, 0]);
console.log('Body hit by ray:', hitRes);

const traceRes = scene.trace([0, 200, 0], [0, -10, 0]);
console.log('Ray traced bodies:', traceRes.map((result) => result.body));


let prevTimeA = 0;
bodyA.on('update', (u) => {
	const t = Date.now();
	if (prevTimeA + 1000 > t) {
		return;
	}
	
	prevTimeA = t;
	console.log('Body A height:', u.pos.y);
});

let prevTimeJ = 0;
joint.on('update', (u) => {
	const t = Date.now();
	if (prevTimeJ + 1000 > t) {
		return;
	}
	
	prevTimeJ = t;
	console.log('Joint body heights:', u.posa.y, u.posb.y);
});


const frame = () => {
	scene.update();
	setTimeout(frame, 15);
};

frame();
