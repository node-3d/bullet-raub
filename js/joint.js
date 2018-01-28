'use strict';

const util = require('util');
const EventEmitter = require('events');

const { Joint } = require('../core');


class JsJoint extends EventEmitter {
	
	constructor() {
		
		super();
		
		this.emit = this.emit.bind(this);
		
		this._joint = new Joint(this);
		
	}
	
	
	get a() { return this._joint.a; }
	set a(v) { this._joint.a = v._body; }
	
	get b() { return this._joint.b; }
	set b(v) { this._joint.b = v._body; }
	
	get broken() { return this._joint.broken; }
	set broken(v) { this._joint.broken = v; }
	
	get posa() { return this._joint.posa; }
	set posa(v) { this._joint.posa = v; }
	
	get posb() { return this._joint.posb; }
	set posb(v) { this._joint.posb = v; }
	
	get rota() { return this._joint.rota; }
	set rota(v) { this._joint.rota = v; }
	
	get rotb() { return this._joint.rotb; }
	set rotb(v) { this._joint.rotb = v; }
	
	get minl() { return this._joint.minl; }
	set minl(v) { this._joint.minl = v; }
	
	get maxl() { return this._joint.maxl; }
	set maxl(v) { this._joint.maxl = v; }
	
	get mina() { return this._joint.mina; }
	set mina(v) { this._joint.mina = v; }
	
	get maxa() { return this._joint.maxa; }
	set maxa(v) { this._joint.maxa = v; }
	
	get maximp() { return this._joint.maximp; }
	set maximp(v) { this._joint.maximp = v; }
	
	get dampl() { return this._joint.dampl; }
	set dampl(v) { this._joint.dampl = v; }
	
	get dampa() { return this._joint.dampa; }
	set dampa(v) { this._joint.dampa = v; }
	
	get stifl() { return this._joint.stifl; }
	set stifl(v) { this._joint.stifl = v; }
	
	get stifa() { return this._joint.stifa; }
	set stifa(v) { this._joint.stifa = v; }
	
	get springl() { return this._joint.springl; }
	set springl(v) { this._joint.springl = v; }
	
	get springa() { return this._joint.springa; }
	set springa(v) { this._joint.springa = v; }
	
	get motorl() { return this._joint.motorl; }
	set motorl(v) { this._joint.motorl = v; }
	
	get motora() { return this._joint.motora; }
	set motora(v) { this._joint.motora = v; }
	
	get motorlf() { return this._joint.motorlf; }
	set motorlf(v) { this._joint.motorlf = v; }
	
	get motoraf() { return this._joint.motoraf; }
	set motoraf(v) { this._joint.motoraf = v; }
	
	get motorlv() { return this._joint.motorlv; }
	set motorlv(v) { this._joint.motorlv = v; }
	
	get motorav() { return this._joint.motorav; }
	set motorav(v) { this._joint.motorav = v; }
	
	destroy(...args) { return this._joint.destroy(...args); }
	
	
	[util.inspect.custom]() { return this.toString(); }
	
	toString() {
		return `Joint { broken: ${this.broken} }`
	}
	
}


module.exports = JsJoint;
