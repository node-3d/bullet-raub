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


### Scene

Wraps around btPhysicsWorld (as if this was relevant). Scene works as a container
for Bodies. Bodies only interact within the same scene. There can be multiple scenes
running simultaneously.

```
const { Scene } = require('node-bullet-raub');
const scene = new Scene();
```

Properties:
* get/set vec3 gravity - free fall acceleration speed for Bodies.


Methods:
* void update( float ?delta ) - advance the scene, optional parameter `delta` is how much time have
supposedly passed since last update **in seconds**. If not set, a precise internal
timer is used instead. Therefore it is prefered to call `scene.update()` without arguments.
* Trace hit( vec3 from, vec3 to ) - conducts a ray trace within the scene and returns a new Trace
containing the result of the first hit against body, if any.
* [Trace] trace( vec3 from, vec3 to ) - conducts a ray trace within the scene and returns a
whole list of hits occuring on its way.



### Body

Wraps around btRigidBody (as if this was relevant). Bodies only interact within the same scene.
A body can take different shapes, and even change them on flight. Also Body has no
methods: eveything is done through props and events.

```
const { Scene, Body } = require('node-bullet-raub');
const scene = new Scene();
const body = new Body({ scene });
```

Properties:
* get/set string type 'box' - defines body shape.  NOTE: set is expensive. One of:
	* `'box'` - btBoxShape
	* `'ball'` - btSphereShape
	* `'roll'` - btCylinderShape
	* `'caps'` - btCapsuleShape
	* `'plane'` - btStaticPlaneShape
	* WIP: `'map'` - btHeightfieldTerrainShape
	* WIP: `'mesh'` - btBvhTriangleMeshShape
* get/set vec3 pos [0,0,0] - body position. NOTE: set is expensive.
* get/set vec3 rot [0,0,0] - body rotation, Euler angles - DEGREES. NOTE: get/set is a bit expensive.
* get/set vec3 vell [0,0,0] - linear velosity.
* get/set vec3 vela [0,0,0] - angular velocity.
* get/set vec3 size [1,1,1] - size in three dimensions. NOTE: set is expensive.
* get/set vec3 factl [1,1,1] - linear movement axis-multipliers. May be you want a 2D
scene with a locked z-axis, then just set it [1,1,0].
* get/set vec3 facta [1,1,1] - angular movement axis-multipliers. May be you want to
create a controlled dynamic character capsule which does not tilt , then just set it [0,0,0].
* get/set {} map null - WIP
* get/set {} mesh null - WIP
* get/set number mass 0.0 - body mass, if 0 - body is static. NOTE: set is expensive.
* get/set number rest 0.0 - restitution, bounciness.
* get/set number dampl 0.1 - something like air friction and how much it is applied to
the linear velocity.
* get/set number dampa 0.1 - something like air friction and how much it is applied to
the angular velocity.
* get/set number frict 0.5 - surface friction on contact points between two bodies.
* get/set boolean sleepy true - if this body tends to "fall asleep" when not moving for
a while. This is a good way to optimize calculation and throughput of the scene. Only
set it to `false` for specific body if its sleepiness causes issues.


Events:
* `'update'` { vec3 pos, quat quat, vec3 vell, vec3 vela } - emitted for every non-sleeping
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


## Joint

doc wip
