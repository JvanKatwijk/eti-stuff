/****************************************************************************
** Meta object code from reading C++ file 'radio.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "radio.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'radio.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RadioInterface_t {
    QByteArrayData data[26];
    char stringdata0[361];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RadioInterface_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RadioInterface_t qt_meta_stringdata_RadioInterface = {
    {
QT_MOC_LITERAL(0, 0, 14), // "RadioInterface"
QT_MOC_LITERAL(1, 15, 16), // "toggle_show_data"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 13), // "clearEnsemble"
QT_MOC_LITERAL(4, 47, 13), // "addtoEnsemble"
QT_MOC_LITERAL(5, 61, 14), // "nameofEnsemble"
QT_MOC_LITERAL(6, 76, 16), // "show_frameErrors"
QT_MOC_LITERAL(7, 93, 13), // "show_rsErrors"
QT_MOC_LITERAL(8, 107, 14), // "show_aacErrors"
QT_MOC_LITERAL(9, 122, 15), // "show_ficSuccess"
QT_MOC_LITERAL(10, 138, 13), // "show_ipErrors"
QT_MOC_LITERAL(11, 152, 9), // "showLabel"
QT_MOC_LITERAL(12, 162, 7), // "showMOT"
QT_MOC_LITERAL(13, 170, 21), // "changeinConfiguration"
QT_MOC_LITERAL(14, 192, 8), // "newAudio"
QT_MOC_LITERAL(15, 201, 14), // "handle_fibData"
QT_MOC_LITERAL(16, 216, 14), // "show_mscErrors"
QT_MOC_LITERAL(17, 231, 9), // "setStereo"
QT_MOC_LITERAL(18, 241, 16), // "show_motHandling"
QT_MOC_LITERAL(19, 258, 18), // "set_streamSelector"
QT_MOC_LITERAL(20, 277, 8), // "setStart"
QT_MOC_LITERAL(21, 286, 16), // "TerminateProcess"
QT_MOC_LITERAL(22, 303, 17), // "updateTimeDisplay"
QT_MOC_LITERAL(23, 321, 13), // "selectService"
QT_MOC_LITERAL(24, 335, 11), // "QModelIndex"
QT_MOC_LITERAL(25, 347, 13) // "set_audioDump"

    },
    "RadioInterface\0toggle_show_data\0\0"
    "clearEnsemble\0addtoEnsemble\0nameofEnsemble\0"
    "show_frameErrors\0show_rsErrors\0"
    "show_aacErrors\0show_ficSuccess\0"
    "show_ipErrors\0showLabel\0showMOT\0"
    "changeinConfiguration\0newAudio\0"
    "handle_fibData\0show_mscErrors\0setStereo\0"
    "show_motHandling\0set_streamSelector\0"
    "setStart\0TerminateProcess\0updateTimeDisplay\0"
    "selectService\0QModelIndex\0set_audioDump"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RadioInterface[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  129,    2, 0x08 /* Private */,
       3,    0,  130,    2, 0x0a /* Public */,
       4,    1,  131,    2, 0x0a /* Public */,
       5,    2,  134,    2, 0x0a /* Public */,
       6,    1,  139,    2, 0x0a /* Public */,
       7,    1,  142,    2, 0x0a /* Public */,
       8,    1,  145,    2, 0x0a /* Public */,
       9,    1,  148,    2, 0x0a /* Public */,
      10,    1,  151,    2, 0x0a /* Public */,
      11,    1,  154,    2, 0x0a /* Public */,
      12,    3,  157,    2, 0x0a /* Public */,
      13,    0,  164,    2, 0x0a /* Public */,
      14,    1,  165,    2, 0x0a /* Public */,
      15,    0,  168,    2, 0x0a /* Public */,
      16,    1,  169,    2, 0x0a /* Public */,
      17,    1,  172,    2, 0x0a /* Public */,
      18,    1,  175,    2, 0x0a /* Public */,
      19,    1,  178,    2, 0x0a /* Public */,
      20,    0,  181,    2, 0x08 /* Private */,
      21,    0,  182,    2, 0x08 /* Private */,
      22,    0,  183,    2, 0x08 /* Private */,
      23,    1,  184,    2, 0x08 /* Private */,
      25,    0,  187,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::Int, QMetaType::QString,    2,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 24,    2,
    QMetaType::Void,

       0        // eod
};

void RadioInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RadioInterface *_t = static_cast<RadioInterface *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->toggle_show_data(); break;
        case 1: _t->clearEnsemble(); break;
        case 2: _t->addtoEnsemble((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->nameofEnsemble((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->show_frameErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->show_rsErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->show_aacErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->show_ficSuccess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->show_ipErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->showLabel((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->showMOT((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 11: _t->changeinConfiguration(); break;
        case 12: _t->newAudio((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->handle_fibData(); break;
        case 14: _t->show_mscErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->setStereo((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: _t->show_motHandling((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->set_streamSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->setStart(); break;
        case 19: _t->TerminateProcess(); break;
        case 20: _t->updateTimeDisplay(); break;
        case 21: _t->selectService((*reinterpret_cast< QModelIndex(*)>(_a[1]))); break;
        case 22: _t->set_audioDump(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject RadioInterface::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_RadioInterface.data,
      qt_meta_data_RadioInterface,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RadioInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RadioInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RadioInterface.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int RadioInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 23;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
