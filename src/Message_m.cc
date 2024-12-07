//
// Generated file, do not edit! Created by opp_msgtool 6.0 from Message.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "Message_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Message_Base::Message_Base(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

Message_Base::Message_Base(const Message_Base& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

Message_Base::~Message_Base()
{
}

Message_Base& Message_Base::operator=(const Message_Base& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void Message_Base::copy(const Message_Base& other)
{
    this->header = other.header;
    this->paylaod = other.paylaod;
    this->trailer = other.trailer;
    this->frameType = other.frameType;
    this->ackNackNumber = other.ackNackNumber;
}

void Message_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->header);
    doParsimPacking(b,this->paylaod);
    doParsimPacking(b,this->trailer);
    doParsimPacking(b,this->frameType);
    doParsimPacking(b,this->ackNackNumber);
}

void Message_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->header);
    doParsimUnpacking(b,this->paylaod);
    doParsimUnpacking(b,this->trailer);
    doParsimUnpacking(b,this->frameType);
    doParsimUnpacking(b,this->ackNackNumber);
}

int Message_Base::getHeader() const
{
    return this->header;
}

void Message_Base::setHeader(int header)
{
    this->header = header;
}

const char * Message_Base::getPaylaod() const
{
    return this->paylaod.c_str();
}

void Message_Base::setPaylaod(const char * paylaod)
{
    this->paylaod = paylaod;
}

char Message_Base::getTrailer() const
{
    return this->trailer;
}

void Message_Base::setTrailer(char trailer)
{
    this->trailer = trailer;
}

int Message_Base::getFrameType() const
{
    return this->frameType;
}

void Message_Base::setFrameType(int frameType)
{
    this->frameType = frameType;
}

int Message_Base::getAckNackNumber() const
{
    return this->ackNackNumber;
}

void Message_Base::setAckNackNumber(int ackNackNumber)
{
    this->ackNackNumber = ackNackNumber;
}

class MessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_header,
        FIELD_paylaod,
        FIELD_trailer,
        FIELD_frameType,
        FIELD_ackNackNumber,
    };
  public:
    MessageDescriptor();
    virtual ~MessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(MessageDescriptor)

MessageDescriptor::MessageDescriptor() : omnetpp::cClassDescriptor("Message", "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

MessageDescriptor::~MessageDescriptor()
{
    delete[] propertyNames;
}

bool MessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Message_Base *>(obj)!=nullptr;
}

const char **MessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *MessageDescriptor::getProperty(const char *propertyName) const
{
    if (!strcmp(propertyName, "customize")) return "true";
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int MessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int MessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_header
        FD_ISEDITABLE,    // FIELD_paylaod
        FD_ISEDITABLE,    // FIELD_trailer
        FD_ISEDITABLE,    // FIELD_frameType
        FD_ISEDITABLE,    // FIELD_ackNackNumber
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *MessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "header",
        "paylaod",
        "trailer",
        "frameType",
        "ackNackNumber",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int MessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "header") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "paylaod") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "trailer") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "frameType") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "ackNackNumber") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *MessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_header
        "string",    // FIELD_paylaod
        "char",    // FIELD_trailer
        "int",    // FIELD_frameType
        "int",    // FIELD_ackNackNumber
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **MessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Message_Base *pp = omnetpp::fromAnyPtr<Message_Base>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void MessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Message_Base *pp = omnetpp::fromAnyPtr<Message_Base>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Message_Base'", field);
    }
}

const char *MessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Message_Base *pp = omnetpp::fromAnyPtr<Message_Base>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Message_Base *pp = omnetpp::fromAnyPtr<Message_Base>(object); (void)pp;
    switch (field) {
        case FIELD_header: return long2string(pp->getHeader());
        case FIELD_paylaod: return oppstring2string(pp->getPaylaod());
        case FIELD_trailer: return long2string(pp->getTrailer());
        case FIELD_frameType: return long2string(pp->getFrameType());
        case FIELD_ackNackNumber: return long2string(pp->getAckNackNumber());
        default: return "";
    }
}

void MessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Message_Base *pp = omnetpp::fromAnyPtr<Message_Base>(object); (void)pp;
    switch (field) {
        case FIELD_header: pp->setHeader(string2long(value)); break;
        case FIELD_paylaod: pp->setPaylaod((value)); break;
        case FIELD_trailer: pp->setTrailer(string2long(value)); break;
        case FIELD_frameType: pp->setFrameType(string2long(value)); break;
        case FIELD_ackNackNumber: pp->setAckNackNumber(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Message_Base'", field);
    }
}

omnetpp::cValue MessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Message_Base *pp = omnetpp::fromAnyPtr<Message_Base>(object); (void)pp;
    switch (field) {
        case FIELD_header: return pp->getHeader();
        case FIELD_paylaod: return pp->getPaylaod();
        case FIELD_trailer: return pp->getTrailer();
        case FIELD_frameType: return pp->getFrameType();
        case FIELD_ackNackNumber: return pp->getAckNackNumber();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Message_Base' as cValue -- field index out of range?", field);
    }
}

void MessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Message_Base *pp = omnetpp::fromAnyPtr<Message_Base>(object); (void)pp;
    switch (field) {
        case FIELD_header: pp->setHeader(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_paylaod: pp->setPaylaod(value.stringValue()); break;
        case FIELD_trailer: pp->setTrailer(omnetpp::checked_int_cast<char>(value.intValue())); break;
        case FIELD_frameType: pp->setFrameType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_ackNackNumber: pp->setAckNackNumber(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Message_Base'", field);
    }
}

const char *MessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr MessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Message_Base *pp = omnetpp::fromAnyPtr<Message_Base>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void MessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Message_Base *pp = omnetpp::fromAnyPtr<Message_Base>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Message_Base'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

