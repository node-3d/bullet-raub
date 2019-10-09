'use strict';

const { inspect, inherits } = require('util');
const Emitter = require('events');

const { Joint } = require('../core');

inherits(Joint, Emitter);


function JsJoint() {
	
	Joint.call(this);
	
}

JsJoint.prototype = {
	
	[inspect.custom]() { return this.toString(); },
	
	toString() {
		return `Joint { broken: ${this.broken} }`;
	},
	
};

inherits(JsJoint, Joint);

module.exports = JsJoint;
