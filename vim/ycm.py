import vim

def FlagsForFile(filename):
    filetype = vim.eval('&filetype')

    if filetype == 'c':
        flags = ['-xc', '-Wall', '-Wextra', '-pedantic', '-I/usr/lib/openmpi/include', '-I/usr/lib/openmpi/include/openmpi']
    elif filetype == 'cpp':
        flags = ['-xc++', '-Wall', '-Wextra', '-pedantic']
    else:
        flags = []

    return {
            'flags': flags,
            'do_cache': True
    }

