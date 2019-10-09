'use strict';

const { inspect, inherits } = require('util');
const Emitter = require('events');

const { Body } = require('../core');

inherits(Body, Emitter);


function JsBody() {
	
	Body.call(this);
	
}

JsBody.prototype = {
	
	[inspect.custom]() { return this.toString(); },
	
	toString() {
		return `Body { type: ${this.type}, pos: [${this.pos}] }`;
	},
	
};

inherits(JsBody, Body);

module.exports = JsBody;
