/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylanguage.h
 * \brief       界面词根头文件
 * \date        2019.10.30
 */
#ifndef MYLANGUAGE_H
#define MYLANGUAGE_H

#include <QIcon>
#include <QtCore>

#define MAX_LANG 64
#define CNF_LANG "/ssd/cnf/cnf.language" //! 语言文件

/*!
 * \brief 语言列表
 */
enum {
  LANG_CHS = 0,     /*! 简体中文 */
  LANG_ENG,         /*! 英文 */
  LANG_RUS,         /*! 俄文 */
  LANG_VIET,        /*! 越南文 */
  LANG_THAILAND,    /*! 泰文 */
  LANG_SPAN,        /*! 西班牙文 */
  LANG_CHT,         /*! 繁体中文 */
  LANG_TURKEY,      /*! 土耳其文 */
  LANG_AFARSI,      /*! 波斯文 */
  LANG_AFRENCH,     /*! 法文 */
  LANG_UYGHUR,      /*! 维吾尔文 */
  LANG_AKOREAN,     /*! 韩文 */
  LANG_AARABIC,     /*! 阿拉伯文 */
  LANG_ABULGARIAN,  /*! 保加利亚文 */
  LANG_ALOVAK,      /*! 斯洛伐克语 */
  LANG_APORTUGUESE, /*! 葡萄牙语 */
  LANG_MYANMAR,     /*! 缅甸语 */
  LANG_ABENGALI,    /*! 孟加拉语 */
  LANG_INDONESIA,   /*! 印度尼西亚语 */
  LANG_POLAND,      /*! 波兰语 */
  LANG_ITALY,       /*! 意大利语 */
  LANG_COMBODIA,    /*! 柬埔寨语 */
  LANG_AGERMAN,     /*! 德语 */
  LANG_AHUNGARIAN,  /*! 匈牙利语 */
  LANG_JAVANESE,    /*! 爪哇语 */
  LANG_NUM
};

/*!
 * \brief 界面词根类
 */
class MyLanguage : public QObject {
  Q_OBJECT
public:
  explicit MyLanguage(QObject *parent = 0);

  void init(void); /*! 初始化词根 */
  void setLang();  /*! 语言设置 */
  int getLang();
  bool IsChineseLang();

  /*! 设置语言(语言使能名称相关)*/
  QList<int> getLanguageList();
  QString getLanguageListName(int index, bool local = true);
  QIcon getLanguageListIcon(int index);

  /*! 语言相关参数 */
  void getLanguageSetting();  /*! 获取网络配置文件参数 */
  void saveLanguageSetting(); /*! 保存网络配置文件参数 */

signals:
  void sigChgLanguage();

public:
  int nLang;               /*! 当前系统使用语言序号 */
  int nLangList[MAX_LANG]; /*! 语言列表，[0] - 当前设置语言总数, [other] -
                              语言编号 */

public:
  QString poweroff;
  QString save;
  QString factor;
  QString user;
  QString detect;
  QString open;
  QString close;

  QString metal;
  QString stagp;
  QString total;

  QString schme;
  QString copy;
  QString rename;

  QString display;
  QString home;
  QString history;
  QString prodouct;
  QString setting;
  QString AI;
  QString SystemSta;
  QString Identifity;
  QString System;

  QString net;
  QString version;
  QString clean;
  QString feeder;
  QString sens;
  QString tick;
  QString belt;

  QString same;
  QString alone;
  QString back;
  QString update;
  QString refresh;
  QString stop;

  QString from;
  QString to;
  QString add;
  QString del;
  QString del_all;
  QString del_db;
  QString ok;
  QString cancel;
  QString operate_sucess;
  QString operate_failed;
  QString selected_item;
  QString clear_selection;
  QString ailabel_count;
  QString cam_AIlabel_count;

  QString batch;
  QString notice;
  QString machine;
  QString serial_num;
  QString password;
  QString params;
  QString sucess;
  QString failed;
  QString auto_sample;
  QString channel;
  QString scheduledfeedEn;
  QString scheduledfeedtime;

  QString detail;
  QString category;
  QString area;
  QString status;
  QString number;
  QString purity;
  QString ratio;
  QString weight;
  QString little;
  QString middle;
  QString big;

   //! 传统算法相关语言词根
  QString gray_a;
  QString gray_b;
  QString discolor_a;
  QString discolor_b;
  QString name;
  QString sensitivity;
  QString mode;
  QString row_count;
  QString bad_point;
  QString infection;
  QString infection_1;
  QString infection_2;
  QString cut_edge;
  QString independent_delay;
  QString lower_limit_thread;
  QString algorithm_enable;
  QString algorithm_params;
  QString more_retain;
  QString retain;
  QString bright;
  QString dark;
  QString red;
  QString green;
  QString blue;
  QString red_green;
  QString red_blue;
  QString green_blue;
  QString nr;
  QString ng;
  QString nb;
  QString lab_set;


  QString print;
  QString exporting;
  QString currentpage;
  QString time;
  QString normal;
  QString abnormal;
  QString all;
  QString date;
  QString record;
  QString duty_shift;

  QString domain;
  QString aging;
  QString fluore;
  QString Light_color;
  QString trans_white;

  //! 升级
  QString upgrade;
  QString user_mode;
  QString factory_mode;
  QString analyze;
  QString camer;
  QString control_board;
  QString light_board;
  QString sample;
  QString GPU;
  QString halogen_board;
  QString elec_valve;
  QString low_valve;
  QString local;
  QString Udisk;
  QString arm_linux;
  QString board_AI;
  QString AImodel;
  QString forbid_cross_version;
  QString type;
  QString download;
  QString enable;
  QString file;
  QString modelIdx;
  QString clear_params;

  //! Net
  QString QR_code;
  QString wechatApp;
  QString remote;
  QString cloud;
  QString connect_server;
  QString dhclient;
  QString Manual_set;
  QString address;
  QString subnet_mask;
  QString gateway;
  QString server;
  QString port;
  QString username;

  QString vibrate_magnitude;
  QString vibrator;

  //! 设置
  QString signal_debug;
  QString label_params;
  QString authority_card;
  QString language;
  QString machine_params;
  QString periph_device;
  QString language_set;
  QString calibrate;
  QString sorter_debug;

  //! 信号
  QString ori_wave;
  QString adj_wave;
  QString adj_signal;
  QString adj_target;
  QString auto_adj;
  QString light_adj;
  QString dark_adj;
  QString frequence_adj;
  QString gain_adj;
  QString lamp_adjust;
  QString mat_lamp;
  QString back_lamp;
  QString inf_lamp;
  QString background_params;
  QString threshold_params;
  QString confirm_calibrate;
  QString pixel_dived;
  QString begin_pixel;
  QString end_pixel;
  QString auto_dived;
  QString infra_bias;
  QString fast_adjust;
  QString fine_adjust;
  QString frequence;
  QString lamp;
  QString background_set;
  QString background_conditon;
  QString background_threshold;
  QString background_uplimit;
  QString black_bg;
  QString red_bg;
  QString green_bg;
  QString yellow_bg;
  QString blue_bg;
  QString purple_bg;
  QString cyan_bg;
  QString white_bg;
  QString forbid_adjust_signal;
  QString start_ejc;
  QString end_ejc;

  QString over_max_picture_num;
  QString triger_lowlimit;
  QString no_found_picture;
  QString import_pic_sucess;
  QString import_pic_failed;
  QString confirm_export_picture;
  QString params_set;
  QString rm_background;
  QString capture_line;

  QString lab_hide;
  QString shapeness_enable;
  QString ori_lab;
  QString meger_lab;
  QString lab_Meger;
  QString mapping_lab;

  QString serial_num_error;
  QString language_enable;
  QString confirm_switch_lang;
  QString switch_lang_sucess;
  QString invalid_file;
  QString update_lang_sucess;
  QString update_lang_failed;
  QString restore_lang_sucess;
  QString restore_lang_failed;
  QString restore;

  QString assist_camer;
  QString major_camer;
  QString sta_type;
  QString sorter_jointdebug;
  QString sorter_adj_tips;
  QString debug_info;
  QString impurity_sta;
  QString triger_type;
  QString lab_process;
  QString statics_gp_set;
  QString camer_name_set;
  QString params_backup;
  QString params_restore;
  QString multilab;
  QString over_max_camer_num;
  QString camera_name;
  QString params_backup_sucess;
  QString params_backup_failed;
  QString udisk_or_file_not_found;
  QString restore_params_sucess;
  QString restore_params_failed;
  QString light_sync;

  QString halogen_set;
  QString halogen_enable;
  QString servo_set;
  QString servo_Init;
  QString Ejector_valve_set;
  QString Ejector_valve_enalbe;
  QString interval_time;
  QString other_periph_device;
  QString clean_tank_alarm_enable;
  QString pressure_alarm_enable;
  QString elec_valve_enable;
  QString clean_tank_reset;
  QString elec_valve_set;
  QString metal_detect;

  QString coordinate_info;
  QString cal_coord;
  QString water_cal;
  QString cal_group;
  QString cal_value;
  QString actual_value;
  QString select_cal_group;
  QString select_cal_group_first;
  QString please_calibrate_first;
  QString please_input_calvalue;

  QString max_channel;
  QString num_threshold;
  QString time_threshold;
  QString sigle_channel_time;
  QString detect_delay;
  QString sample_interval;
  QString reset_coeff;
  QString adj_page;
  QString confirm_reset_coeff;
  QString connect_server_sucess;
  QString connect_server_failed;
  QString commutication_sucess;
  QString commutication_failed;
  QString aimodel_abnormal;
  QString sorter_jointdebug_failed;
  QString Acc_Mode;
  QString Cycle_time;
  QString samplechnelEb;
  QString samplechnelProty;

  QString more_set;
  QString AImodel_info;
  QString camera_index;
  QString label_enable;
  QString AImodel_lst;
  QString retain_line;
  QString cap_line;
  QString overlap_degree_coeff;
  QString area_threshold;
  QString AImodel_type;
  QString capture_set;
  QString raw_pic;
  QString mark_pic;
  QString label_select;
  QString pic_export;
  QString pic_preview;
  QString log_export;
  QString CropImg_export;
  QString pic_clear;
  QString capture_type;
  QString label_priority;
  QString label_mix;
  QString no_such_aimodel;
  QString confirm_switch_model;
  QString mount_udisk_failed;
  QString lab_mutex;
  QString pre_vire_crop_img;
  QString pre_vire_img;
  QString no_find_imgdir;
  QString no_imgfile;
  QString img_load_failed;
  QString unzip_file_failed;
  QString imgconvert;
  QString imgcompress;
  QString unknown_error;
  QString msg_getfilesize;
  QString msg_file_open_failed;
  QString msg_file_too_large;
  QString msg_udisk_not_enough;
  QString msg_data_upload_error;
  QString msg_no_data;
  QString msg_data_error;
  QString msg_data_index_error;
  QString msg_data_check_error;
  QString create_file;
  QString file_copy;
  QString elapsed_time;
  QString ori_labimg;
  QString small_labimg;
  QString previewtype;

  QString label;
  QString target;
  QString capture;
  QString lighting;
  QString lighting_name;

  QString gain;
  QString background;

  QString variation_range;

  QString lower_limit;
  QString up_limit;

  QString resolution;
  QString exposure_value;
  QString Non_exposure_value;
  QString spectral_band;
  QString sensor;
  QString odd;

  QString halogen;
  QString temperature;
  QString threshold;
  QString index;

  QString apply_and_wait;
  QString digital_gain;
  QString analog_gain;

  QString import;

  QString synchronize;
  QString no_found_udisk;
  QString timeout;

  QString properity;
  QString coefficient;
  QString shapeness;
  QString secondary;
  QString majorProduct;
  QString set_majorProduct_first;
  QString length;
  QString width;
  QString scan_by_MeyerAPP;
  QString apply;

  QString no_find_file;

  QString bottle;
  QString grains;

  QString internal;
  QString external;
  QString loT;
  QString backup;
  QString sys_params_fix;

  QString servo;
  QString ONOFF;
  QString speed;
  QString acceleration_time;
  QString PWM;
  QString low_voltage;
  QString high_voltage;
  QString workmode;
  QString background_bar;
  QString reset;
  QString plus_light;

  QString ejector_selfcheck;
  QString start;
  QString hig_speed;
  QString low_speed;
  QString sigle_ejc;
  QString cur_ejec;

  QString contains_specialCharters;
  QString sorter_joint_config;
  QString finished_product;
  QString waste_material;
  QString raw_material;
  QString Normal_open_valve;
  QString str_is_to_long;
  QString empty;
  QString overlap_degree;
  QString center_rect;
  QString low_res;
  QString hig_res;
  QString ultra_hig_res;
  QString do_not_save;
  QString clear;
  QString tradition;
  QString standard;
  QString std_hyperspectral;
  QString ultraviolet;
  QString model_empty_or_no_AI;
  QString pic_size_mismatch;
  QString affect_detect_res;
  QString selct_enable_lab;

  QString merger;
  QString leftMove;
  QString newitem;
  QString historyitem;
  QString input_forbid_empyty;
  QString good;
  QString bad;
  QString particle;

  //! 加解密
  QString encryption;
  QString confirm_sys_time;
  QString input_code;
  QString invalid_code;
  QString dc_length_error;
  QString dc_illegal_char;
  QString dc_illegal;
  QString dc_id_mismatch;
  QString msg_dc_expire;
  QString dc_limit;

  QString limit_remain;
  QString dc_input_time_decode;
  QString msg_time_abnormal;
  QString remind_times;
  QString cfm_input_time_code;

  QString encrypt_remind;
  QString unremind;

  QString Eng;
  QString chinese;

  QString average;
  QString data_analyze;

  QString sys_time_error;
  QString time_remain_days;
  QString time_remain_times;
  QString input_time;

  QString reset_device_code;
  QString set_timezone;
  QString confirm_reset_devicecode;
  QString select_timezone;
  QString confirm_set_timezone;
  QString set_timezone_success;
  QString set_timezone_failed;

  QString please_stop_detect;
  QString confirm_shutdown;
  QString save_params_sucess;
  QString save_params_failed;
  QString confirm_save_params;
  QString switch_to_factorper;
  QString switch_to_userPer;
  QString please_input_password;
  QString password_error;
  QString auto_sample_channel;
  QString stop_auto_sample;
  QString current_schme;
  QString over_max_schme_num;
  QString confirm_copy_schme;
  QString copy_schme_failed;
  QString confirm_del_schme;
  QString forbid_del_schme;
  QString confirm_rename_schme;
  QString invalid_schme_name;
  QString input_schme_name;
  QString switch_board_CL_to_factor;
  QString switch_board_CL_to_user;
  QString erase_board_CL;
  QString transmit_and_burn;
  QString switch_board_Cam_to_user;
  QString switch_board_Cam_to_factor;
  QString switch_ejector_to_factor;
  QString switch_ejector_to_user;
  QString erase_board_Ejector;
  QString erase_board_Cam;
  QString select_file;
  QString no_select_upgrade_board;
  QString no_find_udisk;
  QString no_invalid_file;
  QString open_file_failed;
  QString reset_upgrade_status;
  QString burn_file_failed;
  QString loss_file;
  QString check_file_failed;
  QString download_file_failed;
  QString no_start_upgrade;
  QString switch_factory_failed;
  QString switch_user_failed;
  QString erase_failed;
  QString upgrade_sucess;
  QString file_type_error;
  QString sample_abnormal;
  QString delschme_And_cleandatadb;

  QString wechat_QR;
  QString vpn_connect;
  QString input_error;
  QString dhclient_failed;
  QString disconect;
  QString feeder_set;
  QString clean_interval;
  QString clean_time;
  QString airknife_time;
  QString airknife_channel;
  QString tick_setting;

  QString delay_time;
  QString blow_time;
  QString trunc_time;
  QString deg_time;
  QString cacheline;
  QString hig_fre_protect;
  QString ejector_onoff;
  QString ejector_divide;
  QString ejector_index;
  QString ejector_piexl;
  QString positive_cache;
  QString negative_cache;
  QString Rect_ratio;
  QString times;
  QString endtime_lessthan_starttime;
  QString no_select_item;
  QString not_support_select_multi_item;
  QString require_data_and_wait;
  QString confirm_del;
  QString export_data_and_wait;
  QString no_check_valid_data;
  QString export_sucess;
  QString export_failed;
  QString export_select_item;
  QString export_date_item;
  QString array_row;
  QString array_colum;
  QString select_export_config;

  QString switch_to_advance;
  QString user_already_bind;
  QString confirm_bind;
  QString switch_scheme;
  QString start_detect;
  QString input_batchname;
  QString inputy_shiftname;
  QString invalid_serialnum;
  QString user_list;

  QString pie_chart;
  QString bar_chart;
  QString select_staGb;
  QString calibrate_GGP_wave;
  QString up_camer;
  QString down_camer;
  QString ultra_camer;
  QString hyperspec_camer;
  QString up_bglight;
  QString up_matlight;
  QString down_bglight;
  QString down_matlight;
  QString ultra_light;

  QString small;
  QString switch_to_label;
  QString switch_to_shape;
  QString auto_sample_abnormal;

  QString airalarm;
  QString halogenlamp;
  QString cleanjar;
  QString ctrlboardalarm;
  QString System_Status;
  QString sample_alarm;

  QString warn_capture_Mode;
  QString operate_sucess_system_reboot;

  QString network_abnormal;
  QString screen_params;
  QString sorteradj_params;

  QString acc_runtime;
  QString acc_detecttime;
  QString cur_runtime;

  QString statistic_PPM;
  QString statistic_percentage;

  //! 工程师
  QString engineer;
  QString edit_factoryname;
  QString statisticclassEn;
  QString factoryname;
  QString warn_contains_speical_character;
  QString time_calibrate;

  QString system_com_selfcheck;

  QString yield;

  QString backup_scheme_sucess;
  QString backup_scheme_failed;
  QString current_scheme_alreadybackup;
  QString restore_scheme_sucess;
  QString restore_scheme_failed;
  QString scheme_invalid;
  QString confirm_restore_scheme;
  QString confirm_backup_scheme;
  QString multi_vibrator;
  QString external_vibrator;
  QString select_print_staisticgp;
  QString printing_and_wait;
  QString invalid_data;
  QString detect_time;
  QString device_name;
  QString detector;
  QString clean_set;
  QString opaque;
  QString skip;
  QString application;
  QString confirm_set_cam;
  QString confirm_set_machineAndassist;
  QString confirm_set_machineAndmajor;
  QString onekeyparams;
  QString set_cameranum_first;
  QString yes;
  QString no;
  QString meyer_company;
  QString provide_technical_support;
  QString warn_store;
  QString friendly_reminder;
  QString cam_tempure_alarm;
  QString temp_const_ctrl;
  QString tempture_thd;
  QString current_thd;

  QString simulate_en;
  QString clear_simulate_pic;

  QString setclassifythd;
  QString other;
  QString adjusting_sorter;

  QString merge_ret;

  QString color_and_material;
  QString please_cancle_secondary_lab;


  //! language
  QString space;
  QString english;
  QString chinese_simplified;
  QString chinese_traditonal;
  QString russian;
  QString vietnam;
  QString thailand;
  QString spanish;
  QString turkey;
  QString farsi;
  QString french;
  QString uyghur;
  QString korean;
  QString arabic;
  QString bulgarian;
  QString slovak;
  QString portuguese;
  QString myanmar;
  QString bengali;
  QString indonesia;
  QString poland;
  QString italy;
  QString combodia;
  QString german;
  QString hungarian;
  QString javanese;
};

extern MyLanguage myLan;

#endif // MYLANGUAGE_H
