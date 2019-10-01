'use strict';

require('segfault-raub');

const { inspect } = require('util');

// Add deps dll dirs
require('deps-bullet-raub');

const { bin } = require('addon-tools-raub');

const core = require(`./${bin}/bullet`);


const { Body, Joint, Scene, Trace } = core;


Body.prototype[inspect.custom] = function () {
	return this.toString();
};
Body.prototype.toString = function () {
	return `Body { type: ${this.type}, pos: [${this.pos}] }`;
};


Joint.prototype[inspect.custom] = function () {
	return this.toString();
};
Joint.prototype.toString = function () {
	return `Joint { broken: ${this.broken} }`;
};


Scene.prototype[inspect.custom] = function () {
	return this.toString();
};
Scene.prototype.toString = function () {
	return `Scene { gravity: [${this.gravity}] }`;
};


Trace.prototype[inspect.custom] = function () {
	return this.toString();
};
Trace.prototype.toString = function () {
	return `Trace { hit: ${this.hit}, pos: [${this.pos}] }`;
};

module.exports = core;
