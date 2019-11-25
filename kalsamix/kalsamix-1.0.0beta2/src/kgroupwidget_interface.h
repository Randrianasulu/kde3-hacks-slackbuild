#ifndef __KGROUPWIDGET_INTERFACE_H
#define __KGROUPWIDGET_INTERFACE_H

class KGroupWidgetInterface
{
public:
    KGroupWidgetInterface() {}
    virtual ~KGroupWidgetInterface() {}


private:
    KGroupWidgetInterface( const KGroupWidgetInterface& source );
    void operator = ( const KGroupWidgetInterface& source );
};


#endif // __KGROUPWIDGET_INTERFACE_H
