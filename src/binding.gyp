{
	'variables': {
		'bin': '<!(node -p "require(\'addon-tools-raub\').getBin()")',
		'bullet_include': '<!(node -p "require(\'deps-bullet-raub\').include")',
		'bullet_bin': '<!(node -p "require(\'deps-bullet-raub\').bin")',
	},
	'targets': [{
		'target_name': 'bullet',
		'includes': ['../node_modules/addon-tools-raub/utils/common.gypi'],
		'sources': [
			'cpp/bindings.cpp',
		],
		'include_dirs' : [
			'<!@(node -p "require(\'addon-tools-raub\').getInclude()")',
			'<(bullet_include)',
		],
		'library_dirs': ['<(bullet_bin)'],
		'conditions': [
			['OS=="linux"', {
				'libraries': [
					"-Wl,-rpath,'$$ORIGIN'",
					"-Wl,-rpath,'$$ORIGIN/../node_modules/deps-bullet-raub/<(bin)'",
					"-Wl,-rpath,'$$ORIGIN/../../deps-bullet-raub/<(bin)'",
					'-lBulletDynamics', '-lBulletCollision', '-lLinearMath',
				],
			}],
			['OS=="mac"', {
				'libraries': [
					'-Wl,-rpath,@loader_path',
					'-Wl,-rpath,@loader_path/../node_modules/deps-bullet-raub/<(bin)',
					'-Wl,-rpath,@loader_path/../../deps-bullet-raub/<(bin)',
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
