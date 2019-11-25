#ifndef __KAMSYSTEMTRAY_INTERFACE_H
#define __KAMSYSTEMTRAY_INTERFACE_H

class KamixSystemTrayInterface
{
public:
    KAMSystemTrayInterface() {}
    virtual ~KAMSystemTrayInterface() {}


private:
    KAMSystemTrayInterface( const KAMSystemTrayInterface& source );
    void operator = ( const KAMSystemTrayInterface& source );
};


#endif // __KAMSYSTEMTRAY_INTERFACE_H
