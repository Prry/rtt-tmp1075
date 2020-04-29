from building import *
Import('rtconfig')

src   = []
cwd   = GetCurrentDir()

# add tmp1075 src files.
if GetDepend('PKG_USING_TMP1075'):
    src += Glob('src/tmp1075.c')

if GetDepend('PKG_USING_TMP1075_SAMPLE'):
    src += Glob('examples/tmp1075_sample.c')

# add tmp1075 include path.
path  = [cwd + '/inc']

# add src and include to group.
group = DefineGroup('tmp1075', src, depend = ['PKG_USING_TMP1075'], CPPPATH = path)

Return('group')
