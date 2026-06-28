import { createRequire } from 'node:module';
import { getBin } from '@node-3d/addon-tools';
import '@node-3d/segfault';
import '@node-3d/deps-bullet';
import type {
	TBodyProps,
	TJointProps,
	TSceneProps,
	TTraceHit,
	TTraceHits,
	TVec3Like,
} from './types.ts';

type TNativeCommonInstance = {
	readonly isDestroyed: boolean;
	destroy(): void;
	toString(): string;
};

type TNativeBodyInstance = TNativeCommonInstance & TBodyProps;
type TNativeJointInstance = TNativeCommonInstance & TJointProps;
type TNativeSceneInstance = TNativeCommonInstance & TSceneProps & {
	update(deltaTime?: number): void;
	hit(from: TVec3Like, to: TVec3Like): TTraceHit;
	trace(from: TVec3Like, to: TVec3Like): TTraceHits;
};

type TNative = {
	Body: new(scene: TNativeSceneInstance) => TNativeBodyInstance;
	Scene: new() => TNativeSceneInstance;
	Joint: new() => TNativeJointInstance;
};

const loadAddon = createRequire(import.meta.url);

export const native = loadAddon(`../${getBin()}/bullet.node`) as TNative;
