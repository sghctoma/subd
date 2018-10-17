project(
	'subd',
	'c',
	version: '0.0.1',
	license: 'MIT',
	default_options: [
		'c_std=c11',
		'warning_level=2',
		'werror=true',
	],
)

add_project_arguments('-Wno-unused-parameter', language: 'c')
add_project_arguments('-Wno-missing-braces', language: 'c')

cc = meson.get_compiler('c')
prefix = get_option('prefix')

lib_subd = library(
	meson.project_name(),
	files([
		'subd-core.c',
		'subd-vtable.c',
		'subd-watch.c',
		'list.c',
	]),
	dependencies: dependency('dbus-1'),
	include_directories: include_directories('include'),
	install: true,
)
