# Physics engine for Node.js

Bullet-driven physics API. Offers high-order classes for building 3d simulations.


## Install

```
npm i -s node-bullet-raub
```

Note: as this is a compiled addon, compilation tools must be in place on your system.
Such as MSVS13 for Windows, where `npm install --global windows-build-tools` most probably helps.


## Use

This library is not a direct mapping of Bullet API, rather it is more of a simplified
interpretation for generic purposes.

There are 4 classes right now, and probably that's it. But many things can be achieved.

NOTE: In multiple occasions vector parameters are used. Telling that `pos` (position)
is a `vec3` this doc implies you can use either `[x, y, z]` or `{ x, y, z }` values.
Those are considered equal. The same way you can retrieve values as `pos[0]` or `pos.x`.
There is also `quat` type, which is 4D vector, having additional `w` component.

NOTE: Any **set** property has an event with the same name. The event is emitted whenever
this property is changed by the setter. In some cases such properties are changed
not by the setters, but by the engine itself. Then there is `'update'` event, containing
a limited set of info on what was updated by the engine. If getters are called within
update-handler, they get the newest values as well.


### class Scene

Wraps around `btPhysicsWorld` (as if this was relevant). Scene works as a container
for Bodies. Bodies only interact within the same scene. There can be multiple scenes
running simultaneously.

```
const { Scene } = require('node-bullet-raub');
const scene = new Scene();
```


Constructor:
* Scene()


Properties:
* `get/set vec3 gravity [0,-10,0]` - free fall acceleration speed for Bodies.


Methods:
* void update( float ?delta ) - advance the scene, optional parameter `delta` is how much time have
supposedly passed since last update **in seconds**. If not set, a precise internal
timer is used instead. Therefore it is prefered to call `scene.update()` without arguments.
* Trace hit( vec3 from, vec3 to ) - conducts a ray trace within the scene and returns a new Trace
containing the result of the first hit against body, if any.
* [Trace] trace( vec3 from, vec3 to ) - conducts a ray trace within the scene and returns a
whole list of hits occuring on its way.



### class Body

Wraps around `btRigidBody` (as if this was relevant). Bodies only interact within the same scene.
A body can take different shapes, and even change them on flight.

```
const { Scene, Body } = require('node-bullet-raub');
const scene = new Scene();
const body = new Body({ scene });
```


Constructor:
* Body({ Scene scene })


Properties:
* `get/set string type 'box'` - defines body shape.  NOTE: set is expensive. One of:
	* `'box'` - `btBoxShape`
	* `'ball'` - `btSphereShape`
	* `'roll'` - `btCylinderShape`
	* `'caps'` - `btCapsuleShape`
	* `'plane'` - `btStaticPlaneShape`
	* WIP: `'map'` - `btHeightfieldTerrainShape`
	* WIP: `'mesh'` - `btBvhTriangleMeshShape`
* `get/set vec3 pos [0,0,0]` - body position. NOTE: set is expensive.
* `get/set vec3 rot [0,0,0]` - body rotation, Euler angles - DEGREES. NOTE: get/set is a bit expensive.
* `get/set vec3 vell [0,0,0]` - linear velosity.
* `get/set vec3 vela [0,0,0]` - angular velocity.
* `get/set vec3 size [1,1,1]` - size in three dimensions. NOTE: set is expensive.
* `get/set vec3 factl [1,1,1]` - linear movement axis-multipliers. May be you want a 2D
scene with a locked z-axis, then just set it [1,1,0].
* `get/set vec3 facta [1,1,1]` - angular movement axis-multipliers. May be you want to
create a controlled dynamic character capsule which does not tilt , then just set it [0,0,0].
* `get/set {} map null` - WIP
* `get/set {} mesh null` - WIP
* `get/set number mass 0.0` - body mass, if 0 - body is static. NOTE: set is expensive.
* `get/set number rest 0.0` - restitution, bounciness.
* `get/set number dampl 0.1` - something like air friction and how much it is applied to
the linear velocity.
* `get/set number dampa 0.1` - something like air friction and how much it is applied to
the angular velocity.
* `get/set number frict 0.5` - surface friction on contact points between two bodies.
* `get/set boolean sleepy true` - if this body tends to "fall asleep" when not moving for
a while. This is a good way to optimize calculation and throughput of the scene. Only
set it to `false` for specific body if its sleepiness causes issues.


Events:
* `'update' { vec3 pos, quat quat, vec3 vell, vec3 vela }` - emitted for every non-sleeping
Body per every `scene.update()` call. Instead of `rot` value it caries a raw quaternion.
However you can get the newest `body.rot` yourself. It is done to minimize calculation,
because rotation is internally quaternion and requires conversion to Euler-angles. Also
visualization frameworks tend to treat quaternions way better then angles, and the main
use case of this event is to update visualization.
```
body.on('update', ({ pos, quat }) => {
	mesh.position.set(pos.x, pos.y, pos.z);
	mesh.quaternion.set(quat.x, quat.y, quat.z, quat.w);
});
```


### class Joint

Wraps around `btGeneric6DofSpringConstraint` (as if this was relevant). Can only connect
Bodies within the same scene. A generic constraint can be turned into any other kind
by a carefull setting of parameters. This is why we have a HUGE number of props here.

```
const { Scene, Body } = require('node-bullet-raub');
const scene = new Scene();
const bodyA = new Body({ scene });
const bodyB = new Body({ scene });
const joint = new Joint();
joint.a = bodyA;
joint.b = bodyB;
```


Constructor:
* Joint()


Properties:

* `get/set Body a null` - the first connected body
* `get/set Body b null` - the second connected body
* `get/set boolean broken false` - if the connection was broken.
* `get/set number maximp 9001.f*9001.f` - it's over 9000! A maximum impulse that can be withstood
by this joint. This value should be big: small values simply break upon spawn.
* `get/set vec3 posa [0,0,0]` - joint attachment position within the first connected body.
NOTE: this is not the position of the body in scene.
* `get/set vec3 posb [0,0,0]` - joint attachment position within the second connected body.
NOTE: this is not the position of the body in scene.
* `get/set vec3 rota [0,0,0]` - joint attachment rotation within the first connected body.
NOTE: this is not the rotation of the body in scene.
* `get/set vec3 rotb [0,0,0]` - joint attachment rotation within the second connected body.
NOTE: this is not the rotation of the body in scene.
* `get/set vec3 minl [0,0,0]` - linear lower limit of the connection.
* `get/set vec3 maxl [0,0,0]` - linear upper limit of the connection.
* `get/set vec3 mina [0,0,0]` - angular lower limit of the connection.
* `get/set vec3 maxa [0,0,0]` - angular upper limit of the connection.
* `get/set vec3 dampl [1,1,1]` - linear damping.
* `get/set vec3 dampa [1,1,1]` - angular damping.
* `get/set vec3 stifl [0,0,0]` - linear spring stiffness.
* `get/set vec3 stifa [0,0,0]` - angular spring stiffness.
* `get/set vec3 springl [0,0,0]` - enable/disable linear spring behavior.
* `get/set vec3 springa [0,0,0]` - enable/disable angular spring behavior.
* `get/set vec3 motorl [0,0,0]` - enable/disable linear motor behavior.
* `get/set vec3 motora [0,0,0]` - enable/disable angular motor behavior.
* `get/set vec3 motorlf [0,0,0]` - linear motor max force.
* `get/set vec3 motoraf [0,0,0]` - angular motor max force.
* `get/set vec3 motorlv [0,0,0]` - linear motor target velocity.
* `get/set vec3 motorav [0,0,0]` - angular motor target velocity.


Events:
* `'update' { vec3 posa, vec3 posb, boolean broken }` - emitted for every joint, connecting
two bodies, at least one of which is non-sleeping, per every `scene.update()` call.
Instead of `getter posa/posb`, values passed represent current positions of connected bodies.
Same can be retrieved by `joint.a.pos` and `joint.b.pos`.The main use case of this event
is to update visualization.
```
joint.on('update', ({ posa, posb, broken }) => {
	line.start.set(posa.x, posa.y, posa.z)
	line.end.set(posb.x, posb.y, posb.z)
	line.color = broken ? 0xFF0000 : 0x00FF00;
});
```


### class Trace

A read-only trace result. Holds the information about ray trace.

```
const { Scene, Body } = require('node-bullet-raub');
const scene = new Scene();
const body = new Body({ scene });

const trace = new Trace({ scene, from: [0, 10, 0], to: [0, -10, 0] });
console.log('trace', trace);

const hitRes = scene.hit([0, 10, 0], [0, -10, 0]);
console.log('hitRes', hitRes);

const traceRes = scene.trace([0, 10, 0], [0, -10, 0]);
console.log('traceRes', traceRes);
```


Constructor:
* Trace({ Scene scene, vec3 from, vec3 to })


Properties:

* get boolean hit - if ray hit any body
* get Body body - the body or null
* get vec3 pos - where did it hit
* get vec3 norm - body surface normal
