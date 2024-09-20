#gtest_main_dep = dependency('gtest', main : true, fallback: ['gtest', 'gtest_main_dep'])
#gtest_dep = dependency('gtest', fallback: ['gtest', 'gtest_dep'])
#gmock_main_dep = dependency('gmock', main : true, fallback: ['gmock', 'gmock_main_dep'])
#gmock_dep = dependency('gmock', fallback: ['gmock', 'gmock_dep'])
gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')
gmock_dep = gtest_proj.get_variable('gmock_dep')
gmock_main_dep = gtest_proj.get_variable('gmock_main_dep')

executable('test_a',
           ['test_a.cpp'],
           dependencies : [gst_dep, m_dep, gmock_main_dep])
#
#executable('basic-tutorial-2',
#           ['basic-tutorial-2.c'],
#           dependencies : [gst_dep, m_dep])
#
#executable('basic-tutorial-2-exercise',
#           ['basic-tutorial-2-exercise.c'],
#           dependencies : [gst_dep, m_dep])
#
#executable('basic-tutorial-3',
#           ['basic-tutorial-3.c'],
#           dependencies : [gst_dep, m_dep])
#
#executable('basic-tutorial-3-exercise',
#           ['basic-tutorial-3-exercise.c'],
#           dependencies : [gst_dep, m_dep])
#
#executable('basic-tutorial-4',
#           ['basic-tutorial-4.c'],
#           dependencies : [gst_dep, m_dep])
#
#executable('basic-tutorial-6',
#           ['basic-tutorial-6.c'],
#           dependencies : [gst_dep, m_dep])
#
#executable('basic-tutorial-7',
#           ['basic-tutorial-7.c'],
#           dependencies : [gst_dep, m_dep])