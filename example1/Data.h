DEFINE(primitive, rw, int, v1)
DEFINE(object, w, Kara, kara)

class Data {
  public:
    void set_v1(int v1) { v1_ = v1; }
    int get_v1() const { return v1_; }
    void set_v2(int v2) { v2_ = v2; }
    int get_v2() const { return v2_; }
    Kara& get_mutable_kara() { return kara_; }
    const Kara& get_kara() const { return kara_; }
  private:
    int v1_;
    int v2_;
    Kara kara_;
};


class FsmData : public IFsmData{
  public:
    FsmData(Fsm* fsm) : IFsmData(fsm) {
    }

    KaraModuleData* get_KaraModuleData() {
      return kara_mod_data_;
    }
  private:
    KaraModuleData* kara_mod_data_;
    GuruModuleData* guru_mod_data_;
    Data* data_;
};
