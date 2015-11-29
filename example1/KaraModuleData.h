class KaraModuleData {
  public:
    KaraModuleData(Data* data) : data(data_) {}

    friend class KaraModule;
  private:
    inline void set_v1(int v1) { data_->set_v1(v1); }
    Data* data_;
};



