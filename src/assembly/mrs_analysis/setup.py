from setuptools import find_packages, setup

package_name = 'mrs_analysis'

setup(
    name=package_name,
    version='0.1.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Changju Hwang',
    maintainer_email='hchju23257@gmail.com',
    description='로그->지표 오프라인 통계 파이프라인 (baseline-repro 규격)',
    license='TBD',
    entry_points={
        'console_scripts': [
            'run_analysis = mrs_analysis.metrics_pipeline:main'
        ],
    },
)
