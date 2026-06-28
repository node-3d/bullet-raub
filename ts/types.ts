import type { EventEmitter } from 'node:events';

export type TVec3Array = readonly [number, number, number];
export type TVec3Object = Readonly<{ x: number; y: number; z: number }>;
export type TQuatArray = readonly [number, number, number, number];
export type TQuatObject = Readonly<{ w: number; x: number; y: number; z: number }>;
export type TVec3Like = TVec3Array | TVec3Object;
export type TQuatLike = TQuatArray | TQuatObject;
export type TVec3Value = TVec3Array & TVec3Object;
export type TQuatValue = TQuatArray & TQuatObject;

export type TBodyType = 'ball' | 'roll' | 'pill' | 'plane' | 'box';

export type TTraceHit = Readonly<{
	hit: boolean;
	body: TBodyInstance | null;
	pos: TVec3Value;
	norm: TVec3Value;
}>;

export type TTraceHits = readonly TTraceHit[];

export type TEvent = {
	type: string;
	[key: string]: unknown;
};

export type TEventCallback<T extends TEvent> = (event: T) => boolean | undefined;

export type TCommonProps = EventEmitter & {
	/**
	 * True if `destroy` was called.
	 */
	readonly isDestroyed: boolean;
	/**
	 * Stringification helper. Shows `Body { ... }`, `Scene { ... }`, or
	 * `Joint { ... }`.
	 */
	toString(): string;
	/**
	 * Delete the object and free the resources.
	 *
	 * After calling this, the object is no longer valid and should not be used
	 * from JS. Also emits the `destroy` event.
	 */
	destroy(): void;
};

export type TBodyProps = {
	/**
	 * Body shape type. Default is `box`.
	 *
	 * Can be changed any time, but is slow. Reinterprets all other props as
	 * possible.
	 */
	type: TBodyType;
	/** Position in 3D. Default is `[0, 0, 0]`. */
	pos: TVec3Like;
	/** Rotation euler angles. Default is `[0, 0, 0]`. */
	rot: TVec3Like;
	/** Rotation quaternion. Default is `[0, 0, 0, 1]`. */
	quat: TQuatLike;
	/** Linear velocity. Default is `[0, 0, 0]`. */
	vell: TVec3Like;
	/** Angular velocity. Default is `[0, 0, 0]`. */
	vela: TVec3Like;
	/** Bounding box size or scale. Default is `[1, 1, 1]`. */
	size: TVec3Like;
	/** Heightmap shape data. Reserved for future support. */
	map: object;
	/** Triangle mesh shape data. Reserved for future support. */
	mesh: object;
	/** Body mass in KG. Zero is a static body. Default is `0`. */
	mass: number;
	/**
	 * Restitution, or bounciness. Default is `0`.
	 *
	 * @see https://en.wikipedia.org/wiki/Coefficient_of_restitution
	 */
	rest: number;
	/**
	 * Linear damping. Default is `0.1`.
	 *
	 * Controls how quickly the body loses linear velocity due to air friction.
	 */
	dampl: number;
	/**
	 * Angular damping. Default is `0.1`.
	 *
	 * Controls how quickly the body loses angular velocity due to air friction.
	 */
	dampa: number;
	/**
	 * Linear impulse factor. Default is `[1, 1, 1]`.
	 *
	 * Can constrain motion to specific world-frame axes.
	 */
	factl: TVec3Like;
	/**
	 * Angular impulse factor. Default is `[1, 1, 1]`.
	 *
	 * Can constrain rotation to specific world-frame axes.
	 */
	facta: TVec3Like;
	/** Surface friction. Default is `0.5`. */
	frict: number;
	/**
	 * Allow this body to sleep. Default is `true`.
	 *
	 * Usually should be `true`; this is an optimization for inactive bodies.
	 */
	sleepy: boolean;
};

export type TBodyInstance = TCommonProps & TBodyProps;

export type TOptsBody = Readonly<Partial<TBodyProps>> & Readonly<{
	scene: TSceneInstance;
}>;

export type TJointProps = {
	/** First connected body. */
	a: TBodyInstance | null;
	/** Second connected body. */
	b: TBodyInstance | null;
	/** Is connection broken. */
	broken: boolean;
	/** Attachment position on body A. */
	posa: TVec3Like;
	/** Attachment position on body B. */
	posb: TVec3Like;
	/** Attachment rotation in Euler angles on body A. */
	rota: TVec3Like;
	/** Attachment rotation in Euler angles on body B. */
	rotb: TVec3Like;
	/** Min linear distance between A and B. */
	minl: TVec3Like;
	/** Max linear distance between A and B. */
	maxl: TVec3Like;
	/** Min angular distance between A and B. */
	mina: TVec3Like;
	/** Max angular distance between A and B. */
	maxa: TVec3Like;
	/** Maximum allowed impulse before breaking the connection. */
	maximp: number;
	/** Linear damping. */
	dampl: TVec3Like;
	/** Angular damping. */
	dampa: TVec3Like;
	/** Linear stiffness. */
	stifl: TVec3Like;
	/** Angular stiffness. */
	stifa: TVec3Like;
	/** Linear spring force. */
	springl: TVec3Like;
	/** Angular spring force. */
	springa: TVec3Like;
	/**
	 * Linear motor enabled axes.
	 *
	 * Values greater than `0` mean enabled.
	 */
	motorl: TVec3Like;
	/**
	 * Angular motor enabled axes.
	 *
	 * Values greater than `0` mean enabled.
	 */
	motora: TVec3Like;
	/** Linear motor force. */
	motorlf: TVec3Like;
	/** Angular motor force. */
	motoraf: TVec3Like;
	/** Linear motor velocity. */
	motorlv: TVec3Like;
	/** Angular motor velocity. */
	motorav: TVec3Like;
};

export type TOptsJoint = Readonly<Partial<TJointProps>>;
export type TJointInstance = TCommonProps & TJointProps;

export type TSceneProps = {
	/** Scene gravity. Default is `[0, -10, 0]`. */
	gravity: TVec3Like;
};

export type TOptsScene = Readonly<Partial<TSceneProps>>;

export type TSceneInstance = TCommonProps & TSceneProps & {
	/**
	 * Make a simulation step.
	 *
	 * If `deltaTime` is not defined, the realtime delta will be used.
	 */
	update(deltaTime?: number): void;
	/** Detect the first hit on a ray trace. */
	hit(from: TVec3Like, to: TVec3Like): TTraceHit;
	/** Detect all hits on a ray trace. */
	trace(from: TVec3Like, to: TVec3Like): TTraceHits;
};
