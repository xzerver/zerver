#include "assert.h"
#include <strings.h>
#include <sys/resource.h>
#include "DataFactory.h"
#include "framework/tcp_server.h"
#include "client/tcp_client_pool.h"
#include "conf/config_manager.h"
#include "conf/config_reload.h"
#include "LoadBalancerManager.h"
#include "ReqModule.h"
#include "GenFsReqModule.h"
#include "FsForkModule.h"
#include "FsModule.h"
#include "PsForkModule2.h"
#include "PsBackfillModule2.h"
#include "PsModule2.h"
#include "PsJoinModule2.h"
#include "FsJoinModule.h"
#include "GenPsReqModule.h"
#include "PsForkModule.h"
#include "PsModule.h"
#include "PsBackfillModule.h"
#include "PsJoinModule.h"
#include "RespModule.h"
#include "IfRandModule.h"
#include "RandRespModule.h"
#include "EmptyRespModule.h"
#include "EndModule.h"
#include "proxy_tcp_server.h"

using namespace zerver;

DEFINE_string(conf_path, "./conf/", "the config path.");
DEFINE_string(json_conf, "./conf/config.json", "the file of config information.");
DEFINE_string(run_mode, "thread", "run mode is thread/process");
DEFINE_int32(fs_max_shard_num, 8, "max shard num in GenFsReqModule");
DEFINE_int32(ps_max_shard_num, 3, "max shard num in GenPsReqModule");
DEFINE_int32(max_sub_model_id_size, 5, "max sub model id num it may has");
DEFINE_int32(task_num, 4, "task number for task.");
DEFINE_int32(port, 9990, "listening port");
DEFINE_bool(core_dump, false, "core dump");
DEFINE_bool(enable_reload, true, "enable reload.");
DEFINE_int32(fs_check_time, 20, "check feature server time interval.");
DEFINE_int32(ps_check_time, 60, "check predictor server time interval.");

/*
 *              GenFsReqModule -> FsForkModule -> [FsModule -> PsForkModule2 -> [PsModule2 -> PsBackfillModule2] -> PsJoinModule2] -> FsJoinModule -> 
 * ReqModule ->  
 *              GenPsReqModule -> PsForkModule -> [PsModule -> PsBackfillModule] -> PsJoinModule ->
 *
 *              -> RespModule -> EndModule
 */
int main(int argc, char** argv) {
  int ret = 0;
  run_mode mode = RUN_THREAD;
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging("proxy_server");
  if (FLAGS_core_dump) {
    struct rlimit limit;
    if (getrlimit(RLIMIT_CORE, &limit) == 0) {
      limit.rlim_cur = RLIM_INFINITY;
      if ( setrlimit(RLIMIT_CORE, &limit) != 0 ) {
        F_LOG(ERROR) << "set core dump failed.";
      }
      F_LOG(INFO) << "set core dump.";
    } else {
      F_LOG(ERROR) << "set core dump failed.";
    }
  }
  DataFactory* factory = new DataFactory();
  FsmContext::register_fsm_data_factory(factory);
  if(0 == strcasecmp(FLAGS_run_mode.c_str(), "process")) {
    mode = RUN_PROCESS;
  }

  ProxyTcpServer server(FLAGS_port, FLAGS_task_num);
  TcpClientPool fs_client_pool(FLAGS_fs_check_time, server.io_service());
  TcpClientPool ps_client_pool(FLAGS_ps_check_time, server.io_service());
  if (!FLAGS_enable_reload) { 
    ret = ConfigManager::set_instance(FLAGS_json_conf.c_str());
    assert(1 == ret);
  } else {
    boost::system::error_code error;
    if (!ConfigReload::Load(error) || !ConfigReload::get_instance()) {
      exit(-10);
    }
  }
  conf::Base::set_conf_path(FLAGS_conf_path);
  LoadBalancerManager::get_mutable_instance().initialize(&server.io_service());
  boost::shared_ptr<Fsm>  fsm = server.fsm();

  ReqModule* req_mod = new ReqModule("req_mod");
  fsm->add_module(req_mod);

  GenFsReqModule* gen_fs_req_mod = new GenFsReqModule("gen_fs_req_mod");
  fsm->add_module(gen_fs_req_mod);

  fsm->link_module(req_mod, gen_fs_req_mod, Mod_req_succeed_to_fs);

  static const int MAX_FS_FORK_NUM = FLAGS_fs_max_shard_num;

  FsForkModule* fs_fork_mod = new FsForkModule("fs_fork_mod", MAX_FS_FORK_NUM);
  fsm->add_module(fs_fork_mod);

  fsm->link_module(gen_fs_req_mod, fs_fork_mod, Mod_Succeed);

  FsJoinModule* fs_join_mod = new FsJoinModule("fs_join_mod", MAX_FS_FORK_NUM);
  fsm->add_module(fs_join_mod);

  char mod_name[128];
  for (int i=0; i<MAX_FS_FORK_NUM; i++) {
    snprintf(mod_name, sizeof(mod_name), "fs_mod%d", i);
    FsModule* fs_mod = new FsModule(fs_client_pool, mod_name, i);
    fsm->add_module(fs_mod);

    fsm->link_module(fs_fork_mod, fs_mod, Mod_Fork0 + i);
    
    const int MAX_PS2_FORK_NUM = FLAGS_max_sub_model_id_size;
    snprintf(mod_name, sizeof(mod_name), "ps_fork_mod2_%d", i);
    PsForkModule2* ps_fork_mod2 = new PsForkModule2(mod_name, MAX_PS2_FORK_NUM);
    fsm->add_module(ps_fork_mod2);

    fsm->link_module(fs_mod, ps_fork_mod2, Mod_Succeed);

    snprintf(mod_name, sizeof(mod_name), "ps_join_mod2_%d", i);
    PsJoinModule2* ps_join_mod2 = new PsJoinModule2(mod_name, MAX_PS2_FORK_NUM);
    fsm->add_module(ps_join_mod2);

    for (int j=0; j<MAX_PS2_FORK_NUM; j++) {
      snprintf(mod_name, sizeof(mod_name), "ps_mod2_%d_%d", i, j);
      PsModule2* ps_mod2 = new PsModule2(ps_client_pool, mod_name, i, j);
      fsm->add_module(ps_mod2);

      fsm->link_module(ps_fork_mod2, ps_mod2, Mod_Fork0 + j);
      fsm->link_module(ps_mod2, ps_join_mod2, Mod_Succeed);

      snprintf(mod_name, sizeof(mod_name), "ps_backfill_mod2_%d_%d", i, j);
      PsBackfillModule2* ps_backfill_mod2 = new PsBackfillModule2(mod_name, i, j);
      fsm->add_module(ps_backfill_mod2);
      fsm->link_module(ps_mod2, ps_backfill_mod2, Mod_else);

      fsm->link_module(ps_backfill_mod2, ps_join_mod2, Mod_Succeed);
      fsm->link_module(ps_backfill_mod2, ps_join_mod2, Mod_else);
    }

    fsm->link_module(ps_join_mod2, fs_join_mod, Mod_Succeed);
    fsm->link_module(ps_join_mod2, fs_join_mod, Mod_else);
    fsm->link_module(fs_mod, fs_join_mod, Mod_else);
  }

  GenPsReqModule* gen_ps_req_mod = new GenPsReqModule("gen_ps_req_mod");
  fsm->add_module(gen_ps_req_mod);
  fsm->link_module(req_mod, gen_ps_req_mod, Mod_req_succeed_to_ps);

  const int MAX_PS_FORK_NUM = FLAGS_max_sub_model_id_size * FLAGS_ps_max_shard_num;
  PsForkModule* ps_fork_mod = new PsForkModule("ps_fork_mod", MAX_PS_FORK_NUM);
  fsm->add_module(ps_fork_mod);
  fsm->link_module(gen_ps_req_mod, ps_fork_mod, Mod_Succeed);
   
  PsJoinModule* ps_join_mod = new PsJoinModule("ps_join_mod", MAX_PS_FORK_NUM);
  fsm->add_module(ps_join_mod);
  
  for (int i=0; i<MAX_PS_FORK_NUM; i++) {

    snprintf(mod_name, sizeof(mod_name), "ps_mod_%d", i);
    PsModule* ps_mod = new PsModule(ps_client_pool, mod_name, i);
    fsm->add_module(ps_mod);
    fsm->link_module(ps_fork_mod, ps_mod, Mod_Fork0 + i);

    snprintf(mod_name, sizeof(mod_name), "ps_backfill_mod_%d", i);
    PsBackfillModule* ps_backfill_mod = new PsBackfillModule(mod_name, i);
    fsm->add_module(ps_backfill_mod);
    fsm->link_module(ps_mod, ps_join_mod, Mod_Succeed);
    fsm->link_module(ps_mod, ps_backfill_mod, Mod_else);

    fsm->link_module(ps_backfill_mod, ps_join_mod, Mod_Succeed);
    fsm->link_module(ps_backfill_mod, ps_join_mod, Mod_else);
  }

  IfRandModule* if_rand_mod = new IfRandModule("if_rand_mod");
  fsm->add_module(if_rand_mod);
  fsm->link_module(fs_join_mod, if_rand_mod, Mod_Succeed);
  fsm->link_module(ps_join_mod, if_rand_mod, Mod_Succeed);

  RespModule* resp_mod = new RespModule("resp_mod");
  fsm->add_module(resp_mod);
  fsm->link_module(if_rand_mod, resp_mod, Mod_not_rand_resp);

  RandRespModule* rand_resp_mod = new RandRespModule("rand_resp_mod");
  fsm->add_module(rand_resp_mod);
  fsm->link_module(if_rand_mod, rand_resp_mod, Mod_rand_resp);

  fsm->link_module(req_mod, rand_resp_mod, Mod_req_succeed_to_random);

  EmptyRespModule* empty_resp_mod = new EmptyRespModule("empty_resp_mod");
  fsm->add_module(empty_resp_mod);
  fsm->link_module(fs_join_mod, empty_resp_mod, Mod_else);
  fsm->link_module(ps_join_mod, empty_resp_mod, Mod_else);

  EndModule* end_mod = new EndModule("end_mod");
  fsm->add_module(end_mod);

  fsm->link_module(resp_mod, end_mod, Mod_Succeed);
  fsm->link_module(resp_mod, end_mod, Mod_else);

  fsm->link_module(rand_resp_mod, end_mod, Mod_Succeed);
  fsm->link_module(rand_resp_mod, end_mod, Mod_else);

  fsm->link_module(empty_resp_mod, end_mod, Mod_Succeed);
  fsm->link_module(empty_resp_mod, end_mod, Mod_else);

  fsm->link_module(req_mod, end_mod, Mod_else);
  fsm->link_module(gen_fs_req_mod, empty_resp_mod, Mod_else);
  fsm->link_module(gen_ps_req_mod, empty_resp_mod, Mod_else);

  fsm->set_start_module(req_mod);
  fsm->set_end_module(end_mod);

  server.run(mode);
  ConfigReload::stop_timer();
  F_LOG(INFO) << "run done.";  
  delete factory;
  google::FlushLogFiles(google::INFO);
  google::ShutdownGoogleLogging();
  google::ShutDownCommandLineFlags();
  return 0;
}

