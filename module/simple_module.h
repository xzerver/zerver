


class SimpleModule : public Module {
  public:
    bool is_async() { return false; }

    ModState run(IFsmData* data);

  protected:
    virtual ModState run_impl(IFsmData* data) = 0;
}
