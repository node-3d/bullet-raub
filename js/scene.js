'use strict';

const util = require('util');
const EventEmitter = require('events');

const { Scene } = require('../core');


class JsScene extends EventEmitter {
	
	constructor() {
		
		super();
		
		const emitter = { emit: this.emit.bind(this) };
		
		this._scene = new Scene(emitter);
		
		
	}
	
	
	get gravity() { return this._scene.gravity; }
	set gravity(v) { this._scene.gravity = v; }
	
	update(...args) { return this._scene.update(...args); }
	hit(...args) { return this._scene.hit(...args); }
	trace(...args) { return this._scene.trace(...args); }
	
	[util.inspect.custom]() { return this.toString(); }
	
}


module.exports = JsScene;
