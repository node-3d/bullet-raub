{
	'variables': {
		'bin': '<!(node -p "require(\'addon-tools-raub\').getBin()")',
		'bullet_include': '<!(node -p "require(\'deps-bullet-raub\').include")',
		'bullet_bin': '<!(node -p "require(\'deps-bullet-raub\').bin")',
	},
	'targets': [{
		'target_name': 'bullet',
		'sources': [
			'cpp/bindings.cpp',
			'cpp/body.cpp',
			'cpp/joint.cpp',
			'cpp/scene.cpp',
		],
		'include_dirs' : [
			'<!@(node -p "require(\'addon-tools-raub\').getInclude()")',
			'<(bullet_include)',
		],
		'library_dirs': ['<(bullet_bin)'],
		'cflags_cc': ['-std=c++17', '-fno-exceptions'],
		'cflags': ['-fno-exceptions'],
		'conditions': [
			['OS=="linux"', {
				'libraries': [
					"-Wl,-rpath,'$$ORIGIN'",
					"-Wl,-rpath,'$$ORIGIN/../node_modules/deps-bullet-raub/<(bin)'",
					"-Wl,-rpath,'$$ORIGIN/../../deps-bullet-raub/<(bin)'",
					'-lBulletCollision', '-lBulletDynamics', '-lLinearMath'
				],
				'defines': ['__linux__'],
			}],
			['OS=="mac"', {
				'libraries': [
					'-Wl,-rpath,@loader_path',
					'-Wl,-rpath,@loader_path/../node_modules/deps-bullet-raub/<(bin)',
					'-Wl,-rpath,@loader_path/../../deps-bullet-raub/<(bin)',
					'-lBulletCollision', '-lBulletDynamics', '-lLinearMath'
				],
				'MACOSX_DEPLOYMENT_TARGET': '10.9',
				'defines': ['__APPLE__'],
				'CLANG_CXX_LIBRARY': 'libc++',
				'OTHER_CFLAGS': ['-std=c++17', '-fno-exceptions'],
			}],
			['OS=="win"', {
				'libraries': [
					'-lBulletCollision', '-lBulletDynamics', '-lLinearMath'
				],
				'defines': ['WIN32_LEAN_AND_MEAN', 'VC_EXTRALEAN', '_WIN32', '_HAS_EXCEPTIONS=0'],
				'msvs_settings' : {
					'VCCLCompilerTool' : {
						'AdditionalOptions' : [
							'/O2','/Oy','/GL','/GF','/Gm-', '/std:c++17',
							'/EHa-s-c-','/MT','/GS','/Gy','/GR-','/Gd',
						]
					},
					'VCLinkerTool' : {
						'AdditionalOptions' : ['/DEBUG:NONE', '/LTCG', '/OPT:NOREF'],
					},
				},
			}],
		],
	}],
}
