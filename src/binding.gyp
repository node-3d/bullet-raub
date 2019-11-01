{
	'variables': {
		'bin'            : '<!(node -p "require(\'addon-tools-raub\').bin")',
		'bullet_include' : '<!(node -p "require(\'deps-bullet-raub\').include")',
		'bullet_bin'     : '<!(node -p "require(\'deps-bullet-raub\').bin")',
	},
	'targets': [
		{
			'target_name' : 'bullet',
			'sources' : [
				'cpp/bindings.cpp',
				'cpp/body.cpp',
				'cpp/joint.cpp',
				'cpp/scene.cpp',
			],
			'include_dirs' : [
				'<!@(node -p "require(\'addon-tools-raub\').include")',
				'<(bullet_include)',
			],
			'library_dirs' : [ '<(bullet_bin)' ],
			'libraries'    : [ '-lbullet' ],
			'conditions': [
				
				[
					'OS=="linux"',
					{
						'libraries': [
							"-Wl,-rpath,'$$ORIGIN'",
							"-Wl,-rpath,'$$ORIGIN/../node_modules/deps-bullet-raub/<(bin)'",
							"-Wl,-rpath,'$$ORIGIN/../../deps-bullet-raub/<(bin)'",
						],
						'defines': ['__linux__'],
					}
				],
				
				[
					'OS=="mac"',
					{
						'libraries': [
							'-Wl,-rpath,@loader_path',
							'-Wl,-rpath,@loader_path/../node_modules/deps-bullet-raub/<(bin)',
							'-Wl,-rpath,@loader_path/../../deps-bullet-raub/<(bin)',
						],
						'defines': ['__APPLE__'],
					}
				],
				
				[
					'OS=="win"',
					{
						'defines' : [
							'WIN32_LEAN_AND_MEAN',
							'VC_EXTRALEAN',
							'_WIN32',
						],
						'msvs_settings' : {
							'VCCLCompilerTool' : {
								'AdditionalOptions' : [
									'/GL', '/GF', '/EHsc', '/GS', '/Gy', '/GR-',
								]
							},
							'VCLinkerTool' : {
								'AdditionalOptions' : ['/RELEASE','/OPT:REF','/OPT:ICF','/LTCG'],
							},
						},
					},
				],
				
			],
		},
	]
}
