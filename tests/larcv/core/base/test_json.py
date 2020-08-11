import unittest
import larcv

def test_get_json_from_larcv():
    a = larcv.larcv_base.default_config()
    print(a)
    print(type(a))
    

# def test_pass_json_to_larcv():

#     # From larcv base, get the default config:
#     assert True