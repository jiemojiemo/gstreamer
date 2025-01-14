plugin_c_args = ['-DHAVE_CONFIG_H']
plugin_cpp_args = ['-DHAVE_CONFIG_H', '-std=c++17']
plugins_install_dir = join_paths(get_option('libdir'), 'gstreamer-1.0')

# The myfilter Plugin
gstmyfilter_sources = [
    'gstmyfilter.cpp',
]

gstmyfilterexample = library('gstmyfilter',
                             gstmyfilter_sources,
                             c_args: plugin_c_args,
                             cpp_args: plugin_cpp_args,
                             dependencies : [gst_dep, gstbase_dep],
                             install : true,
                             install_dir : plugins_install_dir,
)
