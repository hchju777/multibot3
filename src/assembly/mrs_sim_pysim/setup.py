from setuptools import find_packages, setup

package_name = 'mrs_sim_pysim'

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
    description='pysim 백엔드 프로세스 — 경량 결정론 시뮬 (unit/pysim 티어)',
    license='TBD',
    entry_points={
        'console_scripts': [
            'pysim_backend_node = mrs_sim_pysim.pysim_backend_node:main'
        ],
    },
)
