#ifndef __MODULE_DEFINES_H__
#define __MODULE_DEFINES_H__

#define BEGIN_CLASS(KaraModule, SimpleModule, KaraModuleData)  \
class KaraModule : public SimpleModule{ \
  public: \
    KaraModuleData* get##KaraModuleData(FsmContextPtr context) { \
      return static_cast<FsmData*>(context->data().get())->get##KaraModuleData(); \
    }


#define ENDCLASS() \
};

#endif
