declare module "bullet-raub" {
	type TArr3 = Readonly<[number, number, number]>;
	type TObj3 = Readonly<{ x: number; y: number; z: number }>;
	type TArr4 = Readonly<[number, number, number, number]>;
	type TObj4 = Readonly<{ x: number; y: number; z: number, w: number }>;
	type TVec3Either = TArr3 | TObj3;
	type TQuatEither = TArr4 | TObj4;
	type TVec3Both = TArr3 & TObj3;
	type TQuatBoth = TArr4 & TObj4;
	
	type TTraceHit = Readonly<{
		hit: boolean;
		body: Body;
		pos: TVec3Both;
		norm: TVec3Both;
	}>;
	
	type TTraceHits = ReadonlyArray<TTraceHit>;
	
	type TEvent = {
		type: string;
		[key: string]: unknown;
	};
	
	type TEventCb<T extends TEvent> = (event: T) => (void | boolean);
	
	type EventEmitter = import('node:events').EventEmitter;
	
	type TPropsCommon = EventEmitter & {
		/**
		 * True if `destroy` was called.
		 */
		readonly isDestroyed: boolean;
		
		/** Stringification helper. */
		toString(): string;
		
		/**
		 * Delete the object and free the resources.
		 * 
		 * After calling this, the object is **no longer valid** and should not be used from JS.
		 * Also emits the "destroy" event.
		 */
		destroy(): void;
	};
	
	type TBodyType = 'ball' | 'roll' | 'pill' | 'plane' | 'box'; // + todo: map mesh
	
	type TPropsBody = {
		/**
		 * Body shape type. Default is "box".
		 * 
		 * Can be changed any time, but is slow. Reinterprets all other props as possible.
		 */
		type: TBodyType;
		
		/** Position in 3D. Default is `[0, 0, 0]`. */
		pos: TVec3Either;
		
		/** Rotation quaternion. Default is `[0, 0, 0, 1]`. */
		rot: TQuatEither;
		
		/** Linear velocity. Default is `[0, 0, 0]`. */
		vell: TVec3Either;
		
		/** Angular velocity. Default is `[0, 0, 0]`. */
		vela: TVec3Either;
		
		/** Bounding box size (or "scale"). Default is `[1, 1, 1]`. */
		size: TVec3Either;
		
		/** TODO */
		// map: unknown;
		
		/** TODO */
		// mesh: unknown;
		
		/** Body mass in KG. Zero is static body. Default is 0 (static). */
		mass: number;
		
		/**
		 * Restitution - bounciness. Default is 0 (no bounce).
		 * 
		 * @see https://en.wikipedia.org/wiki/Coefficient_of_restitution
		 */
		rest: number;
		
		/**
		 * Linear damping. Default is `0.1`.
		 * 
		 * How quickly the body loses linear velocity due to "air friction".
		 */
		dampl: number;
		
		/**
		 * Angular damping. Default is `0.1`.
		 * 
		 * How quickly the body loses angular velocity due to "air friction".
		 */
		dampa: number;
		
		/**
		 * Linear impulse factor. Default is `[1, 1, 1]`.
		 * 
		 * It can be used to constrain object motion to certain world-frame axes.
		 * For example: to constrain an object to only move in the XZ plane,
		 * you would set the Y-component of `factl` to 0, and the XZ components to 1.
		 */
		factl: number;
		
		/**
		 * Angular impulse factor. Default is `[1, 1, 1]`.
		 * 
		 * It can be used to constrain object rotation to certain world-frame axes.
		 * For example: to constrain an object to only rotate around the Y axis,
		 * you would set the XZ-components of `facta` to 0, and the Y-component to 1.
		 */
		facta: number;
		
		/** Surface friction. Default is `0.5`. */
		frict: number;
		
		/**
		 * Allow this body to "sleep". Default is `true`.
		 * 
		 * Usually should be `true` - this is an optimization for inactive bodies.
		 */
		sleepy: boolean;
	};
	
	export type TOptsBody = (
		Readonly<Partial<TPropsBody>> &
		Readonly<{ scene: TSceneInstance }>
	);
	
	type TBodyInstance = TPropsCommon & TPropsBody;
	
	interface TNewableBody {
		new(opts?: TOptsBody): TBodyInstance;
	}
	
	/**
	 * Body
	 * 
	 * Creates `btRigidBody` and its related components.
	*/
	export const Body: TNewableBody;
	
	type TPropsJoint = {
		/** First connected body. */
		a: TBodyInstance | null;
		
		/** Second connected body. */
		b: TBodyInstance | null;
		
		/** Is connection broken. */
		broken: boolean;
		
		/** Attachment position on body A. */
		posa: TVec3Either;
		
		/** Attachment position on body B. */
		posb: TVec3Either;
		
		/** Attachment rotation (Euler) on body A. */
		rota: TVec3Either;
		
		/** Attachment rotation (Euler) on body B. */
		rotb: TVec3Either;
		
		/** Min linear distance between A and B. */
		minl: TVec3Either;
		
		/** Max linear distance between A and B. */
		maxl: TVec3Either;
		
		/** Min angular distance between A and B. */
		mina: TVec3Either;
		
		/** Max angular distance between A and B. */
		maxa: TVec3Either;
		
		/** Maximum allowed impulse before breaking the connection. */
		maximp: TVec3Either;
		
		/** Linear damping. */
		dampl: TVec3Either;
		
		/** Angular damping. */
		dampa: TVec3Either;
		
		/** Linear stiffness. */
		stifl: TVec3Either;
		
		/** Angular stiffness. */
		stifa: TVec3Either;
		
		/** Linear spring force. */
		springl: TVec3Either;
		
		/** Angular spring force. */
		springa: TVec3Either;
		
		/**
		 * Linear motor enabled axes.
		 * 
		 * Values `> 0` mean "enabled".
		 */
		motorl: TVec3Either;
		
		/**
		 * Angular motor enabled axes.
		 * 
		 * Values `> 0` mean "enabled".
		 */
		motora: TVec3Either;
		
		/** Linear motor force. */
		motorlf: TVec3Either;
		
		/** Angular motor force. */
		motoraf: TVec3Either;
		
		/** Linear motor velocity. */
		motorlv: TVec3Either;
		
		/** Angular motor velocity. */
		motorav: TVec3Either;
	};
	
	export type TOptsJoint = Readonly<Partial<TPropsJoint>>;
	
	type TJointInstance = TPropsCommon & TPropsJoint;
	
	interface TNewableJoint {
		new(opts?: TOptsJoint): TJointInstance;
	}
	
	/**
	 * Joint
	 * 
	 * Creates `btGeneric6DofSpringConstraint` and its related components.
	*/
	export const Joint: TNewableJoint;
	
	export type TPropsScene = {
		/**
		 * Scene gravity. Default is `[0, -10, 0]`.
		 */
		gravity: TVec3Either;
	};
	
	export type TOptsScene = Readonly<Partial<TPropsScene>>;
	
	type TSceneInstance = TPropsCommon & TPropsScene & {
		/**
		 * Make a simulation step.
		 * 
		 * If `deltaTime` is not defined, the realtime delta will be used.
		*/
		update(deltaTime?: number): void;
		
		/** Detect the **first** hit on a ray trace. */
		hit(from: TVec3Either, to: TVec3Either): TTraceHit;
		
		/** Detect **all** hits on a ray trace. */
		trace(from: TVec3Either, to: TVec3Either): TTraceHits;
	};
	
	interface TNewableScene {
		new(opts?: TOptsScene): TSceneInstance;
	}
	
	/**
	 * Scene
	 * 
	 * Creates a `btDiscreteDynamicsWorld` and its related components.
	*/
	export const Scene: TNewableScene;
}
