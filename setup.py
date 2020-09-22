from setuptools import setup

def readme():
    with open('README.md') as f:
        return f.read()

setup(name='gunit',
        version = '0.1',
        description = 'App for unit testing with GDB',
        long_description = readme()
        url = 'https://github.com/estcube/GUnit',
        author = 'Mathias Plans',
        author_email = 'mathiasplans15@gmail.com',
        license = 'MIT',
        packages = ['gunit'],
        install_requires = ['junit-xml'],
        zip_safe = False)