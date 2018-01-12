'use strict';

const util = require('util');
const EventEmitter = require('events');

const { Body } = require('../core');


class JsBody extends EventEmitter {
	
	constructor() {
		
		super();
		
		const emitter = { emit: this.emit.bind(this) };
		
		this._body = new Body(emitter);
		
		
	}
	
	
	get type() { return this._body.type; }
	set type(v) { this._body.type = v; }
	
	get pos() { return this._body.pos; }
	set pos(v) { this._body.pos = v; }
	
	get rot() { return this._body.rot; }
	set rot(v) { this._body.rot = v; }
	
	get vell() { return this._body.vell; }
	set vell(v) { this._body.vell = v; }
	
	get vela() { return this._body.vela; }
	set vela(v) { this._body.vela = v; }
	
	get size() { return this._body.size; }
	set size(v) { this._body.size = v; }
	
	get map() { return this._body.map; }
	set map(v) { this._body.map = v; }
	
	get mesh() { return this._body.mesh; }
	set mesh(v) { this._body.mesh = v; }
	
	get mass() { return this._body.mass; }
	set mass(v) { this._body.mass = v; }
	
	get rest() { return this._body.rest; }
	set rest(v) { this._body.rest = v; }
	
	get dampl() { return this._body.dampl; }
	set dampl(v) { this._body.dampl = v; }
	
	get dampa() { return this._body.dampa; }
	set dampa(v) { this._body.dampa = v; }
	
	get factl() { return this._body.factl; }
	set factl(v) { this._body.factl = v; }
	
	get facta() { return this._body.facta; }
	set facta(v) { this._body.facta = v; }
	
	get frict() { return this._body.frict; }
	set frict(v) { this._body.frict = v; }
	
	get sleepy() { return this._body.sleepy; }
	set sleepy(v) { this._body.sleepy = v; }
	
	
	[util.inspect.custom]() { return this.toString(); }
	
}


module.exports = JsBody;
