/****************************************************************************
** Meta object code from reading C++ file 'serialhandler.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/serialhandler.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialhandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_SerialHandler_t {
    uint offsetsAndSizes[32];
    char stringdata0[14];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[5];
    char stringdata4[18];
    char stringdata5[7];
    char stringdata6[15];
    char stringdata7[10];
    char stringdata8[15];
    char stringdata9[9];
    char stringdata10[15];
    char stringdata11[9];
    char stringdata12[9];
    char stringdata13[6];
    char stringdata14[16];
    char stringdata15[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SerialHandler_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SerialHandler_t qt_meta_stringdata_SerialHandler = {
    {
        QT_MOC_LITERAL(0, 13),  // "SerialHandler"
        QT_MOC_LITERAL(14, 12),  // "dataReceived"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 4),  // "data"
        QT_MOC_LITERAL(33, 17),  // "portStatusChanged"
        QT_MOC_LITERAL(51, 6),  // "isOpen"
        QT_MOC_LITERAL(58, 14),  // "eegPacketReady"
        QT_MOC_LITERAL(73, 9),  // "timestamp"
        QT_MOC_LITERAL(83, 14),  // "QList<quint16>"
        QT_MOC_LITERAL(98, 8),  // "channels"
        QT_MOC_LITERAL(107, 14),  // "openSerialPort"
        QT_MOC_LITERAL(122, 8),  // "portName"
        QT_MOC_LITERAL(131, 8),  // "baudRate"
        QT_MOC_LITERAL(140, 5),  // "write"
        QT_MOC_LITERAL(146, 15),  // "closeSerialPort"
        QT_MOC_LITERAL(162, 15)   // "handleReadyRead"
    },
    "SerialHandler",
    "dataReceived",
    "",
    "data",
    "portStatusChanged",
    "isOpen",
    "eegPacketReady",
    "timestamp",
    "QList<quint16>",
    "channels",
    "openSerialPort",
    "portName",
    "baudRate",
    "write",
    "closeSerialPort",
    "handleReadyRead"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SerialHandler[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   56,    2, 0x06,    1 /* Public */,
       4,    1,   59,    2, 0x06,    3 /* Public */,
       6,    2,   62,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      10,    2,   67,    2, 0x0a,    8 /* Public */,
      13,    1,   72,    2, 0x0a,   11 /* Public */,
      14,    0,   75,    2, 0x0a,   13 /* Public */,
      15,    0,   76,    2, 0x08,   14 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 8,    7,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   11,   12,
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject SerialHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SerialHandler.offsetsAndSizes,
    qt_meta_data_SerialHandler,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SerialHandler_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SerialHandler, std::true_type>,
        // method 'dataReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        // method 'portStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'eegPacketReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QList<quint16> &, std::false_type>,
        // method 'openSerialPort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint32, std::false_type>,
        // method 'write'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        // method 'closeSerialPort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void SerialHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SerialHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataReceived((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 1: _t->portStatusChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->eegPacketReady((*reinterpret_cast< std::add_pointer_t<quint32>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<quint16>>>(_a[2]))); break;
        case 3: _t->openSerialPort((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint32>>(_a[2]))); break;
        case 4: _t->write((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 5: _t->closeSerialPort(); break;
        case 6: _t->handleReadyRead(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<quint16> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SerialHandler::*)(const QByteArray & );
            if (_t _q_method = &SerialHandler::dataReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SerialHandler::*)(bool );
            if (_t _q_method = &SerialHandler::portStatusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SerialHandler::*)(quint32 , const QList<quint16> & );
            if (_t _q_method = &SerialHandler::eegPacketReady; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *SerialHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SerialHandler.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SerialHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void SerialHandler::dataReceived(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SerialHandler::portStatusChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SerialHandler::eegPacketReady(quint32 _t1, const QList<quint16> & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
