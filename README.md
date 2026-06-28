# Bullet Physics for Node.js

This is a part of [Node3D](https://github.com/node-3d) project.

[![NPM](https://badge.fury.io/js/%40node-3d%2Fbullet.svg)](https://badge.fury.io/js/@node-3d/bullet)
[![Lint](https://github.com/node-3d/bullet/actions/workflows/lint.yml/badge.svg)](https://github.com/node-3d/bullet/actions/workflows/lint.yml)
[![Test](https://github.com/node-3d/bullet/actions/workflows/test.yml/badge.svg)](https://github.com/node-3d/bullet/actions/workflows/test.yml)
[![Cpplint](https://github.com/node-3d/bullet/actions/workflows/cpplint.yml/badge.svg)](https://github.com/node-3d/bullet/actions/workflows/cpplint.yml)

```console
npm install @node-3d/bullet
```

> This addon is ABI-compatible across Node.js versions. **There is no compilation** during `npm install`.

**Node.js** addon providing a Bullet-driven physics API.

This library is a simplified interpretation of
[Bullet Physics](https://github.com/bulletphysics/bullet3).
Only rigid bodies and DOF-6 constraint are supported.


# Details

Only rigid body primitives are supported. The API is simplified to just 3 classes: `Scene, Body, Joint`.
Body can be static or dynamic and with different shapes. Joint is a DOF6 that you can set up in
many possible ways.

## API

### `Scene`

Owns a Bullet dynamics world and all bodies/joints created for that world.
Useful members:

* `gravity` - world gravity as `{ x, y, z }` or `[x, y, z]`.
* `update(dt = 0)` - advance simulation. If `dt` is `0`, internal timing is used.
* `hit(from, to)` - return the first ray hit.
* `trace(from, to)` - return all ray hits along a segment.
* `destroy()` - release native resources.

### `Body`

Rigid body primitive. Constructor requires `{ scene }` and accepts shape/physics options.
Common properties:

* `type` - body shape, such as `'box'`, `'sphere'`, or `'plane'`.
* `pos`, `quat` - transform.
* `size`, `radius`, `height` - shape dimensions, depending on `type`.
* `mass` - `0` for static bodies, positive values for dynamic bodies.
* `vell`, `vela` - linear and angular velocity.
* `friction`, `restitution`, `damping`, `sleeping` controls.

`Body` emits property-change events and `update` events while active.

### `Joint`

Six-degree-of-freedom constraint between two bodies.
Common properties:

* `a`, `b` - connected bodies.
* `posa`, `posb` - local joint anchors.
* `minl`, `maxl` - linear limits.
* `mina`, `maxa` - angular limits.
* `maximp` - breaking impulse threshold.

`Joint` emits `update` events with current anchor positions and break state.


## Creating Scenes:

```ts
import { Scene } from '@node-3d/bullet';

const scene = new Scene();
scene.gravity = [0, -9.8, 0];
const frame = () => {
	scene.update(); // <-- call update() to progress simulation!
	setTimeout(frame, 15);
};
frame();
```

The `scene` contains all bodies and joints. It is a high-level wrapper that initializes:

* `btDefaultCollisionConfiguration`
* `btCollisionDispatcher`
* `btBroadphaseInterface`
* `btConstraintSolver`
* `btDynamicsWorld`

From `scene` you can also run ray hits or traces. It should be possible to use many scenes
in parallel.

```ts
const { body } = scene.hit(start, end);
```

Here `body` is whatever `Body` the ray hits first on its path. For subclasses of `Body`,
this will respect the dynamic type. I.e. for `class House extends Body {...}` the expression
`body instanceof House` will be `true`.
See `scene.hit` and `scene.trace` in the [example](/examples/main.ts).

## Adding Bodies

```ts
import { Body } from '@node-3d/bullet';

const plane = new Body({ scene }); // static box
plane.type = 'plane'; // change shape to ("ground") plane

const box = new Body({ scene }); // 'box' is default type
box.pos = [0, 100, 0]; // 100 meters above
box.mass = 1; // becomes dynamic
```

Pass `scene` as a required constructor option. All bodies always exist within their parent scenes.


## Connecting Joints

```ts
import { Joint } from '@node-3d/bullet';

const joint = new Joint();
joint.minl = [0, 1, 0];
joint.maxl = [0, 1, 0];
joint.a = bodyA;
joint.b = bodyB;
```

This will connect `bodyA` and `bodyB` and constrain their origins to always be 1 meter apart.
You can also change the origin offsets, add rotation constraints and many other joint parameters.


## Events

Objects of `Scene, Body, Joint` - all have events for every property modification. E.g.
as soon as you set `scene.gravity=[0,0,0]`, and event will fire for `scene.on('gravity', () => {...})`.

You should call `scene.update(dt: number = 0.0)` in order for the scene to progress. Here `dt`
is optional delta time in seconds. If not provided, `dt` will be calculated from internal timers.
It is during this call to `scene.update()` the below `'update'` events will fire.

Objects of `Body` will fire `'update'` events every tick while not sleeping:

```ts
body.on('update', (event) => {
	event.pos; // position of body origin (usually center)
	event.quat; // rotation quaternion
	event.vell; // linear velocity (where it goes)
	event.vela; // angular velocity (how much it spins)
});
```

This is mostly designed to be fed into visualization APIs (such as Three.js), therefore
a Quaternion is used to represent rotation. You may need linear/angular velocity for
interpolation, shading, networking, etc.

Objects of `Joint` will fire `'update'` events every tick while not sleeping:

```ts
joint.on('update', (event) => {
	event.posa; // current position of body A
	event.posb; // current position of body B
	event.broken; // boolean, is this joint broken
});
```

Joints can be broken when overwhelmed with impulse. This is controlled by `joint.maximp` -
by default it's very high. But you can lower it so that, for instance, car wheels fall off
upon extreme impact.
