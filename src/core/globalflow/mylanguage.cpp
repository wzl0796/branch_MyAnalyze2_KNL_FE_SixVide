#include "mylanguage.h"
#include "params/define.h"

MyLanguage myLan;

MyLanguage::MyLanguage(QObject *parent) : QObject(parent) {}

void MyLanguage::init() {
  english = tr("English");
  chinese_simplified = tr("Simplified Chinese");
  chinese_traditonal = tr("Traditional Chinese");
  russian = tr("Russian");
  vietnam = tr("Vietnam");
  thailand = tr("Thailand");
  spanish = tr("Spanish");
  turkey = tr("Turkish");
  farsi = tr("Farsi");
  french = tr("French");
  uyghur = tr("Uyghur");
  korean = tr("Korean");
  arabic = tr("Arabic");
  bulgarian = tr("Bulgarian");
  slovak = tr("Slovak");
  portuguese = tr("Portuguese");
  myanmar = tr("Myanmar");
  bengali = tr("Bengali");
  indonesia = tr("Indonesia");
  poland = tr("Poland");
  italy = tr("Italy");
  combodia = tr("Combodia");
  german = tr("German");
  hungarian = tr("Hungarian");
  javanese = tr("Javanese");

  space = tr("space");

  poweroff = tr("poweroff");
  save = tr("save");
  factor = tr("factor");
  user = tr("user");
  detect = tr("detect");
  open = tr("open");
  close = tr("close");
  schme = tr("schme");
  copy = tr("copy");
  rename = tr("rename");
  metal = tr("metal");
  stagp = tr("stagp");

  display = tr("display");
  home = tr("home");
  history = tr("history");
  prodouct = tr("prodouct");
  setting = tr("setting");
  AI = tr("AI");
  Identifity = tr("Identifity");
  System = tr("System");

  clean = tr("clean");
  feeder = tr("feeder");
  sens = tr("sens");

  from = tr("from");
  to = tr("to");
  same = tr("same");
  alone = tr("alone");
  net = tr("net");
  version = tr("version");
  back = tr("back");
  update = tr("update");
  refresh = tr("refresh");
  stop = tr("stop");
  tick = tr("tick");
  belt = tr("belt");

  batch = tr("batch");
  add = tr("add");
  del = tr("del");
  ok = tr("ok");
  cancel = tr("cancel");
  notice = tr("notice");
  machine = tr("machine");
  serial_num = tr("serial_num");
  password = tr("password");
  params = tr("params");
  sucess = tr("sucess");
  failed = tr("failed");
  auto_sample = tr("auto_sample");
  channel = tr("channel");
  scheduledfeedEn = tr("scheduledfeedEn");
  scheduledfeedtime = tr("scheduledfeedtime");

  detail = tr("detail");
  category = tr("category");
  area = tr("area");
  status = tr("status");
  number = tr("number");
  purity = tr("purity");
  ratio = tr("ratio");
  weight = tr("weight");
  little = tr("little");
  middle = tr("middle");
  big = tr("big");
  total = tr("total");

  gray_a = tr("gray_a");
  gray_b = tr("gray_b");
  discolor_a = tr("discolor_a");
  discolor_b = tr("discolor_b");
  name = tr("name");
  sensitivity = tr("sensitivity");
  mode = tr("mode");
  row_count = tr("row_count");
  bad_point = tr("bad_point");
  infection = tr("infection");
  infection_1 = tr("infection_1");
  infection_2 = tr("infection_2");
  cut_edge = tr("cut_edge");
  independent_delay = tr("independent_delay");
  lower_limit_thread = tr("lower_limit_thread");
  algorithm_enable = tr("algorithm_enable");
  algorithm_params = tr("algorithm_params");
  more_retain = tr("more_retain");
  retain = tr("retain");
  bright = tr("bright");
  dark = tr("dark");
  red = tr("red");
  green = tr("green");
  blue = tr("blue");
  red_green = tr("red_green");
  red_blue = tr("red_blue");
  green_blue = tr("green_blue");
  nr = tr("nr");
  ng = tr("ng");
  nb = tr("nb");
  lab_set = tr("lab_set");

  print = tr("print");
  exporting = tr("exporting");
  currentpage = tr("currentpage");
  time = tr("time");
  normal = tr("normal");
  abnormal = tr("abnormal");
  all = tr("all");
  date = tr("date");
  record = tr("record");
  duty_shift = tr("duty_shift");

  domain = tr("domain");
  aging = tr("aging");
  fluore = tr("fluore");
  Light_color = tr("Light_color");
  trans_white = tr("trans_white");

  upgrade = tr("upgrade");
  user_mode = tr("user_mode");
  factory_mode = tr("factory_mode");

  analyze = tr("analyze");
  camer = tr("camer");
  control_board = tr("control_board");
  light_board = tr("light_board");
  GPU = tr("GPU");
  halogen_board = tr("halogen_board");
  elec_valve = tr("elec_valve");
  low_valve = tr("low_valve");
  local = tr("local");
  Udisk = tr("Udisk");
  arm_linux = tr("arm_linux");
  board_AI = tr("board_AI");
  AImodel = tr("AImodel");
  forbid_cross_version = tr("forbid_cross_version");
  type = tr("type");
  download = tr("download");
  enable = tr("enable");
  file = tr("file");
  modelIdx = tr("modelIdx");
  clear_params = tr("clear_params");
  ailabel_count = tr("ailabel_count");
  cam_AIlabel_count = tr("cam_AIlabel_count");

  QR_code = tr("QR_code");
  wechatApp = tr("wechatApp");
  remote = tr("remote");
  cloud = tr("cloud");
  connect_server = tr("connect_server");
  dhclient = tr("dhclient");
  Manual_set = tr("Manual_set");
  address = tr("address");
  subnet_mask = tr("subnet_mask");
  gateway = tr("gateway");
  server = tr("server");
  port = tr("port");
  username = tr("username");

  vibrate_magnitude = tr("vibrate_magnitude");
  vibrator = tr("vibrator");

  language = tr("language");
  periph_device = tr("periph_device");
  calibrate = tr("calibrate");
  sorter_debug = tr("sorter_debug");
  lab_mutex = tr("lab_mutex");

  label = tr("label");
  target = tr("target");
  capture = tr("capture");
  lighting_name = tr("lighting_name");
  frequence = tr("frequence");
  gain = tr("gain");
  fast_adjust = tr("fast_adjust");
  fine_adjust = tr("fine_adjust");
  lighting = tr("lighting");
  background = tr("background");
  variation_range = tr("variation_range");
  lower_limit = tr("lower_limit");
  up_limit = tr("up_limit");
  resolution = tr("resolution");
  exposure_value = tr("exposure_value");
  Non_exposure_value = tr("Non_exposure_value");
  spectral_band = tr("spectral_band");
  sensor = tr("sensor");
  odd = tr("odd");

  halogen = tr("halogen");
  temperature = tr("temperature");
  threshold = tr("threshold");
  index = tr("index");

  apply_and_wait = tr("apply_and_wait");
  digital_gain = tr("digital_gain");
  analog_gain = tr("analog_gain");

  import = tr("import");
  synchronize = tr("synchronize");
  no_found_udisk = tr("no_found_udisk");
  timeout = tr("timeout");

  properity = tr("properity");
  coefficient = tr("coefficient");
  secondary = tr("secondary");
  shapeness = tr("shapeness");
  majorProduct = tr("majorProduct");
  set_majorProduct_first = tr("set_majorProduct_first");
  length = tr("length");
  width = tr("width");
  apply = tr("apply");
  scan_by_MeyerAPP = tr("scan_by_MeyerAPP");
  no_find_file = tr("no_find_file");

  bottle = tr("bottle");
  grains = tr("grains");

  internal = tr("internal");
  external = tr("external");
  loT = tr("loT");
  backup = tr("backup");
  restore = tr("restore");
  sys_params_fix = tr("sys_params_fix");

  servo = tr("servo");
  ONOFF = tr("ONOFF");
  speed = tr("speed");
  acceleration_time = tr("acceleration_time");
  PWM = tr("PWM");
  low_voltage = tr("low_voltage");
  high_voltage = tr("high_voltage");
  workmode = tr("workmode");
  background_bar = tr("background_bar");
  reset = tr("reset");
  plus_light = tr("plus_light");

  ejector_selfcheck = tr("ejector_selfcheck");
  start = tr("start");
  hig_speed = tr("hig_speed");
  low_speed = tr("low_speed");
  sigle_ejc = tr("sigle_ejc");
  cur_ejec = tr("cur_ejec");

  contains_specialCharters = tr("contains_specialCharters");
  sorter_joint_config = tr("sorter_joint_config");
  finished_product = tr("finished_product");
  waste_material = tr("waste_material");
  raw_material = tr("raw_material");
  Normal_open_valve = tr("Normal_open_valve");
  str_is_to_long = tr("str_is_to_long");
  empty = tr("empty");
  center_rect = tr("center_rect");
  low_res = tr("low_res");
  hig_res = tr("hig_res");
  ultra_hig_res = tr("ultra_hig_res");
  do_not_save = tr("do_not_save");
  clear = tr("clear");

  standard = tr("standard");
  tradition = tr("tradition");
  std_hyperspectral = tr("std_hyperspectral");
  ultraviolet = tr("ultraviolet");
  model_empty_or_no_AI = tr("model_empty_or_no_AI");
  pic_size_mismatch = tr("pic_size_mismatch");
  affect_detect_res = tr("affect_detect_res");
  selct_enable_lab = tr("selct_enable_lab");

  merger = tr("merger");
  leftMove = tr("leftMove");
  newitem = tr("newitem");
  historyitem = tr("historyitem");
  input_forbid_empyty = tr("input_forbid_empyty");
  good = tr("good");
  bad = tr("bad");
  particle = tr("particle");

  encryption = tr("encryption");
  confirm_sys_time = tr("confirm_sys_time");
  input_code = tr("input_code");
  invalid_code = tr("invalid_code");
  dc_length_error = tr("dc_length_error");
  dc_illegal_char = tr("dc_illegal_char");
  dc_id_mismatch = tr("dc_id_mismatch");
  msg_dc_expire = tr("msg_dc_expire");
  dc_illegal = tr("dc_illegal");
  dc_limit = tr("dc_limit");

  limit_remain = tr("limit_remain");
  dc_input_time_decode = tr("dc_input_time_decode");
  msg_time_abnormal = tr("msg_time_abnormal");
  remind_times = tr("remind_times");
  cfm_input_time_code = tr("cfm_input_time_code");

  encrypt_remind = tr("encrypt_remind");
  unremind = tr("unremind");
  Eng = tr("Eng");
  chinese = tr("chinese");
  average = tr("average");

  data_analyze = tr("data_analyze");

  sys_time_error = tr("sys_time_error");
  time_remain_days = tr("time_remain_days");
  time_remain_times = tr("time_remain_times");
  input_time = tr("input_time");

  reset_device_code = tr("reset_device_code");
  set_timezone = tr("set_timezone");
  confirm_reset_devicecode = tr("confirm_reset_devicecode");
  select_timezone = tr("select_timezone");
  confirm_set_timezone = tr("confirm_set_timezone");
  set_timezone_success = tr("set_timezone_success");
  set_timezone_failed = tr("set_timezone_failed");

  please_stop_detect = tr("please_stop_detect");
  confirm_shutdown = tr("confirm_shutdown");
  save_params_sucess = tr("save_params_sucess");
  save_params_failed = tr("save_params_failed");
  confirm_save_params = tr("confirm_save_params");
  switch_to_factorper = tr("switch_to_factorper");
  switch_to_userPer = tr("switch_to_userPer");
  please_input_password = tr("please_input_password");
  password_error = tr("password_error");
  auto_sample_channel = tr("auto_sample_channel");
  stop_auto_sample = tr("stop_auto_sample");
  over_max_schme_num = tr("over_max_schme_num");
  current_schme = tr("current_schme");
  confirm_copy_schme = tr("confirm_copy_schme");
  copy_schme_failed = tr("copy_schme_failed");
  confirm_del_schme = tr("confirm_del_schme");
  forbid_del_schme = tr("forbid_del_schme");
  confirm_rename_schme = tr("confirm_rename_schme");
  invalid_schme_name = tr("invalid_schme_name");
  input_schme_name = tr("input_schme_name");
  sample_abnormal = tr("sample_abnormal");

  switch_board_CL_to_factor = tr("switch_board_CL_to_factor");
  switch_board_CL_to_user = tr("switch_board_CL_to_user");
  erase_board_CL = tr("erase_board_CL");
  transmit_and_burn = tr("transmit_and_burn");
  switch_board_Cam_to_user = tr("switch_board_Cam_to_user");
  switch_board_Cam_to_factor = tr("switch_board_Cam_to_factor");
  switch_ejector_to_factor = tr("switch_ejector_to_factor");
  switch_ejector_to_user = tr("switch_ejector_to_user");
  erase_board_Ejector = tr("erase_board_Ejector");
  erase_board_Cam = tr("erase_board_Cam");
  select_file = tr("select_file");
  no_select_upgrade_board = tr("no_select_upgrade_board");
  no_find_udisk = tr("no_find_udisk");
  no_invalid_file = tr("no_invalid_file");
  open_file_failed = tr("open_file_failed");
  reset_upgrade_status = tr("reset_upgrade_status");
  burn_file_failed = tr("burn_file_failed");
  loss_file = tr("loss_file");
  check_file_failed = tr("check_file_failed");
  download_file_failed = tr("download_file_failed");
  no_start_upgrade = tr("no_start_upgrade");
  switch_factory_failed = tr("switch_factory_failed");
  switch_user_failed = tr("switch_user_failed");
  erase_failed = tr("erase_failed");
  upgrade_sucess = tr("upgrade_sucess");
  file_type_error = tr("file_type_error");

  wechat_QR = tr("wechat_QR");
  vpn_connect = tr("vpn_connect");
  input_error = tr("input_error");
  dhclient_failed = tr("dhclient_failed");
  disconect = tr("disconect");
  feeder_set = tr("feeder_set");
  clean_interval = tr("clean_interval");
  clean_time = tr("clean_time");
  airknife_time = tr("airknife_time");
  airknife_channel = tr("airknife_channel");

  tick_setting = tr("tick_setting");
  delay_time = tr("delay_time");
  blow_time = tr("blow_time");
  Rect_ratio = tr("Rect_ratio");
  trunc_time = tr("trunc_time");
  deg_time = tr("deg_time");
  hig_fre_protect = tr("hig_fre_protect");
  ejector_divide = tr("ejector_divide");
  ejector_onoff = tr("ejector_onoff");
  ejector_index = tr("ejector_index");
  ejector_piexl =tr("ejector_piexl");
  cacheline = tr("cacheline");
  positive_cache = tr("positive_cache");
  negative_cache = tr("negative_cache");
  times = tr("times");
  endtime_lessthan_starttime = tr("endtime_lessthan_starttime");
  no_select_item = tr("no_select_item");
  not_support_select_multi_item = tr("not_support_select_multi_item");
  require_data_and_wait = tr("require_data_and_wait");
  confirm_del = tr("confirm_del");
  export_data_and_wait = tr("export_data_and_wait");
  no_check_valid_data = tr("no_check_valid_data");
  export_sucess = tr("export_sucess");
  export_failed = tr("export_failed");
  del_all = tr("del_all");
  del_db = tr("del_db");
  selected_item = tr("selected_item");
  clear_selection = tr("clear_selection");
  operate_sucess = tr("operate_sucess");
  operate_failed = tr("operate_failed");
  export_select_item = tr("export_select_item");
  export_date_item = tr("export_date_item");
  array_row = tr("array_row");
  array_colum = tr("array_colum");
  select_export_config = tr("select_export_config");

  ori_wave = tr("ori_wave");
  adj_wave = tr("adj_wave");
  adj_signal = tr("adj_signal");
  adj_target = tr("adj_target");
  auto_adj = tr("auto_adj");
  light_adj = tr("light_adj");
  dark_adj = tr("dark_adj");
  frequence_adj = tr("frequence_adj");
  gain_adj = tr("gain_adj");
  lamp_adjust = tr("lamp_adjust");
  mat_lamp = tr("mat_lamp");
  back_lamp = tr("back_lamp");
  inf_lamp = tr("inf_lamp");
  background_params = tr("background_params");
  threshold_params = tr("threshold_params");
  confirm_calibrate = tr("confirm_calibrate");
  pixel_dived = tr("pixel_dived");
  begin_pixel = tr("begin_pixel");
  end_pixel = tr("end_pixel");
  auto_dived = tr("auto_dived");
  infra_bias = tr("infra_bias");
  lamp = tr("lamp");
  background_set = tr("background_set");
  background_conditon = tr("background_conditon");
  background_threshold = tr("background_threshold");
  background_uplimit = tr("background_uplimit");
  black_bg = tr("black_bg");
  red_bg = tr("red_bg");
  green_bg = tr("green_bg");
  yellow_bg = tr("yellow_bg");
  blue_bg = tr("blue_bg");
  purple_bg = tr("purple_bg");
  cyan_bg = tr("cyan_bg");
  white_bg = tr("white_bg");
  start_ejc = tr("start_ejc");
  end_ejc = tr("end_ejc");

  signal_debug = tr("signal_debug");
  label_params = tr("label_params");
  authority_card = tr("authority_card");
  machine_params = tr("machine_params");
  language_set = tr("language_set");
  over_max_picture_num = tr("over_max_picture_num");
  triger_lowlimit = tr("triger_lowlimit");
  no_found_picture = tr("no_found_picture");
  import_pic_sucess = tr("import_pic_sucess");
  import_pic_failed = tr("import_pic_failed");
  confirm_export_picture = tr("confirm_export_picture");
  params_set = tr("params_set");
  rm_background = tr("rm_background");
  capture_line = tr("capture_line");
  lab_hide = tr("lab_hide");
  shapeness_enable = tr("shapeness_enable");
  ori_lab = tr("ori_lab");
  meger_lab = tr("meger_lab");
  lab_Meger = tr("lab_Meger");
  serial_num_error = tr("serial_num_error");
  language_enable = tr("language_enable");
  mapping_lab = tr("mapping_lab");

  confirm_switch_lang = tr("confirm_switch_lang");
  switch_lang_sucess = tr("switch_lang_sucess");
  invalid_file = tr("invalid_file");
  update_lang_sucess = tr("update_lang_sucess");
  update_lang_failed = tr("update_lang_failed");
  restore_lang_sucess = tr("restore_lang_sucess");
  restore_lang_failed = tr("restore_lang_failed");

  assist_camer = tr("assist_camer");
  major_camer = tr("major_camer");
  sta_type = tr("sta_type");
  sorter_jointdebug = tr("sorter_jointdebug");
  sorter_adj_tips = tr("sorter_adj_tips");
  debug_info = tr("debug_info");
  impurity_sta = tr("impurity_sta");
  triger_type = tr("triger_type");
  lab_process = tr("lab_process");
  statics_gp_set = tr("statics_gp_set");
  camer_name_set = tr("camer_name_set");
  params_backup = tr("params_backup");
  params_restore = tr("params_restore");
  multilab = tr("multilab");
  light_sync = tr("light_sync");

  assist_camer = tr("assist_camer");
  major_camer = tr("major_camer");
  sta_type = tr("sta_type");
  sorter_jointdebug = tr("sorter_jointdebug");
  debug_info = tr("debug_info");
  impurity_sta = tr("impurity_sta");
  triger_type = tr("triger_type");
  lab_process = tr("lab_process");
  statics_gp_set = tr("statics_gp_set");
  camer_name_set = tr("camer_name_set");
  params_backup = tr("params_backup");
  params_restore = tr("params_restore");
  over_max_camer_num = tr("over_max_camer_num");
  camera_name = tr("camera_name");
  params_backup_sucess = tr("params_backup_sucess");
  params_backup_failed = tr("params_backup_failed");
  udisk_or_file_not_found = tr("udisk_or_file_not_found");
  restore_params_sucess = tr("restore_params_sucess");
  restore_params_failed = tr("restore_params_failed");

  halogen_set = tr("halogen_set");
  halogen_enable = tr("halogen_enable");
  servo_set = tr("servo_set");
  servo_Init = tr("servo_Init");
  Ejector_valve_set = tr("Ejector_valve_set");
  Ejector_valve_enalbe = tr("Ejector_valve_enalbe");
  interval_time = tr("interval_time");
  other_periph_device = tr("other_periph_device");
  clean_tank_alarm_enable = tr("clean_tank_alarm_enable");
  pressure_alarm_enable = tr("pressure_alarm_enable");
  elec_valve_enable = tr("elec_valve_enable");
  clean_tank_reset = tr("clean_tank_reset");
  elec_valve_set = tr("elec_valve_set");
  metal_detect = tr("metal_detect");

  coordinate_info = tr("coordinate_info");
  cal_coord = tr("cal_coord");
  water_cal = tr("water_cal");
  cal_group = tr("cal_group");
  cal_value = tr("cal_value");
  actual_value = tr("actual_value");
  select_cal_group = tr("select_cal_group");
  select_cal_group_first = tr("select_cal_group_first");
  please_calibrate_first = tr("please_calibrate_first");

  max_channel = tr("max_channel");
  num_threshold = tr("num_threshold");
  time_threshold = tr("time_threshold");
  sigle_channel_time = tr("sigle_channel_time");
  detect_delay = tr("detect_delay");
  sample_interval = tr("sample_interval");
  reset_coeff = tr("reset_coeff");
  adj_page = tr("adj_page");
  confirm_reset_coeff = tr("confirm_reset_coeff");

  connect_server_sucess = tr("connect_server_sucess");
  connect_server_failed = tr("connect_server_failed");
  commutication_sucess = tr("commutication_sucess");
  commutication_failed = tr("commutication_failed");
  aimodel_abnormal = tr("aimodel_abnormal");
  sorter_jointdebug_failed = tr("sorter_jointdebug_failed");
  Acc_Mode = tr("Acc_Mode");
  Cycle_time = tr("Cycle_time");
  samplechnelEb = tr("samplechnelEb");
  samplechnelProty = tr("samplechnelProty");

  more_set = tr("more_set");
  AImodel_info = tr("AImodel_info");
  camera_index = tr("camera_index");
  label_enable = tr("label_enable");
  AImodel_lst = tr("AImodel_lst");
  retain_line = tr("retain_line");
  cap_line = tr("cap_line");
  overlap_degree_coeff = tr("overlap_degree_coeff");
  area_threshold = tr("area_threshold");
  AImodel_type = tr("AImodel_type");
  capture_set = tr("capture_set");
  raw_pic = tr("raw_pic");
  mark_pic = tr("mark_pic");
  label_select = tr("label_select");
  pic_export = tr("pic_export");
  pic_preview = tr("pic_preview");
  log_export = tr("log_export");
  CropImg_export = tr("CropImg_export");
  pic_clear = tr("pic_clear");
  capture_type = tr("capture_type");
  label_priority = tr("label_priority");
  label_mix = tr("label_mix");
  no_such_aimodel = tr("no_such_aimodel");
  confirm_switch_model = tr("confirm_switch_model");
  mount_udisk_failed = tr("mount_udisk_failed");
  pre_vire_crop_img = tr("pre_vire_crop_img");
  pre_vire_img = tr("pre_vire_img");
  no_find_imgdir = tr("no_find_imgdir");
  no_imgfile = tr("no_imgfile");
  img_load_failed = tr("img_load_failed");
  unzip_file_failed = tr("unzip_file_failed");
  imgconvert = tr("imgconvert");
  imgcompress = tr("imgcompress");
  unknown_error = tr("unknown_error");
  msg_getfilesize = tr("msg_getfilesize");
  msg_file_open_failed = tr("msg_file_open_failed");
  msg_file_too_large = tr("msg_file_too_large");
  msg_udisk_not_enough = tr("msg_udisk_not_enough");
  msg_data_upload_error = tr("msg_data_upload_error");
  msg_no_data = tr("msg_no_data");
  msg_data_error = tr("msg_data_error");
  msg_data_index_error = tr("msg_data_index_error");
  msg_data_check_error = tr("msg_data_check_error");
  create_file = tr("create_file");
  file_copy = tr("file_copy");
  elapsed_time = tr("elapsed_time");
  ori_labimg = tr("ori_labimg");
  ;
  small_labimg = tr("small_labimg");
  ;
  previewtype = tr("previewtype");
  ;

  switch_to_advance = tr("switch_to_advance");
  user_already_bind = tr("user_already_bind");
  confirm_bind = tr("confirm_bind");
  switch_scheme = tr("switch_scheme");
  start_detect = tr("start_detect");
  input_batchname = tr("input_batchname");
  inputy_shiftname = tr("inputy_shiftname");
  invalid_serialnum = tr("invalid_serialnum");
  user_list = tr("user_list");

  up_camer = tr("up_camer");
  down_camer = tr("down_camer");
  ultra_camer = tr("ultra_camer");
  hyperspec_camer = tr("hyperspec_camer");

  up_bglight = tr("up_bglight");
  up_matlight = tr("up_matlight");
  down_bglight = tr("down_bglight");
  down_matlight = tr("down_matlight");
  ultra_light = tr("ultra_light");

  pie_chart = tr("pie_chart");
  bar_chart = tr("bar_chart");
  select_staGb = tr("select_staGb");
  calibrate_GGP_wave = tr("calibrate_GGP_wave");

  small = tr("small");
  switch_to_label = tr("switch_to_label");
  switch_to_shape = tr("switch_to_shape");

  auto_sample_abnormal = tr("auto_sample_abnormal");

  airalarm = tr("airalarm");
  halogenlamp = tr("halogenlamp");
  cleanjar = tr("cleanjar");
  ctrlboardalarm = tr("ctrlboardalarm");
  System_Status = tr("System_Status");
  sample_alarm = tr("sample_alarm");

  warn_capture_Mode = tr("warn_capture_Mode");
  operate_sucess_system_reboot = tr("operate_sucess_system_reboot");

  network_abnormal = tr("network_abnormal");
  screen_params = tr("screen_params");
  sorteradj_params = tr("sorteradj_params");

  acc_runtime = tr("acc_runtime");
  acc_detecttime = tr("acc_detecttime");
  cur_runtime = tr("cur_runtime");

  statistic_PPM = tr("statistic_PPM");
  statistic_percentage = tr("statistic_percentage");

  engineer = tr("engineer");
  edit_factoryname = tr("edit_factoryname");
  statisticclassEn = tr("statisticclassEn");
  factoryname = tr("factoryname");
  warn_contains_speical_character = tr("warn_contains_speical_character");
  time_calibrate = tr("time_calibrate");

  system_com_selfcheck = tr("system_com_selfcheck");

  yield = tr("yield");
  delschme_And_cleandatadb = tr("delschme_And_cleandatadb");

  backup_scheme_sucess = tr("backup_scheme_sucess");
  backup_scheme_failed = tr("backup_scheme_failed");
  restore_scheme_sucess = tr("restore_scheme_sucess");
  restore_scheme_failed = tr("restore_scheme_failed");
  scheme_invalid = tr("scheme_invalid");
  confirm_restore_scheme = tr("confirm_restore_scheme");
  confirm_backup_scheme = tr("confirm_backup_scheme");
  multi_vibrator = tr("multi_vibrator");
  external_vibrator = tr("external_vibrator");
  select_print_staisticgp = tr("select_print_staisticgp");
  printing_and_wait = tr("printing_and_wait");
  invalid_data = tr("invalid_data");
  detect_time = tr("detect_time");
  device_name = tr("device_name");
  detector = tr("detector");
  clean_set = tr("clean_set");
  opaque = tr("opaque");
  skip = tr("skip");
  application = tr("application");
  current_scheme_alreadybackup = tr("current_scheme_alreadybackup");
  confirm_set_cam = tr("confirm_set_cam");
  confirm_set_machineAndassist = tr("confirm_set_machineAndassist");
  confirm_set_machineAndmajor = tr("confirm_set_machineAndmajor");
  onekeyparams = tr("onekeyparams");
  set_cameranum_first = tr("set_cameranum_first");
  yes = tr("yes");
  no = tr("no");

  meyer_company = tr("meyer_company");
  provide_technical_support = tr("provide_technical_support");
  warn_store = tr("warn_store");
  friendly_reminder = tr("friendly_reminder");
  cam_tempure_alarm = tr("cam_tempure_alarm");
  temp_const_ctrl = tr("temp_const_ctrl");
  tempture_thd = tr("tempture_thd");
  current_thd = tr("current_thd");

  simulate_en = tr("simulate_en");
  clear_simulate_pic = tr("clear_simulate_pic");

  setclassifythd = tr("setclassifythd");
  other = tr("other");

  adjusting_sorter = tr("adjusting_sorter");

  merge_ret = tr("merge_ret");

  color_and_material = tr("color_and_material");
  please_cancle_secondary_lab = tr("please_cancle_secondary_lab");
}

void MyLanguage::setLang() {
  QTranslator *trans = new QTranslator;
  QString qmFile;

  //! 构建翻译文件路径
  switch (nLang) {
  case LANG_CHS:
    qmFile = QString("%1zh-cn.qm").arg(LANG_DIR);
    break;
  case LANG_CHT:
    qmFile = QString("%1zh-tw.qm").arg(LANG_DIR);
    break;
  case LANG_ENG:
    qmFile = QString("%1en.qm").arg(LANG_DIR);
    break;
  case LANG_RUS:
    qmFile = QString("%1ru.qm").arg(LANG_DIR);
    break;
  case LANG_VIET:
    qmFile = QString("%1vi.qm").arg(LANG_DIR);
    break;
  case LANG_THAILAND:
    qmFile = QString("%1th.qm").arg(LANG_DIR);
    break;
  case LANG_SPAN:
    qmFile = QString("%1es.qm").arg(LANG_DIR);
    break;
  case LANG_TURKEY:
    qmFile = QString("%1tr.qm").arg(LANG_DIR);
    break;
  case LANG_AFARSI:
    qmFile = QString("%1fa.qm").arg(LANG_DIR);
    break;
  case LANG_AFRENCH:
    qmFile = QString("%1fr.qm").arg(LANG_DIR);
    break;
  case LANG_UYGHUR:
    qmFile = QString("%1ug.qm").arg(LANG_DIR);
    break;
  case LANG_AKOREAN:
    qmFile = QString("%1ko.qm").arg(LANG_DIR);
    break;
  case LANG_AARABIC:
    qmFile = QString("%1ar.qm").arg(LANG_DIR);
    break;
  case LANG_ABULGARIAN:
    qmFile = QString("%1bg.qm").arg(LANG_DIR);
    break;
  case LANG_ALOVAK:
    qmFile = QString("%1sk.qm").arg(LANG_DIR);
    break;
  case LANG_APORTUGUESE:
    qmFile = QString("%1pt.qm").arg(LANG_DIR);
    break;
  case LANG_MYANMAR:
    qmFile = QString("%1my.qm").arg(LANG_DIR);
    break;
  case LANG_ABENGALI:
    qmFile = QString("%1bn.qm").arg(LANG_DIR);
    break;
  case LANG_INDONESIA:
    qmFile = QString("%1id.qm").arg(LANG_DIR);
    break;
  case LANG_POLAND:
    qmFile = QString("%1pl.qm").arg(LANG_DIR);
    break;
  case LANG_ITALY:
    qmFile = QString("%1it.qm").arg(LANG_DIR);
    break;
  case LANG_COMBODIA:
    qmFile = QString("%1cb.qm").arg(LANG_DIR);
    break;
  case LANG_AGERMAN:
    qmFile = QString("%1de.qm").arg(LANG_DIR);
    break;
  case LANG_AHUNGARIAN:
    qmFile = QString("%1hu.qm").arg(LANG_DIR);
    break;
  case LANG_JAVANESE:
    qmFile = QString("%1jv.qm").arg(LANG_DIR);
    break;
  default:
    qmFile = QString("%1zh-cn.qm").arg(LANG_DIR);
    break;
  }

  // 检查翻译文件是否存在
  if (!QFile::exists(qmFile)) {
    qWarning() << "Translation file not found:" << qmFile;
    delete trans;
    return;
  }

  // 尝试加载翻译文件
  bool loaded = trans->load(qmFile);
  if (!loaded) {
    qWarning() << "Failed to load translation file:" << qmFile;
    delete trans;
    return;
  }

  // 安装翻译器
  if (!QCoreApplication::installTranslator(trans)) {
    qWarning() << "Failed to install translator for:" << qmFile
               << "App name:" << QCoreApplication::applicationName()
               << "Thread:" << QThread::currentThread();
    delete trans;
    return;
  }

  qInfo() << "Successfully loaded and installed translation:" << qmFile;

  init();
  //    emit sigChgLanguage();
}

int MyLanguage::getLang() { return nLang; }

bool MyLanguage::IsChineseLang() {
  return (nLang == LANG_CHS || nLang == LANG_CHT);
}

QList<int> MyLanguage::getLanguageList() {
  QList<int> list;

  list << LANG_CHS << LANG_CHT << LANG_ENG << LANG_VIET << LANG_THAILAND
       << LANG_MYANMAR << LANG_AKOREAN << LANG_UYGHUR << LANG_AARABIC
       << LANG_AFARSI << LANG_TURKEY << LANG_RUS << LANG_ABULGARIAN
       << LANG_ALOVAK << LANG_AFRENCH << LANG_SPAN << LANG_APORTUGUESE
       << LANG_ABENGALI << LANG_INDONESIA << LANG_POLAND << LANG_ITALY
       << LANG_COMBODIA << LANG_AGERMAN << LANG_AHUNGARIAN << LANG_JAVANESE;

  return list;
}

QString MyLanguage::getLanguageListName(int index, bool local) {
  QString text = "Unknow";
  switch (index) {
  case LANG_CHS:
    text = local ? "简体中文" : chinese_simplified;
    break;
  case LANG_CHT:
    text = local ? "繁体中文" : chinese_traditonal;
    break;
  case LANG_ENG:
    text = local ? "英语" : english;
    break;
  case LANG_VIET:
    text = local ? "越南语" : vietnam;
    break;
  case LANG_THAILAND:
    text = local ? "泰语" : thailand;
    break;
  case LANG_MYANMAR:
    text = local ? "缅甸语" : myanmar;
    break;
  case LANG_AKOREAN:
    text = local ? "韩语" : korean;
    break;
  case LANG_TURKEY:
    text = local ? "土耳其语" : turkey;
    break;
  case LANG_UYGHUR:
    text = local ? "维吾尔语" : uyghur;
    break;
  case LANG_RUS:
    text = local ? "俄语" : russian;
    break;
  case LANG_ABULGARIAN:
    text = local ? "保加利亚语" : bulgarian;
    break;
  case LANG_ALOVAK:
    text = local ? "斯洛伐克语" : slovak;
    break;
  case LANG_AFRENCH:
    text = local ? "法语" : french;
    break;
  case LANG_SPAN:
    text = local ? "西班牙语" : spanish;
    break;
  case LANG_APORTUGUESE:
    text = local ? "葡萄牙语" : portuguese;
    break;
  case LANG_AARABIC:
    text = local ? "阿拉伯语" : arabic;
    break;
  case LANG_AFARSI:
    text = local ? "波斯语" : farsi;
    break;
  case LANG_ABENGALI:
    text = local ? "孟加拉语" : bengali;
    break;
  case LANG_INDONESIA:
    text = local ? "印度尼西亚语" : indonesia;
    break;
  case LANG_POLAND:
    text = local ? "波兰语" : poland;
    break;
  case LANG_ITALY:
    text = local ? "意大利语" : italy;
    break;
  case LANG_COMBODIA:
    text = local ? "柬埔寨语" : combodia;
    break;
  case LANG_AGERMAN:
    text = local ? "德语" : german;
    break;
  case LANG_AHUNGARIAN:
    text = local ? "匈牙利语" : hungarian;
    break;
  case LANG_JAVANESE:
    text = local ? "爪哇语" : javanese;
    break;
  default:
    text = local ? "未定义" : "Unknow";
    break;
  }

  return text;
}

QIcon MyLanguage::getLanguageListIcon(int index) {
  QString str;
  switch (index) {
  case LANG_CHS:
  case LANG_CHT:
  case LANG_UYGHUR:
    str = "Flag_CN.png";
    break;
  case LANG_ENG:
    str = "Flag_GB.png";
    break;
  case LANG_VIET:
    str = "Flag_VN.png";
    break;
  case LANG_THAILAND:
    str = "Flag_TH.png";
    break;
  case LANG_MYANMAR:
    str = "Flag_MM.png";
    break;
  case LANG_AKOREAN:
    str = "Flag_KR.png";
    break;
  case LANG_TURKEY:
    str = "Flag_TR.png";
    break;
  case LANG_RUS:
    str = "Flag_RU.png";
    break;
  case LANG_ABULGARIAN:
    str = "Flag_BG.png";
    break;
  case LANG_ALOVAK:
    str = "Flag_SK.png";
    break;
  case LANG_AFRENCH:
    str = "Flag_FR.png";
    break;
  case LANG_SPAN:
    str = "Flag_ES.png";
    break;
  case LANG_APORTUGUESE:
    str = "Flag_PT.png";
    break;
  case LANG_AARABIC:
    str = "Flag_SA.png";
    break;
  case LANG_AFARSI:
    str = "Flag_IR.png";
    break;
  case LANG_ABENGALI:
    str = "Flag_BD.png";
    break;
  case LANG_JAVANESE:
  case LANG_INDONESIA:
    str = "Flag_ID.png";
    break;
  case LANG_POLAND:
    str = "Flag_PL.png";
    break;
  case LANG_ITALY:
    str = "Flag_IT.jpg";
    break;
  case LANG_COMBODIA:
    str = "Flag_CB.jpg";
    break;
  case LANG_AGERMAN:
    str = "Flag_DE.png";
    break;
  case LANG_AHUNGARIAN:
    str = "Flag_HU.png";
    break;
  default:
    break;
  }

  QIcon icon = QIcon(":/res/png/Flag_png/" + str);
  return icon;
}

void MyLanguage::getLanguageSetting() {
  QSettings setting(CNF_LANG, QSettings::IniFormat);

  nLang = setting.value("Lang", LANG_CHS).toInt();
  qDebug("nLang value=%d", nLang);
  for (int i = 0; i < MAX_LANG; i++) {
    if (i == 0)
      nLangList[i] = setting.value(QString("LangList_%1").arg(i), 1).toInt();
    else
      nLangList[i] =
          setting.value(QString("LangList_%1").arg(i), LANG_CHS).toInt();
  }
}

void MyLanguage::saveLanguageSetting() {
  QSettings setting(CNF_LANG, QSettings::IniFormat);

  setting.setValue("Lang", nLang);

  for (int i = 0; i < MAX_LANG; i++) {
    setting.setValue(QString("LangList_%1").arg(i), nLangList[i]);
  }
  setting.sync();
}
