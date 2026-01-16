from setuptools import setup, find_packages

setup(
    name='caret-tracker',
    version='1.0.0',
    description='A text caret tracking application for Ubuntu/Linux',
    author='Your Name',
    author_email='your.email@example.com',
    packages=find_packages(),
    python_requires='>=3.8',
    install_requires=[
        'PyGObject>=3.40.0',
    ],
    entry_points={
        'console_scripts': [
            'caret-tracker=src.main:main',
        ],
    },
    include_package_data=True,
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Environment :: X11 Applications :: GTK',
        'Intended Audience :: End Users/Desktop',
        'License :: OSI Approved :: MIT License',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
    ],
)
