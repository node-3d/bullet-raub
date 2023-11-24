'use strict';

const { inspect, inherits } = require('node:util');
const Emitter = require('node:events');

const { Joint } = require('../core');

inherits(Joint, Emitter);

let nextId = 1;
const genId = () => nextId++;
const nonGcDict = {};


class JsJoint extends Joint {
	constructor() {
		super();
		
		// Prevent garbage collection until object is intentionally destroyed
		this.__nonGcId = genId();
		nonGcDict[this.__nonGcId] = this;
		this.on('destroy', () => {
			delete nonGcDict[this.__nonGcId];
		});
	}

	[inspect.custom]() { return this.toString(); }
	
	toString() {
		return `Joint { broken: ${this.broken} }`;
	}
}

module.exports = JsJoint;
