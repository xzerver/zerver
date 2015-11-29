

class KaraModule : public SimpleModule{
  public:
    virtual ModState run_impl(IFsmData* data) {
      KaraModuleData* d = get_KaraModuleData();
    }

    KaraModuleData* get_KaraModuleData() {
      return run(static_cast<FsmData*>(data)->kara_mod_data_);
    }

}
