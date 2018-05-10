'use strict';

const util = require('util');

// Add deps dll dirs
require('deps-bullet-raub');

const core = require('./binary/bullet');


const { Body, Joint, Scene, Trace } = core;


Body.prototype[util.inspect.custom] = function () {
	return this.toString();
};
Body.prototype.toString = function () {
	return `Body { type: ${this.type}, pos: [${this.pos}] }`;
};


Joint.prototype[util.inspect.custom] = function () {
	return this.toString();
};
Joint.prototype.toString = function () {
	return `Joint { broken: ${this.broken} }`;
};


Scene.prototype[util.inspect.custom] = function () {
	return this.toString();
};
Scene.prototype.toString = function () {
	return `Scene { gravity: [${this.gravity}] }`;
};


Trace.prototype[util.inspect.custom] = function () {
	return this.toString();
};
Trace.prototype.toString = function () {
	return `Trace { hit: ${this.hit}, pos: [${this.pos}] }`;
};

module.exports = core;
