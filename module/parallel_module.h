
namespace zerver {

template<class BaseModule>
class ParallelModule<BaseModule> : public Module {
  public:
    class ModuleData : public IModuleData {
      private:
        uint32_t para_num;
    };

    ParallelModule(const std::string& name, uint32_t max_para_num) {
    }

    virtual ModuleDataPtr create_data() { 
      return ModuleDataPtr(new ModuleData); 
    }

    bool is_async();
    virtual void pre_run(FsmContextPtr context);
    virtual void post_run(FsmContextPtr context);
    virtual ModState run(FsmContextPtr context);

  protected:
    virtual uint32_t get_para_num(FsmContextPtr context) = 0;

    std::vector<BaseModule*> base_mods_;
};

}
