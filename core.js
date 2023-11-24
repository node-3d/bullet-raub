'use strict';

require('segfault-raub');

// Add deps dll dirs
require('deps-bullet-raub');

const { getBin } = require('addon-tools-raub');

const core = require(`./${getBin()}/bullet`);

module.exports = core;
