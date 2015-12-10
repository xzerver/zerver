

#define BEGIN_CLASS(KaraModule, SimpleModule, KaraModuleData)  \
class KaraModule : public SimpleModule{ \
  public: \
    KaraModuleData* get_KaraModuleData(FsmContextPtr context) { \
      return static_cast<FsmData*>(context->data().get())->get_KaraModuleData(); \
    }


#define ENDCLASS(KaraModule) \
};


