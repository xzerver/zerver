#ifndef __FSM_DATA_DEFINES_H__
#define __FSM_DATA_DEFINES_H__


#define DEFINE_MODULE_DATA(KaraModuleData, kara_mod_data) \
public: \
  KaraModuleData* get_##KaraModuleData() { \
    if (kara_mod_data##_.is_null()) { \
      kara_mod_data##_.reset(new KaraModuleData(data_)); \
    } \
    return kara_mod_data##_.get(); \
  } \
private: \
  boost::shared_ptr<KaraModuleData> kara_mod_data##_;


#define DEFINE_PRIM_VAR_R(type, v1) \
public: \
  type get_##v1() { return v1##_; } \
private: \
  type v1##_;

#define DEFINE_PRIM_VAR_W(type, v1) \
public: \
  type set_##v1(type v1) { v1##_ = v1; } \
private: \
  type v1##_;

#define DEFINE_PRIM_VAR_RW(type, v1) \
public: \
  type get_##v1() { return v1##_; } \
  type set_##v1(type v1) { v1##_ = v1; } \
private: \
  type v1##_;

#define DEFINE_OBJ_VAR_R(type, o1) \
public: \
  const type& get_const_##o1() { return o1##_; } \
private: \
  type o1##_;

#define DEFINE_OBJ_VAR_W(type, o1) \
public: \
  type& get_mutable_##o1() { return o1##_; }  \
private: \
  type o1##_;

#define DEFINE_OBJ_VAR_RW(type, o1) \
public: \
  const type& get_const_##o1() { return o1##_; } \
  type& get_mutable_##o1() { return o1##_; }  \
private: \
  type o1##_;

#endif
