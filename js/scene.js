'use strict';

const { inspect, inherits } = require('util');
const Emitter = require('events');

const { Scene } = require('../core');

inherits(Scene, Emitter);


function JsScene() {
	
	Scene.call(this);
	
}

JsScene.prototype = {
	
	[inspect.custom]() { return this.toString(); },
	
	toString() {
		return `Scene { gravity: [${this.gravity}] }`;
	},
	
};

inherits(JsScene, Scene);

module.exports = JsScene;
