'use strict';

const { inspect, inherits } = require('node:util');
const Emitter = require('node:events');

const { Body } = require('../core');

inherits(Body, Emitter);

let nextId = 1;
const genId = () => nextId++;
const nonGcDict = {};


class JsBody extends Body {
	constructor({ scene, ...opts }) {
		super(scene);
		Object.keys(opts).forEach((key) => (this[key] = opts[key]));
		
		// Prevent garbage collection until object is intentionally destroyed
		this.__nonGcId = genId();
		nonGcDict[this.__nonGcId] = this;
		this.on('destroy', () => {
			delete nonGcDict[this.__nonGcId];
		});
	}

	[inspect.custom]() { return this.toString(); }
	
	toString() {
		return `Body { type: ${this.type}, pos: [${this.pos}] }`;
	}
}

module.exports = JsBody;
