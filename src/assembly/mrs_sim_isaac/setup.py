from setuptools import find_packages, setup

package_name = 'mrs_sim_isaac'

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
    description='isaac 어댑터 자리 — NotImplemented 스텁 (진화 단계 대상)',
    license='TBD',
    entry_points={
        'console_scripts': [
            'isaac_backend_node = mrs_sim_isaac.isaac_backend_node:main'
        ],
    },
)
