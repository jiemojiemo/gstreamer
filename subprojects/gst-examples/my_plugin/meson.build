plugin_c_args = ['-DHAVE_CONFIG_H']
plugins_install_dir = join_paths(get_option('libdir'), 'gstreamer-1.0')

# The myfilter Plugin
gstmyfilter_sources = [
    'gstmyfilter.c',
]

gstmyfilterexample = library('gstmyfilter',
                             gstmyfilter_sources,
                             c_args: plugin_c_args,
                             dependencies : [gst_dep, gstbase_dep],
                             install : true,
                             install_dir : plugins_install_dir,
)
