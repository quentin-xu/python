'''
Created on 2012-5-22
Modified on 2016-08-15

@author: xuqi
'''
from setuptools import  setup


setup(
      name='bbcplatform',
      url='http://www.haiziwang.com', 
      version='1.1.0',
      author='xuqi',
      author_email='',
      packages=['bbcplatform','jinja2'],
      data_files=[['bbcplatform',['bbcplatform/confg4set.so']]],
      install_requires=['MarkupSafe'],
      extras_require={'i18n': ['Babel>=0.8']},
      include_package_data=True,
      entry_points="""
      [babel.extractors]
      jinja2 = jinja2.ext:babel_extract[i18n]
      """
)
