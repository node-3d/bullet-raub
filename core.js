'use strict';

require('segfault-raub');

// Add deps dll dirs
require('deps-bullet-raub');

const { bin } = require('addon-tools-raub');

const core = require(`./${bin}/bullet`);

module.exports = core;
