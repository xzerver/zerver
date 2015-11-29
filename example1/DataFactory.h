


class DataFactory : public IFsmDataFactory {
  virtual IFsmData* create_data(Fsm* fsm) {
    return new FsmData(fsm);
  }
};
