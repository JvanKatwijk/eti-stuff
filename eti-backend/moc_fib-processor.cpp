/****************************************************************************
** Meta object code from reading C++ file 'fib-processor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "includes/backend/fib-processor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fib-processor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_fib_processor_t {
    QByteArrayData data[7];
    char stringdata0[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_fib_processor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_fib_processor_t qt_meta_stringdata_fib_processor = {
    {
QT_MOC_LITERAL(0, 0, 13), // "fib_processor"
QT_MOC_LITERAL(1, 14, 15), // "addEnsembleChar"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 13), // "addtoEnsemble"
QT_MOC_LITERAL(4, 45, 14), // "nameofEnsemble"
QT_MOC_LITERAL(5, 60, 21), // "changeinConfiguration"
QT_MOC_LITERAL(6, 82, 11) // "show_ficCRC"

    },
    "fib_processor\0addEnsembleChar\0\0"
    "addtoEnsemble\0nameofEnsemble\0"
    "changeinConfiguration\0show_ficCRC"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_fib_processor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       3,    1,   44,    2, 0x06 /* Public */,
       4,    2,   47,    2, 0x06 /* Public */,
       5,    0,   52,    2, 0x06 /* Public */,
       6,    1,   53,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Char, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void fib_processor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        fib_processor *_t = static_cast<fib_processor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->addEnsembleChar((*reinterpret_cast< char(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->addtoEnsemble((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->nameofEnsemble((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->changeinConfiguration(); break;
        case 4: _t->show_ficCRC((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (fib_processor::*)(char , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&fib_processor::addEnsembleChar)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (fib_processor::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&fib_processor::addtoEnsemble)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (fib_processor::*)(int , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&fib_processor::nameofEnsemble)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (fib_processor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&fib_processor::changeinConfiguration)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (fib_processor::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&fib_processor::show_ficCRC)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject fib_processor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_fib_processor.data,
      qt_meta_data_fib_processor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *fib_processor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *fib_processor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_fib_processor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int fib_processor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void fib_processor::addEnsembleChar(char _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void fib_processor::addtoEnsemble(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void fib_processor::nameofEnsemble(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void fib_processor::changeinConfiguration()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void fib_processor::show_ficCRC(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
