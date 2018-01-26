'use strict';

const util = require('util');

const { Trace } = require('../core');
const Body = require('./body');


class JsTrace {
	
	constructor(opts = {}) {
		
		if (opts._trace) {
			this._trace = opts._trace;
		} else if (opts.scene) {
			this._trace = new Trace(opts.scene._scene, opts.from, opts.to);
		} else if (opts.pos) {
			this._trace = new Trace(opts.hit, opts.body._body, opts.pos, opts.norm);
		} else {
			this._trace = new Trace(false, null, [0, 0, 0], [0, 1, 0]);
		}
		
	}
	
	get hit() { return this._trace.hit; }
	get body() { return this._trace.body; }
	get pos() { return this._trace.pos; }
	get norm() { return this._trace.norm; }
	
	
	[util.inspect.custom]() { return this.toString(); }
	
	toString() {
		return `Trace { hit: ${this.hit}, pos: [${this.pos}] }`
	}
	
}


module.exports = JsTrace;
