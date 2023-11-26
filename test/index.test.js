'use strict';

const assert = require('node:assert').strict;
const { describe, it, before, after } = require('node:test');

const bullet = require('..');

const scene = new bullet.Scene();

const classes = {
	Scene: {
		create() {
			return scene;
		},
		destroy() {},
		props: ['gravity'],
		methods: ['update', 'hit', 'trace', 'destroy'],
	},
	Body: {
		create() {
			return new bullet.Body({ scene });
		},
		destroy(inst) {
			inst.destroy();
		},
		props: [
			'type', 'pos', 'rot', 'vell', 'vela', 'size', 'factl', 'facta',
			'map','mesh','mass','rest','dampl','dampa','frict','sleepy'
		],
		methods: ['destroy'],
	},
	Joint: {
		create() {
			return new bullet.Joint();
		},
		destroy(inst) {
			inst.destroy();
		},
		props: [
			'a','b','broken','maximp','posa','posb','rota','rotb','minl',
			'maxl','mina','maxa','dampl','dampa','stifl','stifa','springl',
			'springa','motorl','motora','motorlf','motoraf','motorlv','motorav',
		],
		methods: ['destroy'],
	},
};


describe('Bullet', async () => {
	it('exports an object', () => {
		assert.strictEqual(typeof bullet, 'object');
	});
	
	Object.keys(classes).forEach((c) => {
		it(`${c} is exported`, () => {
			assert.ok(bullet[c] !== undefined);
		});
	});
	
	Object.keys(classes).forEach((c) => describe(c, () => {
		const current = classes[c];
		let instance = null;
		
		before(() => {
			instance = current.create();
		});
		
		after(() => {
			current.destroy(instance);
		});
		
		it('can be created', () => {
			assert.ok(instance instanceof bullet[c]);
		});
		
		current.props.forEach((prop) => {
			it(`exposes #${prop} property`, () => {
				assert.ok(instance[prop] !== undefined);
			});
		});
		
		current.methods.forEach((method) => {
			it(`exposes #${method}() method`, () => {
				assert.strictEqual(typeof instance[method], 'function');
			});
		});
	}));
});
