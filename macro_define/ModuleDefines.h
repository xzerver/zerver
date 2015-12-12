

#define BEGIN_CLASS(KaraModule, SimpleModule, KaraModuleData)  \
class KaraModule : public SimpleModule{ \
  public: \
    KaraModuleData* get_##KaraModuleData(FsmContextPtr context) { \
      return static_cast<FsmData*>(context->data().get())->get_##KaraModuleData(); \
    }


#define ENDCLASS() \
};


