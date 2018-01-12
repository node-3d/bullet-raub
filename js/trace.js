'use strict';

const util = require('util');
const EventEmitter = require('events');

const { Trace } = require('../core');


class JsTrace extends EventEmitter {
	
	constructor() {
		
		super();
		
		const emitter = { emit: this.emit.bind(this) };
		
		this._trace = new Trace(emitter);
		
		
	}
	
	
	get hit() { return this._trace.hit; }
	get body() { return this._trace.body; }
	get pos() { return this._trace.pos; }
	get norm() { return this._trace.norm; }
	
	
	[util.inspect.custom]() { return this.toString(); }
	
}


module.exports = JsTrace;
