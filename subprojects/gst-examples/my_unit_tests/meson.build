#gtest_main_dep = dependency('gtest', main : true, fallback: ['gtest', 'gtest_main_dep'])
#gtest_dep = dependency('gtest', fallback: ['gtest', 'gtest_dep'])
#gmock_main_dep = dependency('gmock', main : true, fallback: ['gmock', 'gmock_main_dep'])
#gmock_dep = dependency('gmock', fallback: ['gmock', 'gmock_dep'])
gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')
gmock_dep = gtest_proj.get_variable('gmock_dep')
gmock_main_dep = gtest_proj.get_variable('gmock_main_dep')

executable('gst_gstreamer_tests',
           ['gst_test_main.cpp',
            'test_gstbus.cpp',
            'test_gstcaps.cpp',
            'test_gstevent.cpp',
            'test_gstmessage.cpp',
            'test_gstobject.cpp',
            'test_gstpad.cpp',
            'test_gststructure.cpp'],
           dependencies : [gst_dep, m_dep, gmock_dep])