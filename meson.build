project('sumcheck', 'c', default_options: [
    'optimization=2'
])
sources = files(
    'src/main.c', 
    'src/graph.c', 
    'src/matrix.c', 
    'src/lagrange.c', 
    'src/vector.c', 
    'src/modulo.c',
    'src/counting_triangles_prover.c',
    'src/counting_triangles_verifier.c',
    'src/mat_mult_prover.c',
    'src/mat_mult_verifier.c',
)
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
gmp = dependency('gmp')
gsl = dependency('gsl')
executable(
    'demo', 
    sources, 
    dependencies : [gmp, m_dep, gsl]
)
