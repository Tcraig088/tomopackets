import os
import sys
import platform
import subprocess
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext

__version__ = "1.0.0"

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        super().__init__(name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def run(self):
        try:
            import cmake
        except ImportError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        print("extpath", extdir)
        lib_path = os.path.join(extdir, "build", 'py_tomop.lib')

        if os.path.exists(lib_path):
            print(f"Found existing library: {lib_path}")
        else:
            raise RuntimeError(f"Library {lib_path} not found. Please build it first.")

        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)

        # Move the built library to the correct location
        if platform.system() == "Windows":
            built_lib = os.path.join(self.build_temp, cfg, 'py_tomop.pyd')
        else:
            built_lib = os.path.join(self.build_temp, 'py_tomop.so')

        if os.path.exists(built_lib):
            dest_lib = os.path.join(extdir, os.path.basename(built_lib))
            os.rename(built_lib, dest_lib)
        else:
            raise RuntimeError(f"Built library {built_lib} not found.")

setup(
    name='tomopackets',
    version=__version__,
    author='Jan-Willem Buurlage',
    author_email='janwillembuurlage@gmail.com',
    url='https://github.com/cicwi/tomopackets',
    description='ZeroMQ based streaming for RT tomography',
    long_description='',
    ext_modules=[CMakeExtension('py_tomop')],
    cmdclass=dict(build_ext=CMakeBuild),
    packages=find_packages(include=['tomop']),
    zip_safe=False,
)
