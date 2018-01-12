'use strict';

const util = require('util');
const EventEmitter = require('events');

const { Scene } = require('../core');
const Trace = require('./trace');


class JsScene extends EventEmitter {
	
	constructor() {
		
		super();
		
		this.emit = this.emit.bind(this);
		
		this._scene = new Scene(this);
		
	}
	
	
	get gravity() { return this._scene.gravity; }
	set gravity(v) { this._scene.gravity = v; }
	
	update(...args) { return this._scene.update(...args); }
	
	hit(...args) { return new Trace({ _trace: this._scene.hit(...args) }); }
	
	trace(...args) { return this._scene.trace(...args).map(_trace => new Trace({ _trace })); }
	
	
	[util.inspect.custom]() { return this.toString(); }
	
	toString() {
		return `Scene { gravity: [${this.gravity}] }`
	}
	
}


module.exports = JsScene;
