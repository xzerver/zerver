

#define BEGIN_MODULE_DATA(KaraModuleData, KaraModule) \
class KaraModuleData { \
  public: \
    KaraModuleData(Data* data) : data_(data) {} \
    friend class KaraModule; \
  private: \
    Data* data_;


#define DEFINE_PRIM_VAR_R(type, v1) \
  type get_##v1() { return data_->get_##v1(); } 
#define DEFINE_PRIM_VAR_W(type, v1) \
  void set_v1(type v1) { data_->set_v1(v1); } 
#define DEFINE_PRIM_VAR_RW(type, v1) \
  void set_v1(type v1) { data_->set_v1(v1); } \
  type get_v1() { return data_->get_v1(); } 

#define DEFINE_OBJ_VAR_R(type, o1) \
  const type& get_o1() { return data_->get_o1(); } 
#define DEFINE_OBJ_VAR_W(type, o1) \
  type& get_o1() { return data_->get_o1(); } 
#define DEFINE_OBJ_VAR_RW(type, o1) \
  const type& get_o1() { return data_->get_o1(); } \
  type& get_o1() { return data_->get_o1(); } 

#define END_MODULE_DATA() \
};
