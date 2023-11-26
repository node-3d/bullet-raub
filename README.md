# Bullet Physics for Node.js

This is a part of [Node3D](https://github.com/node-3d) project.

[![NPM](https://badge.fury.io/js/bullet-raub.svg)](https://badge.fury.io/js/bullet-raub)
[![ESLint](https://github.com/node-3d/bullet-raub/actions/workflows/eslint.yml/badge.svg)](https://github.com/node-3d/bullet-raub/actions/workflows/eslint.yml)
[![Test](https://github.com/node-3d/bullet-raub/actions/workflows/test.yml/badge.svg)](https://github.com/node-3d/bullet-raub/actions/workflows/test.yml)
[![Cpplint](https://github.com/node-3d/bullet-raub/actions/workflows/cpplint.yml/badge.svg)](https://github.com/node-3d/bullet-raub/actions/workflows/cpplint.yml)

```console
npm i -s bullet-raub
```

**Node.js** addon providing a Bullet-driven physics API.

This library is a simplified interpretation of
[Bullet Physics](https://github.com/bulletphysics/bullet3).
Only rigid bodies and DOF-6 constraint are supported.

> Note: this **addon uses N-API**, and therefore is ABI-compatible across different
Node.js versions. Addon binaries are precompiled and **there is no compilation**
step during the `npm i` command.

See [TypeScript declarations](/index.d.ts) for more details.
