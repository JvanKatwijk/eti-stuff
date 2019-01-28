/****************************************************************************
** Meta object code from reading C++ file 'mp4processor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "includes/backend/audio/mp4processor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mp4processor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_mp4Processor_t {
    QByteArrayData data[6];
    char stringdata0[69];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_mp4Processor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_mp4Processor_t qt_meta_stringdata_mp4Processor = {
    {
QT_MOC_LITERAL(0, 0, 12), // "mp4Processor"
QT_MOC_LITERAL(1, 13, 16), // "show_frameErrors"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 13), // "show_rsErrors"
QT_MOC_LITERAL(4, 45, 14), // "show_aacErrors"
QT_MOC_LITERAL(5, 60, 8) // "isStereo"

    },
    "mp4Processor\0show_frameErrors\0\0"
    "show_rsErrors\0show_aacErrors\0isStereo"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_mp4Processor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       3,    1,   37,    2, 0x06 /* Public */,
       4,    1,   40,    2, 0x06 /* Public */,
       5,    1,   43,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void mp4Processor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        mp4Processor *_t = static_cast<mp4Processor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->show_frameErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->show_rsErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->show_aacErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->isStereo((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (mp4Processor::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mp4Processor::show_frameErrors)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (mp4Processor::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mp4Processor::show_rsErrors)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (mp4Processor::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mp4Processor::show_aacErrors)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (mp4Processor::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mp4Processor::isStereo)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject mp4Processor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_mp4Processor.data,
      qt_meta_data_mp4Processor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *mp4Processor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mp4Processor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_mp4Processor.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "dabProcessor"))
        return static_cast< dabProcessor*>(this);
    return QObject::qt_metacast(_clname);
}

int mp4Processor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void mp4Processor::show_frameErrors(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void mp4Processor::show_rsErrors(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void mp4Processor::show_aacErrors(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void mp4Processor::isStereo(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
