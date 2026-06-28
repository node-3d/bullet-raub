import { EventEmitter } from 'node:events';
import { inspect, inherits } from 'node:util';
import { native } from './native.ts';
import type {
	TOptsScene,
	TSceneInstance,
	TSceneProps,
	TTraceHit,
	TTraceHits,
	TVec3Like,
} from './types.ts';

type TNativeSceneWithEvents = TSceneInstance & EventEmitter;

const NativeScene = native.Scene as unknown as new() => TNativeSceneWithEvents;

inherits(NativeScene, EventEmitter);

const nonGcRefs = new Set<Scene>();

/**
 * Creates a `btDiscreteDynamicsWorld` and its related components.
 */
export class Scene extends NativeScene {
	declare public gravity: TVec3Like;
	declare public readonly isDestroyed: boolean;
	declare public destroy: () => void;
	declare public update: (deltaTime?: number) => void;
	declare public hit: (from: TVec3Like, to: TVec3Like) => TTraceHit;
	declare public trace: (from: TVec3Like, to: TVec3Like) => TTraceHits;

	public constructor(opts: TOptsScene = {}) {
		super();
		Object.assign(this, opts satisfies Partial<TSceneProps>);
		
		// Prevent garbage collection until object is intentionally destroyed
		nonGcRefs.add(this);
		this.on('destroy', () => {
			nonGcRefs.delete(this);
		});
	}
	
	public [inspect.custom](): string  { return this.toString(); }
	
	public toString(): string  {
		return `Scene { gravity: [${this.gravity}] }`;
	}
}
