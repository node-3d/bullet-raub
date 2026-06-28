{
	'variables': {
		'bin': '<!(node -e "import(\'@node-3d/addon-tools\').then((m) => m.printBin())")',
		'bullet_include': '<!(node -p "require(\'@node-3d/deps-bullet\').include")',
		'bullet_bin': '<!(node -p "require(\'@node-3d/deps-bullet\').bin")',
	},
	'targets': [{
		'target_name': 'bullet',
		'includes': ['common.gypi'],
		'sources': [
			'cpp/bindings.cpp',
		],
		'include_dirs' : [
			'<!@(node -e "import(\'@node-3d/addon-tools\').then((m) => m.printInclude())")',
			'<(bullet_include)',
		],
		'library_dirs': ['<(bullet_bin)'],
		'conditions': [
			['OS=="linux"', {
				'libraries': [
					"-Wl,-rpath,'$$ORIGIN'",
					"-Wl,-rpath,'$$ORIGIN/../node_modules/@node-3d/deps-bullet/<(bin)'",
					"-Wl,-rpath,'$$ORIGIN/../../@node-3d/deps-bullet/<(bin)'",
					'-lBulletDynamics', '-lBulletCollision', '-lLinearMath',
				],
			}],
			['OS=="mac"', {
				'libraries': [
					'-Wl,-rpath,@loader_path',
					'-Wl,-rpath,@loader_path/../node_modules/@node-3d/deps-bullet/<(bin)',
					'-Wl,-rpath,@loader_path/../../@node-3d/deps-bullet/<(bin)',
					'-lBulletDynamics', '-lBulletCollision', '-lLinearMath',
				],
			}],
			['OS=="win"', {
				'libraries': [
					'-lBulletDynamics', '-lBulletCollision', '-lLinearMath',
				],
			}],
		],
	}],
}
