import { EventEmitter } from 'node:events';
import { inspect, inherits } from 'node:util';
import { native } from './native.ts';
import type {
	TBodyInstance,
	TJointInstance,
	TJointProps,
	TOptsJoint,
	TVec3Like,
} from './types.ts';

type TNativeJointWithEvents = TJointInstance & EventEmitter;

const NativeJoint = native.Joint as unknown as new() => TNativeJointWithEvents;

inherits(NativeJoint, EventEmitter);

const nonGcRefs = new Set<Joint>();

/**
 * Creates `btGeneric6DofSpringConstraint` and its related components.
 */
export class Joint extends NativeJoint {
	declare public a: TBodyInstance | null;
	declare public b: TBodyInstance | null;
	declare public broken: boolean;
	declare public posa: TVec3Like;
	declare public posb: TVec3Like;
	declare public rota: TVec3Like;
	declare public rotb: TVec3Like;
	declare public minl: TVec3Like;
	declare public maxl: TVec3Like;
	declare public mina: TVec3Like;
	declare public maxa: TVec3Like;
	declare public maximp: number;
	declare public dampl: TVec3Like;
	declare public dampa: TVec3Like;
	declare public stifl: TVec3Like;
	declare public stifa: TVec3Like;
	declare public springl: TVec3Like;
	declare public springa: TVec3Like;
	declare public motorl: TVec3Like;
	declare public motora: TVec3Like;
	declare public motorlf: TVec3Like;
	declare public motoraf: TVec3Like;
	declare public motorlv: TVec3Like;
	declare public motorav: TVec3Like;
	declare public readonly isDestroyed: boolean;
	declare public destroy: () => void;

	public constructor(opts: TOptsJoint = {}) {
		super();
		Object.assign(this, opts satisfies Partial<TJointProps>);
		
		// Prevent garbage collection until object is intentionally destroyed
		nonGcRefs.add(this);
		this.on('destroy', () => {
			nonGcRefs.delete(this);
		});
	}

	public [inspect.custom](): string  { return this.toString(); }
	
	public toString(): string  {
		return `Joint { broken: ${this.broken} }`;
	}
}
