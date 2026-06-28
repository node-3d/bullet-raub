import assert from 'node:assert/strict';
import { after, before, describe, it } from 'node:test';
import { Body, Joint, Scene } from './index.ts';

type TDestroyable = {
	destroy(): void;
};

type TClassTest = {
	create(): object;
	destroy(instance: object): void;
	props: readonly string[];
	methods: readonly string[];
};

type TClasses = {
	Scene: TClassTest;
	Body: TClassTest;
	Joint: TClassTest;
};

const scene = new Scene();

const classes: TClasses = {
	Scene: {
		create() {
			return scene;
		},
		destroy() { /* nop */ },
		props: ['gravity'],
		methods: ['update', 'hit', 'trace', 'destroy'],
	},
	Body: {
		create() {
			return new Body({ scene });
		},
		destroy(instance) {
			(instance as TDestroyable).destroy();
		},
		props: [
			'type', 'pos', 'rot', 'quat', 'vell', 'vela', 'size', 'factl', 'facta',
			'map', 'mesh', 'mass', 'rest', 'dampl', 'dampa', 'frict', 'sleepy',
		],
		methods: ['destroy'],
	},
	Joint: {
		create() {
			return new Joint();
		},
		destroy(instance) {
			(instance as TDestroyable).destroy();
		},
		props: [
			'a', 'b', 'broken', 'maximp', 'posa', 'posb', 'rota', 'rotb', 'minl',
			'maxl', 'mina', 'maxa', 'dampl', 'dampa', 'stifl', 'stifa', 'springl',
			'springa', 'motorl', 'motora', 'motorlf', 'motoraf', 'motorlv', 'motorav',
		],
		methods: ['destroy'],
	},
};

const testProperty = (getInstance: () => object, prop: string) => {
	it(`exposes #${prop} property`, () => {
		assert.notStrictEqual(Reflect.get(getInstance(), prop), undefined);
	});
};

const testMethod = (getInstance: () => object, method: string) => {
	it(`exposes #${method}() method`, () => {
		assert.strictEqual(typeof Reflect.get(getInstance(), method), 'function');
	});
};

describe('Bullet', () => {
	it('exports classes', () => {
		assert.strictEqual(typeof Scene, 'function');
		assert.strictEqual(typeof Body, 'function');
		assert.strictEqual(typeof Joint, 'function');
	});
	
	for (const className of Object.keys(classes) as (keyof TClasses)[]) {
		describe(className, () => {
			const current = classes[className];
			let instance: object | null = null;
			const getInstance = () => {
				assert.ok(instance);
				return instance;
			};
			
			before(() => {
				instance = current.create();
			});
			
			after(() => {
				current.destroy(getInstance());
			});
			
			it('can be created', () => {
				const ctor = { Body, Joint, Scene }[className];
				assert.ok(getInstance() instanceof ctor);
			});
			
			for (const prop of current.props) {
				testProperty(getInstance, prop);
			}
			
			for (const method of current.methods) {
				testMethod(getInstance, method);
			}
		});
	}
});
