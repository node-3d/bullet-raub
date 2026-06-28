import { EventEmitter } from 'node:events';
import { inspect, inherits } from 'node:util';
import { native } from './native.ts';
import type {
	TBodyProps,
	TBodyInstance,
	TBodyType,
	TOptsBody,
	TQuatLike,
	TSceneInstance,
	TVec3Like,
} from './types.ts';

type TNativeBodyWithEvents = TBodyInstance & EventEmitter;

const NativeBody = native.Body as unknown as new(scene: TSceneInstance) => TNativeBodyWithEvents;

inherits(NativeBody, EventEmitter);

const nonGcRefs = new Set<Body>();

/**
 * Creates `btRigidBody` and its related components.
 */
export class Body extends NativeBody {
	declare public type: TBodyType;
	declare public pos: TVec3Like;
	declare public rot: TVec3Like;
	declare public quat: TQuatLike;
	declare public vell: TVec3Like;
	declare public vela: TVec3Like;
	declare public size: TVec3Like;
	declare public map: object;
	declare public mesh: object;
	declare public mass: number;
	declare public rest: number;
	declare public dampl: number;
	declare public dampa: number;
	declare public factl: TVec3Like;
	declare public facta: TVec3Like;
	declare public frict: number;
	declare public sleepy: boolean;
	declare public readonly isDestroyed: boolean;
	declare public destroy: () => void;

	public constructor({ scene, ...opts }: TOptsBody) {
		super(scene);
		Object.assign(this, opts satisfies Partial<TBodyProps>);
		
		// Prevent garbage collection until object is intentionally destroyed
		nonGcRefs.add(this);
		this.on('destroy', () => {
			nonGcRefs.delete(this);
		});
	}

	public [inspect.custom](): string  { return this.toString(); }
	
	public toString(): string  {
		return `Body { type: ${this.type}, pos: [${this.pos}] }`;
	}
}
