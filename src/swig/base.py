# This file was automatically generated by SWIG (http://www.swig.org).
# Version 3.0.12
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.

from sys import version_info as _swig_python_version_info
if _swig_python_version_info >= (2, 7, 0):
    def swig_import_helper():
        import importlib
        pkg = __name__.rpartition('.')[0]
        mname = '.'.join((pkg, '_base')).lstrip('.')
        try:
            return importlib.import_module(mname)
        except ImportError:
            return importlib.import_module('_base')
    _base = swig_import_helper()
    del swig_import_helper
elif _swig_python_version_info >= (2, 6, 0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_base', [dirname(__file__)])
        except ImportError:
            import _base
            return _base
        try:
            _mod = imp.load_module('_base', fp, pathname, description)
        finally:
            if fp is not None:
                fp.close()
        return _mod
    _base = swig_import_helper()
    del swig_import_helper
else:
    import _base
del _swig_python_version_info

try:
    _swig_property = property
except NameError:
    pass  # Python < 2.2 doesn't have 'property'.

try:
    import builtins as __builtin__
except ImportError:
    import __builtin__

def _swig_setattr_nondynamic(self, class_type, name, value, static=1):
    if (name == "thisown"):
        return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name, None)
    if method:
        return method(self, value)
    if (not static):
        if _newclass:
            object.__setattr__(self, name, value)
        else:
            self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)


def _swig_setattr(self, class_type, name, value):
    return _swig_setattr_nondynamic(self, class_type, name, value, 0)


def _swig_getattr(self, class_type, name):
    if (name == "thisown"):
        return self.this.own()
    method = class_type.__swig_getmethods__.get(name, None)
    if method:
        return method(self)
    raise AttributeError("'%s' object has no attribute '%s'" % (class_type.__name__, name))


def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except __builtin__.Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except __builtin__.Exception:
    class _object:
        pass
    _newclass = 0

kDEBUG = _base.kDEBUG
kINFO = _base.kINFO
kNORMAL = _base.kNORMAL
kWARNING = _base.kWARNING
kERROR = _base.kERROR
kCRITICAL = _base.kCRITICAL
kMSG_TYPE_MAX = _base.kMSG_TYPE_MAX
class logger(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, logger, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, logger, name)
    __repr__ = _swig_repr

    def __init__(self, *args):
        this = _base.new_logger(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_logger
    __del__ = lambda self: None

    def name(self):
        return _base.logger_name(self)

    def set(self, level):
        return _base.logger_set(self, level)

    def level(self):
        return _base.logger_level(self)

    def __lt__(self, rhs):
        return _base.logger___lt__(self, rhs)
    if _newclass:
        get = staticmethod(_base.logger_get)
    else:
        get = _base.logger_get
    if _newclass:
        get_shared = staticmethod(_base.logger_get_shared)
    else:
        get_shared = _base.logger_get_shared
    if _newclass:
        default_level = staticmethod(_base.logger_default_level)
    else:
        default_level = _base.logger_default_level
    if _newclass:
        force_level = staticmethod(_base.logger_force_level)
    else:
        force_level = _base.logger_force_level

    def debug(self):
        return _base.logger_debug(self)

    def info(self):
        return _base.logger_info(self)

    def normal(self):
        return _base.logger_normal(self)

    def warning(self):
        return _base.logger_warning(self)

    def error(self):
        return _base.logger_error(self)

    def send(self, *args):
        return _base.logger_send(self, *args)
logger_swigregister = _base.logger_swigregister
logger_swigregister(logger)
cvar = _base.cvar
kINVALID_LONGLONG = cvar.kINVALID_LONGLONG
kINVALID_ULONGLONG = cvar.kINVALID_ULONGLONG
kINVALID_SIZE = cvar.kINVALID_SIZE
kINVALID_INT = cvar.kINVALID_INT
kINVALID_UINT = cvar.kINVALID_UINT
kINVALID_SHORT = cvar.kINVALID_SHORT
kINVALID_USHORT = cvar.kINVALID_USHORT
kINVALID_FLOAT = cvar.kINVALID_FLOAT
kINVALID_DOUBLE = cvar.kINVALID_DOUBLE
kStringPrefix = cvar.kStringPrefix

def logger_get(name):
    return _base.logger_get(name)
logger_get = _base.logger_get

def logger_get_shared():
    return _base.logger_get_shared()
logger_get_shared = _base.logger_get_shared

def logger_default_level(*args):
    return _base.logger_default_level(*args)
logger_default_level = _base.logger_default_level

def logger_force_level(l):
    return _base.logger_force_level(l)
logger_force_level = _base.logger_force_level

class larcv_base(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, larcv_base, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, larcv_base, name)
    __repr__ = _swig_repr

    def __init__(self, *args):
        this = _base.new_larcv_base(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_larcv_base
    __del__ = lambda self: None

    def logger(self):
        return _base.larcv_base_logger(self)

    def set_verbosity(self, level):
        return _base.larcv_base_set_verbosity(self, level)

    def name(self):
        return _base.larcv_base_name(self)
larcv_base_swigregister = _base.larcv_base_swigregister
larcv_base_swigregister(larcv_base)


def ConfigFile2String(fname):
    return _base.ConfigFile2String(fname)
ConfigFile2String = _base.ConfigFile2String

def CreatePSetFromFile(*args):
    return _base.CreatePSetFromFile(*args)
CreatePSetFromFile = _base.CreatePSetFromFile
class PSet(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PSet, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PSet, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _base.delete_PSet
    __del__ = lambda self: None

    def __init__(self, *args):
        this = _base.new_PSet(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this

    def name(self):
        return _base.PSet_name(self)

    def __eq__(self, rhs):
        return _base.PSet___eq__(self, rhs)

    def __ne__(self, rhs):
        return _base.PSet___ne__(self, rhs)

    def rename(self, name):
        return _base.PSet_rename(self, name)

    def clear(self):
        return _base.PSet_clear(self)

    def add(self, data):
        return _base.PSet_add(self, data)

    def add_value(self, key, value):
        return _base.PSet_add_value(self, key, value)

    def add_pset(self, *args):
        return _base.PSet_add_pset(self, *args)

    def dump(self, indent_size=0):
        return _base.PSet_dump(self, indent_size)

    def data_string(self):
        return _base.PSet_data_string(self)

    def get_pset(self, key):
        return _base.PSet_get_pset(self, key)

    def size(self):
        return _base.PSet_size(self)

    def keys(self):
        return _base.PSet_keys(self)

    def value_keys(self):
        return _base.PSet_value_keys(self)

    def pset_keys(self):
        return _base.PSet_pset_keys(self)

    def contains_value(self, key):
        return _base.PSet_contains_value(self, key)

    def contains_pset(self, key):
        return _base.PSet_contains_pset(self, key)
PSet_swigregister = _base.PSet_swigregister
PSet_swigregister(PSet)

class Watch(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Watch, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Watch, name)
    __repr__ = _swig_repr

    def __init__(self):
        this = _base.new_Watch()
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_Watch
    __del__ = lambda self: None

    def Start(self):
        return _base.Watch_Start(self)

    def WallTime(self):
        return _base.Watch_WallTime(self)

    def CPUTime(self):
        return _base.Watch_CPUTime(self)
Watch_swigregister = _base.Watch_swigregister
Watch_swigregister(Watch)

class ConfigManager(larcv_base):
    __swig_setmethods__ = {}
    for _s in [larcv_base]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, ConfigManager, name, value)
    __swig_getmethods__ = {}
    for _s in [larcv_base]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, ConfigManager, name)
    __repr__ = _swig_repr

    def __init__(self):
        this = _base.new_ConfigManager()
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_ConfigManager
    __del__ = lambda self: None
    if _newclass:
        get = staticmethod(_base.ConfigManager_get)
    else:
        get = _base.ConfigManager_get

    def AddConfigFile(self, cfg_file):
        return _base.ConfigManager_AddConfigFile(self, cfg_file)

    def AddConfigString(self, cfg_str):
        return _base.ConfigManager_AddConfigString(self, cfg_str)

    def GetConfig(self, cfg):
        return _base.ConfigManager_GetConfig(self, cfg)
ConfigManager_swigregister = _base.ConfigManager_swigregister
ConfigManager_swigregister(ConfigManager)

def ConfigManager_get():
    return _base.ConfigManager_get()
ConfigManager_get = _base.ConfigManager_get

class Extents_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Extents_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Extents_t, name)
    __repr__ = _swig_repr
    __swig_setmethods__["first"] = _base.Extents_t_first_set
    __swig_getmethods__["first"] = _base.Extents_t_first_get
    if _newclass:
        first = _swig_property(_base.Extents_t_first_get, _base.Extents_t_first_set)
    __swig_setmethods__["last"] = _base.Extents_t_last_set
    __swig_getmethods__["last"] = _base.Extents_t_last_get
    if _newclass:
        last = _swig_property(_base.Extents_t_last_get, _base.Extents_t_last_set)

    def __init__(self):
        this = _base.new_Extents_t()
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_Extents_t
    __del__ = lambda self: None
Extents_t_swigregister = _base.Extents_t_swigregister
Extents_t_swigregister(Extents_t)
kINVALID_INDEX = cvar.kINVALID_INDEX
kINVALID_INSTANCEID = cvar.kINVALID_INSTANCEID
kINVALID_PROJECTIONID = cvar.kINVALID_PROJECTIONID

kDISCONNECTED = _base.kDISCONNECTED
kDEAD = _base.kDEAD
kLOWNOISE = _base.kLOWNOISE
kNOISY = _base.kNOISY
kGOOD = _base.kGOOD
kUNKNOWN = _base.kUNKNOWN
kUnitUnknown = _base.kUnitUnknown
kUnitCM = _base.kUnitCM
kUnitWireTime = _base.kUnitWireTime
kPoolSum = _base.kPoolSum
kPoolAverage = _base.kPoolAverage
kPoolMax = _base.kPoolMax
kShapeShower = _base.kShapeShower
kShapeTrack = _base.kShapeTrack
kShapeUnknown = _base.kShapeUnknown
kPoint3D = _base.kPoint3D
kPointXY = _base.kPointXY
kPointYZ = _base.kPointYZ
kPointZX = _base.kPointZX
class Point2D(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Point2D, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Point2D, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _base.delete_Point2D
    __del__ = lambda self: None

    def __init__(self, *args):
        this = _base.new_Point2D(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_setmethods__["x"] = _base.Point2D_x_set
    __swig_getmethods__["x"] = _base.Point2D_x_get
    if _newclass:
        x = _swig_property(_base.Point2D_x_get, _base.Point2D_x_set)
    __swig_setmethods__["y"] = _base.Point2D_y_set
    __swig_getmethods__["y"] = _base.Point2D_y_get
    if _newclass:
        y = _swig_property(_base.Point2D_y_get, _base.Point2D_y_set)

    def __eq__(self, rhs):
        return _base.Point2D___eq__(self, rhs)

    def __ne__(self, rhs):
        return _base.Point2D___ne__(self, rhs)

    def __imul__(self, rhs):
        return _base.Point2D___imul__(self, rhs)

    def __itruediv__(self, *args):
        return _base.Point2D___itruediv__(self, *args)
    __idiv__ = __itruediv__



    def __iadd__(self, rhs):
        return _base.Point2D___iadd__(self, rhs)

    def __isub__(self, rhs):
        return _base.Point2D___isub__(self, rhs)

    def __mul__(self, rhs):
        return _base.Point2D___mul__(self, rhs)

    def __truediv__(self, *args):
        return _base.Point2D___truediv__(self, *args)
    __div__ = __truediv__



    def __add__(self, rhs):
        return _base.Point2D___add__(self, rhs)

    def __sub__(self, rhs):
        return _base.Point2D___sub__(self, rhs)

    def squared_distance(self, pt):
        return _base.Point2D_squared_distance(self, pt)

    def distance(self, pt):
        return _base.Point2D_distance(self, pt)

    def direction(self, pt):
        return _base.Point2D_direction(self, pt)
Point2D_swigregister = _base.Point2D_swigregister
Point2D_swigregister(Point2D)
kINVALID_VOXELID = cvar.kINVALID_VOXELID
kNOTPRESENT = cvar.kNOTPRESENT
kNEGATIVEPEDESTAL = cvar.kNEGATIVEPEDESTAL
kINVALID_PRODUCER = cvar.kINVALID_PRODUCER

class Point3D(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Point3D, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Point3D, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _base.delete_Point3D
    __del__ = lambda self: None

    def __init__(self, *args):
        this = _base.new_Point3D(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_setmethods__["x"] = _base.Point3D_x_set
    __swig_getmethods__["x"] = _base.Point3D_x_get
    if _newclass:
        x = _swig_property(_base.Point3D_x_get, _base.Point3D_x_set)
    __swig_setmethods__["y"] = _base.Point3D_y_set
    __swig_getmethods__["y"] = _base.Point3D_y_get
    if _newclass:
        y = _swig_property(_base.Point3D_y_get, _base.Point3D_y_set)
    __swig_setmethods__["z"] = _base.Point3D_z_set
    __swig_getmethods__["z"] = _base.Point3D_z_get
    if _newclass:
        z = _swig_property(_base.Point3D_z_get, _base.Point3D_z_set)

    def __eq__(self, rhs):
        return _base.Point3D___eq__(self, rhs)

    def __ne__(self, rhs):
        return _base.Point3D___ne__(self, rhs)

    def __itruediv__(self, *args):
        return _base.Point3D___itruediv__(self, *args)
    __idiv__ = __itruediv__



    def __imul__(self, rhs):
        return _base.Point3D___imul__(self, rhs)

    def __iadd__(self, rhs):
        return _base.Point3D___iadd__(self, rhs)

    def __isub__(self, rhs):
        return _base.Point3D___isub__(self, rhs)

    def __truediv__(self, *args):
        return _base.Point3D___truediv__(self, *args)
    __div__ = __truediv__



    def __mul__(self, rhs):
        return _base.Point3D___mul__(self, rhs)

    def __add__(self, rhs):
        return _base.Point3D___add__(self, rhs)

    def __sub__(self, rhs):
        return _base.Point3D___sub__(self, rhs)

    def squared_distance(self, pt):
        return _base.Point3D_squared_distance(self, pt)

    def distance(self, pt):
        return _base.Point3D_distance(self, pt)

    def direction(self, pt):
        return _base.Point3D_direction(self, pt)
Point3D_swigregister = _base.Point3D_swigregister
Point3D_swigregister(Point3D)

class Vertex(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Vertex, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Vertex, name)
    __repr__ = _swig_repr

    def __init__(self, *args):
        this = _base.new_Vertex(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this

    def reset(self, *args):
        return _base.Vertex_reset(self, *args)

    def as_point2d(self, point_type):
        return _base.Vertex_as_point2d(self, point_type)

    def as_point3d(self):
        return _base.Vertex_as_point3d(self)

    def as_point(self, point_type, x, y, z):
        return _base.Vertex_as_point(self, point_type, x, y, z)

    def x(self):
        return _base.Vertex_x(self)

    def y(self):
        return _base.Vertex_y(self)

    def z(self):
        return _base.Vertex_z(self)

    def t(self):
        return _base.Vertex_t(self)

    def __eq__(self, rhs):
        return _base.Vertex___eq__(self, rhs)

    def __ne__(self, rhs):
        return _base.Vertex___ne__(self, rhs)

    def __lt__(self, rhs):
        return _base.Vertex___lt__(self, rhs)

    def dump(self):
        return _base.Vertex_dump(self)
    __swig_destroy__ = _base.delete_Vertex
    __del__ = lambda self: None
Vertex_swigregister = _base.Vertex_swigregister
Vertex_swigregister(Vertex)

PARTICLE_PROCESS_STRLEN = _base.PARTICLE_PROCESS_STRLEN
class Particle(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Particle, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Particle, name)
    __repr__ = _swig_repr

    def __init__(self, *args):
        this = _base.new_Particle(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_Particle
    __del__ = lambda self: None

    def px(self):
        return _base.Particle_px(self)

    def py(self):
        return _base.Particle_py(self)

    def pz(self):
        return _base.Particle_pz(self)

    def p(self):
        return _base.Particle_p(self)

    def x(self):
        return _base.Particle_x(self)

    def y(self):
        return _base.Particle_y(self)

    def z(self):
        return _base.Particle_z(self)

    def t(self):
        return _base.Particle_t(self)

    def parent_x(self):
        return _base.Particle_parent_x(self)

    def parent_y(self):
        return _base.Particle_parent_y(self)

    def parent_z(self):
        return _base.Particle_parent_z(self)

    def parent_t(self):
        return _base.Particle_parent_t(self)

    def ancestor_x(self):
        return _base.Particle_ancestor_x(self)

    def ancestor_y(self):
        return _base.Particle_ancestor_y(self)

    def ancestor_z(self):
        return _base.Particle_ancestor_z(self)

    def ancestor_t(self):
        return _base.Particle_ancestor_t(self)

    def id(self, *args):
        return _base.Particle_id(self, *args)

    def mcst_index(self, *args):
        return _base.Particle_mcst_index(self, *args)

    def mct_index(self, *args):
        return _base.Particle_mct_index(self, *args)

    def shape(self, *args):
        return _base.Particle_shape(self, *args)

    def nu_current_type(self, *args):
        return _base.Particle_nu_current_type(self, *args)

    def nu_interaction_type(self, *args):
        return _base.Particle_nu_interaction_type(self, *args)

    def track_id(self, *args):
        return _base.Particle_track_id(self, *args)

    def pdg_code(self, *args):
        return _base.Particle_pdg_code(self, *args)

    def momentum(self, px, py, pz):
        return _base.Particle_momentum(self, px, py, pz)

    def position(self, *args):
        return _base.Particle_position(self, *args)

    def end_position(self, *args):
        return _base.Particle_end_position(self, *args)

    def first_step(self, *args):
        return _base.Particle_first_step(self, *args)

    def last_step(self, *args):
        return _base.Particle_last_step(self, *args)

    def distance_travel(self, *args):
        return _base.Particle_distance_travel(self, *args)

    def energy_init(self, *args):
        return _base.Particle_energy_init(self, *args)

    def energy_deposit(self, *args):
        return _base.Particle_energy_deposit(self, *args)

    def creation_process(self, *args):
        return _base.Particle_creation_process(self, *args)

    def parent_track_id(self, *args):
        return _base.Particle_parent_track_id(self, *args)

    def parent_pdg_code(self, *args):
        return _base.Particle_parent_pdg_code(self, *args)

    def parent_position(self, *args):
        return _base.Particle_parent_position(self, *args)

    def ancestor_track_id(self, *args):
        return _base.Particle_ancestor_track_id(self, *args)

    def ancestor_pdg_code(self, *args):
        return _base.Particle_ancestor_pdg_code(self, *args)

    def ancestor_position(self, *args):
        return _base.Particle_ancestor_position(self, *args)

    def dump(self):
        return _base.Particle_dump(self)
Particle_swigregister = _base.Particle_swigregister
Particle_swigregister(Particle)

class Voxel(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Voxel, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Voxel, name)
    __repr__ = _swig_repr

    def __init__(self, *args):
        this = _base.new_Voxel(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_Voxel
    __del__ = lambda self: None

    def id(self):
        return _base.Voxel_id(self)

    def value(self):
        return _base.Voxel_value(self)

    def set(self, id, value):
        return _base.Voxel_set(self, id, value)

    def __iadd__(self, value):
        return _base.Voxel___iadd__(self, value)

    def __isub__(self, value):
        return _base.Voxel___isub__(self, value)

    def __imul__(self, factor):
        return _base.Voxel___imul__(self, factor)

    def __itruediv__(self, *args):
        return _base.Voxel___itruediv__(self, *args)
    __idiv__ = __itruediv__



    def __eq__(self, *args):
        return _base.Voxel___eq__(self, *args)

    def __lt__(self, *args):
        return _base.Voxel___lt__(self, *args)

    def __le__(self, *args):
        return _base.Voxel___le__(self, *args)

    def __gt__(self, *args):
        return _base.Voxel___gt__(self, *args)

    def __ge__(self, *args):
        return _base.Voxel___ge__(self, *args)
Voxel_swigregister = _base.Voxel_swigregister
Voxel_swigregister(Voxel)

class kINVALID_VOXEL(Voxel):
    __swig_setmethods__ = {}
    for _s in [Voxel]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, kINVALID_VOXEL, name, value)
    __swig_getmethods__ = {}
    for _s in [Voxel]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, kINVALID_VOXEL, name)

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined")
    __repr__ = _swig_repr
    if _newclass:
        getInstance = staticmethod(_base.kINVALID_VOXEL_getInstance)
    else:
        getInstance = _base.kINVALID_VOXEL_getInstance
    __swig_destroy__ = _base.delete_kINVALID_VOXEL
    __del__ = lambda self: None
kINVALID_VOXEL_swigregister = _base.kINVALID_VOXEL_swigregister
kINVALID_VOXEL_swigregister(kINVALID_VOXEL)

def kINVALID_VOXEL_getInstance():
    return _base.kINVALID_VOXEL_getInstance()
kINVALID_VOXEL_getInstance = _base.kINVALID_VOXEL_getInstance

class VoxelSet(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, VoxelSet, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, VoxelSet, name)
    __repr__ = _swig_repr

    def __init__(self):
        this = _base.new_VoxelSet()
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_VoxelSet
    __del__ = lambda self: None

    def as_vector(self):
        return _base.VoxelSet_as_vector(self)

    def find(self, id):
        return _base.VoxelSet_find(self, id)

    def sum(self):
        return _base.VoxelSet_sum(self)

    def mean(self):
        return _base.VoxelSet_mean(self)

    def max(self):
        return _base.VoxelSet_max(self)

    def min(self):
        return _base.VoxelSet_min(self)

    def size(self):
        return _base.VoxelSet_size(self)

    def clear_data(self):
        return _base.VoxelSet_clear_data(self)

    def reserve(self, num):
        return _base.VoxelSet_reserve(self, num)

    def threshold(self, min, max):
        return _base.VoxelSet_threshold(self, min, max)

    def threshold_min(self, min):
        return _base.VoxelSet_threshold_min(self, min)

    def threshold_max(self, max):
        return _base.VoxelSet_threshold_max(self, max)

    def add(self, vox):
        return _base.VoxelSet_add(self, vox)

    def insert(self, vox):
        return _base.VoxelSet_insert(self, vox)

    def emplace(self, *args):
        return _base.VoxelSet_emplace(self, *args)

    def id(self, *args):
        return _base.VoxelSet_id(self, *args)

    def __iadd__(self, value):
        return _base.VoxelSet___iadd__(self, value)

    def __isub__(self, value):
        return _base.VoxelSet___isub__(self, value)

    def __imul__(self, factor):
        return _base.VoxelSet___imul__(self, factor)

    def __itruediv__(self, *args):
        return _base.VoxelSet___itruediv__(self, *args)
    __idiv__ = __itruediv__


VoxelSet_swigregister = _base.VoxelSet_swigregister
VoxelSet_swigregister(VoxelSet)

class VoxelSetArray(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, VoxelSetArray, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, VoxelSetArray, name)
    __repr__ = _swig_repr

    def __init__(self):
        this = _base.new_VoxelSetArray()
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_VoxelSetArray
    __del__ = lambda self: None

    def size(self):
        return _base.VoxelSetArray_size(self)

    def voxel_set(self, id):
        return _base.VoxelSetArray_voxel_set(self, id)

    def as_vector(self):
        return _base.VoxelSetArray_as_vector(self)

    def sum(self):
        return _base.VoxelSetArray_sum(self)

    def mean(self):
        return _base.VoxelSetArray_mean(self)

    def max(self):
        return _base.VoxelSetArray_max(self)

    def min(self):
        return _base.VoxelSetArray_min(self)

    def threshold(self, min, max):
        return _base.VoxelSetArray_threshold(self, min, max)

    def threshold_min(self, min):
        return _base.VoxelSetArray_threshold_min(self, min)

    def threshold_max(self, max):
        return _base.VoxelSetArray_threshold_max(self, max)

    def clear_data(self):
        return _base.VoxelSetArray_clear_data(self)

    def resize(self, num):
        return _base.VoxelSetArray_resize(self, num)

    def writeable_voxel_set(self, id):
        return _base.VoxelSetArray_writeable_voxel_set(self, id)

    def emplace(self, *args):
        return _base.VoxelSetArray_emplace(self, *args)

    def insert(self, voxel_v):
        return _base.VoxelSetArray_insert(self, voxel_v)

    def move(self, orig):
        return _base.VoxelSetArray_move(self, orig)
VoxelSetArray_swigregister = _base.VoxelSetArray_swigregister
VoxelSetArray_swigregister(VoxelSetArray)

class EventBase(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, EventBase, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, EventBase, name)
    __repr__ = _swig_repr

    def clear(self):
        return _base.EventBase_clear(self)

    def initialize(self, arg2):
        return _base.EventBase_initialize(self, arg2)

    def serialize(self, group):
        return _base.EventBase_serialize(self, group)

    def deserialize(self, group, entry):
        return _base.EventBase_deserialize(self, group, entry)

    def __init__(self):
        this = _base.new_EventBase()
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_EventBase
    __del__ = lambda self: None
EventBase_swigregister = _base.EventBase_swigregister
EventBase_swigregister(EventBase)

class DataProductFactoryBase(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, DataProductFactoryBase, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, DataProductFactoryBase, name)

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    __swig_destroy__ = _base.delete_DataProductFactoryBase
    __del__ = lambda self: None

    def create(self):
        return _base.DataProductFactoryBase_create(self)
DataProductFactoryBase_swigregister = _base.DataProductFactoryBase_swigregister
DataProductFactoryBase_swigregister(DataProductFactoryBase)

class DataProductFactory(larcv_base):
    __swig_setmethods__ = {}
    for _s in [larcv_base]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, DataProductFactory, name, value)
    __swig_getmethods__ = {}
    for _s in [larcv_base]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, DataProductFactory, name)
    __repr__ = _swig_repr

    def __init__(self):
        this = _base.new_DataProductFactory()
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_DataProductFactory
    __del__ = lambda self: None
    if _newclass:
        get = staticmethod(_base.DataProductFactory_get)
    else:
        get = _base.DataProductFactory_get

    def add_factory(self, type, factory):
        return _base.DataProductFactory_add_factory(self, type, factory)

    def create(self, *args):
        return _base.DataProductFactory_create(self, *args)

    def list(self):
        return _base.DataProductFactory_list(self)

    def unique_product_count(self):
        return _base.DataProductFactory_unique_product_count(self)

    def product_names(self):
        return _base.DataProductFactory_product_names(self)
DataProductFactory_swigregister = _base.DataProductFactory_swigregister
DataProductFactory_swigregister(DataProductFactory)

def DataProductFactory_get():
    return _base.DataProductFactory_get()
DataProductFactory_get = _base.DataProductFactory_get

class IOManager(larcv_base):
    __swig_setmethods__ = {}
    for _s in [larcv_base]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, IOManager, name, value)
    __swig_getmethods__ = {}
    for _s in [larcv_base]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, IOManager, name)
    __repr__ = _swig_repr
    kREAD = _base.IOManager_kREAD
    kWRITE = _base.IOManager_kWRITE
    kBOTH = _base.IOManager_kBOTH

    def __init__(self, *args):
        this = _base.new_IOManager(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_IOManager
    __del__ = lambda self: None

    def io_mode(self):
        return _base.IOManager_io_mode(self)

    def reset(self):
        return _base.IOManager_reset(self)

    def add_in_file(self, *args):
        return _base.IOManager_add_in_file(self, *args)

    def clear_in_file(self):
        return _base.IOManager_clear_in_file(self)

    def set_out_file(self, name):
        return _base.IOManager_set_out_file(self, name)

    def producer_id(self, name):
        return _base.IOManager_producer_id(self, name)

    def configure(self, cfg):
        return _base.IOManager_configure(self, cfg)

    def initialize(self):
        return _base.IOManager_initialize(self)

    def read_entry(self, index, force_reload=False):
        return _base.IOManager_read_entry(self, index, force_reload)

    def save_entry(self):
        return _base.IOManager_save_entry(self)

    def finalize(self):
        return _base.IOManager_finalize(self)

    def clear_entry(self):
        return _base.IOManager_clear_entry(self)

    def set_id(self, run, subrun, event):
        return _base.IOManager_set_id(self, run, subrun, event)

    def current_entry(self):
        return _base.IOManager_current_entry(self)

    def get_n_entries_out(self):
        return _base.IOManager_get_n_entries_out(self)

    def get_file_out_name(self):
        return _base.IOManager_get_file_out_name(self)

    def get_n_entries(self):
        return _base.IOManager_get_n_entries(self)

    def get_data(self, *args):
        return _base.IOManager_get_data(self, *args)

    def event_id(self):
        return _base.IOManager_event_id(self)

    def last_event_id(self):
        return _base.IOManager_last_event_id(self)

    def producer_list(self, type):
        return _base.IOManager_producer_list(self, type)

    def product_list(self):
        return _base.IOManager_product_list(self)

    def file_list(self):
        return _base.IOManager_file_list(self)
IOManager_swigregister = _base.IOManager_swigregister
IOManager_swigregister(IOManager)

class EventParticle(EventBase):
    __swig_setmethods__ = {}
    for _s in [EventBase]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, EventParticle, name, value)
    __swig_getmethods__ = {}
    for _s in [EventBase]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, EventParticle, name)
    __repr__ = _swig_repr

    def __init__(self):
        this = _base.new_EventParticle()
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_EventParticle
    __del__ = lambda self: None

    def clear(self):
        return _base.EventParticle_clear(self)

    def set(self, part_v):
        return _base.EventParticle_set(self, part_v)

    def append(self, part):
        return _base.EventParticle_append(self, part)

    def emplace_back(self, part):
        return _base.EventParticle_emplace_back(self, part)

    def emplace(self, part_v):
        return _base.EventParticle_emplace(self, part_v)

    def as_vector(self):
        return _base.EventParticle_as_vector(self)

    def size(self):
        return _base.EventParticle_size(self)

    def initialize(self, group):
        return _base.EventParticle_initialize(self, group)

    def serialize(self, group):
        return _base.EventParticle_serialize(self, group)

    def deserialize(self, group, entry):
        return _base.EventParticle_deserialize(self, group, entry)
    if _newclass:
        to_particle = staticmethod(_base.EventParticle_to_particle)
    else:
        to_particle = _base.EventParticle_to_particle
EventParticle_swigregister = _base.EventParticle_swigregister
EventParticle_swigregister(EventParticle)

def EventParticle_to_particle(e):
    return _base.EventParticle_to_particle(e)
EventParticle_to_particle = _base.EventParticle_to_particle

class EventParticleFactory(DataProductFactoryBase):
    __swig_setmethods__ = {}
    for _s in [DataProductFactoryBase]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, EventParticleFactory, name, value)
    __swig_getmethods__ = {}
    for _s in [DataProductFactoryBase]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, EventParticleFactory, name)
    __repr__ = _swig_repr

    def __init__(self):
        this = _base.new_EventParticleFactory()
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _base.delete_EventParticleFactory
    __del__ = lambda self: None

    def create(self):
        return _base.EventParticleFactory_create(self)
EventParticleFactory_swigregister = _base.EventParticleFactory_swigregister
EventParticleFactory_swigregister(EventParticleFactory)

# This file is compatible with both classic and new-style classes.


